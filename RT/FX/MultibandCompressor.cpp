//
//  MultibandCompressor.cpp
//  
//
//  Created by Isaac Roberts on 8/2/20.
//

// Created by Isaac Roberts


#include "MultibandCompressor.h"

#include <math.h>


MultibandCompressor::MultibandCompressor()
: FX(),
filters(NULL),
compressor(NULL),
bandCt(0),
freqs(NULL)
{
}

void MultibandCompressor::setBandAmt(int amt)
{
	bandCt = amt;
	int freqCt = bandCt - 1;
	
	freqs = new float[freqCt];
	
	float baseFreq = 200;
	
	float expBase = pow(20000./baseFreq, 1.0/(freqCt+1.0));
	
	for (int n=0; n < freqCt; ++n)
	{
		freqs[n] = baseFreq * pow(expBase, n);
//		std::cout<<"MultiBand freq "<<n<<" = "<<freqs[n]<<std::endl;
	}
	
	filters = new LinkwitzRiley4th*[bandCt];
	
	for (int n=0; n<bandCt; ++n)
	{
		filters[n]=new LinkwitzRiley4th[2];
		float f1 = n==0		   ? 1 : freqs[n-1] ;
		float f2 = n==bandCt-1 ? 3000 : freqs[n] ;
		
		filters[n][LowCut_].setHigh();
		filters[n][HighCut_].setLow();
		
		filters[n][LowCut_].setFreq(f1);
		filters[n][HighCut_].setFreq(f2);
	}
	
	filters[0][LowCut_].setBypass(true);
	filters[bandCt-1][HighCut_].setBypass(true);
	
	compressor = new RTCompressor*[bandCt];
	for (int n=0; n < bandCt; ++n)
	{
		//	Threshold, Ratio, Attack, Release, KneeWidth, PreGain, PostGain
		compressor[n] = new RTCompressor(0, .3, 0.001, 0.003, 2, 0, 0);
	}
}
void MultibandCompressor::prepare2(Spec& spec)
{
	for (int n=0; n<bandCt; ++n)
	{
		filters[n][LowCut_].prepare(spec);
		filters[n][HighCut_].prepare(spec);
		compressor[n]->prepare(spec);
	}
}
void MultibandCompressor::reset2()
{
	for (int n=0; n<bandCt; ++n)
	{
		filters[n][LowCut_].reset();
		filters[n][HighCut_].reset();
		compressor[n]->reset();
	}
}
void MultibandCompressor::process(ContextR& p)
{
	fx_proc_start(p);
	auto& output = p.getOutput();
	
	BlockPtr orig(p), block(p);
	
	orig->copyFrom(output);
	output.clear();
	
	ContextNR cnr (p.getStorage(), *orig, *block);
	ContextR cr(p.getStorage(), *block);
	
	for (int n=0; n<bandCt; ++n)
	{
		filters[n][LowCut_].process(cnr);
		filters[n][HighCut_].process(cr);

		compressor[n]->process(cr);

		output.add(*block);
	}
}
void MultibandCompressor::process(ContextNR& p)
{
	fx_proc_start(p);
	auto& output = p.getOutput();
	
	BlockPtr block(p);
	
	output.clear();

	ContextNR cnr(p.getStorage(), p.getInput(), *block);
	ContextR cr(p.getStorage(), *block);
	
	for (int n=0; n<bandCt; ++n)
	{
		filters[n][LowCut_].process(cnr);
		filters[n][HighCut_].process(cr);

		output.add(*block);
	}
}
void MultibandCompressor::setFreq(int n, float set)
{
	jassert( n < bandCt - 1);
	
	filters[n][HighCut_].setFreq(set);
	filters[n+1][LowCut_].setFreq(set);
}
//float MultibandCompressor::getFreq(int n)
//{
//	jassert (n < bandCt - 1);
//	if (n < bandCt-1)
//	{
//		return filters[n][HighCut_].get(RT_IIR::Frequency);
//	}
//	return 0;
//}

//void MultibandCompressor::setCompressorParam(int compN, int paramN, float value)
//{
////	jassertfalse;
//	compressor[compN]->set(paramN, value);
//}
void MultibandCompressor::setCompressorOff(int compN, bool value)
{
//	jassertfalse;
	compressor[compN]->setBypass(value);
}
void MultibandCompressor::setCompressorSettings(int compN, float threshold, float ratio, float attack, float release, float kneewidth, float pregain, float postgain)
{
//	jassertfalse;
	compressor[compN]->setParameters(threshold, ratio, attack, release, kneewidth, pregain, postgain);
}


//void MultibandCompressor::setAttacks(float set)
//{
//	for (int n=0; n < bandCt; ++n)
//	{
//		compressor[n]->setAttack(set);
//	}
//}
//void MultibandCompressor::setReleases(float set)
//{
//	for (int n=0; n < bandCt; ++n)
//	{
//		compressor[n]->setRelease(set);
//	}
//}
//void MultibandCompressor::setRatios(float set)
//{
//	for (int n=0; n < bandCt; ++n)
//	{
//		compressor[n]->setRatio(set);
//	}
//}

MultibandCompressor::~MultibandCompressor()
{
	for (int n=0; n<bandCt; ++n)
	{
		//Some apps with delete before preparing
		if (filters != NULL)
			delete[] filters[n];
		if (compressor != NULL)
			delete compressor[n];
	}
	delete[] filters;
	delete[] compressor;
	
	delete[] freqs;
}

