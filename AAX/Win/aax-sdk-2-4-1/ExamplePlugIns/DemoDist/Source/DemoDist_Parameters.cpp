/*================================================================================================*/
/*
 *	Copyright 2009-2015, 2017 by Avid Technology, Inc.
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
#include "DemoDist_Parameters.h"

// DemoDist Includes
#include "DemoDistDefs.h"
#include "DemoDist_Alg.h"

// AAX Includes
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CLogTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CDecibelDisplayDelegateDecorator.h"
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"

// Standard Includes
#include <memory>
#include <new> // for placement new

// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_CEffectParameters * AAX_CALLBACK DemoDist_Parameters::Create ()
{
	return new DemoDist_Parameters();
}

// *******************************************************************************
// METHOD:	DemoDist_Parameters
// *******************************************************************************
DemoDist_Parameters::DemoDist_Parameters () :
	AAX_CEffectParameters()
{
}

// *******************************************************************************
// METHOD:	EffectInit
// *******************************************************************************
AAX_Result DemoDist_Parameters::EffectInit()
{	
	// bypass
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<bool>(
			cDefaultMasterBypassID, AAX_CString("Master Bypass"), false,
			AAX_CBinaryTaperDelegate<bool>(),
			AAX_CBinaryDisplayDelegate<bool>("bypass", "on"), true));
		param->SetNumberOfSteps( 2 );
		param->SetType( AAX_eParameterType_Discrete );
		mParameterManager.AddParameter(param.release());
	}
	
	// mix
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
			DemoDist_MixID, AAX_CString("Mix"), cDefaultParameterMix,
			AAX_CLinearTaperDelegate<float>(0.0f, 1.0f),
			AAX_CNumberDisplayDelegate<float>(), true));
		param->SetNumberOfSteps(55);
		mParameterManager.AddParameter(param.release());
	}
	
	// filter
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
			DemoDist_FiltID, AAX_CString("Lowpass"), cDefaultParameterFilt,
			AAX_CLinearTaperDelegate<float>(0.0f, 1.0f),
			AAX_CNumberDisplayDelegate<float>(), true));
		param->SetNumberOfSteps(55);
		mParameterManager.AddParameter(param.release());
	}
	
	// dist
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
			DemoDist_DistID, AAX_CString("Distortion"), cDefaultParameterDist,
			AAX_CLinearTaperDelegate<float>(0.0f, 2.0f),
			AAX_CNumberDisplayDelegate<float>(), true));
		param->SetNumberOfSteps(55);
		mParameterManager.AddParameter(param.release());
	}
	
	// input gain
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
			DemoDist_InpGID, AAX_CString("Input Gain"), cDefaultParameterInpG,
			AAX_CLogTaperDelegate<float>(0.01f, 10.0f),
			AAX_CDecibelDisplayDelegateDecorator<float>(AAX_CNumberDisplayDelegate<float>()), true));
		param->SetNumberOfSteps(55);
		mParameterManager.AddParameter(param.release());
	}

	// output gain
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
			DemoDist_OutGID, AAX_CString("Output Gain"), cDefaultParameterOutG,
			AAX_CLogTaperDelegate<float>(0.002f, 2.0f),
			AAX_CDecibelDisplayDelegateDecorator<float>(AAX_CNumberDisplayDelegate<float>()), true));
		param->SetNumberOfSteps(55);
		mParameterManager.AddParameter(param.release());
	}
	
	// register packets
	mPacketDispatcher.RegisterPacket(DemoDist_MixID, eAlgPortID_MixIn);
	mPacketDispatcher.RegisterPacket(DemoDist_InpGID, eAlgPortID_InpGIn);
	mPacketDispatcher.RegisterPacket(DemoDist_OutGID, eAlgPortID_OutGIn);
	mPacketDispatcher.RegisterPacket(DemoDist_FiltID, eAlgPortID_CoefsFilterIn, this, &DemoDist_Parameters::UpdatePacket_Filter);
	mPacketDispatcher.RegisterPacket(DemoDist_DistID, eAlgPortID_CoefsDistIn, this, &DemoDist_Parameters::UpdatePacket_Distort);
	mPacketDispatcher.RegisterPacket(cDefaultMasterBypassID, eAlgPortID_BypassIn);
	
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	ResetFieldData
// *******************************************************************************
AAX_Result DemoDist_Parameters::ResetFieldData (AAX_CFieldIndex inFieldIndex, void * oData, uint32_t iDataSize) const
{	
	AAX_Result result = AAX_ERROR_INVALID_FIELD_INDEX;
	
	if (inFieldIndex == eAlgFieldID_Biquads)
	{
		CSimpleBiquad *		elementAddr = static_cast <CSimpleBiquad *> (oData);
		int					count = iDataSize / sizeof (CSimpleBiquad);
		const void *		end = elementAddr + count;

		while (elementAddr < end)
		{
			new (elementAddr) CSimpleBiquad; // Placement new into pre-allocated memory
			elementAddr++;
		}
		
		result = AAX_SUCCESS;
	}
	
	return result;
}
	
// *******************************************************************************
// METHOD:	UpdatePacketDistort
// *******************************************************************************
AAX_Result DemoDist_Parameters::UpdatePacket_Distort( AAX_CPacket& ioPacket )
{
	AAX_IParameter*	 parameter = mParameterManager.GetParameterByID( DemoDist_DistID );
	if (parameter)
	{
		float floatValue;
		if (parameter->GetValueAsFloat(&floatValue))
		{
			SDemoDist_DistCoefs&  packet = *ioPacket.GetPtr<SDemoDist_DistCoefs>();
			
			// Populate packet
			//
			static const float def_PI = 3.14159265358979323846264f;
			packet.mTransWarp = def_PI * (cDistortionPower * floatValue + 0.5f);
			packet.mCutoff = 1.0f / (2.0f * cDistortionPower * floatValue + 1.0f);
		}
	}
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	UpdatePacketFilter
// *******************************************************************************
AAX_Result DemoDist_Parameters::UpdatePacket_Filter( AAX_CPacket& ioPacket )
{
	AAX_IParameter*	 parameter = mParameterManager.GetParameterByID( DemoDist_FiltID );
	if (parameter)
	{
		float floatValue;
		if (parameter->GetValueAsFloat(&floatValue))
		{
			SDemoDist_FiltCoefs& packet = *ioPacket.GetPtr<SDemoDist_FiltCoefs>();
		
			const float adjFiltCtrlVal = floatValue * 0.45f;
			
			// Populate packet
			//
			// Set filter's mix
			packet.mFilterMix = (floatValue == 0.0f) ? 0.0f : pow(floatValue, 0.2f);
			//
			// Process new filter coefficients for use in the Algorithm component's own CSimpleBiquad
			CSimpleBiquad biquad;
			biquad.SetCoeffsForLPFUsingCutoff(adjFiltCtrlVal);
			biquad.GetCoeffs(
				&packet.mCoeffs[CSimpleBiquad::eA0],
				&packet.mCoeffs[CSimpleBiquad::eA1],
				&packet.mCoeffs[CSimpleBiquad::eA2],
				&packet.mCoeffs[CSimpleBiquad::eB0],
				&packet.mCoeffs[CSimpleBiquad::eB1],
				&packet.mCoeffs[CSimpleBiquad::eB2] );
		}
	}
	return AAX_SUCCESS;
}
