//
//  DualLinkwitzRiley.cpp
//  
//
//  Created by Isaac Roberts on 10/19/22.
//

// Created by Isaac Roberts

#include "DualLinkwitzRiley.h"





//==============================================================================
DualLinkwitzRiley::DualLinkwitzRiley()
{
	update();
}
//Destructor
DualLinkwitzRiley::~DualLinkwitzRiley()
{
	
}

void DualLinkwitzRiley::setCutoffFrequency (float newCutoffFrequencyHz)
{
	cutoffFrequency = newCutoffFrequencyHz;
	
	update();
}

//==============================================================================
void DualLinkwitzRiley::prepare (const ProcessSpec& spec)
{
	sampleRate = spec.sampleRate;
	update();

	s1.resize (spec.numChannels);
	s2.resize (spec.numChannels);
	s3.resize (spec.numChannels);
	s4.resize (spec.numChannels);

	reset();
}

void DualLinkwitzRiley::reset()
{
	for (auto s : { &s1, &s2, &s3, &s4 })
		std::fill (s->begin(), s->end(), static_cast<float> (0));
}

void DualLinkwitzRiley::snapToZero() noexcept
{
	for (auto s : { &s1, &s2, &s3, &s4 })
	{
		for (auto& element : *s)
		{
			if (! (element < -1.0e-8f || element > 1.0e-8f)) element = 0;
		}
	}
}

void DualLinkwitzRiley::process(InputBlock& input, OutputBlock& low, OutputBlock& high)
{
	size_t N = input.getNumSamples();
	size_t C = (size_t)input.getNumChannels();
	
	for (size_t c=0; c < C; ++c)
	{
		auto* i = input[c];
		auto* l = low[c];
		auto* h = high[c];
		
		for (int n=0; n < N; ++n)
		{
			processSample(c, i[n], l[n], h[n]);
		}
	}
	endProc();
}

void DualLinkwitzRiley::process(InputBlock& input, OutputBlock& low, OutputBlock& high, OutputBlock& allpass)
{
	size_t N = input.getNumSamples();
	size_t C = (size_t)input.getNumChannels();
	
	for (size_t c=0; c < C; ++c)
	{
		auto* i = input[c];
		auto* l = low[c];
		auto* h = high[c];
		auto* a = allpass[c];
		
		for (int n=0; n < N; ++n)
		{
			processSample(c, i[n], l[n], h[n], a[n]);
		}
	}
	endProc();
}
void DualLinkwitzRiley::process_allpass(InputBlock& input, OutputBlock& allpass)
{
	size_t N = input.getNumSamples();
	size_t C = (size_t)input.getNumChannels();
	
	float l;
	float h;
	for (size_t c=0; c < C; ++c)
	{
		auto* i = input[c];
		auto* a = allpass[c];
		
		for (int n=0; n < N; ++n)
		{
			processSample(c, i[n], l, h, a[n]);
		}
	}
	endProc();

}
void DualLinkwitzRiley::process_addLow(InputBlock& input, OutputBlock& low, OutputBlock& high)
{
	float al;
	
	size_t N = input.getNumSamples();
	size_t C = (size_t)input.getNumChannels();
	
	for (size_t c=0; c < C; ++c)
	{
		auto* i = input[c];
		auto* l = low[c];
		auto* h = high[c];
		
		for (int n=0; n < N; ++n)
		{
			processSample(c, i[n], al, h[n]);
			l[n]+=al;
		}
	}
	endProc();
}
void DualLinkwitzRiley::process_addHigh(InputBlock& input, OutputBlock& low, OutputBlock& high)
{
	float ah;
	
	size_t N = input.getNumSamples();
	size_t C = (size_t)input.getNumChannels();
	
	for (size_t c=0; c < C; ++c)
	{
		auto* i = input[c];
		auto* l = low[c];
		auto* h = high[c];
		
		for (int n=0; n < N; ++n)
		{
			processSample(c, i[n], l[n], ah);
			h[n]+=ah;
		}
	}
	endProc();
}
void DualLinkwitzRiley::process_addBoth(InputBlock& input, OutputBlock& low, OutputBlock& high)
{
	float al, ah;
	
	size_t N = input.getNumSamples();
	size_t C = (size_t)input.getNumChannels();
	
	for (size_t c=0; c < C; ++c)
	{
		auto* i = input[c];
		auto* l = low[c];
		auto* h = high[c];
		
		for (int n=0; n < N; ++n)
		{
			processSample(c, i[n], al, ah);
			l[n]+=al;
			h[n]+=ah;
		}
	}
	endProc();
}
void DualLinkwitzRiley::process_skipLow(InputBlock& input, OutputBlock& high)
{
	float l;
	size_t N = input.getNumSamples();
	size_t C = (size_t)input.getNumChannels();
	
	for (size_t c=0; c < C; ++c)
	{
		auto* i = input[c];
		auto* h = high[c];
		
		for (int n=0; n < N; ++n)
		{
			processSample(c, i[n], l, h[n]);
		}
	}
	endProc();

}
void DualLinkwitzRiley::process_skipHigh(InputBlock& input, OutputBlock& low)
{
	float h;
	size_t N = input.getNumSamples();
	size_t C = (size_t)input.getNumChannels();
	
	for (size_t c=0; c < C; ++c)
	{
		auto* i = input[c];
		auto* l = low[c];
		
		for (int n=0; n < N; ++n)
		{
			processSample(c, i[n], l[n], h);
		}
	}
	endProc();
}


void DualLinkwitzRiley::endProc()
{
	sanCheckState();
#if JUCE_DSP_ENABLE_SNAP_TO_ZERO
	snapToZero();
#endif
}


void DualLinkwitzRiley::bypassProcess(int N)
{
	size_t C = s1.size();
	
	for (size_t c=0; c < C; ++c)
	{
		for (int n=0; n < N; ++n)
			bypassSample(c);
	}
}

void DualLinkwitzRiley::bypassSample(size_t c)
{
	if (status==Active)
	{
		auto yH = ( - (sqrt2 + g) * s1[c] - s2[c]) * h;
		
		auto yB = g * yH + s1[c];
		s1[c] = g * yH + yB;
		
		auto yL = g * yB + s2[c];
		s2[c] = g * yB + yL;
		
		auto yH2 = (yL - (sqrt2 + g) * s3[c] - s4[c]) * h;
		
		auto yB2 = g * yH2 + s3[c];
		s3[c] = g * yH2 + yB2;
		
		auto yL2 = g * yB2 + s4[c];
		s4[c] = g * yB2 + yL2;
	}
	else if (status==DirectHigh)
	{
		//state variables don't move
	}
	else //if (status==DirectLow || status==Silence)
	{
		s1[c] = s2[c] = s3[c] = s4[c] = 0;
	}
	
}

void DualLinkwitzRiley::processSample (size_t c, float inputValue, float &outputLow, float &outputHigh)
{
	if (status==Active)
	{
		auto yH = (inputValue - (sqrt2 + g) * s1[c] - s2[c]) * h;
		
		auto yB = g * yH + s1[c];
		s1[c] = g * yH + yB;
		
		auto yL = g * yB + s2[c];
		s2[c] = g * yB + yL;
		
		auto yH2 = (yL - (sqrt2 + g) * s3[c] - s4[c]) * h;
		
		auto yB2 = g * yH2 + s3[c];
		s3[c] = g * yH2 + yB2;
		
		auto yL2 = g * yB2 + s4[c];
		s4[c] = g * yB2 + yL2;
		
		outputLow = yL2;
		outputHigh = yL - sqrt2 * yB + yH - yL2;
	}
	else if (status==DirectHigh)
	{
		//All the state variables cancel out to be themselves
		outputLow = 0;
		outputHigh = inputValue;
	}
	else if (status==DirectLow)
	{
		//State variables get fucked beyond recognition
		s1[c] = s2[c] = s3[c] = s4[c] = 0;
		
		outputLow = inputValue;
		outputHigh = 0;

	}
	else //if (status==Silence)
	{
		jassertfalse;
		s1[c] = s2[c] = s3[c] = s4[c] = 0;
		outputLow = 0;
		outputHigh = 0;
	}
}
void DualLinkwitzRiley::processSample (size_t c, float inputValue, float &outputLow, float &outputHigh, float& allPassOut)
{
	if (status==Active)
	{
		auto yH = (inputValue - (sqrt2 + g) * s1[c] - s2[c]) * h;
		
		auto yB = g * yH + s1[c];
		s1[c] = g * yH + yB;
		
		auto yL = g * yB + s2[c];
		s2[c] = g * yB + yL;
		
		allPassOut = yL - sqrt2 * yB + yH;

		auto yH2 = (yL - (sqrt2 + g) * s3[c] - s4[c]) * h;
		
		auto yB2 = g * yH2 + s3[c];
		s3[c] = g * yH2 + yB2;
		
		auto yL2 = g * yB2 + s4[c];
		s4[c] = g * yB2 + yL2;
		
		outputLow = yL2;
		outputHigh = yL - sqrt2 * yB + yH - yL2;
	}
	else if (status==DirectHigh)
	{
//		g=0;
//		h  = 1.0f;

		//All the state variables cancel out to be themselves
//		s1[c] = s1[c];
//		s2[c] = s2[c];
//		s3[c] = s3[c];
//		s4[c] = s4[c];
		allPassOut = inputValue;
		outputLow = 0;
		outputHigh = inputValue;
	}
	else if (status==DirectLow)
	{
//		g  =  inf;
//		h  = 0;
		
		//State variables get fucked beyond recognition
		
//		s1[c] += 2;
//		s2[c] += inf;
//		s3[c] += 2;
//		s4[c] += inf;
		s1[c] = s2[c] = s3[c] = s4[c] = 0;
		
		allPassOut = inputValue;
		outputLow = inputValue;
		outputHigh = 0;

	}
	else //if (status==Silence)
	{
		jassertfalse;
		s1[c] = s2[c] = s3[c] = s4[c] = 0;
		allPassOut = inputValue;
		outputLow = 0;
		outputHigh = 0;
	}
}

void DualLinkwitzRiley::update()
{
	if (sampleRate > 0)
	{
		if (cutoffFrequency <= 0)
			status = DirectHigh;
		else if (cutoffFrequency >= sampleRate/2)
			status = DirectLow;
		else
			status = Active;
	}
	
	if (status==Active)
	{
		//		sqrt2 =  std::sqrt (2.0f);
		g  =  std::tan (MathConstants<float>::pi * cutoffFrequency / sampleRate);
		h  = (1.0f / (1.0f + sqrt2 * g + g * g));
//		g * h -> 0
	}
	else if (status==DirectHigh)
	{
		g=0;
		h  = 1.0f;
	}
	else if (status==DirectLow)
	{
		//g = inf
		g  =  1.0f;
		h  = 0;
	}
}
