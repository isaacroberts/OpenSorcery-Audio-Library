/*================================================================================================*/
/*
 *	Copyright 2009-2015, 2018 by Avid Technology, Inc.
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

// Self Include
#include "DemoGain_Parameters.h"

// DemoGain Includes
#include "DemoGain_Defs.h"
#include "DemoGain_Alg.h"

// AAX Includes
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_Assert.h"

// Standard Includes
#include <memory>



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
	{
		AAX_CString id = cDefaultMasterBypassID;
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<bool>(
			id, AAX_CString("Master Bypass"), false,
			AAX_CBinaryTaperDelegate<bool>(),
			AAX_CBinaryDisplayDelegate<bool>("bypass", "on"), true));
		param->SetNumberOfSteps( 2 );
		param->SetType( AAX_eParameterType_Discrete );
		mParameterManager.AddParameter(param.release());
		mPacketDispatcher.RegisterPacket(id.CString(), eAlgPortID_BypassIn);
	}
	
	// gain
	{
		AAX_CString id = DemoGain_GainID;
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
			id, AAX_CString("Gain"), 1.0f,
			AAX_CLinearTaperDelegate<float>(0.0f, 1.0f),
			AAX_CNumberDisplayDelegate<float>(), true));
		param->SetNumberOfSteps(55);
		param->SetType( AAX_eParameterType_Continuous );
		mParameterManager.AddParameter(param.release());
		mPacketDispatcher.RegisterPacket(id.CString(), eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Gain);
	}
	
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
