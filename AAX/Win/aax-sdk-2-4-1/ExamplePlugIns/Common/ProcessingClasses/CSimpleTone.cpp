/*================================================================================================*/
/*
 *	Copyright 2014-2015, 2018 by Avid Technology, Inc.
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
#include "CSimpleTone.h"

// Standard Includes
#include <cmath>
#include <climits>
#include <algorithm>



// *******************************************************************************
// Private utility functions
// *******************************************************************************

static float MIDINoteToHz(unsigned char inMIDINoteVal, float inAHz)
{
	static const float sOneOverTwelve = (1.f/12.f);
	return (inAHz * powf(2.f, sOneOverTwelve * (inMIDINoteVal-69))); // 69 == Reference A
}

static unsigned char HzToMIDINote(float inHz, float inAHz)
{
	static const float sThirtyTwoTimesTwoToTheThreeQuarters = 32.f * powf(2.f, 0.75f);
	static const float sTwelveOverLogTwo = 12.f / log(2.f);
	const float noteFlt = sTwelveOverLogTwo * log((sThirtyTwoTimesTwoToTheThreeQuarters * inHz)/inAHz);
	return (unsigned char)((noteFlt > (UCHAR_MAX-0.5f)) ? UCHAR_MAX : noteFlt + 0.5f); // Rounding
}

static float MIDIVelocityToAmplitude(unsigned char inMIDIVelocity)
{
	static const float sOneOverOneTwentySeven = (1.f/127.f);
	return (sOneOverOneTwentySeven * inMIDIVelocity); // Linear amplitude scaling
}


// *******************************************************************************
// CSimpleTone
// *******************************************************************************
CSimpleTone::CSimpleTone()
: mSampleRateHz(0)
, mMIDINoteVal(0)
, mCurPos(0.0f)
, mAHz(440.0f)
, mHz(MIDINoteToHz(mMIDINoteVal, mAHz))
, mAmplitude(MIDIVelocityToAmplitude(0))
{
}

CSimpleTone::CSimpleTone(int32_t inSampleRateHz, float inAHz)
: mSampleRateHz(inSampleRateHz)
, mMIDINoteVal(0)
, mCurPos(0.0f)
, mAHz(inAHz)
, mHz(MIDINoteToHz(mMIDINoteVal, mAHz))
, mAmplitude(MIDIVelocityToAmplitude(0))
{
}

CSimpleTone::CSimpleTone(unsigned char inMIDINoteVal, unsigned char inMIDIVelocity, int32_t inSampleRateHz, float inAHz)
: mSampleRateHz(inSampleRateHz)
, mMIDINoteVal(inMIDINoteVal)
, mCurPos(0.0f)
, mAHz(inAHz)
, mHz(MIDINoteToHz(mMIDINoteVal, mAHz))
, mAmplitude(MIDIVelocityToAmplitude(inMIDIVelocity))
{
}

bool CSimpleTone::IsEnabled() const
{
	return (0.0f < mAmplitude && 0.0f < mSampleRateHz);
}

void CSimpleTone::SetMIDINote(unsigned char inMIDINoteVal, unsigned char inMIDIVelocity)
{
	mMIDINoteVal = inMIDINoteVal;
	mHz = MIDINoteToHz(mMIDINoteVal, mAHz);
	mAmplitude = MIDIVelocityToAmplitude(inMIDIVelocity);
}

void CSimpleTone::SetFrequency(float inHz)
{
	mMIDINoteVal = HzToMIDINote(inHz, mAHz);
	mHz = inHz;
}

void CSimpleTone::SetAmplitude(float inAmplitude)
{
	mAmplitude = std::max(0.f, std::min(1.f, inAmplitude));
}

void CSimpleTone::SetTuning(float inAHz)
{
	mAHz = inAHz;
	mHz = MIDINoteToHz(mMIDINoteVal, mAHz);
}

float CSimpleTone::ProcessOneSample(const IToneGeneratorDelegate& inDelegate)
{
	// Increment internal position
	mCurPos = ((0 == mSampleRateHz) ? mCurPos : fmodf(mCurPos + (mHz / mSampleRateHz), 2.f));
	
	// Compute a sample using the delegate
	return (inDelegate.Sample(*this) * mAmplitude);
}


// *******************************************************************************
// CSawToneGeneratorDelegate
// *******************************************************************************
float CSawToneGeneratorDelegate::Sample(const ISimpleTone& inDelegator) const
{
	return (inDelegator.CurPos() - 1.0f);
}


// *******************************************************************************
// CTriangleToneGeneratorDelegate
// *******************************************************************************
float CTriangleToneGeneratorDelegate::Sample(const ISimpleTone& inDelegator) const
{
	const float doubleRampValue = (inDelegator.CurPos()*2.0f);
	return (doubleRampValue > 2.0f ? (2.0f - doubleRampValue) + 1.0f : doubleRampValue - 1.0f);
			
}


// *******************************************************************************
// CSquareToneGeneratorDelegate
// *******************************************************************************
float CSquareToneGeneratorDelegate::Sample(const ISimpleTone& inDelegator) const
{
	return (inDelegator.CurPos() > 1.0f ? 1.0f : -1.0f);
}
