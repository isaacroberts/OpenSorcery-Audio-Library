//
//  LinkwitzRileyAllPass.cpp
//  
//
//  Created by Isaac Roberts on 10/19/22.
//

// Created by Isaac Roberts

#include "LinkwitzRileyAllPass.h"




//==============================================================================
LinkwitzRileyAllPass::LinkwitzRileyAllPass()
{
	update();
}
//Destructor
LinkwitzRileyAllPass::~LinkwitzRileyAllPass()
{
	
}

void LinkwitzRileyAllPass::setCutoffFrequency (float newCutoffFrequencyHz)
{
	cutoffFrequency = newCutoffFrequencyHz;
	
	update();
}

//==============================================================================
void LinkwitzRileyAllPass::prepare (const ProcessSpec& spec)
{
	sampleRate = spec.sampleRate;
	update();

	s1.resize (spec.numChannels);
	s2.resize (spec.numChannels);

	reset();
}

void LinkwitzRileyAllPass::reset()
{
	for (auto s : { &s1, &s2 })
		std::fill (s->begin(), s->end(), static_cast<float> (0));
}

void LinkwitzRileyAllPass::snapToZero() noexcept
{
	for (auto s : { &s1, &s2 })
	{
		for (auto& element : *s)
		{
			if (! (element < -1.0e-8f || element > 1.0e-8f)) element = 0;
		}
	}
}

void LinkwitzRileyAllPass::process(InputBlock& input, OutputBlock& allpass)
{
	size_t N = input.getNumSamples();
	size_t C = (size_t)input.getNumChannels();
	
	for (size_t c=0; c < C; ++c)
	{
		auto* i = input[c];
		auto* a = allpass[c];
		
		for (int n=0; n < N; ++n)
		{
			processSample(c, i[n], a[n]);
		}
	}
	endProc();
}
void LinkwitzRileyAllPass::process(OutputBlock& allpass)
{
	size_t N = allpass.getNumSamples();
	size_t C = (size_t)allpass.getNumChannels();
	
	for (size_t c=0; c < C; ++c)
	{
		auto* a = allpass[c];
		
		for (int n=0; n < N; ++n)
		{
			processSample(c, a[n], a[n]);
		}
	}
	endProc();
}


void LinkwitzRileyAllPass::endProc()
{
	sanCheckState();
#if JUCE_DSP_ENABLE_SNAP_TO_ZERO
	snapToZero();
#endif
}


void LinkwitzRileyAllPass::bypassProcess(int N)
{
	size_t C = s1.size();
	
	for (size_t c=0; c < C; ++c)
	{
		for (int n=0; n < N; ++n)
			bypassSample(c);
	}
}

void LinkwitzRileyAllPass::bypassSample(size_t c)
{
	if (!filterBypass)
	{
		auto yH = ( - (sqrt2 + g) * s1[c] - s2[c]) * h;
		
		auto yB = g * yH + s1[c];
		s1[c] = g * yH + yB;
		
		auto yL = g * yB + s2[c];
		s2[c] = g * yB + yL;
	}
	else //if (status==DirectLow || status==Silence)
	{
		s1[c] = s2[c] = 0;
	}
}

void LinkwitzRileyAllPass::processSample (size_t c, float inputValue, float &allPassOut)
{
	if (!filterBypass)
	{
		auto yH = (inputValue - (sqrt2 + g) * s1[c] - s2[c]) * h;
		
		auto yB = g * yH + s1[c];
		s1[c] = g * yH + yB;
		
		auto yL = g * yB + s2[c];
		s2[c] = g * yB + yL;
		
		allPassOut = yL - sqrt2 * yB + yH;
	}
	else
	{
		s1[c] = s2[c] = 0;
		allPassOut = inputValue;
	}
}

void LinkwitzRileyAllPass::update()
{
	if (sampleRate > 0)
	{
		if (cutoffFrequency <= 0)
			filterBypass=true;
		else if (cutoffFrequency >= sampleRate/2)
			filterBypass=true;
		else
			filterBypass=false;
	}
	else
		filterBypass=true;
	
	if (!filterBypass)
	{
		g  =  std::tan (MathConstants<float>::pi * cutoffFrequency / sampleRate);
		h  = (1.0f / (1.0f + sqrt2 * g + g * g));
	}
	else
	{
		g = 0.f;
		h = 0.f;
	}
}
