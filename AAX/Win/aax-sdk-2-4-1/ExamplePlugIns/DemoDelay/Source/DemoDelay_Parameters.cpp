/*================================================================================================*/
/*
 *	Copyright 2014-2015 by Avid Technology, Inc.
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
#include "DemoDelay_Parameters.h"
#include "DemoDelay_Defs.h"
#include "DemoDelay_Alg.h"
#include "CSimpleDelayLine.h"

#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"

#include "AAX_UtilsNative.h"
#include "AAX_Assert.h"


// *******************************************************************************
#pragma mark - Local constants

// Parameter IDs
static const char* sDemoDelay_ParamID_Delay =				"Delay";
static const char* sDemoDelay_ParamID_Mix =					"Mix";

// Custom error values
const AAX_Result cDemoDelay_Error_PlacementNewFailed =		-9999;
const AAX_Result cDemoDelay_Error_InvalidDataSize =			-9998;
const AAX_Result cDemoDelay_Error_ParameterGetValueFailed =	-9997;

// Parameter values
const float cDemoDelay_MinDelayMS =							30.0f;
const float cDemoDelay_MaxDelayMS =							500.0f;
const float cDemoDelay_DefaultMix =							0.3f;


// *******************************************************************************
#pragma mark - DemoDelay_Parameters ctor/dtor and factory methods

AAX_CEffectParameters *AAX_CALLBACK DemoDelay_Parameters::Create()
{
	return new DemoDelay_Parameters();
}

DemoDelay_Parameters::DemoDelay_Parameters () :
AAX_CEffectParameters(),
mMinDelayMS(cDemoDelay_MinDelayMS),
mMaxDelayMS(cDemoDelay_MaxDelayMS)
{
}

DemoDelay_Parameters::~DemoDelay_Parameters()
{
}


// *******************************************************************************
#pragma mark - DemoDelay_Parameters public methods

AAX_Result DemoDelay_Parameters::EffectInit()
{
	// Constrain minimum and maximum delay lengths
	//
	// Set the maximum delay length based on the size of the delay line
	int32_t maxPossibleDelaySamples = CSimpleDelayLine::GetMaxDelay();
	float maxPossibleDelayMS = 0.0f;
	AAX_Result conversionResult = ConvertBetweenMSAndSamples(eConversionDirection_SamplesToMS, maxPossibleDelayMS, maxPossibleDelaySamples);
	if (AAX_SUCCESS == conversionResult)
	{
		mMaxDelayMS = std::min(cDemoDelay_MaxDelayMS, maxPossibleDelayMS);
	}
	
	
	// Register parameters and packet generation callbacks
	//
	
	// bypass
	AAX_CString bypassID = cDefaultMasterBypassID;
	AAX_IParameter * masterBypass = new AAX_CParameter<bool>(
		bypassID.CString(), AAX_CString("Bypass"), false,
		AAX_CBinaryTaperDelegate<bool>(),
		AAX_CBinaryDisplayDelegate<bool>("bypass", "on"), true);
	masterBypass->SetNumberOfSteps( 2 );
	masterBypass->SetType( AAX_eParameterType_Discrete );
	mParameterManager.AddParameter(masterBypass);
	
	// Wet/Dry Mix
	AAX_CParameter<float>* mixParameter = new AAX_CParameter<float>(
		sDemoDelay_ParamID_Mix, AAX_CString("Mix"), cDemoDelay_DefaultMix,
		AAX_CLinearTaperDelegate<float>(0.0f, 1.0f),
		AAX_CNumberDisplayDelegate<float>(), true);
	mixParameter->SetNumberOfSteps(55);
	mParameterManager.AddParameter(mixParameter);
    
    // Delay
    AAX_CParameter<float>* delayParameter = new AAX_CParameter<float>(
		sDemoDelay_ParamID_Delay, AAX_CString("Delay"), mMaxDelayMS,
		AAX_CLinearTaperDelegate<float, 1>(mMinDelayMS, mMaxDelayMS),
		AAX_CUnitDisplayDelegateDecorator<float>(AAX_CNumberDisplayDelegate<float>(), " mS"), true);
	delayParameter->SetNumberOfSteps(55);
	mParameterManager.AddParameter(delayParameter);
	
	
	// register packet generator functions (this is just a convenience layer in plug-ins.  Doesn't affect Host)
	mPacketDispatcher.RegisterPacket(bypassID.CString(), AAX_FIELD_INDEX (SDemoDelay_AlgContext, mCoefsDelayP), this, &DemoDelay_Parameters::GenerateCoefficientPacket);
	mPacketDispatcher.RegisterPacket(sDemoDelay_ParamID_Mix, AAX_FIELD_INDEX (SDemoDelay_AlgContext, mCoefsDelayP), this, &DemoDelay_Parameters::GenerateCoefficientPacket);
	mPacketDispatcher.RegisterPacket(sDemoDelay_ParamID_Delay, AAX_FIELD_INDEX(SDemoDelay_AlgContext, mDelaySamplesP), this, &DemoDelay_Parameters::GenerateDelaySamplesPacket);
	
	return AAX_SUCCESS;
}

AAX_Result DemoDelay_Parameters::ResetFieldData (AAX_CFieldIndex iFieldIndex, void * oData, uint32_t iDataSize) const
{
	AAX_Result result = AAX_SUCCESS;
	
	switch (iFieldIndex)
	{
		case AAX_FIELD_INDEX(SDemoDelay_AlgContext, mDelayLineP):
		{
			if (oData && sizeof(CSimpleDelayLine) == iDataSize)
			{
				CSimpleDelayLine* newObj = new(oData) CSimpleDelayLine;
				result = newObj ? AAX_SUCCESS : cDemoDelay_Error_PlacementNewFailed;
			}
			else
			{
				result = cDemoDelay_Error_InvalidDataSize;
			}
			break;
		}
		default:
		{
			result = AAX_CEffectParameters::ResetFieldData(iFieldIndex, oData, iDataSize);
			break;
		}
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

AAX_Result DemoDelay_Parameters::GenerateDelaySamplesPacket(AAX_CPacket &ioPacket)
{
	AAX_Result result = AAX_SUCCESS;
	
	if (false == BypassEnabled())
	{
		int32_t* const delaySamplesCoef = ioPacket.GetPtr<int32_t>();
		if (delaySamplesCoef)
		{
			result = GetDelaySamples(*delaySamplesCoef);
		}
		else
		{
			result = AAX_ERROR_NULL_OBJECT;
		}
	}
	
	return result;
}
