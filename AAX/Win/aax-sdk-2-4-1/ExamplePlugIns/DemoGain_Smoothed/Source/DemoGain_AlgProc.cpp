/*================================================================================================*/
/*
 *	Copyright 2008-2015 by Avid Technology, Inc.
 *	All rights reserved.
 *
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 */
/*================================================================================================*/


// Component includes
#include "DemoGain_Alg.h"

// AAX includes
#include "AAX.h"

// Standard includes
#include <math.h>		// for fabsf, tanf()
#include <algorithm>	// for max()


//==============================================================================
// Processing function definition
//==============================================================================

void
AAX_CALLBACK
DemoGain_AlgorithmProcessFunction (
	SDemoGain_Alg_Context * const	inInstancesBegin [],
	const void *					inInstancesEnd)
{
	// Get a pointer to the beginning of the memory block table
	SDemoGain_Alg_Context* AAX_RESTRICT instance = inInstancesBegin [0];


	//--------- Iterate over plug-in instances ---------//
	//
	for (SDemoGain_Alg_Context * const * walk = inInstancesBegin; walk < inInstancesEnd; ++walk)
	{
		instance = *walk;

		//--------- Retrieve instance-specific information ---------//
		//
		// Memory blocks
		const SDemoGain_CoefsGain* const AAX_RESTRICT	coefsGainP = instance->mTargetCoefsGainP;	// Input (const)		
		const int32_t	bypass		= *instance->mCtrlBypassP; // 'int' not 'bool' for optimization
		const float		targgain	= coefsGainP->mGain;
		const int32_t	buffersize	= *instance->mBufferSize;
		const AAX_CSampleRate samplerate = *instance->mSampleRate;
		// Private data
		SDemoGain_CoefsGain* const AAX_RESTRICT	coefsCurGainP = instance->mCurCoefsGainP;	// Private data (non-const)
		float curgain = coefsCurGainP->mGain;
		
		// Audio channels
		const float* const AAX_RESTRICT pdI = instance->mInputPP [0]; // First channel
		float* const AAX_RESTRICT pdO = instance->mOutputPP [0]; // First channel
		// Meter taps
		float* const AAX_RESTRICT	meterTaps	= *instance->mMetersPP;

		// Compute smoothing coefficients
		//
		// This could also be performed on the host when dispatching
		// coefficient packets to the alg.
		const float lpCoef2 = samplerate <= 50000.0f ?  0.991179722f :
		                      samplerate <= 100000.0f ? 0.995580134f :
		                                                0.997787626f;
		const float targGainMultLPCoef1 = targgain * (1.0f - lpCoef2);
		
		//--------- Run processing loop over each sample ---------//
		//
		for (int t = 0; t < buffersize; t++)
		{
			// Smooth coefficients
			curgain = curgain * lpCoef2 + targGainMultLPCoef1;
			
			// Process audio samples
			//
			pdO [t] = curgain * pdI [t];
			if (bypass) { pdO [t] = pdI [t]; }
		} // Go to the next sample
		
		// "Clamp" gain value if close enough to target
		curgain = (curgain - targgain < 0.000001f) && (targgain - curgain < 0.000001f) ? targgain : curgain;

		// Do metering
		//
		// Accumulate the max value for metering. This will get cleared for us by the shell
		// when it sends the accumulated value up to the host.
		for (int t = 0; t < buffersize; t++)
		{
			meterTaps[eMeterTap_PreGain] = std::max(fabsf(pdI [t]), meterTaps[eMeterTap_PreGain]);
			meterTaps[eMeterTap_PostGain] = std::max(fabsf(pdO [t]), meterTaps[eMeterTap_PostGain]);
		} // Go to the next sample
		
		// Set new state
		//
		coefsCurGainP->mGain = curgain;
		
	} // End instance-iteration loop
}
