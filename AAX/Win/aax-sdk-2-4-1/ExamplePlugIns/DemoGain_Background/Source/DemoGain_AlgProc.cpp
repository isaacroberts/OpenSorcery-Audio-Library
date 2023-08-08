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
#include <math.h>		// for fabsf()
#include <algorithm>	// for max()


extern SDemoGain_BGBuffer * gBGBuffer[cMaxNumInstances];
extern float * gBGGain[cMaxNumInstances];
extern AAX_CBoolean gInstanceIsActive[cMaxNumInstances];
extern uint32_t gNumInstances;


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
	
	// Retrieve data that applies to all instances
	//const int32_t reqdelaysamp = *instance->mBufferSizeP * cDemoGainBGBufferMultiplier;
	const int32_t reqdelaysamp = cMaxRenderBuffer * cDemoGainBGBufferMultiplier; // FIXIT: Need to retrieve the actual 
	                                                                             // hardware buffer size, not the sub-
	                                                                             // buffer size (or else the write 
	                                                                             // pointer chases the read too closely
	                                                                             // and the BG thread never has a chance
	                                                                             // to effect the samples.)
	
	//--------- Iterate over plug-in instances ---------//
	//
	for (SDemoGain_Alg_Context * const * walk = inInstancesBegin; walk < inInstancesEnd; ++walk)
	{
		instance = *walk;

		//--------- Retrieve instance-specific information ---------//
		//
		// Memory blocks
		const SDemoGain_CoefsGain* const AAX_RESTRICT	coefsGainP	=	instance->mCoefsGainP;	// Input (const)
		const float		gain		= coefsGainP->mGain;
		const int32_t	bypass		= *instance->mCtrlBypassP;
		const int32_t	buffersize	= *instance->mBufferSizeP;
		//
		SDemoGain_BGBuffer* const AAX_RESTRICT	bgBufferP =		instance->mBGBufferP;	// Private data (non-const)
		float* const  	bgbuffer	= bgBufferP->mBuffer; // \todo Restrict?
		const int32_t	rtwrite		= bgBufferP->mRTWrite;
		//
		SDemoGain_Delay* const AAX_RESTRICT	delayP = instance->mDelaySamplesP; // \todo Restrict?
		const int32_t	curdelay	= delayP->mCurDelay;
		//
		float* const AAX_RESTRICT meterTaps	= *instance->mMetersPP;
		const float* const AAX_RESTRICT pdI	= instance->mInputPP [0]; // First channel
		float* const AAX_RESTRICT pdO		= instance->mOutputPP [0]; // First channel
		
		//--------- Update instance-specific information ---------//
		//
		*instance->mCurGainP = gain; // This is the private gain value used by the BG process
		//
		delayP->mReqDelay = (bypass == 1) ? 0 : reqdelaysamp; // When bypassed, reads input values directly.  This bypasses the BG gain processing.
		//
		const int32_t rtread = wrap_to_constraint (rtwrite - curdelay, cDemoGainBGBufferSize);
		
		
		//--------- Run processing loop over each sample ---------//
		//
		// Write input audio samples to background buffer		
		for (int t = 0; t < buffersize; t++)
		{
			const int32_t curwrite = wrap_to_constraint (rtwrite + t, cDemoGainBGBufferSize);
			bgbuffer[curwrite] = pdI [t];
		}
		//
		// Read output audio samples from background buffer
		for (int t = 0; t < buffersize; t++)
		{
			const int32_t curread = wrap_to_constraint (rtread + t, cDemoGainBGBufferSize);
			pdO [t] = bgbuffer[curread];
		}
		//
		// Do metering
		for (int t = 0; t < buffersize; t++)
		{
			// Do metering
			//
			// Accumulate the max value for metering. This will get cleared for us by the shell
			// when it sends the accumulated value up to the host.
			meterTaps[eMeterTap_PreGain] = std::max(fabsf(pdI [t]), meterTaps[eMeterTap_PreGain]);
			meterTaps[eMeterTap_PostGain] = std::max(fabsf(pdO [t]), meterTaps[eMeterTap_PostGain]);
		}
		
		bgBufferP->mRTWrite = wrap_to_constraint (rtwrite + buffersize, cDemoGainBGBufferSize);
	} // End instance-iteration loop
}



//==============================================================================
// Algorithm init function definition
//==============================================================================

int32_t
AAX_CALLBACK
DemoGain_AlgorithmInitFunction (
								SDemoGain_Alg_Context const *    inInstance,
								AAX_EComponentInstanceInitAction inAction)
{
	// If this is the only instance on the chip, init the global data
    if (gNumInstances == 0)
    {
        for (uint32_t i = 0; i < cMaxNumInstances; i++)
        {
            gInstanceIsActive[i] = false;
        }
    }
    
    // Each instance gets its own global data
	if (inAction == AAX_eComponentInstanceInitAction_AddingNewInstance)
	{
		int32_t firstAvailableInstanceIndex = -1; // First iter will be zero
        while (gInstanceIsActive[++firstAvailableInstanceIndex]); // Find first available index
        gInstanceIsActive[firstAvailableInstanceIndex] = true;
        *inInstance->mInstanceIndexP = firstAvailableInstanceIndex;
        
        gBGBuffer[firstAvailableInstanceIndex] = inInstance->mBGBufferP;
		gBGGain[firstAvailableInstanceIndex] = inInstance->mCurGainP;
        
		gNumInstances++;
	}
	else if (inAction == AAX_eComponentInstanceInitAction_RemovingInstance)
	{
        gNumInstances--;
        gInstanceIsActive[*inInstance->mInstanceIndexP] = false;
	}
	
	return 0;
}


//==============================================================================
// Algorithm background function definition
//==============================================================================

int32_t
AAX_CALLBACK
DemoGain_AlgorithmBackgroundFunction ()
{
	for (unsigned int i = 0; i < cMaxNumInstances; i++)
	{
		if (gInstanceIsActive[i])
        {
            // Read data from global pointers to the instances'
            // private data fields
            //
            const int32_t rtwrite = gBGBuffer[i]->mRTWrite;
            const float gain = *gBGGain[i];
            float * const buffer = gBGBuffer[i]->mBuffer; // Restrict?
            
            // Process one sample at a time until caught up with write pointer.
            // Do not process write pointer sample itself to avoid possible
            // contention when algorithm is bypassed.
            int32_t bgwrite = gBGBuffer[i]->mBGWrite;
            const int32_t safeBound = wrap_to_constraint(rtwrite - 1, cDemoGainBGBufferSize);
            while (bgwrite != safeBound && bgwrite != rtwrite)
            {
                buffer[bgwrite] *= gain;
                bgwrite = wrap_to_constraint(bgwrite+1, cDemoGainBGBufferSize);
            }
            
            // Write new position back to global
            //
            gBGBuffer[i]->mBGWrite = bgwrite;
        }
	}
	return 0;
}
