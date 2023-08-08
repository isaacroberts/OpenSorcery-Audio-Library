/*================================================================================================*/
/*
 *	Copyright 2012-2015, 2019 by Avid Technology, Inc.
 *	All rights reserved.
 *
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 */

/**  
 *	\file   DemoGain_AlgProc_ScatterGather.h
 *
 *	\brief  Declaration of GetEffectDescriptions function.
 *
 *  \author Rob Majors
 *
 *	
 *	DEMOGAIN_DMA - SCATTER/GATHER ALGORITHM
 *	
 *	This plug-in implements a constant-offset delay line in external memory, which 
 *	is accessed via the system's DMA facilities.  
 *	
 *	Delay for this algorithm is twice the maximum possible buffer length, since it 
 *	takes two alg processing passes to move the audio to the output via DMA:
 *	
 *	\verbatim
 *	    1: Copy the audio from input to the external delay line buffer
 *	        (Wait 1 callback for DMA transfer to complete)
 *	    2: Copy the audio from the exteral delay line buffer to an internal buffer
 *	        (Wait 1 callback for DMA transfer to complete)
 *	    3: Apply the audio from the internal buffer to the output
 *	\endverbatim
 *	
 *	This algorithm uses two circular audio buffers (one external, one internal) in
 *	order to maintain a constant delay regardless of potential changes to the
 *	processing buffer size.  Writes to the external buffer "lead" reads by the
 *	maximum audio buffer length, and reads from the external buffer to the
 *	internal buffer lead the output sample tap by the same length.  Here is an
 *	example:
 *	
 *	\verbatim
 *		
 *		Render buffer size: 2
 *		Max buffer size: 4
 *		(Buffer sizes used in this example ar arbitrary)
 *		
 *		Input
 *		[A B C D E F G H I ...]
 *		
 *		First render:
 *		in[A B] out[0 0]
 *		
 *		          write point >|< (small caps: pending transfer)
 *		    External: [0 0 0 0 a b 0 0 0 0 0 0 0 0 0 0]
 *		              >|< read point
 *		
 *		         gather point >|< (gathers from external read point)
 *		    Internal: [0 0 0 0 0 0 0 0 0 0 0 0]
 *		              >|< output tap
 *		
 *		Second render:
 *		in[C D] out[0 0]
 *		                          >|<
 *		    External: [0 0 0 0 A B c d 0 0 0 0 0 0 0 0]
 *		                  >|<
 *		
 *		                          >|<
 *		    Internal: [0 0 0 0 0 0 0 0 0 0 0 0]
 *		                  >|<
 *		
 *		Third render:
 *		in[E F] out[0 0]
 *		                              >|<
 *		    External: [0 0 0 0 A B C D e f 0 0 0 0 0 0]
 *		                      >|<
 *		
 *		                              >|<
 *		    Internal: [0 0 0 0 0 0 0 0 a b 0 0]
 *		                      >|<
 *		
 *		Fourth render:
 *		in[G H] out[0 0]
 *		                                  >|<
 *		    External: [0 0 0 0 A B C D E F g h 0 0 0 0]
 *		                          >|<
 *		
 *		                                  >|<
 *		    Internal: [0 0 0 0 0 0 0 0 A B c d]
 *		                          >|<
 *		
 *		Fifth render:
 *		in[I J] out[A B]
 *		                                      >|<
 *		    External: [0 0 0 0 A B C D E F G H i j 0 0]
 *		                              >|<
 *		
 *		              >|< (circular wraparound)
 *		    Internal: [e f 0 0 0 0 0 0 A B C D]
 *		                              >|<
 *	\endverbatim
 *	
 *	In order to support variable buffer sizes, the internal buffer is padded such 
 *	that it can accept up to one maximum buffer lenth's worth of samples beyond 
 *	its loop point:
 *	
 *	\verbatim
 *		
 *		Example if render buffer length changes to 4
 *		(max) at fourth render
 *		
 *		Fourth render:
 *		in[G H I J] out[0 0 A B]
 *		                                  >|<
 *		    External: [0 0 0 0 A B C D E F g h i j 0 0]
 *		                          >|<
 *		
 *		                                  >|< {   pad   }
 *		    Internal: [0 0 0 0 0 0 0 0 A B c d] e f 0 0 }
 *		                          >|<
 *		
 *		Fifth render:
 *		in[K L M N] out[C D E F]
 *		    (DMA Scatter/Gather facilities automatically handle wraparound)
 *		                                          >|<
 *		    External: [m n 0 0 A B C D E F G H I J k l]
 *		                                  >|<
 *		
 *		                  >|< (circular wraparound)
 *		    Internal: [E F g h i j 0 0 A B C D] e f 0 0 }
 *		                                  >|<
 *	\endverbatim
 *	
 *	These operations are complicated by the fact that AAX Native plug-ins must 
 *	support variable buffer sizes.  AAX native buffer lengths are bounded in the 
 *	range defined by \ref AAX_eAudioBufferLengthNative_Min and 
 *	\ref AAX_eAudioBufferLengthNative_Max and specific possible buffer values are 
 *	defined by \ref AAX_EAudioBufferLength.  All possible buffer values are powers
 *	of two.
 *	
 *	This requirement presents difficulties for a DMA-based external delay line 
 *	that uses the Scatter/Gather DMA modes, since these modes spread written 
 *	values out over a series of 'taps' in the external buffer.  Therefore, when 
 *	using these modes, it is not possible to simply write samples linearly to the 
 *	external buffer using whatever buffer size is provided.
 *	
 *	In order to function properly with this Scatter/Gather behavior, this 
 *	algorithm performs a kind of matrix transposition transform over incoming 
 *	samples before writing them to the external "delay line" taps.  This 
 *	transposition allows the algorithm to append new data across all of the taps 
 *	in a linear fashion, no matter how large the data transfer is, without making 
 *	any assumptions about the size of prior transfers.  The only requirement is 
 *	that the data transfer be a multiple of the number of taps, such that an 
 *	integer number of 'layers' are added to the external buffer with each 
 *	transfer:
 *	
 *	\verbatim
 *		
 *		 Input buffer:
 *		 [A B C D E F G H I J K L]
 *		
 *		 For:
 *		   Transfer size: 12
 *		   Number of taps: 4
 *		
 *		 |---|---|---|---|
 *		 | 1 | 2 | 3 | 4 |
 *		 |---|---|---|---|
 *		 |---|---|---|---|
 *		 | A | B | C | D |
 *		 |---|---|---|---| 
 *		 | E | F | G | H |
 *		 |---|---|---|---|
 *		 | I | J | K | L |
 *		 |---|---|---|---|
 *		
 *		 Transformed buffer:
 *		 [A E I B F J C G K D H L]
 *		
 *		 Lin > Trans        Trans > Lin
 *		 0 > 0         |          0 > 0
 *		 1 > 3         |          1 > 4
 *		 2 > 6         |          2 > 8
 *		 3 > 9         |          3 > 1
 *		 4 > 1         |          4 > 5
 *		 5 > 4         |          5 > 9
 *		
 *		
 *		 EXAMPLES:
 *		
 *		    For 2 taps:
 *		
 *		        Buffer length: 2
 *		        local              external
 *		        [A B]              [A / / / / / / / B / / / / / / /]
 *		
 *		        Buffer length: 8
 *		        local              external
 *		        [A C E G B D F H]  [A C E G / / / / B D F H / / / /]
 *		
 *		
 *		    For 4 taps:
 *		
 *		        Buffer length: 4
 *		        local              external
 *		       [A B C D]          [A / / / B / / / C / / / D / / /]
 *		
 *		       Buffer length: 8
 *		       local              external
 *		       [A E B F C G D H]  [A E / / B F / / C G / / D H / /]
 *		
 *	\endverbatim
 *	
 *	A second complication is the wide range of buffer sizes that the algorithm 
 *	must support: from a minimum of 16 samples on DSP platforms to a maximum of 
 *	1024 samples on Native platforms.  As mentioned above, every data transfer 
 *	must be broken into pieces in an integer multiple of the number of taps.  In 
 *	addition, there is a maximum burst length of 64 Bytes per individual Scatter 
 *	or Gather transfer.  This algorithm therefore uses 64 taps and transposes 
 *	audio buffer data Bytewise across these taps:
 *	
 *	\verbatim
 *	
 *	 	Buffer length   Buffer size   Transfer size per tap
 *	 	16 (float)      64B           1B  (minimum possible)
 *	 	1024 (float)    4096B         64B (maximum possible)
 *	
 *	\endverbatim
 *
 */
/*================================================================================================*/
#pragma once
#ifndef DEMOGAIN_ALGPROC_SCATTERGATHER_H
#define DEMOGAIN_ALGPROC_SCATTERGATHER_H

#include "DemoGain_AlgCommon.h"

// AAX includes
#include "AAX_IDma.h"
#include "AAX_Enums.h"
#include "AAX.h"

// Set this macro to 1 to engage a data integrity
// and latency offset test mode.
#define DEMOGAIN_DMA_TEST_MODE 0



//==============================================================================
// Forward declarations
//==============================================================================

inline uint32_t TransformOffset (const uint32_t inOffset, const uint32_t inDepth);



//==============================================================================
// General definitions
//==============================================================================

//
// DMA transfer properties
//

// The lowest burst size we can manage is 1B, so set the number of R/W
// points such that the minimum audio buffer size results in 1B bursts
const uint32_t cDemoGain_DMASG_NumReadWritePoints = sizeof(float) * (1 << cDemoGain_DMA_TIBufferSizeProperty); // Must be power-of-two
const uint32_t cDemoGain_DMASG_GatherTransferLength = cDemoGain_DMA_MaxBufferLength; // Must be power-of-two
const uint32_t cDemoGain_DMASG_GatherTransferSize = sizeof(float) * cDemoGain_DMASG_GatherTransferLength;
const uint32_t cDemoGain_DMASG_GatherBurstLength = cDemoGain_DMASG_GatherTransferLength / cDemoGain_DMASG_NumReadWritePoints;
const uint32_t cDemoGain_DMASG_GatherBurstSize = sizeof(float) * cDemoGain_DMASG_GatherBurstLength;


//
// Audio buffer properties
//

// External memory buffer
const uint32_t cDemoGain_DMASG_ExternalBufferSize = sizeof(float) * (2 * cDemoGain_DMA_MaxBufferLength);
const uint32_t cDemoGain_DMASG_ExternalBufferSizeMask = cDemoGain_DMASG_ExternalBufferSize - 1;
const uint32_t cDemoGain_DMASG_ExternalBufferSegmentSize = cDemoGain_DMASG_ExternalBufferSize / cDemoGain_DMASG_NumReadWritePoints; // Assuming power-of-two division
const uint32_t cDemoGain_DMASG_ExternalBufferSegmentSizeMask = cDemoGain_DMASG_ExternalBufferSegmentSize - 1;

// Internal read buffer
const uint32_t cDemoGain_DMASG_LocalReadBufferLength = 2 * cDemoGain_DMA_MaxBufferLength;
const uint32_t cDemoGain_DMASG_LocalReadBufferLengthMask = cDemoGain_DMASG_LocalReadBufferLength - 1;
const uint32_t cDemoGain_DMASG_LocalReadBufferPaddedLength = cDemoGain_DMASG_LocalReadBufferLength + cDemoGain_DMA_MaxBufferLength;

// Internal write (temp) buffer
const uint32_t cDemoGain_DMASG_LocalWriteBufferSize = sizeof(float) * cDemoGain_DMA_MaxBufferLength;
const uint32_t cDemoGain_DMASG_LocalWriteBufferSizeMask = cDemoGain_DMASG_LocalWriteBufferSize - 1;



//==============================================================================
// Memory block structure definitions
//==============================================================================

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_ALG
#include AAX_ALIGN_FILE_END

// Scatter/Gather mode requires that the DMA external buffer be padded by the maximum burst size
const uint32_t cDemoGain_DMASG_ExternalBufferPadding = cDemoGain_DMASG_GatherBurstSize;
struct SDemoGain_DMASG_RWBuffer
{
	uint8_t mSample[cDemoGain_DMASG_ExternalBufferSize + cDemoGain_DMASG_ExternalBufferPadding];
};

struct SDemoGain_DMASG_ScratchBuffer
{
	uint8_t mSample[cDemoGain_DMA_MaxBufferSize];
};

struct SDemoGain_DMASG_DMALocalData
{
	uint32_t	mCurLocalReadPosition;
	uint32_t	mCurExternalReadOffset;
	uint32_t	mCurExternalWriteOffset;
	int32_t		mLastBufferLength;
	
	float		mReadBuffer[cDemoGain_DMASG_LocalReadBufferPaddedLength];
	uint8_t		mWriteBuffer[cDemoGain_DMASG_LocalWriteBufferSize];
	
	int32_t		mExtBufferOffsetTable[cDemoGain_DMASG_NumReadWritePoints];
	
#if DEMOGAIN_DMA_TEST_MODE
	static const uint32_t TestModeBufferLength = 0.5 * cDemoGain_DMASG_ExternalBufferSize; // 2 / sizeof(float)
	float mTestModeInputSamples[TestModeBufferLength + cDemoGain_DMASG_MaxBufferLength]; // Padded
	float mTestModeOutputSamples[TestModeBufferLength + cDemoGain_DMASG_MaxBufferLength]; // Padded
	uint32_t mTestModeSampleCount;
	
	void TestModeInitBlock (void)
	{
		for (uint32_t i = 0; i < TestModeBufferLength; i++)
		{
			memcpy (&(mTestModeInputSamples[i]), &i, sizeof(float));
		}
	}
	
	void TestModeRecordSamples (const float* inSampleBuffer, uint32_t inNumSamples)
	{
		memcpy(&mTestModeInputSamples[mTestModeSampleCount], inSampleBuffer, inNumSamples);
		if (mTestModeSampleCount > TestModeBufferLength)
		{
			memcpy(&mTestModeInputSamples[0], &mTestModeInputSamples[TestModeBufferLength], mTestModeSampleCount - TestModeBufferLength);
		}
		mTestModeSampleCount = (mTestModeSampleCount + inNumSamples) & ((TestModeBufferLength)-1);
	}	
#endif // DEMOGAIN_DMA_TEST_MODE
};

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_RESET
#include AAX_ALIGN_FILE_END



//==============================================================================
// Component context definitions
//==============================================================================

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_ALG
#include AAX_ALIGN_FILE_END

// Context structure
struct SDemoGain_DMASG_AlgContextExt
{
	AAX_IDma						* mDmaScatterP;			// DMA Scatter interface
	AAX_IDma						* mDmaGatherP;			// DMA Gather interface
	
	SDemoGain_DMASG_RWBuffer		* mDMAAudioBufferP;		// External private data
	SDemoGain_DMASG_DMALocalData	* mDMALocalDataP;		// Internal private data
	SDemoGain_DMASG_ScratchBuffer	* mScratchBufferP;		// Internal private data (Can be "scratch" memory whenever that feature is implemented)
};

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_RESET
#include AAX_ALIGN_FILE_END

typedef SDemoGain_DMA_Alg_Context<SDemoGain_DMASG_AlgContextExt> SDemoGain_DMASG_Alg_Context;

// Physical addresses within the context
enum EDemoGain_DMASG_Alg_PortID
{
	eAlgFieldID_ScatterGather_DmaScatter			= AAX_FIELD_INDEX (SDemoGain_DMASG_Alg_Context, ext.mDmaScatterP)
	,eAlgFieldID_ScatterGather_DmaGather			= AAX_FIELD_INDEX (SDemoGain_DMASG_Alg_Context, ext.mDmaGatherP)
	
	,eAlgFieldID_ScatterGather_DMAAudioBuffer		= AAX_FIELD_INDEX (SDemoGain_DMASG_Alg_Context, ext.mDMAAudioBufferP)
	,eAlgFieldID_ScatterGather_DMALocalData			= AAX_FIELD_INDEX (SDemoGain_DMASG_Alg_Context, ext.mDMALocalDataP)
	,eAlgFieldID_ScatterGather_ScratchBuffer		= AAX_FIELD_INDEX (SDemoGain_DMASG_Alg_Context, ext.mScratchBufferP)
};



//==============================================================================
// Callback declarations
//==============================================================================

void
AAX_CALLBACK
DemoGain_DMASG_AlgorithmProcessFunction (
	SDemoGain_DMASG_Alg_Context * const	inInstancesBegin [],
	const void *						inInstancesEnd);



//==============================================================================
// Inline function definitions
//==============================================================================

// Transform offset based on a width of cDemoGain_DMASG_NumReadWritePoints
inline uint32_t TransformOffset (const uint32_t inOffset, const uint32_t inDepth)
{
	return ((inOffset % cDemoGain_DMASG_NumReadWritePoints) * inDepth) + (Pow2Div(inOffset, cDemoGain_DMASG_NumReadWritePoints));
}

//// Inverse-transform offset based on a width of cDemoGain_DMASG_NumReadWritePoints
//inline const uint32_t InverseTransformOffset (const uint32_t inIndex, const uint32_t inDepth)
//{
//	return ((inIndex % inDepth) * cDemoGain_DMASG_NumReadWritePoints) + (Pow2Div(inIndex, inDepth));
//}

#endif //DEMOGAIN_ALGPROC_SCATTERGATHER_H
