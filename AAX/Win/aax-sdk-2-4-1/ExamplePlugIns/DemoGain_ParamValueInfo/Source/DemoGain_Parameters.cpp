/*================================================================================================*/
/*
 *	Copyright 2012-2016, 2018 by Avid Technology, Inc.
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
#include "AAX_CLogTaperDelegate.h"

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
	
	// gain
	AAX_CParameter<float>* gainParam = new AAX_CParameter<float>(
		DemoGain_GainID, AAX_CString("Gain"), 1.0f,
		AAX_CLinearTaperDelegate<float>(0.0f, 1.0f),
		AAX_CNumberDisplayDelegate<float>(), true);
	gainParam->SetNumberOfSteps(55);
	mParameterManager.AddParameter(gainParam);

	// EQ Filter type
	const char * kFilterTypeStrings[] = { "High-Pass", "Low-Shelf", "Peak", "High-Shelf", "Low-Pass", 0 };
	AAX_CStateDisplayDelegate<int32_t> filterTypeDisplayDelegate(kFilterTypeStrings);

	AAX_CParameter<int32_t>* filterTypeParam = new AAX_CParameter<int32_t>(DemoGain_FilterTypeID, AAX_CString(DemoGain_FilterTypeID), eFilterType_HighPass,
		AAX_CStateTaperDelegate<int32_t>(eFilterType_HighPass, eFilterType_LowPass), filterTypeDisplayDelegate, true);
	filterTypeParam->SetNumberOfSteps( eFilterType_Count );
	filterTypeParam->SetType ( AAX_eParameterType_Discrete );
	mParameterManager.AddParameter( filterTypeParam );

	// EQ status
	const char* csStatus[] = { "Disabled", "Bypassed", "Enabled", 0 };
	AAX_CStateDisplayDelegate<int32_t> statusDisplayDelegate( csStatus, 0 );

	AAX_CParameter<int32_t>* lowPassStatusParam = new AAX_CParameter<int32_t>( DemoGain_LowPassStatus, AAX_CString( DemoGain_LowPassStatus ), eEQStatus_Enabled, 
		AAX_CStateTaperDelegate<int32_t>(eEQStatus_Disabled, eEQStatus_Enabled), statusDisplayDelegate, true);
	lowPassStatusParam->SetNumberOfSteps( eEQStatus_Count );
	lowPassStatusParam->SetType( AAX_eParameterType_Discrete );
	mParameterManager.AddParameter( lowPassStatusParam );

	// Filter Q
	AAX_CParameter<double>* filterQ = new AAX_CParameter<double>( DemoGain_Q, AAX_CString(DemoGain_Q), 1.0,
		AAX_CLogTaperDelegate<double, 0>( 0.1, 10.0 ), AAX_CNumberDisplayDelegate<double>(), true);
	filterQ->SetNumberOfSteps ( 144 );
	filterQ->SetOrientation( AAX_eParameterOrientation_RotarySpreadMode | AAX_eParameterOrientation_RotaryRightMinLeftMax );
	mParameterManager.AddParameter( filterQ );

	// Filter slope
	const char * kSlopeStrings[] = { "6 dB/oct", "12 dB/oct", "18 dB/oct", "24 dB/oct", 0 };
	AAX_CStateDisplayDelegate<int32_t> slopeDisplayDelegate( kSlopeStrings, 0 );

	AAX_CParameter<int32_t>* filterSlopeParam = new AAX_CParameter<int32_t>( DemoGain_Slope, AAX_CString(DemoGain_Slope), eFilterSlope_12dBperOct,
		AAX_CStateTaperDelegate<int32_t>(eFilterSlope_6dBperOct, eFilterSlope_24dBperOct), slopeDisplayDelegate, true);
	filterSlopeParam->SetNumberOfSteps( eFilterSlope_Count );
	filterSlopeParam->SetType( AAX_eParameterType_Discrete );
	mParameterManager.AddParameter( filterSlopeParam );

	// register packets
	mPacketDispatcher.RegisterPacket(bypassID.CString(), eAlgPortID_BypassIn);
	mPacketDispatcher.RegisterPacket(DemoGain_GainID, eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Gain);
	mPacketDispatcher.RegisterPacket(DemoGain_FilterTypeID, eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Gain);
	mPacketDispatcher.RegisterPacket(DemoGain_LowPassStatus, eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Gain);
	mPacketDispatcher.RegisterPacket(DemoGain_Slope, eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Gain);
	mPacketDispatcher.RegisterPacket(DemoGain_Q, eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Gain);

	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	UpdatePacketGain
// *******************************************************************************
AAX_Result DemoGain_Parameters::UpdatePacket_Gain( AAX_CPacket& ioPacket )
{
	AAX_IParameter*	 parameter = mParameterManager.GetParameterByID( DemoGain_GainID );
	if (parameter)
	{
		float floatValue;
		if (parameter->GetValueAsFloat(&floatValue))
		{
			SDemoGain_CoefsGain&  packet = *ioPacket.GetPtr<SDemoGain_CoefsGain>();
			
			// Populate packet
			packet.mGain = floatValue;
		}
	}
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	GetParameterValueInfo
// *******************************************************************************
AAX_Result DemoGain_Parameters::GetParameterValueInfo( AAX_CParamID iParameterID, int32_t iSelector, int32_t* oValue ) const
{
	const AAX_IParameter * parameter = mParameterManager.GetParameterByID( iParameterID );

	if ( !parameter )
		return AAX_ERROR_INVALID_PARAMETER_ID;

	if ( iSelector == AAX_ePageTable_EQ_Band_Type )
	{
		if ( parameter->Name() == DemoGain_FilterTypeID )
		{
			switch( parameter->GetStepValue() )
			{
				case eFilterType_HighPass:		*oValue = AAX_eEQBandType_HighPass; break;
				case eFilterType_LowShelf:		*oValue = AAX_eEQBandType_LowShelf; break;
				case eFilterType_Bell:			*oValue = AAX_eEQBandType_Parametric; break;
				case eFilterType_HighShelf:		*oValue = AAX_eEQBandType_HighShelf; break;
				case eFilterType_LowPass:		*oValue = AAX_eEQBandType_LowPass; break;
				default:						*oValue = -1; return AAX_ERROR_UNIMPLEMENTED; break;
			}
			return AAX_SUCCESS;
		}
	}
	else if ( iSelector == AAX_ePageTable_EQ_InCircuitPolarity )
	{
		if ( parameter->Name() == DemoGain_HighPassStatus || 
			parameter->Name() == DemoGain_LowPassStatus || 
			parameter->Name() == DemoGain_HighShelfStatus || 
			parameter->Name() == DemoGain_LowShelfStatus )
		{
			switch( parameter->GetStepValue() )
			{
				case eEQStatus_Enabled:			*oValue = AAX_eEQInCircuitPolarity_Enabled; break;
				case eEQStatus_Bypassed:		*oValue = AAX_eEQInCircuitPolarity_Bypassed; break;
				case eEQStatus_Disabled:		*oValue = AAX_eEQInCircuitPolarity_Disabled; break;
				default:						*oValue = -1; return AAX_ERROR_UNIMPLEMENTED; break;
			}
			return AAX_SUCCESS;
		}
	}
	else if (iSelector == AAX_ePageTable_UseAlternateControl)
	{
		if ( parameter->Name() == DemoGain_FilterTypeID || 
			parameter->Name() == DemoGain_Q || 
			parameter->Name() == DemoGain_Slope )
		{
			*oValue = (parameter->GetStepValue() == eFilterType_HighPass ) || (parameter->GetStepValue() == eFilterType_LowPass )
				? AAX_eUseAlternateControl_Yes : AAX_eUseAlternateControl_No;
			return AAX_SUCCESS;
		}
	}

	return AAX_ERROR_UNIMPLEMENTED;
}

