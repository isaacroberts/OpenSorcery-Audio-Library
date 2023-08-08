//
//  DynamicEQ.cpp
//  
//
//  Created by Isaac Roberts on 8/27/20.
//

// Created by Isaac Roberts


#include "DynamicEQ.h"

#if 0


DynamicEQ::DynamicEQ()
: FX(),
band (NULL)
{
}
/*
------------ ------------ ------------
  All untested
------------ ------------ ------------
 */
void DynamicEQ::setBands(int amt, float* peaks, float* qs)
{
	bandCt = amt;
	band = new Band[amt];
	for (int b=0; b < bandCt; ++b)
	{
		
//		band[b].freq = peaks[b];
//		band[b].q = qs[b];
		band[b].filter.setParameters(peaks[b], qs[b], 1);
		band[b].duck.setParameters(peaks[b], qs[b], -1);
		//Threshold, Ratio, PreGain, Attack, Release, KneeWidth
//		band[b].compressor.setParameters();
	}
}
int DynamicEQ::paramAmt()
{
	return bandCt * paramPerBand();
}
void DynamicEQ::setBand(int b, float f, float q, float thresh, float ratio, float attack, float release, float knee, float pregain, float postgain)
{
	set(b, f);
	set(b, q);
	band[b].compressor.setParameters(thresh, ratio, attack, release, knee, pregain, postgain);
}
void DynamicEQ::setBand(int b, float thresh, float ratio, float attack, float release, float knee, float pregain, float postgain)
{
	band[b].compressor.setParameters(thresh, ratio, attack, release, knee, pregain, postgain);
}
void DynamicEQ::set(int b, int n, float set)
{
	//Freq, Q, Threshold, Ratio, Gain, Attack, Release, KneeWidth;
	if (n==Freq)
	{
		band[b].filter.setFreq(set);
		band[b].duck.setFreq(set);
	}
	else if (n==Q)
	{
		band[b].filter.setOrder(set);
		band[b].filter.setOrder(set);
	}
	else if (n>=Threshold)
		band[b].compressor.set(n-Threshold, set);
}
float DynamicEQ::get(int b, int n)
{
	if (n==Freq) {
		return band[b].filter.get(RTFilter::Frequency);
	}
	else if (n==Q)
		return band[b].filter.get(RTFilter::Frequency);
	else// if (n >= Threshold)
		return band[b].compressor.get(n-Threshold);
}
void DynamicEQ::set(int n, float value)
{//Across: Every param for band 0, every param for band 1, etc
	int b = n / paramPerBand();
	int p = n % paramPerBand();
	set(b, p, value);
}
float DynamicEQ::get(int n)
{//Across: Every param for band 0, every param for band 1, etc
	int b = n / paramPerBand();
	int p = n % paramPerBand();
	return get(b, p);
}

void DynamicEQ::prepare2(Spec& spec)
{
	for (int b=0; b < bandCt; ++b)
	{
		band[b].filter.prepare(spec);
		band[b].duck.prepare(spec);
		band[b].compressor.prepare(spec);
	}
}
void DynamicEQ::process (ContextR& c)
{
	fx_proc_start(c);
	BlockPtr orig(c);
//	orig->copyFrom(block);
	auto& block = c.getOutputBlock();
	auto& bs = c.getStorage();
	Block::copy(*orig, block);
	BlockPtr workspace(c);
	BlockPtr cut(c);
	for (int b=0; b < bandCt; ++b)
	{
		workspace->copyFrom(*orig);
		Block::copy(*workspace, *orig);
		band[b].filter.process(*workspace, bs);
		band[b].compressor.process(*workspace, *cut, bs);
		band[b].duck.process(*orig, *workspace, bs);
		*workspace *= *cut;
		for (int c=0; c < workspace->getNumChannels(); ++c)
		{
			for (int n=0; n < workspace->getNumSamples(); ++n)
			{
				workspace[c][n] += (1-cut[c][n]) * orig[c][n];
			}
		}
		block.add(*workspace);
	}
}
void DynamicEQ::process(ContextNR& s)
{
	fx_proc_start(c);
	BlockPtr workspace(s);
	BlockPtr cut(s);
	auto& orig = s.getInput();
	auto& block = s.getOutput();
	auto& bs = s.getStorage();
	for (int b=0; b < bandCt; ++b)
	{
//		workspace->copyFrom(orig);
		Block::copy(*workspace, orig);
		band[b].filter.process(*workspace, bs);
		band[b].compressor.process(*workspace, *cut, bs);
		band[b].duck.process(orig, *workspace, bs);
		*workspace *= *cut;
		for (int c=0; c < workspace->getNumChannels(); ++c)
		{
			for (int n=0; n < workspace->getNumSamples(); ++n)
			{
				workspace[c][n] += (1-cut[c][n]) * orig.getSample(c, n);
			}
		}
//		block.add(*workspace);
		Block::add(block, *workspace);
	}
}
void DynamicEQ::reset2()
{
	for (int b=0; b < bandCt; ++b)
	{
		band[b].filter.reset();
		band[b].duck.reset();
		band[b].compressor.reset();
	}
}


DynamicEQ::~DynamicEQ()
{
	delete[] band;
}


#endif 
