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

#include "DemoDelay_HostProcessor.h"
#include "DemoDelay_Parameters.h"
#include "CSimpleDelayLine.h"


// *******************************************************************************
#pragma mark - Local constants

// Custom error values
const AAX_Result cDemoDelay_Error_InvalidRenderParams =			-9989;


// *******************************************************************************
#pragma mark - DemoDelay_HostProcessor ctor/dtor and factory methods

AAX_CHostProcessor *AAX_CALLBACK DemoDelay_HostProcessor::Create()
{
	return new DemoDelay_HostProcessor();
}

DemoDelay_HostProcessor::DemoDelay_HostProcessor() :
AAX_CHostProcessor(),
mDelayLines(NULL),
mDelaySamples(0)
{
}

DemoDelay_HostProcessor::~DemoDelay_HostProcessor()
{
	DeleteDelayLines();
}


// *******************************************************************************
#pragma mark - DemoDelay_HostProcessor public methods

AAX_Result DemoDelay_HostProcessor::RenderAudio (const float * const inAudioIns [], int32_t inAudioInCount, float * const inAudioOuts [], int32_t inAudioOutCount, int32_t * ioWindowSize)
{
	AAX_Result result = AAX_SUCCESS;
	
	if ((inAudioInCount == inAudioOutCount) && (NULL != ioWindowSize))
	{
		for (int32_t i = 0; i < inAudioInCount; ++i)
		{
			const float * const	inputBuffer  = inAudioIns [i];
			float       * const	outputBuffer = inAudioOuts [i];
			
			if ( inputBuffer && outputBuffer )
			{
				mDelayLines[i].ProcessSamples(*ioWindowSize, mDelaySamples, inputBuffer, outputBuffer);
			}
		}
    }
	else
	{
		result = cDemoDelay_Error_InvalidRenderParams;
	}
	
	return result;
}

AAX_Result DemoDelay_HostProcessor::PreRender (int32_t iAudioInCount, int32_t /*iAudioOutCount*/, int32_t /*iWindowSize*/)
{
	AAX_Result result = AAX_SUCCESS;
	
	CreateDelayLines(iAudioInCount);
	
	// Initialize mDelaySamples
	result = GetEffectParameters()->GetCustomData(DemoDelay_Parameters::sDataBlockID_DelaySamples, sizeof(int32_t), &mDelaySamples, NULL);
	
	return result;
}

AAX_Result DemoDelay_HostProcessor::PostRender ()
{
	DeleteDelayLines();
	return AAX_SUCCESS;
}

void DemoDelay_HostProcessor::CreateDelayLines(int32_t inNumDelayLines)
{
	DeleteDelayLines();
	mDelayLines = new CSimpleDelayLine[inNumDelayLines];
}

void DemoDelay_HostProcessor::DeleteDelayLines()
{
	if (NULL != mDelayLines)
	{
		delete[] mDelayLines;
		mDelayLines = NULL;
	}
}

