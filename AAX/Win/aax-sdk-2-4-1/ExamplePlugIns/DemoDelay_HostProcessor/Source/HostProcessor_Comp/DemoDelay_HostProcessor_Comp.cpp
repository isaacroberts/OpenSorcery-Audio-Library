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
#include <algorithm>
#include "DemoDelay_HostProcessor_Comp.h"
#include "DemoDelay_Parameters.h"
#include "CSimpleDelayLine.h"


// *******************************************************************************
#pragma mark - Local constants

// Custom error values
const AAX_Result cDemoDelay_Error_InvalidRenderParams =			-9989;


// *******************************************************************************
#pragma mark - DemoDelay_HostProcessor ctor/dtor and factory methods

AAX_CHostProcessor *AAX_CALLBACK DemoDelay_HostProcessor_Comp::Create()
{
	return new DemoDelay_HostProcessor_Comp();
}

DemoDelay_HostProcessor_Comp::DemoDelay_HostProcessor_Comp() :
AAX_CHostProcessor(),
mIsFirstPass(false),
mDelayLines(NULL),
mDelaySamples(0)
{
}

DemoDelay_HostProcessor_Comp::~DemoDelay_HostProcessor_Comp()
{
	DeleteDelayLines();
}


// *******************************************************************************
#pragma mark - DemoDelay_HostProcessor public methods

AAX_Result DemoDelay_HostProcessor_Comp::RenderAudio (const float * const inAudioIns [], int32_t inAudioInCount, float * const inAudioOuts [], int32_t inAudioOutCount, int32_t * ioWindowSize)
{
	AAX_Result result = AAX_SUCCESS;
	
	if ((inAudioInCount == inAudioOutCount) && (NULL != ioWindowSize))
	{
		for (int32_t i = 0; i < inAudioInCount; ++i)
		{
			const float * inputBuffer  = inAudioIns [i];
			float       * outputBuffer = inAudioOuts [i];
			
			if ( inputBuffer && outputBuffer )
			{
				if (true == mIsFirstPass)
				{
					// Our "algorithm" incurs mDelaySamples of delay; we can shift the output
					// samples back to the correct position by feeding mDelaySamples into the
					// algorithm to prime it.
					float* const tempBuffer = new float[*ioWindowSize];
					for (int32_t remainingDelaySamplesToPrime = mDelaySamples; remainingDelaySamplesToPrime > 0; /* decrement in loop */)
					{
						int32_t numSamplesToPrime = std::min(*ioWindowSize, remainingDelaySamplesToPrime);
						const int64_t firstSampleLocation = GetLocation() + mDelaySamples - remainingDelaySamplesToPrime;
						
						GetAudio(inAudioIns, inAudioInCount, firstSampleLocation, &numSamplesToPrime);
						mDelayLines[i].ProcessSamples(numSamplesToPrime, mDelaySamples, inputBuffer, tempBuffer);
						remainingDelaySamplesToPrime -= numSamplesToPrime;
					}
					delete[] tempBuffer;
				}
				
				// Look ahead in the input audio by mDelaySamples. After this call to GetAudio(),
				// inAudioIns will be populated with the randomly-accessed lookahead samples.
				GetAudio(inAudioIns, inAudioInCount, GetLocation()+mDelaySamples, ioWindowSize);
				inputBuffer  = inAudioIns [i];
				
				// Run the algorithm
				mDelayLines[i].ProcessSamples(*ioWindowSize, mDelaySamples, inputBuffer, outputBuffer);
			}
		}
    }
	else
	{
		result = cDemoDelay_Error_InvalidRenderParams;
	}
	
	mIsFirstPass = false;
	return result;
}

AAX_Result DemoDelay_HostProcessor_Comp::PreRender (int32_t iAudioInCount, int32_t iAudioOutCount, int32_t iWindowSize)
{
	AAX_Result result = AAX_SUCCESS;
	
	CreateDelayLines(iAudioInCount);
	
	// Initialize mDelaySamples
	result = GetEffectParameters()->GetCustomData(DemoDelay_Parameters::sDataBlockID_DelaySamples, sizeof(int32_t), &mDelaySamples, NULL);
	mIsFirstPass = true;
	
	return result;
}

AAX_Result DemoDelay_HostProcessor_Comp::PostRender ()
{
	DeleteDelayLines();
	return AAX_SUCCESS;
}

void DemoDelay_HostProcessor_Comp::CreateDelayLines(int32_t inNumDelayLines)
{
	DeleteDelayLines();
	mDelayLines = new CSimpleDelayLine[inNumDelayLines];
}

void DemoDelay_HostProcessor_Comp::DeleteDelayLines()
{
	if (NULL != mDelayLines)
	{
		delete[] mDelayLines;
		mDelayLines = NULL;
	}
}
