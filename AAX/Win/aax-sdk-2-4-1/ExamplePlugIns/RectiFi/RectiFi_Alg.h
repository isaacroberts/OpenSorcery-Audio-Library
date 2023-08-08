/*================================================================================================*/
/*
 *	Copyright 2008-2015, 2017, 2019 by Avid Technology, Inc.
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
 *	\file   RectiFi_Alg.h
 *
 *	\brief  Algorithm component definitions for DemoDelay.
 *
 *  \author Frederick Umminger
 */ 
/*================================================================================================*/

#ifndef RECTIFI_ALG_H
#define RECTIFI_ALG_H

// AAX includes
#include "AAX.h"

// Constants
enum ERectiFi_MaxNumOutputs
{
	kMaxNumInputs = 2,
	kMaxNumOutputs = 2
};

class IACFUnknown;

//==============================================================================
// General definitions
//==============================================================================

// Meter IDs
template<int kNumChannels>
struct SMeterTaps
{
	enum
	{
		eMeter_InRectifiLeft			= 0,
		eMeter_InRectifiRight,
		eMeter_OutRectifiLeft			= eMeter_InRectifiLeft + kNumChannels,
		eMeter_OutRectifiRight,

		eMeterTap_Count					= eMeter_OutRectifiLeft + kNumChannels,
	};
};

typedef SMeterTaps<1> SMeterTapsM;
typedef SMeterTaps<2> SMeterTapsS;

//==============================================================================
// Memory block structure definitions
//==============================================================================
#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_ALG
#include AAX_ALIGN_FILE_END
struct SRectiFiCoefs_PostFilter
{
	double	mCoeffPostA0stage1;
	double	mCoeffPostA1stage1;
	double	mCoeffPostA2stage1;
	double	mCoeffPostB1stage1;
	double	mCoeffPostB2stage1;
	double	mCoeffPostA0stage2;
	double	mCoeffPostA1stage2;
	double	mCoeffPostB1stage2;
};

struct SRectiFiCoefs_PreFilter
{
	double	mCoeffPreA0stage1;
	double	mCoeffPreA1stage1;
	double	mCoeffPreA2stage1;
	double	mCoeffPreB1stage1;
	double	mCoeffPreB2stage1;
	double	mCoeffPreA0stage2;
	double	mCoeffPreA1stage2;
	double	mCoeffPreB1stage2;
};

struct SRectiFiStateBlock
{
	double 	mDelayPreA1stage1[kMaxNumOutputs];
	double	mDelayPreA2stage1[kMaxNumOutputs];
	double	mDelayPreB1stage1[kMaxNumOutputs];
	double	mDelayPreB2stage1[kMaxNumOutputs];
	double	mDelayPreA1stage2[kMaxNumOutputs];
	double	mDelayPreB1stage2[kMaxNumOutputs];
	
	double	mDelayPostA1stage1[kMaxNumOutputs];
	double	mDelayPostA2stage1[kMaxNumOutputs];
	double	mDelayPostB1stage1[kMaxNumOutputs];
	double	mDelayPostB2stage1[kMaxNumOutputs];
	double	mDelayPostA1stage2[kMaxNumOutputs];
	double	mDelayPostB1stage2[kMaxNumOutputs];

	float	mRectiLastInput[kMaxNumOutputs];
	float	mRectiLastOutput[kMaxNumOutputs];
	int		mRectiType;
	int		mRectiPositive[kMaxNumOutputs];
};

struct SRectiFiCoefs_EffectMix
{
	float	mDryMix;
	float	mRectiMix;
};

//==============================================================================
// Component context definitions
//==============================================================================

// Context structure
struct SRectiFi_Algorithm_Context
{
	const float * const					* mInputPP;					// Audio signal destination
	float * const						* mOutputPP;				// Audio signal source
	const int32_t						* mBufferSizeP;				// Buffer size
	
	const int32_t						* mMasterBypassControlP;	// Control message destination

	const int32_t						* mCtrlRectiTypeP;			// Control message destination

	const float							* mGainP;					// Inter-component message destination
	const SRectiFiCoefs_PreFilter		* mCoefsPreFiltP;			// Inter-component message destination
	const SRectiFiCoefs_PostFilter		* mCoefsPostFiltP;			// Inter-component message destination
	const SRectiFiCoefs_EffectMix		* mCoefsEffectMixP;			// Inter-component message destination

	SRectiFiStateBlock					* mStateP;					// Private data
	//Meter block
	float*								* mMetersPP;
};
#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_RESET
#include AAX_ALIGN_FILE_END
// Physical addresses within the context
enum ERectiFi_AlgPortID
{
	eAlgFieldIndex_AudioIn				= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mInputPP),
	eAlgFieldIndex_AudioOut				= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mOutputPP),
	eAlgFieldIndex_BufferSize			= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mBufferSizeP),

	eAlgPortID_MasterBypassControlIn	= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mMasterBypassControlP),
	eAlgPortID_RectiTypeIn				= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mCtrlRectiTypeP),

	eAlgPortID_GainIn					= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mGainP),
	eAlgPortID_CoefsPreFiltIn			= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mCoefsPreFiltP),
	eAlgPortID_CoefsPostFiltIn			= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mCoefsPostFiltP),
	eAlgPortID_CoefsEffectMixIn			= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mCoefsEffectMixP),

	eAlgFieldIndex_State				= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mStateP),
	eAlgFieldID_Meters					= AAX_FIELD_INDEX (SRectiFi_Algorithm_Context, mMetersPP),
};

#endif // RECTIFI_ALG_H
