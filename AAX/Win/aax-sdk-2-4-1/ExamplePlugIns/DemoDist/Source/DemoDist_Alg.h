/*================================================================================================*/
/*
 *	Copyright 2008-2015, 2019 by Avid Technology, Inc.
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
 *	\file   DemoDist_Alg.h
 *
 *	\brief  Algorithm component definitions for DemoDist.
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMODIST_ALG_H
#define DEMODIST_ALG_H


// DemoDist includes
#include "CSimpleBiquad.h"

// AAX includes
#include "AAX.h"

class IACFUnknown;

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_ALG
#include AAX_ALIGN_FILE_END

//==============================================================================
// General definitions
//==============================================================================

// Meter tap IDs
template<int kNumChannels>
struct SMeterTaps
{
	enum
	{
		eMeterTap_InPostTrimLeft		= 0,
		eMeterTap_InPostTrimRight,
		eMeterTap_DistLeft				= eMeterTap_InPostTrimLeft + kNumChannels,
		eMeterTap_DistRight,

		eMeterTap_Count					= eMeterTap_DistLeft + kNumChannels
	};
};

typedef SMeterTaps<1> SMeterTapsM;
typedef SMeterTaps<2> SMeterTapsS;


//==============================================================================
// Memory block structure definitions
//==============================================================================

// Coefficient data block for distortion state data
struct SDemoDist_DistCoefs
{
	float	mTransWarp;
	float	mCutoff;
};

// Coefficient data block structure for filter data
struct SDemoDist_FiltCoefs
{
	float	mFilterMix;
	float	mCoeffs[CSimpleBiquad::eNumBiquadCoefs];
};


//==============================================================================
// Component context definitions
//==============================================================================

// Context structure
struct SDemoDist_Alg_Context
{
	int32_t					* mCtrlBypassP;					// Coefficient message destination
	float					* mCtrlMixP;					// Coefficient message destination
	float					* mCtrlInpGP;					// Coefficient message destination
	float					* mCtrlOutGP;					// Coefficient message destination
	SDemoDist_DistCoefs		* mCoefsDistP;					// Coefficient message destination
	SDemoDist_FiltCoefs		* mCoefsFiltP;					// Coefficient message destination
	
	CSimpleBiquad			* mBiquads;						// Private data
	
	float*					* mInputPP;						// Audio signal destination
	float*					* mOutputPP;					// Audio signal source
	int32_t					* mBufferLengthP;				// Number of samples to process
	float*					* mMeterTapsPP;					// Meter signal source
};


// Physical addresses within the context
enum EDemoDist_Alg_PortID
{
	 eAlgPortID_BypassIn					= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mCtrlBypassP)
	,eAlgPortID_MixIn						= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mCtrlMixP)
	,eAlgPortID_InpGIn						= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mCtrlInpGP)
	,eAlgPortID_OutGIn						= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mCtrlOutGP)
	,eAlgPortID_CoefsDistIn					= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mCoefsDistP)
	,eAlgPortID_CoefsFilterIn				= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mCoefsFiltP)
	
	,eAlgFieldID_Biquads					= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mBiquads)

	,eDemoDist_AlgFieldID_AudioIn			= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mInputPP)
	,eDemoDist_AlgFieldID_AudioOut			= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mOutputPP)
	,eDemoDist_AlgFieldID_BufferLength		= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mBufferLengthP)
	,eAlgFieldID_MeterTaps					= AAX_FIELD_INDEX (SDemoDist_Alg_Context, mMeterTapsPP)
};


//==============================================================================
// Callback declarations
//==============================================================================

#ifndef TI_VERSION
void AAX_CALLBACK DemoDist_AlgorithmResetBiquads ( AAX_CFieldIndex, void*, int32_t, IACFUnknown* const);
#endif

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_RESET
#include AAX_ALIGN_FILE_END

#endif
