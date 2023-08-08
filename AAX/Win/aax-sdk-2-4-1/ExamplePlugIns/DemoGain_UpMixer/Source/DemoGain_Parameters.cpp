/*================================================================================================*/
/*
 *	Copyright 2009-2017 by Avid Technology, Inc.
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
#include "AAX_CStateDisplayDelegate.h"
#include "AAX_CStateTaperDelegate.h"
#include "AAX_CString.h"

#include "AAX_StringUtilities.h"
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
	, mNumInputChannels(0)
    , mNumOutputChannels(0)
{
}

// *******************************************************************************
// METHOD:	EffectInit
// *******************************************************************************
AAX_Result DemoGain_Parameters::EffectInit()
{	
	AAX_EStemFormat stemFormatInput;
    AAX_EStemFormat stemFormatOutput;
    Controller()->GetInputStemFormat(&stemFormatInput);
    Controller()->GetOutputStemFormat(&stemFormatOutput);
    mNumInputChannels  = AAX_STEM_FORMAT_CHANNEL_COUNT(stemFormatInput);
    mNumOutputChannels = AAX_STEM_FORMAT_CHANNEL_COUNT(stemFormatOutput);
    
	//
	// Register parameters
	//
	
	// bypass
	{
		AAX_CString bypassID = cDefaultMasterBypassID;
		AAX_IParameter * masterBypass = new AAX_CParameter<bool>(
			bypassID.CString(), AAX_CString("Master Bypass"), false,
			AAX_CBinaryTaperDelegate<bool>(),
			AAX_CBinaryDisplayDelegate<bool>("bypass", "on"), true);
		masterBypass->SetNumberOfSteps( 2 );
		masterBypass->SetType( AAX_eParameterType_Discrete );
		mParameterManager.AddParameter(masterBypass);
		
		mPacketDispatcher.RegisterPacket(bypassID.CString(), eAlgPortID_BypassIn);
	}
    
	// gain
	{
		AAX_CParameter<float>* gainParameter = nullptr;
		for (uint16_t channelIndex = 0 ; channelIndex < mNumOutputChannels; ++channelIndex )
		{
			const char * parameterID = cDemoGain_GainIDs[channelIndex];
			const AAX_CString parameterName("Gain " + AAX::AsStringStemChannel(stemFormatOutput, channelIndex, true));
			
			gainParameter = new AAX_CParameter<float>(parameterID,
													  parameterName,
													  1.0f,
													  AAX_CLinearTaperDelegate<float>(0.0f, 1.0f),
													  AAX_CNumberDisplayDelegate<float>(), true);
			gainParameter->SetNumberOfSteps(55);
			
			mParameterManager.AddParameter(gainParameter);
			mPacketDispatcher.RegisterPacket(parameterID, eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Gain);
		}
	}
    
    // Available channels, construct vertor of strings
    std::vector<AAX_IString*> channelsStrings;
    channelsStrings.reserve(mNumInputChannels);
    for (uint16_t channelIndex = 0; channelIndex < mNumInputChannels; ++channelIndex)
        channelsStrings.push_back(new AAX_CString( AAX::AsStringStemChannel(stemFormatInput, channelIndex, true) ));
    
    AAX_CStateDisplayDelegate<int32_t> channelsDisplayDelegate(channelsStrings);
    
    // routing parameters
    {
        AAX_CParameter<int32_t>* routingParameter = nullptr;
        for (int32_t channelIndex = 0 ; channelIndex < mNumOutputChannels; ++channelIndex )
        {
            const char * parameterID = cDemoGain_RoutingIDs[channelIndex];
            const AAX_CString parameterName("Source " + AAX::AsStringStemChannel(stemFormatOutput, channelIndex, true));
            
            routingParameter = new AAX_CParameter<int32_t>(parameterID,
                                                           parameterName,
                                                           channelIndex % mNumInputChannels,
                                                           AAX_CStateTaperDelegate<int32_t>(0, mNumInputChannels - 1),
                                                           channelsDisplayDelegate, true);
            routingParameter->SetNumberOfSteps(mNumInputChannels);
            routingParameter->SetType(AAX_eParameterType_Discrete);
            
            mParameterManager.AddParameter(routingParameter);
            mPacketDispatcher.RegisterPacket(parameterID, eAlgPortID_RoutingMapIn, this, &DemoGain_Parameters::UpdatePacket_Routing);
        }
    }
    
    // Clean-up memory in vector, items are already copied inside display delegate
    for (auto & item : channelsStrings)
    {
        delete item;
    }

	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	UpdatePacketGain
// *******************************************************************************
AAX_Result DemoGain_Parameters::UpdatePacket_Gain( AAX_CPacket& ioPacket )
{
	AAX_Result result = AAX_SUCCESS;
	SDemoGain_CoefsGain* packet = ioPacket.GetPtr<SDemoGain_CoefsGain>();
	if (packet)
	{
		// Populate the packet
		for (uint16_t i = 0; i < mNumOutputChannels; ++i)
		{
			const char * parameterID = cDemoGain_GainIDs[i];
			AAX_IParameter* const parameter = mParameterManager.GetParameterByID(parameterID);
			AAX_ASSERT(parameter);
			
			float floatValue;
			if (parameter && parameter->GetValueAsFloat(&floatValue))
			{
				packet->mGain[i] = floatValue;
			}
			else
			{
				packet->mGain[i] = 0.f;
				result = AAX_ERROR_INVALID_PARAMETER_INDEX;
			}
		}
	}
	else
	{
		result = AAX_ERROR_NULL_OBJECT;
	}

	return result;
}

AAX_Result DemoGain_Parameters::UpdatePacket_Routing(AAX_CPacket &ioPacket)
{
    AAX_Result result = AAX_SUCCESS;
    SDemoGain_RoutingMap* packet = ioPacket.GetPtr<SDemoGain_RoutingMap>();
    if (packet)
    {
        // Populate the packet
        for (int32_t i = 0; i < mNumOutputChannels; ++i)
        {
            const char * parameterID = cDemoGain_RoutingIDs[i];
            AAX_IParameter* const parameter = mParameterManager.GetParameterByID(parameterID);
            AAX_ASSERT(parameter);
            
            int32_t intValue;
            if (parameter && parameter->GetValueAsInt32(&intValue))
            {
                packet->mRoutingMap[i] = intValue;
            }
            else
            {
                packet->mRoutingMap[i] = i;
                result = AAX_ERROR_INVALID_PARAMETER_INDEX;
            }
        }
    }
    else
    {
        result = AAX_ERROR_NULL_OBJECT;
    }
    return result;
}
