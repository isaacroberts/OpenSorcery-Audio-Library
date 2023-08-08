/*================================================================================================*/
/*
 *	Copyright 2012-2015, 2017 by Avid Technology, Inc.
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
#include "DemoSampler_Parameters.h"

// DemoSampler Includes
#include "DemoSampler_SampleBase.h"
#include "DemoSampler_Defs.h"

// AAX Includes
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CStateTaperDelegate.h"
#include "AAX_CStateDisplayDelegate.h"
#include "AAX_UtilsNative.h"
#include "AAX_MIDIUtilities.h"

// Standard Includes
#include <memory>


// *******************************************************************************
// Local constants
static const char* cPattern_Strings[] = {"Off", "Pattern1", "Pattern2", "Pattern3", 0};
static const int cPattern_OffSelectorIndex = 0;

static const AAX_CParamID sDemoSampler_PatternID = "Pattern";

// *******************************************************************************
// Utility methods

static int GetPatternIndexFromSelectorIndex(int inSelectorIndex)
{
	int result = -1;
	switch (inSelectorIndex)
	{
		case 1:
			result = 0;
			break;
		case 2:
			result = 1;
			break;
		case 3:
			result = 2;
			break;
		case (cPattern_OffSelectorIndex):
		default:
			result = -1;
			break;
	}
	return result;
}


// *******************************************************************************
// AAX_CEffectParameters methods

AAX_CEffectParameters *AAX_CALLBACK DemoSampler_Parameters::Create()
{
	return new DemoSampler_Parameters();
}

DemoSampler_Parameters::DemoSampler_Parameters () 
: AAX_CMonolithicParameters()
, mPendingBeat(eNone)
, mTicksPerBeat(960)
, mTempo(120)
, mBeatNumber(0)
, mTickNumber(0)
, mSampleNumber(0)
{
	for (int32_t i = 0; i < eDrum_Count; ++i)
	{
		mSampleBanks[i] = NULL;
		mBanksLength[i] = 0;
		mPendingDrums[i] = 0;
	}
	
	for (int32_t i = 0; i < cNumPatterns-1; ++i)
	{
		mPatterns[i] = NULL;
	}
}

DemoSampler_Parameters::~DemoSampler_Parameters ()
{
	for (int32_t i = 0; i < cNumPatterns-1; i++)
		if (mPatterns[i] != NULL )
			delete mPatterns[i];
}

AAX_Result DemoSampler_Parameters::EffectInit()
{
	// Create parameters
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<bool>(
			cDefaultMasterBypassID, AAX_CString("Master Bypass"), false,
			AAX_CBinaryTaperDelegate<bool>(),
			AAX_CBinaryDisplayDelegate<bool>("bypass", "on"),
			true));
		param->SetNumberOfSteps( 2 );
		param->SetType( AAX_eParameterType_Discrete );
		AddSynchronizedParameter(*param);
		mParameterManager.AddParameter(param.release());
	}
	{
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<int32_t>(
			sDemoSampler_PatternID, AAX_CString("Pattern"), 0,
			AAX_CStateTaperDelegate<int32_t>(0, cNumPatterns), // Zero-indexed, but add one for the "off" state
			AAX_CStateDisplayDelegate<int32_t>(cPattern_Strings),
			true));
		param->SetNumberOfSteps(cNumPatterns+1);
		param->SetType( AAX_eParameterType_Discrete );
		AddSynchronizedParameter(*param);
		mParameterManager.AddParameter(param.release());
	}
	
	// Initialize state
	ResetSampler();

	return AAX_SUCCESS;
}

void DemoSampler_Parameters::RenderAudio(AAX_SInstrumentRenderInfo* ioRenderInfo, const TParamValPair* inSynchronizedParamValues[], int32_t inNumSynchronizedParamValues)
{
	// Update the sampler coefficients
	UpdateCoefficients(inSynchronizedParamValues, inNumSynchronizedParamValues);
	
	// Setup MIDI node pointers
	AAX_IMIDINode* const midiIn = ioRenderInfo->mInputNode;
	const AAX_CMidiStream* const midiBuffer = midiIn->GetNodeBuffer();
	const AAX_CMidiPacket* curMIDIInputPacket = midiBuffer->mBuffer;
	uint32_t numInputPacketsRemaining = midiBuffer->mBufferSize;

	AAX_IMIDINode* const globalNode = ioRenderInfo->mGlobalNode;
	const AAX_CMidiStream* const globalBuffer = globalNode->GetNodeBuffer();
	const AAX_CMidiPacket* curMIDIGlobalPacket = globalBuffer->mBuffer;
	uint32_t numGlobalPacketsRemaining = globalBuffer->mBufferSize;

	AAX_IMIDINode* const transportNode = ioRenderInfo->mTransportNode;
	const AAX_ITransport * const transport = transportNode->GetTransport();
	bool isPlaying = false;
	if (!transport || AAX_SUCCESS != transport->IsTransportPlaying(&isPlaying))
	{
		isPlaying = false;
	}

	int32_t numSamples = *(ioRenderInfo->mNumSamples);
    AAX_EStemFormat outputStemFormat;
    Controller()->GetOutputStemFormat(&outputStemFormat);
	int32_t numChannels = AAX_STEM_FORMAT_CHANNEL_COUNT(outputStemFormat);
	float * const AAX_RESTRICT outBuf = ioRenderInfo->mAudioOutputs[0];
	
	/*
	// Debug logging
	if (0 < packets_count)
	{
		char buffer[4096];
		AAX::AsStringMIDIStream_Debug(*midiBuffer, buffer, 4096);
		AAX_TRACE(kAAX_Trace_Priority_Low, "DemoSampler_Parameters::RenderAudio - midiBuffer: %s", buffer);
	}
	if (0 < global_packets)
	{
		char buffer[4096];
		AAX::AsStringMIDIStream_Debug(*globalBuffer, buffer, 4096);
		AAX_TRACE(kAAX_Trace_Priority_Low, "DemoSampler_Parameters::RenderAudio - globalBuffer: %s", buffer);
	}
	*/
	
	// Run processing loop over each sample
	for (int32_t t = 0; t < numSamples; t++)
	{
		ProcessInputMIDI(curMIDIInputPacket, numInputPacketsRemaining, t);
		if (isPlaying)
		{
			uint32_t ticksPerBeat = 0;
			if (AAX_SUCCESS == transport->GetCurrentTicksPerBeat(&ticksPerBeat))
			{
				mTicksPerBeat = ticksPerBeat / 1000;	// to be the same as shown in ProTools ruler.
			}
			
			double tempoBPM = 0.0;
			if (AAX_SUCCESS == transport->GetCurrentTempo(&tempoBPM))
			{
				mTempo = tempoBPM;
			}
			
			ProcessGlobalMIDI(curMIDIGlobalPacket, numGlobalPacketsRemaining, t);
			ProcessPattern();
		}

		// Copy samples from sample banks to output
		outBuf[t] = 0;
		for (int i = 0; i < eDrum_Count; i++)
		{
			if (mPendingDrums[i] >= 0)
			{
				outBuf[t] += mSampleBanks[i][ mPendingDrums[i]++ ];
				if (mPendingDrums[i] > (mBanksLength[i] - 1))
					mPendingDrums[i] = -1;
			}
		}

		// Process bypass
		if (mAlgBypass)
		{
			outBuf[t] = 0;
		}

		// Copy first channel output to other channels
		for (int32_t ch = 1; ch < numChannels; ch++)
			ioRenderInfo->mAudioOutputs [ch][t] = ioRenderInfo->mAudioOutputs [0][t];

	}
}

void DemoSampler_Parameters::ResetSampler()
{
	// Init sample banks
	for (int i = 0; i < eDrum_Count; i++)
		mPendingDrums[i] = -1;
	
	mSampleBanks[0] = bass_bank; 	mBanksLength[0] = bass_size;
	mSampleBanks[1] = snare_bank;	mBanksLength[1] = snare_size;
	mSampleBanks[2] = tom_bank;		mBanksLength[2] = tom_size;
	
	// Create drum patterns
	CPattern* drumPattern = new CPattern();
	drumPattern->AddDrum(eBass,  0, 0);
	drumPattern->AddDrum(eSnare, 1, 0);
	drumPattern->AddDrum(eBass,  2, 0);
	drumPattern->AddDrum(eBass,  2, 480);
	drumPattern->AddDrum(eSnare, 3, 0);
	mPatterns[0] = drumPattern;
	
	drumPattern = new CPattern();
	drumPattern->AddDrum(eBass,  0, 0);
	drumPattern->AddDrum(eBass,  0, 480);
	drumPattern->AddDrum(eBass,  0, 645);
	drumPattern->AddDrum(eBass,  0, 767);
	drumPattern->AddDrum(eSnare, 0, 945);
	drumPattern->AddDrum(eBass,  1, 460);
	drumPattern->AddDrum(eBass,  2, 287);
	drumPattern->AddDrum(eBass,  2, 735);
	drumPattern->AddDrum(eBass,  2, 825);
	drumPattern->AddDrum(eSnare, 2, 940);
	mPatterns[1] = drumPattern;
	
	drumPattern = new CPattern();
	drumPattern->AddDrum(eBass,  0, 0);
	drumPattern->AddDrum(eTom,   0, 0);
	drumPattern->AddDrum(eBass,  0, 480);
	drumPattern->AddDrum(eTom,   0, 480);
	drumPattern->AddDrum(eSnare, 1, 0);
	drumPattern->AddDrum(eBass,  2, 0);
	drumPattern->AddDrum(eSnare, 3, 0);
	drumPattern->AddDrum(eSnare, 3, 480);
	mPatterns[2] = drumPattern;
}

void DemoSampler_Parameters::UpdateCoefficients(const TParamValPair **inSynchronizedParamValues, int32_t inNumSynchronizedParamValues)
{
	for (int32_t i = 0; i < inNumSynchronizedParamValues; ++i)
	{
		AAX_CParamID const curParamID = inSynchronizedParamValues[i]->first;
		const AAX_IParameterValue* const curParamVal = inSynchronizedParamValues[i]->second;
		
		if (AAX::IsParameterIDEqual(curParamID, cDefaultMasterBypassID))
		{
			curParamVal->GetValueAsBool(&mAlgBypass);
		}
		
		if (AAX::IsParameterIDEqual(curParamID, sDemoSampler_PatternID))
		{
			int32_t patternSelectorIndex = cPattern_OffSelectorIndex;
			if (true == curParamVal->GetValueAsInt32(&patternSelectorIndex))
			{
				const int patternIndex = GetPatternIndexFromSelectorIndex(patternSelectorIndex);
				mCurrentPattern = (patternIndex >= 0) ? mPatterns[patternIndex] : NULL;
			}
		}
	}
}

void DemoSampler_Parameters::ProcessInputMIDI(const AAX_CMidiPacket*& ioPacketPtr, uint32_t& ioPacketsRemaining, uint32_t inSampleTime)
{
	while ((ioPacketsRemaining > 0) &&
		   (NULL != ioPacketPtr) &&
		   (ioPacketPtr->mTimestamp <= inSampleTime))
	{
		if (AAX::IsNoteOn(ioPacketPtr))
		{
			const unsigned char note = ioPacketPtr->mData[1];
			
			if ((note % 12) == 0)
			{
				// C
				mPendingDrums[0] = 0;
			}
			else if ((note - 2) % 12 == 0)
			{
				// D
				mPendingDrums[1] = 0;
			}
			else if ((note - 4) % 12 == 0)
			{
				// E
				mPendingDrums[2] = 0;
			}
		}
		else if (AAX::IsAllNotesOff(ioPacketPtr))
		{
			for (int i = 0; i < eDrum_Count; i++)
			{
				mPendingDrums[i] = -1;
			}
			mPendingBeat = eNone;
		}
		
		ioPacketPtr++;
		ioPacketsRemaining--;
	}
}

void DemoSampler_Parameters::ProcessGlobalMIDI(const AAX_CMidiPacket*& ioPacketPtr, uint32_t& ioPacketsRemaining, uint32_t inSampleTime)
{
	mPendingBeat = eNone;
	
	while ((ioPacketsRemaining > 0) &&
		   (NULL != ioPacketPtr) &&
		   (ioPacketPtr->mTimestamp <= inSampleTime))
	{
		if (AAX::IsAccentedClick(ioPacketPtr))
		{
			mPendingBeat = eAccented;
		}
		else if (AAX::IsUnaccentedClick(ioPacketPtr))
		{
			mPendingBeat = eUnaccented;
		}
		else if ((AAX::IsAllNotesOff(ioPacketPtr)) ||
				 (AAX::eStatusByte_Stop == ioPacketPtr->mData[0]) ||
				 (AAX::eStatusByte_Reset == ioPacketPtr->mData[0]))
		{
			for (int i = 0; i < eDrum_Count; i++)
			{
				mPendingDrums[i] = -1;
			}
			mPendingBeat = eNone;
		}
		
		ioPacketPtr++;
		ioPacketsRemaining--;
	}
}

int32_t DemoSampler_Parameters::GetSamplesPerTick()
{

	const int32_t sampleRate = kSupportedSR;
	uint32_t samplesPerBeat = 22050;	// default value
	if (mTempo != 0)
		samplesPerBeat = sampleRate * 60 / (int32_t)mTempo;

	return samplesPerBeat / mTicksPerBeat;
}

void DemoSampler_Parameters::ProcessPattern()
{
	if (mCurrentPattern == NULL)
		return;

	bool newTick = false;
	if (mPendingBeat == eAccented)
	{
		// Note: for this example we only move to a new measure when we encounter an
		// accented beat. A real productin plug-in should minitor transport information
		// for the exact beat/tick position rather than relying on click messages
		mBeatNumber = 0;
		mSampleNumber = 0;
		mTickNumber = 0;
		newTick = true;
	} else if (mPendingBeat == eUnaccented)
	{
		mBeatNumber++;
		mSampleNumber = 0;
		mTickNumber = 0;
		newTick = true;
	}
	
	// calculate current tick inside beat
	int32_t samplesPerTick = GetSamplesPerTick();
	
	if (mSampleNumber++ >= samplesPerTick)
	{
		newTick = true;
		mTickNumber++;
		mSampleNumber = 0;
	}

	if (newTick)
	{
		for (int32_t i = 0; i < eDrum_Count; i++)
		{
			if (mCurrentPattern->isDrumTime(i, mBeatNumber, mTickNumber))
				mPendingDrums[i] = 0;
		}
	}
}

void CPattern::AddDrum(EDrumType drum, int32_t beatNumber, int32_t beatOffset)
{
	SSoundPosition newSound;
	newSound.mDrum = drum;
	newSound.mBeatNumber = beatNumber;
	newSound.mBeatOffset = beatOffset;
	mSounds.push_back(newSound);
}

bool CPattern::isDrumTime(int32_t inDrum, int32_t beat, int32_t tick) const
{
	// found drum which should sound at this time position
	size_t numSounds = mSounds.size();
	for (size_t i = 0; i < numSounds; i++)
	{
		if ( mSounds[i].mDrum == inDrum && 
			 mSounds[i].mBeatNumber == beat && 
			 mSounds[i].mBeatOffset == tick )
					return true;
	}
	return false;
}
