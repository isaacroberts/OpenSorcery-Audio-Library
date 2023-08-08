/*================================================================================================*/
/*
 *	Copyright 2014-2015, 2017, 2021 by Avid Technology, Inc.
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

// RectiFi includes
#include "RectiFi_Alg.h"

// AAX includes
#include "AAX.h"

// Standard includes
#include <cmath>
#ifdef TI_VERSION
#include "math.h"
#endif					// for fabsf()

using namespace std;
//==============================================================================
// Processing function definition
//==============================================================================

template<int kNumChannelsIn, int kNumChannelsOut, int kAudioWindowSize>
void AAX_CALLBACK RectiFi_Algorithm_ProcessFunction(
	SRectiFi_Algorithm_Context * const	inInstancesBegin [],
	const void *						inInstancesEnd)
{
	//--------- Iterate over plug-in instances ---------//
#pragma MUST_ITERATE(1)
	for (SRectiFi_Algorithm_Context * const * walk = inInstancesBegin; walk != inInstancesEnd; ++walk)
	{
		typedef SMeterTaps<kNumChannelsIn> SMeterTapsCh;
		SRectiFi_Algorithm_Context* const AAX_RESTRICT contextP = *walk;

		//--------- Retrieve instance-specific information ---------//
		//
		// Memory blocks
		const SRectiFiCoefs_EffectMix* const AAX_RESTRICT	coefsEffectMixP =	contextP->mCoefsEffectMixP;	// Input (const)
		const SRectiFiCoefs_PreFilter* const AAX_RESTRICT	coefsPreFiltP =		contextP->mCoefsPreFiltP;	// Input (const)
		const SRectiFiCoefs_PostFilter* const AAX_RESTRICT	coefsPostFiltP =	contextP->mCoefsPostFiltP;	// Input (const)
		SRectiFiStateBlock* const AAX_RESTRICT				stateP =			contextP->mStateP;			// Private (non-const)

#ifdef TI_VERSION
		const int32_t cAudioBufferSize = kAudioWindowSize;
#else
		const int32_t cAudioBufferSize = *contextP->mBufferSizeP;
#endif

		for (int32_t sampleOffset = 0; sampleOffset < cAudioBufferSize; sampleOffset += kAudioWindowSize)
		{
			//
			// Coefficients stored as local copies
			const int32_t									rectiType			= *contextP->mCtrlRectiTypeP;

			const float										gain				= *contextP->mGainP;
				  float* const AAX_RESTRICT * const AAX_RESTRICT meterPP		= contextP->mMetersPP;	

			float dummy;

			const int32_t rectiTypeChanged = (rectiType != stateP->mRectiType);
			stateP->mRectiType = rectiType;

			for ( int channel = 0; channel < kNumChannelsIn; ++channel)
				stateP->mRectiPositive[channel] = (rectiTypeChanged * (rectiType != 0)) + ((!rectiTypeChanged) * stateP->mRectiPositive[channel]);

			//--------- Run processing loop over each input channel ---------//
			//
			for (int i = 0; i < kNumChannelsIn; i++) // Iterate over all input channels
			{
				//--------- Run processing loop over each sample ---------//
				//
				const	float*	const AAX_RESTRICT	inputsP = contextP->mInputPP[i] + sampleOffset; 
						float*	const AAX_RESTRICT	outputsP = contextP->mOutputPP[i] + sampleOffset; 

				float currentSample[kAudioWindowSize];


				double	DelayPreA1stage1 = stateP->mDelayPreA1stage1[i];
				double	DelayPreA2stage1 = stateP->mDelayPreA2stage1[i];
				double	DelayPreB1stage1 = stateP->mDelayPreB1stage1[i];
				double	DelayPreB2stage1 = stateP->mDelayPreB2stage1[i];
				double	DelayPreA1stage2 = stateP->mDelayPreA1stage2[i];
				double	DelayPreB1stage2 = stateP->mDelayPreB1stage2[i];

	#pragma UNROLL(kAudioWindowSize)
				for (int t = 0; t < kAudioWindowSize; t++)
				{
					float currentSample_t = inputsP[t];

					//*************************************** Do Pre-Filter ************************************
 					double thePreFilterSum;
 					double thePreFilterOutput;

					thePreFilterSum	=	DelayPreA2stage1	* coefsPreFiltP->mCoeffPreA2stage1;
					thePreFilterSum	+=	currentSample_t		* coefsPreFiltP->mCoeffPreA0stage1;
					thePreFilterSum	+=	DelayPreA1stage1	* coefsPreFiltP->mCoeffPreA1stage1;				

					thePreFilterSum	+=	-DelayPreB2stage1	* coefsPreFiltP->mCoeffPreB2stage1;	
					thePreFilterSum	+=	-DelayPreB1stage1	* coefsPreFiltP->mCoeffPreB1stage1;			
				
					thePreFilterSum	=	thePreFilterSum		* 2.0;				

					thePreFilterOutput	=	thePreFilterSum		* coefsPreFiltP->mCoeffPreA0stage2;
					thePreFilterOutput	+=	DelayPreA1stage2	* coefsPreFiltP->mCoeffPreA1stage2;	
 
  					thePreFilterOutput	+=	-DelayPreB1stage2	* coefsPreFiltP->mCoeffPreB1stage2;
  				
  					thePreFilterOutput	=	thePreFilterOutput	* 2.0;  

					DelayPreA2stage1 = DelayPreA1stage1;
					DelayPreA1stage1 = currentSample_t;
					DelayPreB2stage1 = DelayPreB1stage1;
					DelayPreB1stage1 = thePreFilterSum;
					DelayPreA1stage2 = thePreFilterSum;
  					DelayPreB1stage2 = thePreFilterOutput;
  
					thePreFilterOutput	+=	currentSample_t * 0.5;
					currentSample_t	= static_cast<float>(thePreFilterOutput * 2.0);
		
					currentSample_t = !(currentSample_t < 3.0e-34f && currentSample_t > -3.0e-34f)  * currentSample_t;
				
					currentSample[t] = currentSample_t;
				}

				stateP->mDelayPreA1stage1[i] = DelayPreA1stage1;
				stateP->mDelayPreA2stage1[i] = DelayPreA2stage1;
				stateP->mDelayPreB1stage1[i] = DelayPreB1stage1;
				stateP->mDelayPreB2stage1[i] = DelayPreB2stage1;
				stateP->mDelayPreA1stage2[i] = DelayPreA1stage2;
				stateP->mDelayPreB1stage2[i] = DelayPreB1stage2;



				int32_t rectiPositive = stateP->mRectiPositive[i];
 				float rectiLastInput = stateP->mRectiLastInput[i];
 				float rectiLastOutput = stateP->mRectiLastOutput[i];

	#pragma UNROLL(kAudioWindowSize)
 				for (int t = 0; t < kAudioWindowSize; t++)
 				{
 					float currentSample_t = currentSample[t];

					//*************************************** Do Rectify ************************************

  					if (rectiType >= 1)			//"alternating" and "alt-max"
   					{
						if ((rectiLastInput < 0.0f) && (currentSample_t >= 0.0f))
   							rectiPositive = !rectiPositive;
   
   						rectiLastInput = currentSample_t; 
  					}

					if (( rectiPositive && currentSample_t < 0.0f) 
						|| (!rectiPositive && currentSample_t > 0.0f))
   					{
   						currentSample_t = -currentSample_t;
   					}

  					if (rectiType == 2)		//"alt-max"
   					{
   						if ( (rectiPositive && (currentSample_t < rectiLastOutput))
   							|| (!rectiPositive && (currentSample_t > rectiLastOutput)) )
   							currentSample_t = rectiLastOutput;
   
   						rectiLastOutput = currentSample_t;
   					}

					//*************************************** Do Gain Stage *************************************

					currentSample_t = currentSample_t * gain;
 
 					currentSample[t] = currentSample_t;
 				}
 				stateP->mRectiPositive[i] = rectiPositive;
 				stateP->mRectiLastInput[i] = rectiLastInput;
 				stateP->mRectiLastOutput[i] = rectiLastOutput;


 				double	DelayPostA1stage1 = stateP->mDelayPostA1stage1[i];
 				double	DelayPostA2stage1 = stateP->mDelayPostA2stage1[i];
 				double	DelayPostB1stage1 = stateP->mDelayPostB1stage1[i];
 				double	DelayPostB2stage1 = stateP->mDelayPostB2stage1[i];
 				double	DelayPostA1stage2 = stateP->mDelayPostA1stage2[i];
 				double	DelayPostB1stage2 = stateP->mDelayPostB1stage2[i];

	#pragma UNROLL(kAudioWindowSize)
 				for (int t = 0; t < kAudioWindowSize; t++)
 				{
					float currentSample_t = currentSample[t];

					//*************************************** Do Post-Filter ************************************
 					double thePostFilterSum;
 					double thePostFilterOutput;
 
 					thePostFilterSum	=	DelayPostA2stage1	* coefsPostFiltP->mCoeffPostA2stage1;
					thePostFilterSum	+=	currentSample_t	* coefsPostFiltP->mCoeffPostA0stage1;
					thePostFilterSum	+=	DelayPostA1stage1	* coefsPostFiltP->mCoeffPostA1stage1;
 
 					thePostFilterSum	+=	-DelayPostB2stage1	* coefsPostFiltP->mCoeffPostB2stage1;
 					thePostFilterSum	+=	-DelayPostB1stage1	* coefsPostFiltP->mCoeffPostB1stage1;

					thePostFilterSum	=	thePostFilterSum	* 2.0;
 
 					thePostFilterOutput	=	thePostFilterSum	* coefsPostFiltP->mCoeffPostA0stage2;
 					thePostFilterOutput	+=	DelayPostA1stage2	* coefsPostFiltP->mCoeffPostA1stage2;

 					thePostFilterOutput +=	-DelayPostB1stage2	* coefsPostFiltP->mCoeffPostB1stage2;
 				
					thePostFilterOutput	=	thePostFilterOutput	* 2.0;
 
 					DelayPostA2stage1 = DelayPostA1stage1;
 					DelayPostA1stage1 = currentSample_t;
 					DelayPostB2stage1 = DelayPostB1stage1;
 					DelayPostB1stage1 = thePostFilterSum;
 					DelayPostA1stage2 = thePostFilterSum;
 					DelayPostB1stage2 = thePostFilterOutput;
 
 					thePostFilterOutput	+=	currentSample_t * 0.5;
 					currentSample_t	= static_cast<float>(thePostFilterOutput * 2.0);

					const float drySample = inputsP[t];
					//****************************************** Do Mix *****************************************

					outputsP[t] = (drySample * coefsEffectMixP->mDryMix) + (currentSample_t * coefsEffectMixP->mRectiMix);

					//********************************** Send output on its way *********************************
					//NOTE: bypass is processed in Mix component
					//outputsP[t] = (float (bypass) * drySample) + (float (!bypass) * currentSample[t]);
				} // Go to next sample time
				//********************************** Do Meters ***********************************
				float* const AAX_RESTRICT meterInP = meterPP[SMeterTapsCh::eMeter_InRectifiLeft + i];
				float meter_in = *meterInP;

				float* const AAX_RESTRICT meterOutP = meterPP[SMeterTapsCh::eMeter_OutRectifiLeft + i];
				float meter_out = *meterOutP;

				for (int32_t t = 0; t < kAudioWindowSize; t++)
				{
					dummy = fabsf(inputsP[t]);
					if (dummy > meter_in)
						meter_in = dummy;

					dummy = fabsf(outputsP[t]);
					if (dummy > meter_out)
						meter_out = dummy;
				}	
				*meterInP = meter_in;
				*meterOutP = meter_out;

 				stateP->mDelayPostA1stage1[i] = DelayPostA1stage1;
 				stateP->mDelayPostA2stage1[i] = DelayPostA2stage1;
 				stateP->mDelayPostB1stage1[i] = DelayPostB1stage1;
 				stateP->mDelayPostB2stage1[i] = DelayPostB2stage1;
 				stateP->mDelayPostA1stage2[i] = DelayPostA1stage2;
 				stateP->mDelayPostB1stage2[i] = DelayPostB1stage2;	

			} // Go to the next channel
		} // End audio buffer iteration loop
	} // End instance-iteration loop

}
