//
//  VCACompressor.cpp
//  
//
//  Created by Isaac Roberts on 11/21/22.
//

// Created by Isaac Roberts

#include "VCACompressor.h"


//==============================================================================
VCACompressor::VCACompressor()
{
	update();
}

//==============================================================================
void VCACompressor::prepare (const ProcessSpec& spec)
{
	jassert (spec.sampleRate > 0);
	jassert (spec.numChannels > 0);

	sampleRate = spec.sampleRate;

	envelopeFilter.prepare (spec);

	update();
	reset();
}

void VCACompressor::reset()
{
	envelopeFilter.reset();
}

//==============================================================================
float VCACompressor::processSample (int channel, float inputValue)
{
	// Ballistics filter with peak rectifier
	inputValue *= pregain;
	auto env = envelopeFilter.processSample (channel, inputValue);

	// VCA
	auto gain = (env < threshold) ? 1.0f : std::pow (env * thresholdInverse, ratioFactor);

	last_gain = std::min(gain, last_gain);
	// Output
	return gain * inputValue * postgain;
}

void VCACompressor::update()
{
	if (thresholddB > -200)
	{
		threshold = Util::db (thresholddB);
		thresholdInverse =  (1.0) / threshold;
	}
	else
	{
		threshold = 0;
		thresholdInverse = 1;
	}
	//1/1-1 = 0
	//1/2- 1= -.5
	//1/100-1 = -.99
	//1/•-1 = -1
	
//	float toDenom = 1/ratio;
//	ratioFactor     =  (1.0) / toDenom - 1;
	ratioFactor = ratio - 1;

	envelopeFilter.setAttackTime (attackTime);
	envelopeFilter.setReleaseTime (releaseTime);
}

float VCACompressor::getCompressLevel()
{
	//log10 of lastGain
	float lg = last_gain;
	last_gain = 1;
	if (lg <= 0)
		return -400;
	return -Util::ampToDb(lg);
}

void VCACompressor::bypassProcess(ContextR& c, bool allowTail, bool isInputSilent)
{
	if (isInputSilent)
		envelopeFilter.silenceProcess(c.getNumSamples());
	else
		envelopeFilter.bypassProcess(c.getInput());
}
void VCACompressor::bypassProcess(ContextNR& c, bool allowTail, bool isInputSilent)
{
	if (isInputSilent)
		envelopeFilter.silenceProcess(c.getNumSamples());
	else
		envelopeFilter.bypassProcess(c.getInput());
}


//==============================================================================
void VCACompressor::setThreshold (float newThreshold)
{
	thresholddB = newThreshold;
	update();
}

void VCACompressor::setRatio (float newRatio)
{
//	jassert (newRatio <=  (1.0));

	ratio = newRatio;
	update();
}

void VCACompressor::setAttack (float newAttack)
{
	attackTime = newAttack;
	update();
}

void VCACompressor::setRelease (float newRelease)
{
	releaseTime = newRelease;
	update();
}

void VCACompressor::setPregain(float set)
{
	pregain = Util::dbToAmp(set);
}
void VCACompressor::setPostgain(float set)
{
	postgain = Util::dbToAmp(set);
}


void VCACompressor::setParameters(float thresh, float newRatio, float attack, float release, float pre, float post)
{
	jassert (newRatio <=  (1.0));

	thresholddB = thresh;
	ratio = newRatio;
	attackTime = attack;
	releaseTime = release;
	pregain = Util::dbToAmp(pre);
	postgain = Util::dbToAmp(post);

	update();
}
void VCACompressor::setAttackAndRelease(float atk, float rel)
{
	attackTime = atk;
	releaseTime = rel;
	update();
}
//Destructor
VCACompressor::~VCACompressor()
{
	
}

