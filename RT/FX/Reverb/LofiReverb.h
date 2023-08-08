//
//  LofiReverb.h
//  BusinessCard - Shared Code
//
//  Created by Isaac Roberts on 8/13/22.
//  Copyright © 2022 Algorithmic Software. All rights reserved.
//

// Created by Isaac Roberts 

#ifndef LofiReverb_h
#define LofiReverb_h


#include <stdio.h>

#include "SpectrumReverb.h"

class LofiReverb : public SpectrumReverb
{
public:
	LofiReverb();
	void setPeakAmt(int n);
	virtual ~LofiReverb();
	
	int getPeakAmt() { return gaussAmt; }
	
	void setFreq(int n, double set) {
		jassert(n < gaussAmt);
		cfp[n] = log2(set);
		triggerUpdate();
	}
	void setLogFreq(int n, double set) {
		cfp[n] = set;
		triggerUpdate();
	}
	void setWidth(int n, float set) {
		width[n] = set;
		triggerUpdate();
	}
	void setAmp(int n, float set) {
		amp[n] = set;
		triggerUpdate();
	}
	
	//Frequence is in Hz
	void setPeak(int n, double freq, float width, float amp);
	void setPeakLog(int n, double logFreq, float width, float amp);

	double getNFreq(int n) {
		return pow(2, cfp[n]);
	}
	
protected:
	float getAmp(size_t n);
	
protected:
	int gaussAmt;
	
	float* orig;
	int seed;
	float l2nyq;
	
	double* cfp;
	float* width;
	float* amp;
	
protected:
	void createSpectra(float* re, float* im) override;
	void addEnv(float*) override;
	void init(size_t impulse_len, size_t fft_len) override;
	
private:
	LofiReverb(const LofiReverb& other) = delete;
	LofiReverb& operator=(const LofiReverb& other) = delete;
	
};


#endif /* LofiReverb_h */
