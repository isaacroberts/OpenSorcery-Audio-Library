/*================================================================================================*/
/*
 *	Copyright 2009-2015, 2021 by Avid Technology, Inc.
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

#include "DemoGain_Parameters.h"
#include "DemoGain_Defs.h"
#include "DemoGain_Alg.h"

#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CStateTaperDelegate.h"
#include "AAX_CStateDisplayDelegate.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"

#include "AAX_CommonConversions.h" // for DBToGain()
#include "AAX_UtilsNative.h"
#include "AAX_Assert.h"



// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_CEffectParameters *AAX_CALLBACK DemoGain_Parameters::Create()
{
	return new DemoGain_Parameters();
}

// *******************************************************************************
// METHOD:	DemoGain_Parameters
// *******************************************************************************
DemoGain_Parameters::DemoGain_Parameters () :
	AAX_CEffectParameters()
{
}

// *******************************************************************************
// METHOD:	EffectInit
// *******************************************************************************
AAX_Result DemoGain_Parameters::EffectInit()
{	
	// bypass
	AAX_CString bypassID = cDefaultMasterBypassID;
	AAX_IParameter * masterBypass = new AAX_CParameter<bool>(
		bypassID.CString(), AAX_CString("Master Bypass"), false,
		AAX_CBinaryTaperDelegate<bool>(),
		AAX_CBinaryDisplayDelegate<bool>("bypass", "on"), true);
	masterBypass->SetNumberOfSteps( 2 );
	masterBypass->SetType( AAX_eParameterType_Discrete );
	mParameterManager.AddParameter(masterBypass);

	// gain left
	AAX_CParameter<double>* gainLeftParameter = new AAX_CParameter<double>(
		DemoGain_GainLeftID, AAX_CString("Gain Left"), 0.0,
		AAX_CLinearTaperDelegate<double, 0>(-36.0, 0.0),
		AAX_CUnitDisplayDelegateDecorator<double>( AAX_CNumberDisplayDelegate<double,1,1>(), "dB" ), true);
	gainLeftParameter->SetNumberOfSteps(55);
	mParameterManager.AddParameter(gainLeftParameter);
	
	// gain right
	AAX_CParameter<double>* gainRightParameter = new AAX_CParameter<double>(
		DemoGain_GainRightID, AAX_CString("Gain Right"), 0.0,
		AAX_CLinearTaperDelegate<double, 0>(-36.0, 0.0),
		AAX_CUnitDisplayDelegateDecorator<double>( AAX_CNumberDisplayDelegate<double,1,1>(), "dB" ), true);
	gainRightParameter->SetNumberOfSteps(55);
	mParameterManager.AddParameter(gainRightParameter);
	
	// link toggle
    AAX_CParameter<bool> *	linkParam = new AAX_CParameter<bool>(
		DemoGain_LinkToggleID, AAX_CString("Link"), true,
		AAX_CBinaryTaperDelegate<bool>(),
		AAX_CBinaryDisplayDelegate<bool>("Off", "On"), false );
	linkParam->SetNumberOfSteps ( 2 );
	linkParam->SetType ( AAX_eParameterType_Discrete );
	mParameterManager.AddParameter( linkParam );
	
	// type toggle
    const char * typeStrings[] = {"Pan", "Gain", NULL};
	AAX_CParameter<int32_t> *	typeParam = new AAX_CParameter<int32_t>(
		DemoGain_TypeToggleID, AAX_CString("Type"), eType_Gain,
		AAX_CStateTaperDelegate<int32_t>(eType_Pan, eType_Gain),
		AAX_CStateDisplayDelegate<int32_t>(typeStrings, eType_Pan), true );
	typeParam->SetNumberOfSteps ( eType_NumTypes );
	typeParam->SetType ( AAX_eParameterType_Discrete );
	mParameterManager.AddParameter( typeParam );
	
	// register packets
	mPacketDispatcher.RegisterPacket(bypassID.CString(), eAlgPortID_BypassIn);
	mPacketDispatcher.RegisterPacket(DemoGain_GainLeftID, eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Gains);
	mPacketDispatcher.RegisterPacket(DemoGain_GainRightID, eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Gains);

	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	SetChunk
// *******************************************************************************

AAX_Result DemoGain_Parameters::SetChunk ( AAX_CTypeID chunkID, const AAX_SPlugInChunk * chunk )
{
	AAX_Result err = AAX_SUCCESS;
	
	const AAX_CTypeID CONTROLS_CHUNK_ID = 'elck'; // Note: Must match definition in AAX_CEffectParameters
	if (chunkID != CONTROLS_CHUNK_ID) 
		return AAX_CEffectParameters::SetChunk(chunkID, chunk);

	mChunkParser.LoadChunk(chunk);
	
	AAX_IParameter* parameter;
	int32_t intValue;
	double doubleValue;
	
	// The order of some parameter updates matters for linked parameters, since
	// an update to one parameter can affect other parameters' characteristics
	// such as their taper.  In general, this is only applicable when loading
	// plug-in settings presets (chunks).  When a chunk is loaded, the following
	// order of operations must be used:
	//
	//	1) Read all chunk data
	//	2) Set new taper values
	//	3) Set linked control values
	//
	// In this example plug-in, the Type parameter affects the taper of the Gain
	// parameters.  Therefore, when a new chunk is set, the Type parameter's update
	// logic must be run before the gains are updated.

	// type selector
	err = GetParameter(DemoGain_TypeToggleID, &parameter);
	if (err == AAX_SUCCESS && mChunkParser.FindInt32(DemoGain_TypeToggleID, &intValue)) 
		parameter->SetValueWithInt32(intValue);
	else
		return AAX_ERROR_MALFORMED_CHUNK;
	
	// gain left
	err = GetParameter(DemoGain_GainLeftID, &parameter);
	if (err == AAX_SUCCESS && mChunkParser.FindDouble(DemoGain_GainLeftID, &doubleValue)) 
		parameter->SetValueWithDouble(doubleValue);
	else
		return AAX_ERROR_MALFORMED_CHUNK;	
	
	// gain right
	err = GetParameter(DemoGain_GainRightID, &parameter);
	if (err == AAX_SUCCESS && mChunkParser.FindDouble(DemoGain_GainRightID, &doubleValue)) 
		parameter->SetValueWithDouble(doubleValue);
	else
		return AAX_ERROR_MALFORMED_CHUNK;	
	
	// link toggle
	err = GetParameter(DemoGain_LinkToggleID, &parameter);
	if (err == AAX_SUCCESS && mChunkParser.FindInt32(DemoGain_LinkToggleID, &intValue)) 
		parameter->SetValueWithBool(intValue != 0);
	else
		return AAX_ERROR_MALFORMED_CHUNK;
	
	// Note: Master Bypass is not included in standard chunk by default
	
	return err;
}

// *******************************************************************************
// METHOD:	UpdateParameterNormalizedValue
// *******************************************************************************
AAX_Result DemoGain_Parameters::UpdateParameterNormalizedValue ( AAX_CParamID inParameterID, double inValue, AAX_EUpdateSource inSource )
{
	AAX_Result	result = AAX_CEffectParameters::UpdateParameterNormalizedValue ( inParameterID, inValue, inSource );
	
	AAX_IParameter *	typeParameter = mParameterManager.GetParameterByID ( DemoGain_TypeToggleID );
	if ( typeParameter )
	{
		int32_t	type;
		if ( typeParameter->GetValueAsInt32( &type ) )
		{
			AAX_CParamID linkedControl = this->GetLinkedControl ( inParameterID );
			if ( linkedControl )
			{
				if ( this->IsParameterLinkReady ( inParameterID, inSource ) )
				{
					if ( type == eType_Pan )
						this->SetParameterNormalizedValue( linkedControl, (1.0 - inValue) );
					else if ( type == eType_Gain )
						this->SetParameterNormalizedValue( linkedControl, inValue );
				}
			}
			else 
			{
				if ( AAX::IsParameterIDEqual ( inParameterID, DemoGain_TypeToggleID ) )
				{
					AAX_CUnitDisplayDelegateDecorator<double>	panDisplay( AAX_CNumberDisplayDelegate<double,1>(), "" );
					AAX_CLinearTaperDelegate<double, 0>			panTaper ( 0.0, 1.0 );
					AAX_CUnitDisplayDelegateDecorator<double>	gainDisplay( AAX_CNumberDisplayDelegate<double,1,1>(), "dB" );
					AAX_CLinearTaperDelegate<double, 0>			gainTaper(-36.0, 0.0);
					
					// Default to gain setup
					AAX_IDisplayDelegateBase *					displayDelegate = &gainDisplay;
					AAX_ITaperDelegateBase *					taperDelegate = &gainTaper;
					double										leftValue = 1.0;
					double										rightValue = 1.0;
					
					if ( type == eType_Pan )
					{
						displayDelegate = &panDisplay;
						taperDelegate = &panTaper;
						leftValue = 0.0;
						rightValue = 1.0;
					}

					this->SetDisplayDelegate ( DemoGain_GainLeftID, *displayDelegate );
					this->SetDisplayDelegate ( DemoGain_GainRightID, *displayDelegate );
					this->SetTaperDelegate ( DemoGain_GainLeftID, *taperDelegate, false );
					this->SetTaperDelegate ( DemoGain_GainRightID, *taperDelegate, false );
					
					if ( this->IsParameterLinkReady ( inParameterID, inSource ) )
					{
						this->SetParameterNormalizedValue( DemoGain_GainLeftID, leftValue );
						this->SetParameterNormalizedValue( DemoGain_GainRightID, rightValue );
					}
				}
			}
		}
	}
	
	return result;
}
	
// *******************************************************************************
// METHOD:	UpdateParameterTouch
// *******************************************************************************
AAX_Result DemoGain_Parameters::UpdateParameterTouch ( AAX_CParamID inParameterID, AAX_CBoolean inTouchState )
{
	if ( inTouchState )
	{
		AAX_CParamID linkedControl = this->GetLinkedControl ( inParameterID );
		if ( linkedControl )
		{
			this->TouchParameter ( linkedControl );
			mLinkTouchMap.insert ( std::pair<std::string,std::string>( inParameterID, linkedControl ) );
		}
	}
	else
	{
		std::multimap<std::string,std::string>::iterator	iter = mLinkTouchMap.find ( inParameterID );
		if ( iter != mLinkTouchMap.end () )
		{
			this->ReleaseParameter ( iter->second.c_str () );
			mLinkTouchMap.erase ( iter );
		}
	}
	
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	GetLinkedControl
// *******************************************************************************
AAX_CParamID DemoGain_Parameters::GetLinkedControl ( AAX_CParamID inParameterID )
{
	AAX_IParameter *	linkParameter = mParameterManager.GetParameterByID ( DemoGain_LinkToggleID );
	if ( linkParameter )
	{
		bool	linked;
		if ( linkParameter->GetValueAsBool( &linked ) )
		{
			if ( linked )
			{
				if ( AAX::IsParameterIDEqual ( inParameterID, DemoGain_GainLeftID ) )
					return DemoGain_GainRightID;
				else if ( AAX::IsParameterIDEqual ( inParameterID, DemoGain_GainRightID ) )
					return DemoGain_GainLeftID;
			}
		}
	}
	
	return 0;
}

// *******************************************************************************
// METHOD:	SetGains
// *******************************************************************************
void DemoGain_Parameters::SetGains ( SDemoGain_CoefsGain * ioGains, bool inType, double inValue, DemoGain_ESide inSide )
{
	if ( inType == eType_Pan )
	{
		ioGains->mGainL[inSide] = static_cast<float>(1.0 - inValue);
		ioGains->mGainR[inSide] = static_cast<float>(inValue);
	}
	else
	{
		if ( inSide == eSide_Left )
		{
			ioGains->mGainL[inSide] = static_cast<float>(DBToGain(inValue));
			ioGains->mGainR[inSide] = 0.0;
		}
		else
		{
			ioGains->mGainL[inSide] = 0.0;
			ioGains->mGainR[inSide] = static_cast<float>(DBToGain(inValue));
		}
	}
	
}

// *******************************************************************************
// METHOD:	UpdatePacketGain
// *******************************************************************************
AAX_Result DemoGain_Parameters::UpdatePacket_Gains( AAX_CPacket& ioPacket )
{
	AAX_IParameter *	leftParameter = mParameterManager.GetParameterByID ( DemoGain_GainLeftID );
	AAX_IParameter *	rightParameter = mParameterManager.GetParameterByID ( DemoGain_GainRightID );
	AAX_IParameter *	typeParameter = mParameterManager.GetParameterByID ( DemoGain_TypeToggleID );
	
	if ( leftParameter && rightParameter && typeParameter )
	{
		SDemoGain_CoefsGain *	payload = ioPacket.GetPtr<SDemoGain_CoefsGain>();
		if ( payload )
		{
			double	value;
			int32_t	type;
			if ( ! typeParameter->GetValueAsInt32( &type ) )
				type = eType_Pan;

			if ( leftParameter->GetValueAsDouble( &value ) )
				SetGains ( payload, type, value, eSide_Left );
			
			if ( rightParameter->GetValueAsDouble( &value ) )
				SetGains ( payload, type, value, eSide_Right );
		}
	}
	
	return AAX_SUCCESS;
}
