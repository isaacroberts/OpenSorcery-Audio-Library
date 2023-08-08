/*================================================================================================*/
/*
 *	Copyright 2016-2017, 2019 by Avid Technology, Inc.
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
 *	\brief  Algorithm component definitions for DemoGain.
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/

#pragma once
#ifndef DEMOGAIN_ALG_H
#define DEMOGAIN_ALG_H



// AAX includes
#include "AAX.h"

// DemoGainMultichannel includes
#include "DemoGain_Defs.h"

// Standard includes
#include "string.h"
#include <math.h>		// for fabsf()
#include <algorithm>	// for max()


//This number is emprically derived with minimal trial and error
const float cDemoGainMinMeteredValue = 3.0e-19f;

//==============================================================================
// Memory block structure definitions
//==============================================================================

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_ALG
#include AAX_ALIGN_FILE_END
	struct SDemoGain_CoefsGain
	{
        SDemoGain_CoefsGain()
		{
			for (int i = 0; i < cMaxNumChannels; ++i)
			{
				mGain[i] = 0.f;
			}
		}
		
		float mGain[cMaxNumChannels];
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
	float*						* mInputPP;						// Audio signal destination
	float*						* mOutputPP;					// Audio signal source
	int32_t						* mBufferSizeP;					// Buffer size

	int32_t						* mCtrlBypassP;					// Control message destination
	SDemoGain_CoefsGain			* mCoefsGainP;					// Inter-component message destination

	float*						* mMetersPP;					// Meter taps
};

// Physical addresses within the context
enum EDemoGain_Alg_PortID
{
	 eAlgPortID_AudioIn						= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mInputPP)
	,eAlgPortID_AudioOut					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mOutputPP)
	,eAlgPortID_BufferSize					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mBufferSizeP)

	,eAlgPortID_BypassIn					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mCtrlBypassP)
	,eAlgPortID_CoefsGainIn					= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mCoefsGainP)
    
	,eAlgPortID_Meters						= AAX_FIELD_INDEX (SDemoGain_Alg_Context, mMetersPP)
};


//==============================================================================
// Callback declarations
//==============================================================================
template <uint16_t kNumChannels>
void inline AAX_CALLBACK DemoGain_AlgorithmProcessFunction (
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
        const SDemoGain_CoefsGain * const AAX_RESTRICT coefsGainP = instance->mCoefsGainP;	// Input (const)
        const int32_t	bypass		= *instance->mCtrlBypassP; // 'int' not 'bool' for optimization
        const int32_t	buffersize	= *instance->mBufferSizeP;
        // Meter taps
        float* const AAX_RESTRICT meterTaps = *instance->mMetersPP;
		float grMeter = 0;

		// These coefficients are used just to visually distinguish CL and EG gain reduction meters. 
		const float coefCL = 1.0f;
		const float coefEG = 0.95f;

        for (uint16_t channel = 0; channel < kNumChannels; ++channel)
        {
            // Audio channels
            const float* const AAX_RESTRICT pdI = instance->mInputPP [channel];
            float* const AAX_RESTRICT pdO = instance->mOutputPP [channel];
            // Gain
            const float gain = coefsGainP->mGain [channel];
            
            //--------- Run processing loop over each sample ---------//
            //
            for (int t = 0; t < buffersize; ++t)
            {
                // Process audio samples
                //
       			pdO [t] = gain * pdI [t];
                if (bypass) { pdO [t] = pdI [t]; }
            } // Go to the next sample
            
            // Do metering
            //
            // Accumulate the max value for metering. This will get cleared for us by the shell
            // when it sends the accumulated value up to the host.
            float inMeter = meterTaps [channel];
            float outMeter = meterTaps [channel + kNumChannels];
            for (int t = 0; t < buffersize; ++t)
            {
				const float inputValue = fabsf(pdI [t]);
				const float outputValue = fabsf(pdO [t]);
				
				inMeter = std::max( inputValue, inMeter );
                outMeter = std::max( outputValue, outMeter );
				
				// Gain reduction metering: don't compute for negligible input
				float grValue = 0.f;
				if (inputValue > cDemoGainMinMeteredValue)
					grValue = (inputValue - outputValue) / inputValue;
				
				grMeter = std::max( grValue, grMeter );
            } // Go to the next sample

			meterTaps [channel] = inMeter;
			meterTaps [channel + kNumChannels] = outMeter;
        } // End channel iteration loop

		meterTaps [2 * kNumChannels] = coefCL * grMeter;
		meterTaps [2 * kNumChannels + 1] = coefEG * grMeter;

    } // End instance-iteration loop
}

#endif //DEMOGAIN_ALG_H
