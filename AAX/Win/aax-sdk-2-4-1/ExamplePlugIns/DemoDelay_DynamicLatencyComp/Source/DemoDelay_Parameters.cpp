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
static const char* sDemoDelay_ParamID_DelayComp =			"Delay Comp";
static const char* sDemoDelay_ParamID_AlgDelay =			"Alg Delay";

// Custom error values
const AAX_Result cDemoDelay_Error_PlacementNewFailed =		-9999;
const AAX_Result cDemoDelay_Error_InvalidDataSize =			-9998;
const AAX_Result cDemoDelay_Error_ParameterGetValueFailed =	-9997;
const AAX_Result cDemoDelay_Error_SetParamValueFailed =		-9996;

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
mMaxDelayMS(cDemoDelay_MaxDelayMS),
mMixCache(cDemoDelay_DefaultMix)
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
	
	// Delay compensation toggle
	AAX_CParameter<bool>* adcToggleParameter = new AAX_CParameter<bool>(
		sDemoDelay_ParamID_DelayComp, AAX_CString("Delay Comp"), false,
		AAX_CBinaryTaperDelegate<bool>(),
		AAX_CBinaryDisplayDelegate<bool>("off", "on"), false);
	adcToggleParameter->SetType(AAX_eParameterType_Discrete);
	adcToggleParameter->SetNumberOfSteps(2);
	mParameterManager.AddParameter(adcToggleParameter);
	
	// Demonstration-only parameters
	Demo_RegisterInternalDataParameters();
	
	
	// register packet generator functions (this is just a convenience layer in plug-ins.  Doesn't affect Host)
	mPacketDispatcher.RegisterPacket(bypassID.CString(), AAX_FIELD_INDEX (SDemoDelay_AlgContext, mCoefsDelayP), this, &DemoDelay_Parameters::GenerateCoefficientPacket);
	mPacketDispatcher.RegisterPacket(sDemoDelay_ParamID_Mix, AAX_FIELD_INDEX (SDemoDelay_AlgContext, mCoefsDelayP), this, &DemoDelay_Parameters::GenerateCoefficientPacket);
		
	return AAX_SUCCESS;
}

AAX_Result DemoDelay_Parameters::NotificationReceived( /* AAX_ENotificationEvent */ AAX_CTypeID iNotificationType, const void * iNotificationData, uint32_t	iNotificationDataSize)
{
	AAX_Result result = AAX_CEffectParameters::NotificationReceived(iNotificationType, iNotificationData, iNotificationDataSize);
	
	// If the plug-in is in latency compensation mode then update the algorithm's 
	// delay state to match the delay that is currently being compensated by the host
	if ((AAX_eNotificationEvent_SignalLatencyChanged == iNotificationType) && (true == DelayCompEnabled()))
	{
		// If this were a real plug-in we would queue a packet for dispatch to the alg
		// here (see GenerateDelaySamplesPacket().) For demonstration, we update the
		// display-only AlgDelay param instead and it triggers the packet dispatch.
		result = Demo_UpdateAlgDelay();
	}
	
	return result;
}

AAX_Result DemoDelay_Parameters::UpdateParameterTouch ( AAX_CParamID iParameterID, AAX_CBoolean iTouchState )
{
	// The Delay Comp toggle is linked to the Mix parameter, so we don't want other clients
	// to be able to manipulate the Mix parameter while the Delay Comp toggle is touched
	if (AAX::IsParameterIDEqual(sDemoDelay_ParamID_DelayComp, iParameterID))
	{
		iTouchState ? TouchParameter(sDemoDelay_ParamID_Mix) : ReleaseParameter(sDemoDelay_ParamID_Mix);
	}
	else if (AAX::IsParameterIDEqual(sDemoDelay_ParamID_Mix, iParameterID))
	{
		// This is a little gratuitous since the delay comp toggle is not automatable and is
		// not exposed in the plug-in's page tables, so the GUI will be the only client that
		// ever accesses this parameter. Nevertheless, this is good general practice.
		iTouchState ? TouchParameter(sDemoDelay_ParamID_DelayComp) : ReleaseParameter(sDemoDelay_ParamID_DelayComp);
	}
	
	// Update demonstration-only parameter touch state
	Demo_UpdateParameterTouch(iParameterID, iTouchState);
	
	return AAX_CEffectParameters::UpdateParameterTouch(iParameterID, iTouchState);
}

AAX_Result DemoDelay_Parameters::UpdateParameterNormalizedValue (AAX_CParamID iParameterID, double iValue, AAX_EUpdateSource iSource )
{
	AAX_Result result = AAX_SUCCESS;
	
	if (false == Demo_BlockParameterChange(iParameterID))
	{
		OverrideParameterChange(iParameterID, false, iValue);
		result = AAX_CEffectParameters::UpdateParameterNormalizedValue(iParameterID, iValue, iSource);
		HandleParameterUpdate(iParameterID, iSource);
	}
	
	return result;
}

AAX_Result DemoDelay_Parameters::UpdateParameterNormalizedRelative (AAX_CParamID iParameterID, double iValue )
{
	AAX_Result result = AAX_SUCCESS;

	if (false == Demo_BlockParameterChange(iParameterID))
	{
		OverrideParameterChange(iParameterID, true, iValue);
		result = AAX_CEffectParameters::UpdateParameterNormalizedRelative(iParameterID, iValue);
		HandleParameterUpdate(iParameterID, AAX_eUpdateSource_Unspecified);
	}
	
	return result;
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

bool DemoDelay_Parameters::OverrideParameterChange(AAX_CParamID iParameterID, bool iRelative, double& ioValue)
{
	const double inputValue = ioValue;
	
	// Mix parameter may only be set to 100% while delay comp is enabled
	if ((AAX::IsParameterIDEqual(sDemoDelay_ParamID_Mix, iParameterID)) && (true == DelayCompEnabled()))
	{
		const AAX_IParameter* mixParam = mParameterManager.GetParameterByID(iParameterID);
		const AAX_CParameter<float>* mixParamConcrete = dynamic_cast<const AAX_CParameter<float>*>(mixParam);
		if (mixParamConcrete)
		{
			const AAX_ITaperDelegate<float>* mixParamTaper = mixParamConcrete->TaperDelegate();
			if (mixParamTaper)
			{
				ioValue = mixParamTaper->RealToNormalized(mixParamTaper->GetMaximumValue());
			}
		}
	}
	
	return (inputValue != ioValue);
}

void DemoDelay_Parameters::HandleParameterUpdate(AAX_CParamID iParameterID, AAX_EUpdateSource iSource)
{
	bool doSetSignalLatency = false;
	
	if (AAX::IsParameterIDEqual(sDemoDelay_ParamID_DelayComp, iParameterID))
	{
		// Set mix parameter to 100% when in delay compensation mode.
		// The call to IsParameterLinkReady() here, combined with the
		// linking touch logic in UpdateParameterTouch(), prevents
		// "fighting" between different parameter controllers.
		if (IsParameterLinkReady(sDemoDelay_ParamID_DelayComp, iSource))
		{
			AAX_IParameter* mixParameter = mParameterManager.GetParameterByID(sDemoDelay_ParamID_Mix);
			if (mixParameter)
			{
				if (DelayCompEnabled())
				{
					// Going into delay comp mode; cache the curent mix value
					mixParameter->GetValueAsFloat(&mMixCache);
					mixParameter->SetValueWithFloat(1.0f);
				}
				else
				{
					// Coming out of delay comp mode; restore the cached mix value
					mixParameter->SetValueWithFloat(mMixCache);
				}
			}
		}
		
		doSetSignalLatency = true;
	}
	else if (AAX::IsParameterIDEqual(sDemoDelay_ParamID_Delay, iParameterID))
	{
		if (false == DelayCompEnabled())
		{
			// Delay comp is not enabled, so the Delay control
			// directly sets the algorithmic delay.
			//
			// In a "real" plug-in we would just set an internal
			// variable here. See the note about demonstration
			// parameters in the header.
			Demo_SetAlgDelay(iSource);
		}
		else
		{
			// Delay comp is enabled, so the Delay control cannot
			// set the algorithm latency directly; instead, the
			// plug-in must wait for an updated latency value from
			// the host before altering the algorithm state.
			doSetSignalLatency = true;
		}
	}
	
	if ((true == doSetSignalLatency) && (NULL != Controller()))
	{
		// Set the signal latency to the current delay value if delay
		// comp is enabled; otherwise, set the signal latency to zero
		int32_t signalLatency = 0;
		if (true == DelayCompEnabled())
		{
			const AAX_Result getDelayResult = GetRequestedDelaySamples(signalLatency);
			AAX_ASSERT(AAX_SUCCESS == getDelayResult);
		}
		
		Controller()->SetSignalLatency(signalLatency);
	}
}

AAX_Result DemoDelay_Parameters::GetRequestedDelaySamples(int32_t& oSamples) const
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

bool DemoDelay_Parameters::DelayCompEnabled() const
{
	bool success = false;
	bool result = false;
	
	const AAX_IParameter* delayCompParam = mParameterManager.GetParameterByID(sDemoDelay_ParamID_DelayComp);
	if (delayCompParam)
	{
		success = delayCompParam->GetValueAsBool(&result);
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
			result = Demo_GetAlgDelaySamples(*delaySamplesCoef);
		}
		else
		{
			result = AAX_ERROR_NULL_OBJECT;
		}
	}
	
	return result;
}


// *******************************************************************************
#pragma mark - DemoDelay_Parameters demonstration-only methods
// See comment in header regarding these methods

void DemoDelay_Parameters::Demo_RegisterInternalDataParameters()
{
	// These parameters are not exposed on control surfaces and are not automatable. If we
	// were to write a custom GUI for the plug-in these would just be member variables
	
	// Real delay that will be applied to the algorithm
	AAX_CParameter<float>* algDelayParameter = new AAX_CParameter<float>(
		sDemoDelay_ParamID_AlgDelay, AAX_CString("Alg Delay"), mMaxDelayMS,
		AAX_CLinearTaperDelegate<float, 1>(mMinDelayMS, mMaxDelayMS),
		AAX_CUnitDisplayDelegateDecorator<float>(AAX_CNumberDisplayDelegate<float>(), " mS"), false);
	algDelayParameter->SetNumberOfSteps(55);
	mParameterManager.AddParameter(algDelayParameter);
	
	// Register alg delay packet
	mPacketDispatcher.RegisterPacket(sDemoDelay_ParamID_AlgDelay, AAX_FIELD_INDEX(SDemoDelay_AlgContext, mDelaySamplesP), this, &DemoDelay_Parameters::GenerateDelaySamplesPacket);
}

void DemoDelay_Parameters::Demo_UpdateParameterTouch(AAX_CParamID iParameterID, AAX_CBoolean iTouchState)
{
	// The Delay and Alg Delay parameters are linked
	if (AAX::IsParameterIDEqual(sDemoDelay_ParamID_Delay, iParameterID))
	{
		iTouchState ? TouchParameter(sDemoDelay_ParamID_AlgDelay) : ReleaseParameter(sDemoDelay_ParamID_AlgDelay);
	}
	else if (AAX::IsParameterIDEqual(sDemoDelay_ParamID_AlgDelay, iParameterID))
	{
		// The Alg Delay parameter cannot be edited, so no need to link it here. (See Demo_BlockParameterChange())
	}
}

bool DemoDelay_Parameters::Demo_BlockParameterChange(AAX_CParamID iParameterID) const
{
	bool doBlock = false;
	
	// NOTE: This logic is ONLY necessary here because we are using an auto-generated
	// GUI and cannot hide these parameters from it. A "real" plug-in should never require
	// this kind of override.
	if (AAX::IsParameterIDEqual(sDemoDelay_ParamID_AlgDelay, iParameterID))
	{
		// Block changes to the Alg Delay parameter if the Delay parameter is not being edited
		doBlock = (false == IsParameterTouched(sDemoDelay_ParamID_Delay));
	}
	
	return doBlock;
}

AAX_Result DemoDelay_Parameters::Demo_UpdateAlgDelay()
{
	AAX_Result result = AAX_SUCCESS;
	
	if (Controller())
	{
		int32_t numSamplesLatency = 0;
		result = Controller()->GetSignalLatency(&numSamplesLatency);
		if (AAX_SUCCESS == result)
		{
			AAX_IParameter* algDelayParam = mParameterManager.GetParameterByID(sDemoDelay_ParamID_AlgDelay);
			if (algDelayParam)
			{
				float delayMS;
				result = ConvertBetweenMSAndSamples(eConversionDirection_SamplesToMS, delayMS, numSamplesLatency);
				if (AAX_SUCCESS == result)
				{
					result = algDelayParam->SetValueWithFloat(delayMS) ? AAX_SUCCESS : cDemoDelay_Error_SetParamValueFailed;
				}
			}
			else
			{
				result = AAX_ERROR_INVALID_PARAMETER_ID;
			}
		}
	}
	else
	{
		result = AAX_ERROR_NULL_OBJECT;
	}
	
	return result;
}

void DemoDelay_Parameters::Demo_SetAlgDelay(AAX_EUpdateSource iSource)
{
	if (IsParameterLinkReady(sDemoDelay_ParamID_Delay, iSource))
	{
		AAX_IParameter* algDelayParameter = mParameterManager.GetParameterByID(sDemoDelay_ParamID_AlgDelay);
		if (algDelayParameter)
		{
			int32_t requestedDelaySamples;
			AAX_Result internalOperationResult = GetRequestedDelaySamples(requestedDelaySamples);
			if (AAX_SUCCESS == internalOperationResult)
			{
				float requestedDelayMS;
				internalOperationResult = ConvertBetweenMSAndSamples(eConversionDirection_SamplesToMS, requestedDelayMS, requestedDelaySamples);
				if (AAX_SUCCESS == internalOperationResult)
				{
					algDelayParameter->SetValueWithFloat(requestedDelayMS);
				}
			}
		}
	}
}

AAX_Result DemoDelay_Parameters::Demo_GetAlgDelaySamples(int32_t& oSamples) const
{
	AAX_Result result = AAX_ERROR_NULL_OBJECT;
	
	// Calculate alg delay based on the alg delay parameter setting and sample rate.
    const AAX_IParameter* delayParameter = mParameterManager.GetParameterByID( sDemoDelay_ParamID_AlgDelay );
	if (delayParameter && Controller())
	{
		float delayInMs;
		const bool getValueSuccess = delayParameter->GetValueAsFloat(&delayInMs);
		result = (true == getValueSuccess) ? ConvertBetweenMSAndSamples(eConversionDirection_MSToSamples, delayInMs, oSamples) : cDemoDelay_Error_ParameterGetValueFailed;
	}
	
	return result;
}
