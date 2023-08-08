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
#include "DemoDist_Alg.h"
#include "CSimpleBiquad.h"

// AAX includes
#include "AAX.h"

// Standard includes
#include <float.h>   // for FLT_MIN
#include <algorithm> // for max()
#include <math.h>    // for fabsf(), sinf()


//==============================================================================
// Processing function definition
//==============================================================================

template<int kNumChannelsIn, int kNumChannelsOut>
void
AAX_CALLBACK
DemoDist_AlgorithmProcessFunction(
	SDemoDist_Alg_Context * const	inInstancesBegin [],
	const void *					inInstancesEnd)
{
	typedef SMeterTaps<kNumChannelsIn> SMeterTapsCh;

	// Get a pointer to the beginning of the memory block table
	const SDemoDist_Alg_Context* AAX_RESTRICT instance = inInstancesBegin [0];

	
	//--------- Iterate over plug-in instances ---------//
	//
	for (SDemoDist_Alg_Context * const * walk = inInstancesBegin; walk < inInstancesEnd; ++walk)
	{
		instance = *walk;

		//--------- Retrieve instance-specific information ---------//
		//
		// Memory blocks
		const SDemoDist_DistCoefs* const AAX_RESTRICT	coefsDistP =	instance->mCoefsDistP;	// Input (const)
		const SDemoDist_FiltCoefs* const AAX_RESTRICT	coefsFilterP =	instance->mCoefsFiltP;	// Input (const)
		CSimpleBiquad* const AAX_RESTRICT				biquad =		instance->mBiquads;		// Private (non-const)
		float* AAX_RESTRICT								metersInPostTrimP = instance->mMeterTapsPP[SMeterTapsCh::eMeterTap_InPostTrimLeft];
		float* AAX_RESTRICT								metersDistP =	instance->mMeterTapsPP[SMeterTapsCh::eMeterTap_DistLeft];
		//
		// Store coefficients as local copies (using load-update-store optimization pattern)
		const int32_t buffersize = *instance->mBufferLengthP;
		const float filterMix =	coefsFilterP->mFilterMix;
		const float * const AAX_RESTRICT coeffs = coefsFilterP->mCoeffs;
		const int32_t bypass =	*instance->mCtrlBypassP; // Note: 'int' not 'bool' for optimization
		const float mix =		*instance->mCtrlMixP;
		const float transWarp =	coefsDistP->mTransWarp;
		const float cutoff =	coefsDistP->mCutoff;
		const float inpG =		*instance->mCtrlInpGP;
		const float outG =		*instance->mCtrlOutGP;
		//
		// Prepare values for processing
		//
		// Update biquad coefficients
		for (int i = 0; i < kNumChannelsIn; i++)
			biquad[i].SetCoeffsArray(coeffs);


		//--------- Run processing loop over each input channel ---------//
		//
		for (int ch = 0; ch < kNumChannelsIn; ch++) // Iterate over all input channels
		{			
			// Set up per-channel data
			//
			// Audio channels
			const float* const AAX_RESTRICT pdI = instance->mInputPP [ch];
			float* const AAX_RESTRICT pdO = instance->mOutputPP [ch];
			
			// Create initial buffer of soft-distorted samples
			//   TI optimization note: This loop is disqualified for
			//   pipelining due to sin function call.
			for (int t = 0; t < buffersize; ++t)
			{
				// Using the output buffer as a temp buffer for intermediate samples
				pdO [t] = sinf(transWarp * pdI [t] * inpG);
			} // Go to next sample
		} // Go to the next channel

		for (int ch = 0; ch < kNumChannelsIn; ch++) // Iterate over all input channels
		{
			// Set up per-channel data
			//
			// Audio channels
			const float* const AAX_RESTRICT pdI = instance->mInputPP [ch];
			float* const AAX_RESTRICT pdO = instance->mOutputPP [ch];

			//--------- Run processing loop over each sample ---------//
			// 
			// Do the main audio processing
			for (int t = 0; t < buffersize; t++)
			{
				// Calculate a post-gain input sample value...
				const float inSamp = pdI [t] * inpG;
				const float inSampSign = (inSamp >= 0.0f) ? 1.0f : -1.0f;

				// If the signal is in the distorting range, do the distortion
				const float distSampPreFiltNoCut = mix * pdO [t]; // Output buffer was used as temp buffer of intermediate values
				const float distSampPreFiltCut = inSampSign * (mix - FLT_MIN);
				float distSampPreFilt = fabsf(inSamp) >= cutoff ? distSampPreFiltCut : distSampPreFiltNoCut;

				// Add the unmixed post-gain component
				distSampPreFilt += (1.0f - mix) * inSamp;
				const float meterPostTrim = pdI [t] - ((!bypass) * distSampPreFilt);
				//
				// Filter the difference between the distortion and the input
				const float inSampMinusDistSamp = inSamp - distSampPreFilt;
				const float filteredDifference = biquad[ch].DoFilt(inSampMinusDistSamp);
				const float distSampPostFilt = (filterMix * filteredDifference) + ( (1.0f - filterMix) * inSampMinusDistSamp );
				const float meterDist = (!bypass) * distSampPostFilt;
				//
				// ...and add the filtered difference back onto the input
				pdO [t] = outG * (inSamp - distSampPostFilt);
				
				// Do metering
				//
				metersInPostTrimP [ch] = std::max( fabsf( meterPostTrim ), metersInPostTrimP [ch] );
				metersDistP [ch] = std::max( fabsf( meterDist ), metersDistP [ch] );

				if (bypass)
				{	// If we are bypassing, just wire input to output
					pdO [t] = pdI [t]; 
				}
			} // Go to next sample
		} // Go to the next channel
	} // End instance-iteration loop
}
