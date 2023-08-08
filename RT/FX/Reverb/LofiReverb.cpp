//
//  LofiReverb.cpp
//  BusinessCard - Shared Code
//
//  Created by Isaac Roberts on 8/13/22.
//  Copyright © 2022 Algorithmic Software. All rights reserved.
//

// Created by Isaac Roberts

#include "LofiReverb.h"

#include <random>

LofiReverb::LofiReverb()
: SpectrumReverb(.5f),
gaussAmt(0),
orig(NULL)
{
	srand((int)time(0));
	seed = rand();
	
	cfp=NULL;
	width=NULL;
	amp= NULL;
}
void LofiReverb::setPeakAmt(int g)
{
	gaussAmt = g;
	
	cfp = new double[gaussAmt];
	width = new float[gaussAmt];
	amp = new float[gaussAmt];
	
	for (int g=0; g < gaussAmt; ++g)
	{
		cfp[g] = Util::rand1()*13;
		width[g] = 2.5;
		amp[g] = Util::rand1();
	}
}
void LofiReverb::init(size_t il, size_t fl)
{
	l2nyq = log2(rate);
	
	delete orig;
	orig = new float[fft_len];
//	srand(seed);
	for (int n=0; n < fft_len; ++n)
		orig[n] = Util::rand1()*2-1;
	
	::memset(im, 0, fft_len);
}
float LofiReverb::getAmp(size_t n)
{
	if (n==0)
		return 0;
	double freq = getFreq(n);
	float l2f = log2(abs(freq));
	
	float total = 0;
	
	for (int g=0; g < gaussAmt; ++g)
	{
		if (std::isfinite(width[g]))
			total += amp[g] * std::exp(-.5f * pow((l2f-cfp[g]) / width[g], 2));
		else
			total += amp[g]; // e ^ (k/∞) = e^0 = 1
	}
	if (total<0)
		return 0;
	return total;
}
void LofiReverb::createSpectra(float* re, float* im)
{
	::memset(im, 0, fft_len);
	for (size_t n=0; n < fft_len; ++n)
	{
		float v= orig[n] * getAmp(n);
//		float p = Util::rand1()*M_PI*2;
		re[n] = v ;
//		im[n] = 0;
		
	}
	
}
void LofiReverb::addEnv(float* impulse)
{
	for (size_t n=0; n < impulse_len; ++n)
	{
//		impulse[n] *= 1-getT(n)/totalT;
		impulse[n] *= pow(2.f, float(n)/impulse_len * -7.f);
	}
}

//Destructor
LofiReverb::~LofiReverb()
{
	delete[] cfp;
	delete[] width;
	delete[] amp;
	delete orig;
}


void LofiReverb::setPeak(int n, double freq, float w, float a) {
	cfp[n] = log2(freq);
	width[n] = w;
	amp[n] = a;
	triggerUpdate();
}

void LofiReverb::setPeakLog(int n, double logFreq, float w, float a) {
	cfp[n] = logFreq;
	width[n] = w;
	amp[n] = a;
	triggerUpdate();
}


