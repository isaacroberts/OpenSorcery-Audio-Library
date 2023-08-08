/*================================================================================================*/
/*
 *	Copyright 2013-2015 by Avid Technology, Inc.
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
#include "DemoDelay_Alg.h"

// AAX includes
#include "AAX.h"

// Standard includes
#include <math.h>		// for fabsf()
#include <algorithm>	// for max()

//==============================================================================
// Processing function definition
//==============================================================================
void
AAX_CALLBACK
DemoDelay_AlgorithmProcessFunction (
	SDemoDelayHybrid_LowLatency_Context * const	inInstancesBegin [],
	const void *					inInstancesEnd)
{
	// Iterate over plug-in instances
    SDemoDelayHybrid_LowLatency_Context* AAX_RESTRICT instance = inInstancesBegin [0];
	for (SDemoDelayHybrid_LowLatency_Context * const * walk = inInstancesBegin; walk < inInstancesEnd; ++walk)
	{
		instance = *walk;
		
		//
		// Cache the context pointers
		//
		
		const SDemoDelay_Coefs* const AAX_RESTRICT	coefsGainP = instance->mCoefsDelayP;
		const int32_t	numSamples	= *instance->mBufferLengthP;
        const float* const AAX_RESTRICT audioInput = instance->mInputPP[0];
        const float* const AAX_RESTRICT hybridInput = instance->mInputPP[1];   //hybrid inputs are tacked onto the end of the inputs array.
		float* const AAX_RESTRICT audioOutput = instance->mOutputPP[0];
        float* const AAX_RESTRICT hybridOutput = instance->mOutputPP[1];   //hybrid outputs are tacked onto the end of the outputs array.
		
		// Coefficients
		const float dryGain = coefsGainP->mDryGain;
        const float wetGain = coefsGainP->mWetGain;
		
		
		//
		// Process audio samples
		//
        
        //Copy the input to the hybrid stream to stream it up to High Latency algorithm.  (unmodified stream)
        //It doesn't matter where you do this in the algorithm since it is buffered up to the host processor.
        for (int32_t n = 0; n < numSamples; n++)
        {
            hybridOutput[n] = audioInput [n];
        }

		// Run processing, combining the audio streaming back from High Latency with the Low Latency input.
        for (int32_t n = 0; n < numSamples; n++)
        {
            audioOutput[n] = dryGain * audioInput[n];	//Dry Mix
            audioOutput[n] += wetGain * hybridInput[n];	//Wet Mix
        }

		// Do metering
		float* const AAX_RESTRICT	meterOutput	= *instance->mMeterOutputPP;
		for (int32_t n = 0; n < numSamples; n++)
		{
			meterOutput[eMeterTap_Input] = std::max(fabsf(audioInput[n]), meterOutput[eMeterTap_Input]);
			meterOutput[eMeterTap_Output] = std::max(fabsf(audioOutput[n]), meterOutput[eMeterTap_Output]);
		} 
	} 
}
