//
//  MoogLadderFilter.cpp
//  
//
//  Created by Isaac Roberts on 7/17/21.
//

// Created by Isaac Roberts


#include "MoogLadderFilter.h"

#include "Moog/ImprovedModel.h"
#include "Moog/RKSimulationModel.h"

MoogLadderFilter::MoogLadderFilter()
: FX(),
filter(NULL),
freq(2200),
resonance(1)
{
}

void MoogLadderFilter::prepare2(Spec& spec)
{
	if (filter)
	{
		for (int c=0; c < C; ++c)
		{
			delete filter[c];
		}
		delete[] filter;
	}
	C = spec.numChannels;
	filter = new LadderFilterBase*[C];
	
	for (int c=0; c < C; ++c)
	{
		filter[c] = new RKSimulationMoog(spec.sampleRate);
		filter[c]->SetCutoff(freq);
		filter[c]->SetResonance(resonance);
	}
}
void MoogLadderFilter::process (ContextR& cnt)
{
	fx_proc_start(cnt);
	for (int c=0; c < C && c < cnt.getNumChannels(); ++c)
	{
		filter[c]->Process(cnt.getOutput()[c], cnt.getNumSamples());
	}
}
void MoogLadderFilter::process(ContextNR& cnt)
{
	fx_proc_start(cnt);
	cnt.getOutput().copyFrom(cnt.getInput());
	for (int c=0; c < C && c < cnt.getNumChannels(); ++c)
	{
		filter[c]->Process(cnt.getOutput()[c], cnt.getNumSamples());
	}
}
void MoogLadderFilter::reset2()
{
}


void MoogLadderFilter::setFreq(float set)
{
	freq = set;
	if (filter) {
		for (int c=0; c < C; ++c)
		{
			filter[c]->SetCutoff(freq);
		}
	}
}
void MoogLadderFilter::setResonance(float set)
{
	resonance = set;
	if (filter) {
		for (int c=0; c < C; ++c)
		{
			filter[c]->SetResonance(resonance);
		}
	}
}

float MoogLadderFilter::getFreq()
{
	return freq;
}
float MoogLadderFilter::getResonance()
{
	return resonance;
}

MoogLadderFilter::~MoogLadderFilter()
{
	if (filter)
	{
		for (int c=0; c < C; ++c)
		{
			delete filter[c];
		}
		delete[] filter;
	}
}

