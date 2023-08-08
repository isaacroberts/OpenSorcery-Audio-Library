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
 *	\file   DemoDelay_Alg.h
 *
 *	\brief  Algorithm component definitions for DemoDelay_Hybrid.
 *
 *  \author Dave Tremblay
 */ 
/*================================================================================================*/
#pragma once
#ifndef DemoDelay_ALG_H
#define DemoDelay_ALG_H



// AAX includes
#include "AAX.h"


//==============================================================================
// General definitions
//==============================================================================

// Meter layout
enum EDemoDelay_MeterTaps
{
	eMeterTap_Input = 0,
	eMeterTap_Output,

	eMeterTap_Count
};


//==============================================================================
// Algorithm Structures
//==============================================================================

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_ALG
#include AAX_ALIGN_FILE_END

// Gain coefficient structure (extend this to add more coeffs)
struct SDemoDelay_Coefs
{
    bool    mBypass;
    float	mDryGain;
    float   mWetGain;
};

// Context structure
struct SDemoDelayHybrid_LowLatency_Context
{
	SDemoDelay_Coefs		* mCoefsDelayP;					// Inter-component message destination
	
	float*					* mInputPP;						// Audio signal destination
	float*					* mOutputPP;					// Audio signal source
	int32_t					* mBufferLengthP;				// Buffer size

	float*					* mMeterOutputPP;				// Meter taps
};
#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_RESET
#include AAX_ALIGN_FILE_END


//==============================================================================
// Callback declarations
//==============================================================================

void
AAX_CALLBACK
DemoDelay_AlgorithmProcessFunction (
    SDemoDelayHybrid_LowLatency_Context * const	inInstancesBegin [],
    const void *					inInstancesEnd);

#endif //DemoDelay_ALG_H
