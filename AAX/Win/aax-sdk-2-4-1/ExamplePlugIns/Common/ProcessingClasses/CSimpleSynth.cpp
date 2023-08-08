/*================================================================================================*/
/*
 *	Copyright 2015 by Avid Technology, Inc.
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
#include "CSimpleSynth.h"

// Standard Includes
#include <cstddef>


CSimpleSynth::CSimpleSynth()
: mVoices()
, mBypassed(false)
, mGain(1.f)
, mToneGenerator(NULL)
{
}


void CSimpleSynth::SetBypass(bool inBypass) { mBypassed = inBypass; }
void CSimpleSynth::SetGain(float inGain) { mGain = inGain; }
void CSimpleSynth::SetWaveType(EWaveType inWaveType) { mToneGenerator = GetToneGenerator(inWaveType); }

void CSimpleSynth::SetTuning(float inAHz)
{
	for (size_t voiceNum = MaxNumVoices(); voiceNum > 0; --voiceNum)
	{
		mVoices[voiceNum-1].SetTuning(inAHz);
	}
}

void CSimpleSynth::InitializeVoices(float inSampleRate, float inAHz)
{
	for (size_t voiceNum = MaxNumVoices(); voiceNum > 0; --voiceNum)
	{
		mVoices[voiceNum-1] = CSimpleTone((int32_t)inSampleRate, inAHz);
	}
}

int32_t CSimpleSynth::MaxNumVoices() const
{
	// size of mVoices will always be the same
	static const int32_t sVoices = static_cast<int32_t>(sizeof(mVoices) / sizeof(CSimpleTone));
	return sVoices;
}

/* static */ const char* CSimpleSynth::GetWaveTypeName(EWaveType inWaveType)
{
	switch (inWaveType)
	{
		case eWaveType_Saw: return "Saw";
		case eWaveType_Tri: return "Tri";
		case eWaveType_Square: return "Square";
		default: return "<unknown>";
	}
}

void CSimpleSynth::GetSamples(float* outBuffer, int32_t outBufferSize)
{
	// Count the active voices
	size_t numActiveVoices = 0;
	for (size_t voiceNum = MaxNumVoices(); voiceNum > 0; --voiceNum)
	{
		if (mVoices[voiceNum-1].IsEnabled())
		{
			++numActiveVoices;
		}
	}
	
	// Generate samples using a simple sample-by-sample procedure
	for (int32_t i = 0; i < outBufferSize; ++i)
	{
		float sample = 0.f;
		for (size_t voiceNum = MaxNumVoices(); voiceNum > 0; --voiceNum)
		{
			ISimpleTone& curVoice = mVoices[voiceNum-1];
			if (curVoice.IsEnabled())
			{
				const float curNormalizedSample = (curVoice.ProcessOneSample(*mToneGenerator));
				sample += curNormalizedSample;
			}
		}
		
		// Scale output by the number of active voices to avoid overload
		sample = (mBypassed || 0 == numActiveVoices) ? 0.f : mGain * (sample / numActiveVoices);
		outBuffer[i] = sample;
	}
}

void CSimpleSynth::HandleNoteOn(unsigned char inNote, unsigned char inVelocity)
{
	if (inNote < MaxNumVoices())
	{
		// Use inNote as the key into the mVoices array
		mVoices[inNote].SetMIDINote(inNote, inVelocity);
	}
}

void CSimpleSynth::HandleNoteOff(unsigned char inNote, unsigned char inVelocity)
{
	if (inNote < MaxNumVoices())
	{
		// Use inNote as the key into the mVoices array
		mVoices[inNote].SetMIDINote(0, 0);
	}
}

void CSimpleSynth::HandleAllNotesOff()
{
	for (unsigned char note = 0; note < MaxNumVoices(); ++note)
	{
		HandleNoteOff(note, 0);
	}
}

/* static */
const IToneGeneratorDelegate* CSimpleSynth::GetToneGenerator(EWaveType inWaveType)
{
	static const CSawToneGeneratorDelegate sSawToneGenerator;
	static const CTriangleToneGeneratorDelegate sTriangeToneGenerator;
	static const CSquareToneGeneratorDelegate sSquareToneGenerator;
	
	switch (inWaveType)
	{
		case eWaveType_Saw: return &sSawToneGenerator;
		case eWaveType_Tri: return &sTriangeToneGenerator;
		case eWaveType_Square: return &sSquareToneGenerator;
		default: return NULL;
	}
}

