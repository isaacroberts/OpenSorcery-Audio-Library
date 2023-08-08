/*================================================================================================*/
/*
 *	Copyright 2013-2017, 2019 by Avid Technology, Inc.
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

// DemoDelay Includes
#include "DemoDelay_Parameters.h"
#include "DemoDelay_Defs.h"
#include "DemoDelay_Alg.h"

// AAX Includes
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"
#include "AAX_Assert.h"

// Standard Includes
#include <memory>


// *******************************************************************************
#pragma mark - Local constants

namespace
{
	// Parameter IDs
	static const char* sDemoDelay_ParamID_Delay =				"Delay";
	static const char* sDemoDelay_ParamID_Mix =					"Mix";

	// Custom error values
	const AAX_Result cDemoDelay_Error_ParameterGetValueFailed =	-9997;
	const AAX_Result cDemoDelay_Error_InvalidDelayValue =		-9995;
}

// *******************************************************************************
#pragma mark - DemoDelay_Parameters ctor/dtor and factory methods

AAX_CEffectParameters *AAX_CALLBACK DemoDelay_Parameters::Create()
{
	return new DemoDelay_Parameters();
}

DemoDelay_Parameters::DemoDelay_Parameters () :
AAX_CEffectParameters(),
mDelayLine()
{
}

DemoDelay_Parameters::~DemoDelay_Parameters()
{
}


// *******************************************************************************
#pragma mark - DemoDelay_Parameters public methods

AAX_Result DemoDelay_Parameters::EffectInit()
{
	AAX_Result result = AAX_SUCCESS;
	
	// Constrain minimum and maximum delay lengths
	//
	// Set the minimum delay length based on the round-trip signal latency (see RenderAudio_Hybrid)
	float minDelayMS = 0.0f;
	if (AAX_SUCCESS == result)
	{
		int32_t hybridLatency = 2096;
		AAX_Result hybridResult = Controller()->GetHybridSignalLatency(&hybridLatency);
		
		// The host may not support hybrid processing. This may be fine if the host is not configured
		// to run the plug-in's algorithm at all (e.g. a test host) so just set the latency to zero.
		if (AAX_ERROR_UNIMPLEMENTED == hybridResult)
		{
			hybridLatency = 0;
			hybridResult = AAX_SUCCESS;
		}
		result = hybridResult;
		
		result = (AAX_SUCCESS == result) ? ConvertBetweenMSAndSamples(eConversionDirection_SamplesToMS, minDelayMS, hybridLatency) : result;
	}
	// Set the maximum delay length based on the size of the delay line
	float maxDelayMS = 0.0f;
	if (AAX_SUCCESS == result)
	{
		int32_t maxDelaySamples = mDelayLine.GetMaxDelay();
		result = ConvertBetweenMSAndSamples(eConversionDirection_SamplesToMS, maxDelayMS, maxDelaySamples);
	}
	
	// Register parameters and packet generation callbacks
	//
	if (AAX_SUCCESS == result)
	{
		// Bypass
		{
			std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<bool>(
				cDefaultMasterBypassID, AAX_CString("Bypass"), false,
				AAX_CBinaryTaperDelegate<bool>(),
				AAX_CBinaryDisplayDelegate<bool>("bypass", "on"), true));
			param->SetNumberOfSteps( 2 );
			param->SetType( AAX_eParameterType_Discrete );
			mParameterManager.AddParameter(param.release());
		}
		
		// Wet/Dry Mix
		{
			std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
				sDemoDelay_ParamID_Mix, AAX_CString("Mix"), 0.5f,
				AAX_CLinearTaperDelegate<float>(0.0f, 1.0f),
				AAX_CNumberDisplayDelegate<float>(), true));
			param->SetNumberOfSteps(55);
			mParameterManager.AddParameter(param.release());
		}
		
		// Delay
		// note: this won't go to the low latency algorithm via
		// the Packet Dispatcher because we are applying this
		// delay directly in the high latency render callback.
		{
			std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
				sDemoDelay_ParamID_Delay, AAX_CString("Delay"), maxDelayMS,
				AAX_CLinearTaperDelegate<float, 1>(minDelayMS, maxDelayMS),
				AAX_CUnitDisplayDelegateDecorator<float>(AAX_CNumberDisplayDelegate<float>(), "mS"), true));
			param->SetNumberOfSteps(55);
			mParameterManager.AddParameter(param.release());
		}
		
		
		// register packet generator functions (this is just a convenience layer in plug-ins.  Doesn't affect Host)
		mPacketDispatcher.RegisterPacket(cDefaultMasterBypassID, AAX_FIELD_INDEX (SDemoDelayHybrid_LowLatency_Context, mCoefsDelayP), this, &DemoDelay_Parameters::GenerateCoefficientPacket);
		mPacketDispatcher.RegisterPacket(sDemoDelay_ParamID_Mix, AAX_FIELD_INDEX (SDemoDelayHybrid_LowLatency_Context, mCoefsDelayP), this, &DemoDelay_Parameters::GenerateCoefficientPacket);
	}
		
	return result;
}

AAX_Result DemoDelay_Parameters::RenderAudio_Hybrid(AAX_SHybridRenderInfo* ioRenderInfo)
{
	AAX_Result result = AAX_SUCCESS;
	
    //Just Mono here, but you can do many different stem formats if you look at AAX_SHybridRenderInfo struct.
    const float* const audioInput = ioRenderInfo->mAudioInputs[0];
    float* const audioOutput = ioRenderInfo->mAudioOutputs[0];
    const int32_t numSamples = *ioRenderInfo->mNumSamples;
	
    // Calculate raw delay based on the delay setting and sample rate.
    int32_t numDelaySamples = 0;
	result = GetDelaySamples(numDelaySamples);
	
    // Factor in the round-trip latency for moving the samples up to high latency thread and back.  Usually, this is 2096,
    // but we can make sure by querying the controller. Since we are implementing a delay line algorithm, we simply subtract
	// off this round-trip latency so that the resulting plug-in output is placed with the desired delay on the timeline.
    int32_t hybridLatency = 2096;
    Controller()->GetHybridSignalLatency(&hybridLatency);
    numDelaySamples -= hybridLatency;
	
	if (0 <= numDelaySamples && mDelayLine.GetMaxDelay() >= numDelaySamples)
	{
		// Apply the adjusted delay amount to the hybrid audio output stream
		mDelayLine.ProcessSamples(numSamples, numDelaySamples, audioInput, audioOutput);
	}
	else
	{
		result = cDemoDelay_Error_InvalidDelayValue;
	}
	
    return result;
}


// *******************************************************************************
#pragma mark - DemoDelay_Parameters private methods

AAX_Result DemoDelay_Parameters::GetDelaySamples(int32_t& oSamples) const
{
	AAX_Result result = AAX_ERROR_NULL_OBJECT;
	
	// Calculate delay based on the delay parameter setting and sample rate.
    const AAX_IParameter* delayParameter = mParameterManager.GetParameterByID( sDemoDelay_ParamID_Delay );
	if (delayParameter && Controller())
	{
		float delayInMs;
		const bool getValueSuccess = delayParameter->GetValueAsFloat(&delayInMs);
		result = (true == getValueSuccess) ? ConvertBetweenMSAndSamples(eConversionDirection_MSToSamples, delayInMs, oSamples) : cDemoDelay_Error_ParameterGetValueFailed;
	}
	
	return result;
}

AAX_Result DemoDelay_Parameters::ConvertBetweenMSAndSamples(EConversionDirection iConversionDirection, float& ioMS, int32_t& ioSamples) const
{
	float sampleRate;
	AAX_Result result = Controller()->GetSampleRate(&sampleRate);
	if (AAX_SUCCESS == result)
	{
		switch (iConversionDirection)
		{
			case eConversionDirection_MSToSamples:
				ioSamples = static_cast<int32_t>(sampleRate * ioMS / 1000.0f);
				break;
			case eConversionDirection_SamplesToMS:
				ioMS = static_cast<float>((1000.0f * ioSamples) / sampleRate);
				break;
		}
	}
	
	return result;
}

bool DemoDelay_Parameters::BypassEnabled() const
{
	bool success = false;
	bool result = false;
	
	const AAX_IParameter* bypassParam = mParameterManager.GetParameterByID(cDefaultMasterBypassID);
	if (bypassParam)
	{
		success = bypassParam->GetValueAsBool(&result);
	}
	
	AAX_ASSERT(success);
	return result;
}

AAX_Result DemoDelay_Parameters::GenerateCoefficientPacket( AAX_CPacket& ioPacket )
{
    AAX_Result result = AAX_SUCCESS;
	float mixValue = 0.0;
	
	if (false == BypassEnabled())
	{
		AAX_IParameter*	 mixParameter = mParameterManager.GetParameterByID( sDemoDelay_ParamID_Mix );
		if (mixParameter)
		{
			mixParameter->GetValueAsFloat(&mixValue);
		}
		else
		{
			result = AAX_ERROR_NULL_OBJECT;
		}
	}
	
	SDemoDelay_Coefs&  coeffs = *ioPacket.GetPtr<SDemoDelay_Coefs>();
    coeffs.mWetGain = mixValue;
    coeffs.mDryGain = 1.0f - mixValue;
	
    return result;
}
