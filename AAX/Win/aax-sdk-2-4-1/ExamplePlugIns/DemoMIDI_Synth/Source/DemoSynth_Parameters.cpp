/*================================================================================================*/
/*
 *	Copyright 2012-2015 by Avid Technology, Inc.
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

// DemoSynth Includes
#include "DemoSynth_Parameters.h"
#include "DemoSynth_Defs.h"
#include "CSimpleSynth.h"

// AAX Includes
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CStateTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CStateDisplayDelegate.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"
#include "AAX_CDecibelDisplayDelegateDecorator.h"
#include "AAX_UtilsNative.h"
#include "AAX_MIDIUtilities.h"

// Standard Includes
#include <utility>


static const AAX_CParamID sBypassParameterID = cDefaultMasterBypassID;
static const AAX_CParamID sTypeParameterID = "Type";
static const AAX_CParamID sGainParameterID = "Gain";
static const AAX_CParamID sTuningParameterID = "Tuning";


//================================================================================
// DemoSynth_Parameters

// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_CEffectParameters *AAX_CALLBACK DemoSynth_Parameters::Create()
{
	return new DemoSynth_Parameters();
}

// *******************************************************************************
// DemoSynth_Parameters
// *******************************************************************************
DemoSynth_Parameters::DemoSynth_Parameters ()
: AAX_CMonolithicParameters()
, mSynthesizer(new CSimpleSynth())
{
}

DemoSynth_Parameters::~DemoSynth_Parameters ()
{
	delete mSynthesizer;
}

AAX_Result DemoSynth_Parameters::EffectInit()
{
	static const int32_t sNumWaveTypes = CSimpleSynth::eWaveType_End - CSimpleSynth::eWaveType_Begin;
	static const char * sTypeStateStrings[sNumWaveTypes + 1] = {
		CSimpleSynth::GetWaveTypeName(CSimpleSynth::eWaveType_Saw),
		CSimpleSynth::GetWaveTypeName(CSimpleSynth::eWaveType_Tri),
		CSimpleSynth::GetWaveTypeName(CSimpleSynth::eWaveType_Square),
		NULL
	};
	
	// Create parameters
	{
		AAX_IParameter* param = new AAX_CParameter<bool>(
			sBypassParameterID, AAX_CString("Master Bypass"), false,
			AAX_CBinaryTaperDelegate<bool>(),
			AAX_CBinaryDisplayDelegate<bool>("bypass", "on"),
			true);
		param->SetNumberOfSteps( 2 );
		param->SetType( AAX_eParameterType_Discrete );
		mParameterManager.AddParameter(param);
		AddSynchronizedParameter(*param);
	}
	{
		AAX_IParameter* param = new AAX_CParameter<int32_t>(
			 sTypeParameterID, AAX_CString("Wave Type"), CSimpleSynth::eWaveType_Begin,
			 AAX_CStateTaperDelegate<int32_t>(CSimpleSynth::eWaveType_Begin, CSimpleSynth::eWaveType_End-1),
			 AAX_CStateDisplayDelegate<int32_t>(sTypeStateStrings, CSimpleSynth::eWaveType_Begin),
			 true);
		param->SetNumberOfSteps( sNumWaveTypes );
		param->SetType( AAX_eParameterType_Discrete );
		mParameterManager.AddParameter(param);
		AddSynchronizedParameter(*param);
	}
	{
		AAX_IParameter* param = new AAX_CParameter<float>(
			sGainParameterID, AAX_CString("Level"), 0.5f,
			AAX_CLinearTaperDelegate<float>(0.f, 1.f),
			AAX_CDecibelDisplayDelegateDecorator<float>(AAX_CNumberDisplayDelegate<float, 1, 1>()),
			true);
		param->SetNumberOfSteps( 512 );
		param->SetType( AAX_eParameterType_Continuous );
		mParameterManager.AddParameter(param);
		AddSynchronizedParameter(*param);
	}
	{
		AAX_IParameter* param = new AAX_CParameter<float>(
			sTuningParameterID, AAX_CString("Tuning"), 440.f,
			AAX_CLinearTaperDelegate<float>(440.f - 3.f, 440.f + 3.f),
			AAX_CUnitDisplayDelegateDecorator<float>(AAX_CNumberDisplayDelegate<float, 2, 1>(), "Hz"),
			false); // Non-automatable
		param->SetNumberOfSteps( 256 );
		param->SetType( AAX_eParameterType_Continuous );
		mParameterManager.AddParameter(param);
		
		// The tuning parameter is non-automatable, and updating
		// this parameter is too CPU-intensive to perform in the
		// real-time thread, so we do not add it to the
		// synchronization list here.
		// See UpdateParameterNormalizedValue()
	}
	
	// Initialize state
	InitializeSynthesizerState(*mSynthesizer);
	
	return AAX_SUCCESS;
}

AAX_Result DemoSynth_Parameters::UpdateParameterNormalizedValue(AAX_CParamID iParamID, double aValue, AAX_EUpdateSource inSource)
{
	// Call inherited
	AAX_Result result = AAX_CMonolithicParameters::UpdateParameterNormalizedValue(iParamID, aValue, inSource);
	if (AAX_SUCCESS != result) { return result; }
	
	const AAX_IParameter* const param = mParameterManager.GetParameterByID(iParamID);
	
	// The tuning parameter is not synchronized, so we update
	// it directly here
	if (AAX::IsParameterIDEqual(sTuningParameterID, iParamID))
	{
		float tuning = 440.f;
		param->GetValueAsFloat(&tuning);
		mSynthesizer->SetTuning(tuning);
	}
	
	return result;
}

void DemoSynth_Parameters::RenderAudio(AAX_SInstrumentRenderInfo* ioRenderInfo, const TParamValPair* inSynchronizedParamValues[], int32_t inNumSynchronizedParamValues)
{
	// Update the synthesizer coefficients
	UpdateCoefficients(inSynchronizedParamValues, inNumSynchronizedParamValues);
	
	// Setup MIDI node pointers
	AAX_IMIDINode* const midiIn = ioRenderInfo->mInputNode;
	AAX_CMidiStream* const midiBuffer = midiIn->GetNodeBuffer();
	const AAX_CMidiPacket* curMidiPacket = midiBuffer->mBuffer;
	uint32_t packets_remaining = midiBuffer->mBufferSize;
	
	const int32_t numSamples = *(ioRenderInfo->mNumSamples);
	AAX_EStemFormat outputStemFormat = AAX_eStemFormat_None;
    Controller()->GetOutputStemFormat(&outputStemFormat);
	const int32_t numOutputChannels = AAX_STEM_FORMAT_CHANNEL_COUNT(outputStemFormat);
	float * const AAX_RESTRICT outBuf = ioRenderInfo->mAudioOutputs[0];
		
	// Run processing loop over each sample
	for (int32_t t = 0; t < numSamples; t++)
	{
		ProcessInputMIDI(*mSynthesizer, t, curMidiPacket, packets_remaining);
		mSynthesizer->GetSamples(&outBuf[t], 1);
	}
	
	// Copy first channel output to other output channels
	for (int32_t ch = 1; ch < numOutputChannels; ch++)
	{
		memcpy(ioRenderInfo->mAudioOutputs[ch], outBuf, numSamples);
	}
}

void DemoSynth_Parameters::InitializeSynthesizerState(CSimpleSynth& ioSynth)
{
	{
		AAX_IParameter* param = mParameterManager.GetParameterByID(sBypassParameterID);
		bool bypass = false;
		param->GetValueAsBool(&bypass);
		ioSynth.SetBypass(bypass);
	}
	{
		AAX_IParameter* param = mParameterManager.GetParameterByID(sTypeParameterID);
		CSimpleSynth::EWaveType waveType = CSimpleSynth::eWaveType_Saw;
		param->GetValueAsInt32((int32_t*)&waveType);
		ioSynth.SetWaveType(waveType);
	}
	{
		AAX_IParameter* param = mParameterManager.GetParameterByID(sGainParameterID);
		float gain = 1.f;
		param->GetValueAsFloat(&gain);
		ioSynth.SetGain(gain);
	}
	{
		AAX_IParameter* param = mParameterManager.GetParameterByID(sTuningParameterID);
		float tuning = 440.f;
		param->GetValueAsFloat(&tuning);
		AAX_CSampleRate sampleRate = 0;
		Controller()->GetSampleRate (&sampleRate);
		ioSynth.InitializeVoices(sampleRate, tuning);
	}
}

void DemoSynth_Parameters::UpdateCoefficients(const TParamValPair* inSynchronizedParamValues[], int32_t inNumSynchronizedParamValues)
{
	for (int32_t i = 0; i < inNumSynchronizedParamValues; ++i)
	{
		AAX_CParamID const curParamID = inSynchronizedParamValues[i]->first;
		const AAX_IParameterValue* const curParamVal = inSynchronizedParamValues[i]->second;
		
		if (AAX::IsParameterIDEqual(curParamID, sBypassParameterID))
		{
			bool bypass = false;
			curParamVal->GetValueAsBool(&bypass);
			mSynthesizer->SetBypass(bypass);
		}
		
		if (AAX::IsParameterIDEqual(curParamID, sTypeParameterID))
		{
			CSimpleSynth::EWaveType waveType = CSimpleSynth::eWaveType_Saw;
			curParamVal->GetValueAsInt32((int32_t*)&waveType);
			mSynthesizer->SetWaveType(waveType);
		}
		
		if (AAX::IsParameterIDEqual(curParamID, sGainParameterID))
		{
			float gain = 1.f;
			curParamVal->GetValueAsFloat(&gain);
			mSynthesizer->SetGain(gain);
		}
	}
}

void DemoSynth_Parameters::ProcessInputMIDI(CSimpleSynth& ioSynth, uint32_t inSample, const AAX_CMidiPacket*& ioPacketPtr, uint32_t& ioPacketsRemaining)
{
	// Process all packets up to the current sample
	while ((ioPacketsRemaining > 0) &&
		   (NULL != ioPacketPtr) &&
		   (ioPacketPtr->mTimestamp <= inSample))
	{
		if (AAX::IsNoteOn(ioPacketPtr))
		{
			ioSynth.HandleNoteOn(ioPacketPtr->mData[1], ioPacketPtr->mData[2]);
		}
		else if (AAX::IsNoteOff(ioPacketPtr))
		{
			ioSynth.HandleNoteOff(ioPacketPtr->mData[1], ioPacketPtr->mData[2]);
		}
		else if (AAX::IsAllNotesOff(ioPacketPtr))
		{
			ioSynth.HandleAllNotesOff();
		}
		
		++ioPacketPtr;
		--ioPacketsRemaining;
	}
}
