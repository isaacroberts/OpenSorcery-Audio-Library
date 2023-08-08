/*================================================================================================*/
/*
 *	Copyright 2012-2017 by Avid Technology, Inc.
 *	All rights reserved.
 *
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 */

/**  
 *	\file   DemoSynth_Parameters.h
 *
 *	\brief  DemoSynth_Parameters class declaration.
 *
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
#include <memory>
#include <utility>
#include <string>
#include <sstream>


static const AAX_CParamID sBypassParameterID = cDefaultMasterBypassID;
static const AAX_CParamID sTypeParameterBaseID = "Type";
static const AAX_CParamID sGainParameterBaseID = "Gain";
static const AAX_CParamID sTuningParameterBaseID = "Tuning";
static const AAX_CParamID sNodeParameterBaseID = "Node";


//================================================================================
// Private utilities

#define RETURN_ID_STRINGS_FOR_PARAM(param_id_, index_) \
	do { \
		static AAX_CString sIDStrings[kNumInputMIDINodes]; \
		if (sIDStrings[0].Empty()) \
		{ \
			for (int32_t n = 0; n < kNumInputMIDINodes; ++n) \
			{ \
				const char nodeLetter = 'a' + n; \
				std::stringstream ss; \
				ss << param_id_ << "-" << nodeLetter; \
				sIDStrings[n] = ss.str(); \
			} \
		} \
		return sIDStrings[index_]; \
	} while (0)

#define RETURN_NAME_STRINGS_FOR_PARAM(param_id_, index_) \
	do { \
		static AAX_CString sIDStrings[kNumInputMIDINodes]; \
		if (sIDStrings[0].Empty()) \
		{ \
			for (int32_t n = 0; n < kNumInputMIDINodes; ++n) \
			{ \
				const char nodeLetter = 'a' + n; \
				std::stringstream ss; \
				ss << nodeLetter << ": " << param_id_; \
				sIDStrings[n] = ss.str(); \
			} \
		} \
		return sIDStrings[index_]; \
	} while (0)

static const AAX_CString& GetTypeID(int32_t idx) { RETURN_ID_STRINGS_FOR_PARAM(sTypeParameterBaseID, idx); }
static const AAX_CString& GetTypeNameString(int32_t idx) { RETURN_NAME_STRINGS_FOR_PARAM(sTypeParameterBaseID, idx); }
static const AAX_CString& GetGainID(int32_t idx) { RETURN_ID_STRINGS_FOR_PARAM(sGainParameterBaseID, idx); }
static const AAX_CString& GetGainNameString(int32_t idx) { RETURN_NAME_STRINGS_FOR_PARAM(sGainParameterBaseID, idx); }
static const AAX_CString& GetTuningID(int32_t idx) { RETURN_ID_STRINGS_FOR_PARAM(sTuningParameterBaseID, idx); }
static const AAX_CString& GetTuningNameString(int32_t idx) { RETURN_NAME_STRINGS_FOR_PARAM(sTuningParameterBaseID, idx); }
static const AAX_CString& GetNodeID(int32_t idx) { RETURN_ID_STRINGS_FOR_PARAM(sNodeParameterBaseID, idx); }
static const AAX_CString& GetNodeNameString(int32_t idx) { RETURN_NAME_STRINGS_FOR_PARAM(sNodeParameterBaseID, idx); }



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
, mSynthesizers(kNumInputMIDINodes, CSimpleSynth())
{
	for (int32_t n = 0; n < kNumAuxOutputStems; ++n)
	{
		mNodeAssignments[n] = eNodeAssignment_Default;
	}
}

DemoSynth_Parameters::~DemoSynth_Parameters ()
{
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
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<bool>(
			sBypassParameterID, AAX_CString("Master Bypass"), false,
			AAX_CBinaryTaperDelegate<bool>(),
			AAX_CBinaryDisplayDelegate<bool>("bypass", "on"),
			true));
		param->SetNumberOfSteps( 2 );
		param->SetType( AAX_eParameterType_Discrete );
		AddSynchronizedParameter(*param);
		mParameterManager.AddParameter(param.release());
	}
	for (int32_t n = 0; n < kNumInputMIDINodes; ++n)
	{
		{
			std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<int32_t>(
				GetTypeID(n),
				GetTypeNameString(n),
				CSimpleSynth::eWaveType_Begin,
				AAX_CStateTaperDelegate<int32_t>(CSimpleSynth::eWaveType_Begin, CSimpleSynth::eWaveType_End-1),
				AAX_CStateDisplayDelegate<int32_t>(sNumWaveTypes, sTypeStateStrings, CSimpleSynth::eWaveType_Begin),
				true));
			param->SetNumberOfSteps( sNumWaveTypes );
			param->SetType( AAX_eParameterType_Discrete );
			AddSynchronizedParameter(*param);
			mParameterManager.AddParameter(param.release());
		}
		{
			std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
				GetGainID(n),
				GetGainNameString(n),
				0.5f,
				AAX_CLinearTaperDelegate<float>(0.f, 1.f),
				AAX_CDecibelDisplayDelegateDecorator<float>(AAX_CNumberDisplayDelegate<float, 1, 1>()),
				true));
			param->SetNumberOfSteps( 512 );
			param->SetType( AAX_eParameterType_Continuous );
			AddSynchronizedParameter(*param);
			mParameterManager.AddParameter(param.release());
		}
		{
			std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
				GetTuningID(n),
				GetTuningNameString(n),
				440.f,
				AAX_CLinearTaperDelegate<float>(440.f - 3.f, 440.f + 3.f),
				AAX_CUnitDisplayDelegateDecorator<float>(AAX_CNumberDisplayDelegate<float, 2, 1>(), "Hz"),
				false)); // Non-automatable
			param->SetNumberOfSteps( 256 );
			param->SetType( AAX_eParameterType_Continuous );
			mParameterManager.AddParameter(param.release());
			
			// The tuning parameter is non-automatable, and updating
			// this parameter is too CPU-intensive to perform in the
			// real-time thread, so we do not add it to the
			// synchronization list here.
			// See UpdateParameterNormalizedValue()
		}
		{
			std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<int32_t>(
				GetNodeID(n),
				GetNodeNameString(n),
				eNodeAssignment_Default,
				AAX_CStateTaperDelegate<int32_t>(0, eNodeAssignment_NumStates-1),
				AAX_CStateDisplayDelegate<int32_t>(GetNodeAssignmentStateStrings()),
				true));
			param->SetNumberOfSteps(eNodeAssignment_NumStates);
			param->SetType(AAX_eParameterType_Discrete);
			AddSynchronizedParameter(*param);
			mParameterManager.AddParameter(param.release());
		}
	}
	
	// Initialize state
	for (int32_t n = 0; n < kNumInputMIDINodes; ++n)
	{
		InitializeSynthesizerState(mSynthesizers[n], n);
	}
	
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
	for (int32_t n = 0; n < kNumInputMIDINodes; ++n)
	{
		if (GetTuningID(n).Equals(iParamID))
		{
			float tuning = 440.f;
			param->GetValueAsFloat(&tuning);
			mSynthesizers[n].SetTuning(tuning);
		}
	}
	
	return result;
}

void DemoSynth_Parameters::RenderAudio(AAX_SInstrumentRenderInfo* ioRenderInfo, const TParamValPair* inSynchronizedParamValues[], int32_t inNumSynchronizedParamValues)
{
	// Update the synthesizer coefficients
	UpdateCoefficients(inSynchronizedParamValues, inNumSynchronizedParamValues);
	
	const int32_t numSamples = *(ioRenderInfo->mNumSamples);
	AAX_EStemFormat outputStemFormat = AAX_eStemFormat_None;
    Controller()->GetOutputStemFormat(&outputStemFormat);
	const int32_t numOutputChannels = AAX_STEM_FORMAT_CHANNEL_COUNT(outputStemFormat);
	float * const AAX_RESTRICT outBuf = ioRenderInfo->mAudioOutputs[0];
	float * const * const AAX_RESTRICT aos = &(ioRenderInfo->mAudioOutputs[numOutputChannels]);
	int32_t numActiveNodes = 0;
	
	// Clear the output buffers
	memset(outBuf, 0, sizeof(float) * numSamples);
	for (int32_t m = 0; m < kNumAuxOutputStems; ++m)
	{
		if (NULL != aos[m])
		{
			memset(aos[m], 0, sizeof(float) * numSamples);
		}
	}
	
	// Loop over MIDI nodes
	for (int32_t n = 0; n < kNumInputMIDINodes; ++n)
	{
		const int32_t nodeRouting = mNodeAssignments[n];
		if (eNodeAssignment_None != nodeRouting)
		{
			++numActiveNodes;
		}
			
		// Setup MIDI node pointers
		AAX_IMIDINode* const midiIn = (0 == n) ? ioRenderInfo->mInputNode : ioRenderInfo->mAdditionalInputMIDINodes[n-1];
		AAX_CMidiStream* const midiBuffer = midiIn->GetNodeBuffer();
		AAX_CMidiPacket* curMidiPacket = midiBuffer->mBuffer;
		uint32_t packets_remaining = midiBuffer->mBufferSize;
	
		// Run processing loop over each sample
		for (int32_t t = 0; t < numSamples; t++)
		{
			ProcessInputMIDI(mSynthesizers[n], t, curMidiPacket, packets_remaining);
			float synthSample = 0.f;
			mSynthesizers[n].GetSamples(&synthSample, 1);
			
			// Route the synth output to the selected stems
			if (eNodeAssignment_Direct == nodeRouting)
			{
				outBuf[t] += synthSample;
			}
			else if (eNodeAssignment_All == nodeRouting)
			{
				outBuf[t] = synthSample;
				for (int32_t m = 0; m < kNumAuxOutputStems; ++m)
				{
					float * const AAX_RESTRICT aosBuf = aos[m];
					if (NULL != aosBuf)
					{
						aosBuf[t] += synthSample;
					}
				}
			}
			else if (eNodeAssignment_SingleStemBegin <= nodeRouting)
			{
				float * const AAX_RESTRICT aosBuf = aos[nodeRouting - eNodeAssignment_SingleStemBegin];
				if (NULL != aosBuf)
				{
					aosBuf[t] += synthSample;
				}
			}
		}
	}
	
	if (0 == numActiveNodes)
	{
		memset(outBuf, 0, sizeof(float) * numSamples);
	}
	
	// Copy first channel output to other output channels
	for (int32_t ch = 1; ch < numOutputChannels; ch++)
	{
		memcpy(ioRenderInfo->mAudioOutputs[ch], outBuf, sizeof(float) * numSamples);
	}
}

void DemoSynth_Parameters::InitializeSynthesizerState(CSimpleSynth& ioSynth, int32_t inIndex)
{
	{
		AAX_IParameter* param = mParameterManager.GetParameterByID(sBypassParameterID);
		bool bypass = false;
		param->GetValueAsBool(&bypass);
		ioSynth.SetBypass(bypass);
	}
	{
		const AAX_CParamID id = GetTypeID(inIndex).Get();
		AAX_IParameter* param = mParameterManager.GetParameterByID(id);
		CSimpleSynth::EWaveType waveType = CSimpleSynth::eWaveType_Saw;
		param->GetValueAsInt32((int32_t*)&waveType);
		ioSynth.SetWaveType(waveType);
	}
	{
		const AAX_CParamID id = GetGainID(inIndex).Get();
		AAX_IParameter* param = mParameterManager.GetParameterByID(id);
		float gain = 1.f;
		param->GetValueAsFloat(&gain);
		ioSynth.SetGain(gain);
	}
	{
		const AAX_CParamID id = GetTuningID(inIndex).Get();
		AAX_IParameter* param = mParameterManager.GetParameterByID(id);
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
			for (int32_t n = 0; n < kNumInputMIDINodes; ++n)
			{
				mSynthesizers[n].SetBypass(bypass);
			}
		}
		
		for (int32_t n = 0; n < kNumInputMIDINodes; ++n)
		{
			{
				const AAX_CParamID id = GetTypeID(n).Get();
				if (AAX::IsParameterIDEqual(curParamID, id))
				{
					CSimpleSynth::EWaveType waveType = CSimpleSynth::eWaveType_Saw;
					curParamVal->GetValueAsInt32((int32_t*)&waveType);
					mSynthesizers[n].SetWaveType(waveType);
				}
			}
			{
				const AAX_CParamID id = GetGainID(n).Get();
				if (AAX::IsParameterIDEqual(curParamID, id))
				{
					float gain = 1.f;
					curParamVal->GetValueAsFloat(&gain);
					mSynthesizers[n].SetGain(gain);
				}
			}
			{
				const AAX_CParamID id = GetNodeID(n).Get();
				if (AAX::IsParameterIDEqual(curParamID, id))
				{
					ENodeAssignment assignment = eNodeAssignment_Default;
					curParamVal->GetValueAsInt32((int32_t*)&assignment);
					mNodeAssignments[n] = assignment;
				}
			}
		}
	}
}

void DemoSynth_Parameters::ProcessInputMIDI(CSimpleSynth& ioSynth, uint32_t inSample, AAX_CMidiPacket*& ioPacket, uint32_t& ioPacketsRemaining)
{
	if ((ioPacketsRemaining > 0) && (NULL != ioPacket))
	{
		// is this message for current sample?
		if (ioPacket->mTimestamp <= inSample)
		{
			if (AAX::IsNoteOn(ioPacket))
			{
				const unsigned char velocity = ioPacket->mData[2];
				if (0 < velocity)
				{
					ioSynth.HandleNoteOn(ioPacket->mData[1], velocity);
				}
				else
				{
					ioSynth.HandleNoteOff(ioPacket->mData[1], velocity);
				}
			}
			else if (AAX::IsNoteOff(ioPacket))
			{
				ioSynth.HandleNoteOff(ioPacket->mData[1], ioPacket->mData[2]);
			}
			else if (AAX::IsAllNotesOff(ioPacket))
			{
				ioSynth.HandleAllNotesOff();
			}
			
			++ioPacket;
			--ioPacketsRemaining;
		}
	}
}

/* static */
const std::vector<AAX_IString*>& DemoSynth_Parameters::GetNodeAssignmentStateStrings()
{
	static std::vector<AAX_IString*> sNodeAssignmentStateStrings(eNodeAssignment_NumStates, nullptr);
	
	if (nullptr == sNodeAssignmentStateStrings[0])
	{
		for (int i = 0; i < eNodeAssignment_NumStates; ++i)
			sNodeAssignmentStateStrings[i] = new AAX_CString;
		
		sNodeAssignmentStateStrings[eNodeAssignment_None]->Set("None");
		sNodeAssignmentStateStrings[eNodeAssignment_Direct]->Set("Direct");
		sNodeAssignmentStateStrings[eNodeAssignment_All]->Set("All");
		
		for (int i = eNodeAssignment_SingleStemBegin; i < eNodeAssignment_SingleStemEnd; ++i)
		{
			std::stringstream ss;
			const int32_t nodeIndex = i-eNodeAssignment_SingleStemBegin;
			ss << "Stem " << (1 + nodeIndex);
			sNodeAssignmentStateStrings[i]->Set(ss.str().c_str());
		}
	}
	
	return sNodeAssignmentStateStrings;
}
