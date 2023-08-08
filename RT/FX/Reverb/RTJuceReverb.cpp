//
//  RTReverb.cpp
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/4/20.
//

// Created by Isaac Roberts


#include "RTJuceReverb.h"

using namespace juce::dsp;

RTJuceReverb::RTJuceReverb()
: FX(),
conv(),
hasReset(false)
{
}
//RTJuceReverb::RTJuceReverb(dsp::Convolution&& c)
//: FX(),
//conv(c),
//hasReset(false)
//{
//}
void RTJuceReverb::prepare2(const ProcessSpec& spec)
{//values are already set in Constants
	rate = spec.sampleRate;
	conv.prepare(spec);
	conv.reset();
	hasReset=true;//if it starts bypassed it won't need to reset
}
void RTJuceReverb::loadImpulseResponse(const void *data, size_t dataSize, bool normalize)
{
	/* oh yeah this is big brain time */
	auto nrm = normalize ? Convolution::Normalise::yes : Convolution::Normalise::no;
	
	conv.loadImpulseResponse(data, dataSize,
		 Convolution::Stereo::yes,
		 Convolution::Trim::yes, 0,
		 nrm);
}
void RTJuceReverb::loadImpulseFromFile(File f, bool stereo, bool normalize)
{
	auto nrm = normalize ? Convolution::Normalise::yes : Convolution::Normalise::no;
	auto str = stereo ? Convolution::Stereo::yes : Convolution::Stereo::no;
	conv.loadImpulseResponse(f, str, Convolution::Trim::yes, 0, nrm);
}

void RTJuceReverb::process(ContextR& p)
{
	fx_proc_start(p);
	conv.process(p.getContext());
//	p.getOutputBlock() *= 8;
	hasReset=false;
}
void RTJuceReverb::process(ContextNR& p)
{
	fx_proc_start(p);
	conv.process(p.getContext());
//	p.getOutputBlock() *= 8;
	hasReset=false;
}

void RTJuceReverb::bypassProcess(ContextR& c, bypassParams)
{
	if (allowTail)
	{
		conv.bypassProcess(c.getContext());
	}
	else if (!hasReset)
	{
		conv.reset();
		hasReset=true;
	}
}
void RTJuceReverb::bypassProcess(ContextNR& c, bypassParams)
{
	if (allowTail)
	{
		conv.bypassProcess(c.getContext());
	}
	else if (!hasReset)
	{
		conv.reset();
		hasReset=true;
	}
}

RTJuceReverb::~RTJuceReverb()
{
}

