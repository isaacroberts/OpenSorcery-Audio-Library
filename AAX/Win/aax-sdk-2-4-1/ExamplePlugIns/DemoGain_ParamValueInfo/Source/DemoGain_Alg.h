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
 *	\file   DemoGain_Alg.h
 *
 *	\brief  Algorithm component definitions for DemoGain_ParamValueInfo.
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOGAIN_ALG_H
#define DEMOGAIN_ALG_H



// AAX includes
#include "AAX.h"


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
	
	float*						* mInputPP;						// Audio signal destination
	float*						* mOutputPP;					// Audio signal source
	int32_t						* mBufferSize;					// Buffer size

	float*						* mMetersPP;					// Meter taps
};


// Physical addresses within the context
enum EDemoGain_Alg_PortID
{
	 eAlgPortID_BypassIn					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mCtrlBypassP)
	,eAlgPortID_CoefsGainIn					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mCoefsGainP)
	
	,eAlgFieldID_AudioIn					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mInputPP)
	,eAlgFieldID_AudioOut					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mOutputPP)
	,eAlgFieldID_BufferSize					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mBufferSize)

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

#endif //DEMOGAIN_ALG_H
