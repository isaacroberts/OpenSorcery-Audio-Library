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
#include "AAX_UtilsNative.h"

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
	AAX_CEffectParameters(),
	mDirtyBypassFlag(false),
	mDirtyMixFlag(false),
	mDirtyFiltFlag(false),
	mDirtyDistFlag(false),
	mDirtyInpGainFlag(false),
	mDirtyOutGainFlag(false)
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
// METHOD:	UpdateParameterNormalizedValue
// *******************************************************************************
AAX_Result DemoDist_Parameters::UpdateParameterNormalizedValue(AAX_CParamID iParamID, double aValue, AAX_EUpdateSource inSource)
{
	AAX_Result	result = AAX_SUCCESS;
	
	AAX_IParameter*	 parameter = mParameterManager.GetParameterByID(iParamID);
	
	if (parameter==0)
		return AAX_ERROR_INVALID_PARAMETER_ID;
	
	parameter->UpdateNormalizedValue(aValue);
	
	// Check for Control state changes
	if (AAX::IsParameterIDEqual(iParamID, cDefaultMasterBypassID))
	{
		mDirtyBypassFlag = true;
	}
	else if (AAX::IsParameterIDEqual(iParamID, DemoDist_MixID))	
	{
		mDirtyMixFlag = true;
	}
	else if (AAX::IsParameterIDEqual(iParamID, DemoDist_FiltID))	
	{
		mDirtyFiltFlag = true;
	}
	else if (AAX::IsParameterIDEqual(iParamID, DemoDist_DistID))	
	{
		mDirtyDistFlag = true;
	}
	else if (AAX::IsParameterIDEqual(iParamID, DemoDist_InpGID))	
	{
		mDirtyInpGainFlag = true;
	}
	else if (AAX::IsParameterIDEqual(iParamID, DemoDist_OutGID))	
	{
		mDirtyOutGainFlag = true;
	}
	
	// Let clients know that a change has occurred
	mNumPlugInChanges++;
	
	return result;
}

// *******************************************************************************
// METHOD:	GenerateCoefficients
// *******************************************************************************
AAX_Result DemoDist_Parameters::GenerateCoefficients()
{
	AAX_Result	result = AAX_SUCCESS;
	
	//Post Packets and reset "dirty" flags
	if (mDirtyBypassFlag == true)
	{
		AAX_IParameter* parameter = mParameterManager.GetParameterByID(cDefaultMasterBypassID);
		if (parameter)
		{
			bool theBypassValue;
			if (parameter->GetValueAsBool(&theBypassValue))
			{
				int32_t theNewBypassValue = int32_t(theBypassValue);
				result = Controller()->PostPacket(eAlgPortID_BypassIn, &theNewBypassValue, sizeof(theNewBypassValue));
				if (result == AAX_SUCCESS)
					mDirtyBypassFlag = false;
				else 
					return AAX_ERROR_NULL_OBJECT;
			}
		}
	}
	
	if (mDirtyMixFlag == true)
	{
		AAX_IParameter*	 parameter = mParameterManager.GetParameterByID(DemoDist_MixID);
		if (parameter)
		{
			float theMixValue;
			if (parameter->GetValueAsFloat(&theMixValue))
			{
				result = Controller()->PostPacket(eAlgPortID_MixIn, &theMixValue, sizeof(theMixValue));
				if (result == AAX_SUCCESS)
					mDirtyMixFlag = false;
				else 
					return AAX_ERROR_NULL_OBJECT;
			}
		}
	}
	
	if (mDirtyDistFlag == true)
	{		
		AAX_IParameter*	 parameter = mParameterManager.GetParameterByID(DemoDist_DistID);
		if (parameter)
		{
			float theDistValue;
			if (parameter->GetValueAsFloat(&theDistValue))
			{
				SDemoDist_DistCoefs	distCoefs;
				
				static const float def_PI = 3.14159265358979323846264f;
				distCoefs.mTransWarp = def_PI * (cDistortionPower * theDistValue + 0.5f);
				distCoefs.mCutoff = 1.0f / (2.0f * cDistortionPower * theDistValue + 1.0f);;
				
				result = Controller()->PostPacket(eAlgPortID_CoefsDistIn, &distCoefs, sizeof(distCoefs));
				if (result == AAX_SUCCESS)
					mDirtyDistFlag = false;
				else
					return AAX_ERROR_NULL_OBJECT;
			}
		}
	}
	
	if (mDirtyFiltFlag == true)
	{
		AAX_IParameter*	 parameter = mParameterManager.GetParameterByID(DemoDist_FiltID);
		if (parameter)
		{
			float theFiltValue;
			if (parameter->GetValueAsFloat(&theFiltValue))
			{
				SDemoDist_FiltCoefs filtCoefs;
				const float adjFiltCtrlVal = theFiltValue * 0.45f;
				
				//
				// Set filter's mix
				filtCoefs.mFilterMix = (theFiltValue == 0.0f) ? 0.0f : pow(theFiltValue, 0.2f); 
				
				//
				// Process new filter coefficients for use in the Algorithm component's own CSimpleBiquad
				CSimpleBiquad biquad;
				biquad.SetCoeffsForLPFUsingCutoff(adjFiltCtrlVal);
				biquad.GetCoeffs(
					&filtCoefs.mCoeffs[CSimpleBiquad::eA0],
					&filtCoefs.mCoeffs[CSimpleBiquad::eA1],
					&filtCoefs.mCoeffs[CSimpleBiquad::eA2],
					&filtCoefs.mCoeffs[CSimpleBiquad::eB0],
					&filtCoefs.mCoeffs[CSimpleBiquad::eB1],
					&filtCoefs.mCoeffs[CSimpleBiquad::eB2] );
				
				result = Controller()->PostPacket(eAlgPortID_CoefsFilterIn, &filtCoefs, sizeof(filtCoefs));
				if (result == AAX_SUCCESS)
					mDirtyFiltFlag = false;
				else
					return AAX_ERROR_NULL_OBJECT;
			}
		}
	}
	
	if (mDirtyInpGainFlag == true)
	{
		AAX_IParameter*	 parameter = mParameterManager.GetParameterByID(DemoDist_InpGID);
		if (parameter)
		{
			float theInpGainValue;
			if (parameter->GetValueAsFloat(&theInpGainValue))
			{
				result = Controller()->PostPacket(eAlgPortID_InpGIn, &theInpGainValue, sizeof(theInpGainValue));
				if (result == AAX_SUCCESS)
					mDirtyInpGainFlag = false;
				else
					return AAX_ERROR_NULL_OBJECT;
			}
		}
	}
	
	if (mDirtyOutGainFlag == true)
	{
		AAX_IParameter*	 parameter = mParameterManager.GetParameterByID(DemoDist_OutGID);
		if(parameter)
		{
			float theOutGainValue;
			if (parameter->GetValueAsFloat(&theOutGainValue))
			{
				result = Controller()->PostPacket(eAlgPortID_OutGIn, &theOutGainValue, sizeof(theOutGainValue));
				if (result == AAX_SUCCESS)
					mDirtyOutGainFlag = false;
				else
					return AAX_ERROR_NULL_OBJECT;
			}
		}
	}
	
	return result;
}
