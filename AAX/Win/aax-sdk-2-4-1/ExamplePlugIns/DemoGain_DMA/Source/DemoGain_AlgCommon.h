/*================================================================================================*/
/*
 *	Copyright 2013-2015, 2019 by Avid Technology, Inc.
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
 *	\file   DemoGain_AlgCommon.h
 *
 *	\brief  Common algorithm component definitions for DemoGain_DMA.
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOGAIN_ALGCOMMON_H
#define DEMOGAIN_ALGCOMMON_H


// AAX includes
#include "AAX_IDma.h"
#include "AAX_Enums.h"
#include "AAX.h"



//==============================================================================
// Forward declarations
//==============================================================================

inline uint32_t Pow2Div (uint32_t inNum, uint32_t inDen);
inline uint32_t IsPowerOfTwo (uint32_t inNum);



//==============================================================================
// General definitions
//==============================================================================

//
// General properties
//

const uint32_t cDemoGain_DMA_TIBufferSizeProperty = AAX_eAudioBufferLengthDSP_16;
const uint32_t cDemoGain_DMA_MaxBufferLength = 1 << AAX_eAudioBufferLengthNative_Max; // Assuming that this is greater than TI buffer size
const uint32_t cDemoGain_DMA_MaxBufferSize = sizeof(float) * cDemoGain_DMA_MaxBufferLength;

// Delay for these algorithms is twice the maximum buffer length, since
// it takes two alg processing passes to move the audio to the output:
//    1: Copy the audio from input to the external buffer
//         (Wait 1 callback for DMA transfer to complete)
//    2: Copy the audio from the exteral buffer to the internal buffer
//         (Wait 1 callback for DMA transfer to complete)
//    3: Apply the audio from the internal buffer to the output
const uint32_t cDemoGain_DMA_LatencyContribution = 2 * cDemoGain_DMA_MaxBufferLength;



//==============================================================================
// Memory block structure definitions
//==============================================================================

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_ALG
#include AAX_ALIGN_FILE_END

// Gain coefficient structure (extend this to add more coeffs)
struct SDemoGain_DMA_CoefsGain
{
	float	mGain;
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
template <typename T_EXT>
struct SDemoGain_DMA_Alg_Context
{
	int32_t							* mCtrlBypassP;			// Control message destination
	SDemoGain_DMA_CoefsGain			* mCoefsGainP;			// Inter-component message destination
	
	float*							* mInputPP;				// Audio signal destination
	float*							* mOutputPP;			// Audio signal source
	int32_t							* mBufferLength;		// Buffer size
	float*							* mMetersPP;			// Meter taps
	
	T_EXT ext; // Extension of struct based on context template
		       // Nested structs can often cause alignment issues,
	           // but we should be fine with structures of pointers.
};

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_RESET
#include AAX_ALIGN_FILE_END

// Physical addresses within the context
struct SDemoGain_DMA_NoExt {};
enum EDemoGain_DMA_Alg_PortID
{
	eAlgPortID_BypassIn				= AAX_FIELD_INDEX (SDemoGain_DMA_Alg_Context<SDemoGain_DMA_NoExt>, mCtrlBypassP)
	,eAlgPortID_CoefsGainIn			= AAX_FIELD_INDEX (SDemoGain_DMA_Alg_Context<SDemoGain_DMA_NoExt>, mCoefsGainP)
	
	,eAlgFieldID_AudioIn			= AAX_FIELD_INDEX (SDemoGain_DMA_Alg_Context<SDemoGain_DMA_NoExt>, mInputPP)
	,eAlgFieldID_AudioOut			= AAX_FIELD_INDEX (SDemoGain_DMA_Alg_Context<SDemoGain_DMA_NoExt>, mOutputPP)
	,eAlgFieldID_BufferSize			= AAX_FIELD_INDEX (SDemoGain_DMA_Alg_Context<SDemoGain_DMA_NoExt>, mBufferLength)	
	,eAlgFieldID_Meters				= AAX_FIELD_INDEX (SDemoGain_DMA_Alg_Context<SDemoGain_DMA_NoExt>, mMetersPP)
};


//==============================================================================
// Inline function definitions
//==============================================================================

// Constant-time, pipelineable power-of-2 division
inline uint32_t Pow2Div (uint32_t inNum, uint32_t inDen)
{
	// Better implementations would use platform-specific
	// commands like ctz, but this is just an example.
	//
	// Binary lookup time!
	int8_t shift = inDen > 0x8000 ? 16 : 0;
	shift += (inDen >> shift) > 0x80 ? 8 : 0;
	shift += (inDen >> shift) > 0x8 ? 4 : 0;
	shift += (inDen >> shift) > 0x2 ? 2 : 0;
	shift += (inDen >> shift) > 0x1 ? 1 : 0;
	return inNum >> shift;
}

inline uint32_t IsPowerOfTwo (uint32_t inNum)
{
	return (inNum > 0 && !(inNum & (inNum - 1)));
}

#endif // DEMOGAIN_ALGCOMMON_H
