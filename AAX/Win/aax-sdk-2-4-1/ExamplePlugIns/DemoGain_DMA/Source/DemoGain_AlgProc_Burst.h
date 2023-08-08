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
 *	\file   DemoGain_AlgProc_Burst.h
 *
 *	\brief  Definitions shared by the DemoGain_DMA classes in one easy-to-maintain file.
 * 
 *	\author Rob Majors
 *	
 *	
 *	DEMOGAIN_DMA - BURST ALGORITHM
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
 *		                                          >|< {   pad   }
 *		    External: [0 0 0 0 A B C D E F G H I J k l] m n 0 0}
 *		                                  >|<
 *		
 *		                  >|< (circular wraparound)
 *		    Internal: [0 0 g h i j 0 0 A B C D] E F 0 0 }
 *		                                  >|<
 *		
 *		Sixth render:
 *		in[O P Q R] out[G H I J]
 *		                  >|<
 *		    External: [0 0 o p q r E F G H I J K L] M N 0 0}
 *		                                      >|<
 *		
 *		                  >|< (circular wraparound)
 *		    Internal: [0 0 k l m n 0 0 A B C D] E F 0 0 }
 *		                                  >|<
 *
 *	\todo This isn't a robust handling of wraparound.  What if the buffer size
 *	increases between the fifth and sixth renders above?  We need some way to
 *	always rely on the non-padded data when writing to output.
 *	- When the internal buffer gather position wraps around, it should fill in
 *	  the data from the pad to the start of the buffer at the beginning of the
 *	  next render
 *
 *	\endverbatim
 *	
 */ 
/*================================================================================================*/

#pragma once
#ifndef DEMOGAIN_ALGPROC_BURST_H
#define DEMOGAIN_ALGPROC_BURST_H

#include "DemoGain_AlgCommon.h"

// AAX includes
#include "AAX_IDma.h"
#include "AAX_Enums.h"
#include "AAX.h"


//==============================================================================
// General definitions
//==============================================================================

const uint32_t cDemoGain_DMABurst_ReadOffset = cDemoGain_DMA_MaxBufferLength;


//
// DMA transfer properties
//

const uint32_t cDemoGain_DMABurst_NumBursts = 16;
const uint32_t cDemoGain_DMABurs_MaxBurstSize = cDemoGain_DMA_MaxBufferSize;
const uint32_t cDemoGain_DMABurst_BurstLength = 64; // Magic number as per AAX_IDma::SetBurstLength


//
// Audio buffer properties
//

// External memory buffer
const uint32_t cDemoGain_DMABurst_ExternalBufferLength = (1 << AAX_eAudioBufferLength_Max) << 1; //TODO: Don't need the factor of 2 if using scatter/gather?
const uint32_t cDemoGain_DMABurst_ExternalBufferLengthMask = cDemoGain_DMABurst_ExternalBufferLength - 1;
const uint32_t cDemoGain_DMABurst_ExternalBufferPaddedLength = cDemoGain_DMABurst_ExternalBufferLength + cDemoGain_DMA_MaxBufferLength;

// Internal read buffer
const uint32_t cDemoGain_DMABurst_LocalReadBufferLength = 2 * cDemoGain_DMA_MaxBufferLength;
const uint32_t cDemoGain_DMABurst_LocalReadBufferLengthMask = cDemoGain_DMABurst_LocalReadBufferLength - 1;
const uint32_t cDemoGain_DMABurst_LocalReadBufferPaddedLength = cDemoGain_DMABurst_LocalReadBufferLength + cDemoGain_DMA_MaxBufferLength;



//==============================================================================
// Memory block structure definitions
//==============================================================================

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_ALG
#include AAX_ALIGN_FILE_END
struct SDemoGain_DMABurst_RWBuffer
{
	float mSample[cDemoGain_DMABurst_ExternalBufferPaddedLength];
};

struct SDemoGain_Burst_DMALocalData
{
	uint32_t	curLocalWritePosition;
	uint32_t	curExternalWritePosition;
	
	float		mSample[cDemoGain_DMABurst_LocalReadBufferPaddedLength];
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
struct SDemoGain_DMABurst_AlgContextExt
{
	AAX_IDma						* mDmaBurstP;					// DMA Burst interface
	
	SDemoGain_DMABurst_RWBuffer		* mDMAAudioBufferP;             // External private data
	SDemoGain_Burst_DMALocalData	* mDMALocalDataP;				// Internal private data
};

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_RESET
#include AAX_ALIGN_FILE_END

typedef SDemoGain_DMA_Alg_Context<SDemoGain_DMABurst_AlgContextExt> SDemoGain_DMABurst_Alg_Context;

// Physical addresses within the context
enum EDemoGain_Alg_PortID
{
	eAlgFieldID_Burst_DmaBurst			= AAX_FIELD_INDEX (SDemoGain_DMABurst_Alg_Context, ext.mDmaBurstP)
	
	,eAlgFieldID_Burst_DMAAudioBuffer	= AAX_FIELD_INDEX (SDemoGain_DMABurst_Alg_Context, ext.mDMAAudioBufferP)
	,eAlgFieldID_Burst_DMALocalData		= AAX_FIELD_INDEX (SDemoGain_DMABurst_Alg_Context, ext.mDMALocalDataP)
};


//==============================================================================
// Callback declarations
//==============================================================================

void
AAX_CALLBACK
DemoGain_DMABurst_AlgorithmProcessFunction (
    SDemoGain_DMABurst_Alg_Context * const	inInstancesBegin [],
    const void *							inInstancesEnd);


#endif //DEMOGAIN_ALGPROC_BURST_H
