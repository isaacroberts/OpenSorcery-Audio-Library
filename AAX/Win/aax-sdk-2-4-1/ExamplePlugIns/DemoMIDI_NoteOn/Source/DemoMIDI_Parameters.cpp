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

// DemoMIDI Includes
#include "DemoMIDI_Parameters.h"
#include "DemoMIDI_Defs.h"
#include "DemoMIDI_Alg.h"

// AAX Includes
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_MIDIUtilities.h"
#include "AAX_Assert.h"

// Standard Includes
#include <memory>

// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_CEffectParameters *AAX_CALLBACK DemoMIDI_Parameters::Create()
{
	return new DemoMIDI_Parameters();
}

// *******************************************************************************
// METHOD:	DemoMIDI_Parameters
// *******************************************************************************
DemoMIDI_Parameters::DemoMIDI_Parameters () :
AAX_CEffectParameters()
{
}

// *******************************************************************************
// METHOD:	EffectInit
// *******************************************************************************
AAX_Result DemoMIDI_Parameters::EffectInit()
{	
	//Add parameters
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<bool>(
			cDefaultMasterBypassID, AAX_CString("Master Bypass"), false,
			AAX_CBinaryTaperDelegate<bool>(),
			AAX_CBinaryDisplayDelegate<bool>("bypass", "on"), true));
		param->SetNumberOfSteps( 2 );
		param->SetType( AAX_eParameterType_Discrete );
		mParameterManager.AddParameter(param.release());
	}
	
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<bool>(
			DemoMIDI_HoldID, AAX_CString("Hold"), false,
			AAX_CBinaryTaperDelegate<bool>(),
			AAX_CBinaryDisplayDelegate<bool>("off", "on"), true));
		param->SetNumberOfSteps( 2 );
		param->SetType( AAX_eParameterType_Discrete );
		mParameterManager.AddParameter(param.release());
	}
	
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
			DemoMIDI_GainID, AAX_CString("Gain"), 0.5f,
			AAX_CLinearTaperDelegate<float>(0.0f, 1.0f),
			AAX_CNumberDisplayDelegate<float>(), true));
		param->SetNumberOfSteps(55);
		mParameterManager.AddParameter(param.release());
	}
	
	// register packets
	mPacketDispatcher.RegisterPacket(cDefaultMasterBypassID, eAlgPortID_BypassIn);
	mPacketDispatcher.RegisterPacket(DemoMIDI_GainID, eAlgPortID_CoefsGainIn);
	mPacketDispatcher.RegisterPacket(DemoMIDI_HoldID, eAlgPortID_HoldIn);
	
	return AAX_SUCCESS;
}


// ***************************************************************************
// METHOD:	AAX_UpdateMIDINodes
// This will be called by the host if there are MIDI packets that need
// to be handled in the Data Model.
// ***************************************************************************
AAX_Result DemoMIDI_Parameters::UpdateMIDINodes ( AAX_CFieldIndex inFieldIndex,	AAX_CMidiPacket& inPacket )
{
	AAX_Result result;
	
	// Do some MIDI work if necessary. There is no work to be
	// done in this simple example plug-in.
	if (AAX::IsAllNotesOff(&inPacket))
	{
		// do all note off stuff...
	}
	else if (AAX::IsNoteOff(&inPacket))
	{
		// do note off stuff...
	}
	else if (AAX::IsNoteOff(&inPacket))
	{
		// do note on stuff...
	}
	
 	result = AAX_SUCCESS;
	
	return result;
}




