/*================================================================================================*/
/*
 *	Copyright 2008-2016, 2018 by Avid Technology, Inc.
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
 *	\file   DemoGain_Alg.h
 *
 *	\brief  Algorithm component definitions for DemoGain_Background.
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOGAIN_ALG_H
#define DEMOGAIN_ALG_H

// AAX includes
#include "AAX.h"


#if defined(_TMS320C6X) // TI
const int32_t cMaxRenderBuffer = 1 << AAX_eAudioBufferLengthDSP_Max;
#else // Native
const int32_t cMaxRenderBuffer = 1 << AAX_eAudioBufferLength_Max;
#endif

const uint32_t cDemoGainBGBufferMultiplier = 2;
const uint32_t cDemoGainBGBufferSize = cDemoGainBGBufferMultiplier * cMaxRenderBuffer * 2; // Mult. by two to prevent overlap at maximum buffer size
const uint32_t cMaxNumInstances = 52; // \todo: set this to the real plaform-dependent value


//==============================================================================
// General definitions
//==============================================================================

// Meter layout
enum EDemoGain_MeterTaps
{
	eMeterTap_PreGain = 0,
	eMeterTap_PostGain,

	eMeterTap_Count
};


//==============================================================================
// Memory block structure definitions
//==============================================================================

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_ALG
#include AAX_ALIGN_FILE_END
	// Gain coefficient structure (extend this to add more coeffs)
	struct SDemoGain_CoefsGain
	{
		float	mGain;
	};
	
	// Algorithmic delay
	typedef int32_t DelaySamp;
	struct SDemoGain_Delay
	{
		DelaySamp mReqDelay;
		DelaySamp mCurDelay;
	};
	
	struct SDemoGain_BGBuffer
	{
		float mBuffer[cDemoGainBGBufferSize];
		int32_t mBGWrite;
		int32_t mRTWrite;
		
		SDemoGain_BGBuffer () :
			mBGWrite (0),
			mRTWrite (0)
		{
			for (uint32_t i = 0; i < cDemoGainBGBufferSize; i++)
			{
				mBuffer[i] = 0.0f;
			}
		}
	};
#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_RESET
#include AAX_ALIGN_FILE_END

//==============================================================================
// Component context definitions
//==============================================================================

// Context structure
struct SDemoGain_Alg_Context
{
	int32_t						* mCtrlBypassP;					// Control message destination

	SDemoGain_CoefsGain			* mCoefsGainP;					// Inter-component message destination
	
	SDemoGain_BGBuffer			* mBGBufferP;                   // Private data
	float						* mCurGainP;					// Private data
	SDemoGain_Delay				* mDelaySamplesP;				// Private data
	uint32_t                    * mInstanceIndexP;              // Private data
	
	float*						* mInputPP;						// Audio signal destination
	float*						* mOutputPP;					// Audio signal source
	int32_t						* mBufferSizeP;					// Buffer size

	float*						* mMetersPP;					// Meter taps
};


// Physical addresses within the context
enum EDemoGain_Alg_PortID
{
	 eAlgPortID_BypassIn					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mCtrlBypassP)
	,eAlgPortID_CoefsGainIn					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mCoefsGainP)
	
	,eAlgPortID_BGBuffer                    = AAX_FIELD_INDEX (SDemoGain_Alg_Context, mBGBufferP)
	,eAlgPortID_CurGain						= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mCurGainP)
	,eAlgPortID_DelaySamples				= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mDelaySamplesP)
	,eAlgPortID_InstanceIndex                = AAX_FIELD_INDEX (SDemoGain_Alg_Context, mInstanceIndexP)
	
	,eAlgFieldID_AudioIn					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mInputPP)
	,eAlgFieldID_AudioOut					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mOutputPP)
	,eAlgFieldID_BufferSize					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mBufferSizeP)

	,eAlgFieldID_Meters						= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mMetersPP)
};


//==============================================================================
// Callback declarations
//==============================================================================

void
AAX_CALLBACK
DemoGain_AlgorithmProcessFunction (
	SDemoGain_Alg_Context * const	inInstancesBegin [],
	const void *					inInstancesEnd);

int32_t
AAX_CALLBACK
DemoGain_AlgorithmBackgroundFunction (
	void);

int32_t
AAX_CALLBACK
DemoGain_AlgorithmInitFunction (
	SDemoGain_Alg_Context const *    inInstance,
	AAX_EComponentInstanceInitAction inAction);


//==============================================================================
// Inline functions
//==============================================================================

inline
int32_t
wrap_to_constraint (
	int32_t inVal,
	const int32_t constraint)
{
	// Takes negative input values into account
	return ((inVal % constraint) + constraint) % constraint;
}

#endif
