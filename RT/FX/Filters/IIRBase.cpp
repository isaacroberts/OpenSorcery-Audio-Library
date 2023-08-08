//
//  RTFilter.cpp
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/4/20.
//

// Created by Isaac Roberts


#include "IIRBase.h"

//using namespace dsp;

RT_IIR::RT_IIR() :
	FX(),
	filter(NULL),
	frequency(440),
	q(defaultQ),
	amt(1),
	channels(-1)
//	filterLock()
{
	/*
	 N = (1.0 / std::tan (MathConstants<double>::pi * frequency / sampleRate) )
	 N = 31.89 @ 440Hz
	 
	 c1 = 1.0 / (1.0 + 1.0 / Q * n + nSquared)
	 
	 */
	
}

RT_IIR::RT_IIR(float freq_, float q_, float amt_)
: 	FX(),
	filter(NULL),
	frequency(freq_),
	q(q_),
	amt(amt_),
	channels(-1)
{
}
RT_IIR::RT_IIR(float freq_)
: 	FX(),
	filter(NULL),
	frequency(freq_),
	q(defaultQ),
	amt(1),
	channels(-1)
{
}

void RT_IIR::prepare2(Spec& s)
{
	
	channels = s.numChannels;

	if (filter == NULL)
		filter = new IIRFilter[channels];
	
	_lockAndMakeFilter();
}
void RT_IIR::_lockAndMakeFilter()
{
	if (rate > 0)
	{
		DSPCoeff coeff = makeFilter();

		for (int c=0;c<channels; ++c)
		{
//			filter[c].reset();
			filter[c].setCoefficients(coeff);
		}
	}
}

void RT_IIR::process(ContextR& p)
{
	fx_proc_start(p);
	
	if (amt==0 || filter == NULL)
		return;
	
	if (needsOrigBlock())
	{
		BlockPtr origBlock(p);
		origBlock->copyFrom(p.getOutput());
		
		for (int c=0; c < channels; ++c)
		{
			filter[c].processSamples(p.getOutput().getChannelPointer(c), (int)p.getOutput().getNumSamples());
		}
		handleAmt(*origBlock, p.getOutput());
	}
	else
	{
		for (int c=0; c < channels; ++c)
		{
			filter[c].processSamples(p.getOutput().getChannelPointer(c), (int)p.getOutput().getNumSamples());
		}
	}

}
void RT_IIR::process(ContextNR& p)
{
	fx_proc_start(p);
	
	if (amt==0 || filter == NULL)
	{
		p.bypassProcess();
		return;
	}
	
	p.getOutput().copyFrom(p.getInput());
	
	//	filterLock.lock();
	for (int c=0; c < channels; ++c)
	{
		filter[c].processSamples(p.getOutput().getChannelPointer(c), (int)p.getOutput().getNumSamples());
	}
	handleAmt(p.getInput(), p.getOutput());
}

void RT_IIR::reset2() {
	for (int n=0;n<channels; ++n)
	{
		filter[n].reset();
	}
}

RT_IIR::~RT_IIR()
{
	delete[] filter;
}

