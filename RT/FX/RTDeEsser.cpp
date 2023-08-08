//
//  RTDeEsser.cpp
//  
//
//  Created by Isaac Roberts on 9/2/20.
//

// Created by Isaac Roberts


#include "RTDeEsser.h"

#if 0

#include "RTGraphics/DebugWindow.h"

RTDeEsser::RTDeEsser()
: FX()
{
	filter.setParameters(1500);
	duck.setParameters(1500, 20, 1);
	compressor.setParameters(-6, 0, 0, .0025, .16, 0);
}
void RTDeEsser::setParameters(float f, float q, float thresh, float ratio, float gain, float attack, float release, float knee)
{
	filter.setParameters(f, q);
	duck.setParameters(f, q);
	compressor.setParameters(thresh, ratio, gain, attack, release, knee);
}
void RTDeEsser::set(int n, float set)
{
	//Freq, Q, Threshold, Ratio, Gain, Attack, Release, KneeWidth;
	if (n==Freq)
	{
		filter.setFreq(set);
		duck.setFreq(set);
	}
	else if (n==Order)
	{
		filter.setOrder(set);
		duck.setOrder(set);
	}
	else if (n>=Threshold) {
		compressor.set(n-Threshold, set);
	}
}
float RTDeEsser::get(int n)
{
	if (n==Freq) {
		return filter.get(RTFilter::Frequency);
	}
	else if (n==Order)
		return filter.get(RTFilter::Order);
	else// if (n >= Threshold)
		return compressor.get(n-Threshold);
}
void RTDeEsser::prepare2(Spec& spec)
{
	filter.prepare(spec);
	duck.prepare(spec);
	compressor.prepare(spec);
}
void RTDeEsser::process(ContextR& r)
{
	fx_proc_start(r);
	BlockPtr orig(r);
	
	auto& block = r.getOutput();
	
	orig->copyFrom(block);
	
	BlockPtr cut(r);
	
	filter.process(block, r.getStorage());
	block *= 1e3;
	compressor.process(block, *cut,r.getStorage());
//	block *= 1e-3;
	
	float actual_cut = 0;
	
	
	for (int c=0; c < block.getNumChannels(); ++c)
	{
		for (int n=0; n < block.getNumSamples(); ++n)
		{
			actual_cut += cut[c][n];
		}
	}
	actual_cut /= block.getNumSamples();
	actual_cut /= block.getNumChannels();
	
//	Debug::print(actual_cut, compressor.get(RTCompressor::Ratio));
	
	if (actual_cut < 1E-9)
	{
		duck.setAmountOnly(actual_cut);
		duck.process(*orig, block, r.getStorage());
	}
	else
	{
		block.copyFrom(*orig);
	}
}
void RTDeEsser::processNR(ContextNR& s)
{//Untested
	
	fx_proc_start(r);
	BlockPtr cut(s);
	
	auto& orig = s.getInput();
	auto& block = s.getOutput();
	
	
	filter.process(orig, block, s.getStorage());
	compressor.process(block, *cut, s.getStorage());
	duck.process(orig, block, s.getStorage());
	
	block *= *cut;
	
	for (int c=0; c < block.getNumChannels(); ++c)
	{
		for (int n=0; n < block.getNumSamples(); ++n)
		{
			block[c][n] += (1-cut[c][n]) * orig[c][n];
		}
	}
}
void RTDeEsser::reset2()
{
	filter.reset();
	duck.reset();
	compressor.reset();
}


RTDeEsser::~RTDeEsser()
{
}

#endif
