//
//  SpectrumReverb.h
//  
//
//  Created by Isaac Roberts on 8/13/22.
//

// Created by Isaac Roberts 

#ifndef SpectrumReverb_h
#define SpectrumReverb_h

#include "ReverbCommon.h"

#include "../MobileConvolver.h"

#include "RT/FX/FX.h"

class SpectrumReverb : public FX, public Thread
{
	/* Overrides:
	
	 void createSpectra(float* re, float* im) override;
	 void addEnv(float*) override;
	 void init(size_t impulse_len, size_t fft_len) override {}

	 */
public:
	SpectrumReverb(float len);
	
	virtual ~SpectrumReverb();
	
	void process(ContextR& context) ;
	void process(ContextNR& context) ;
	
	void prepare2(const ProcessSpec& spec)override;
	
	//TODO: Allow tail
	void bypassProcess(ContextR& , bypassParams)  {
		if (!hasCleared)
			clear();
	}
	void bypassProcess(ContextNR&, bypassParams)  {
		if (!hasCleared)
			clear();
	}
	
	void clear();
	
//	Getters & Setters
	int paramAmt() override {return 0;}
	void set(int n, float set) override {}
	float get(int n) override { return extraGet(); }
	
protected:
	virtual void createSpectra(float* re, float* im) = 0;
	virtual void addEnv(float*) = 0;
	virtual void init(size_t impulse_len, size_t fft_len) {}
	//Utility
	inline float getFreq(size_t n) {
		//TODO: Check this 
		return (float(n)/impulse_len) * (rate);
	}
	inline float getT(size_t n) {
		return n/rate;
	}

	//Calc things
	void triggerUpdate();
	
	void run() override;
	void runUpdate();
private:
	fftconvolver::MobileConvolver convolver;
	bool hasCleared;
	bool needsUpdate=false;
	int blockSize;
	std::mutex lock;
protected:
	float* impulse;
	
	float lenSec;
	size_t impulse_len;
	size_t fft_len;
	
	audiofft::AudioFFT fft;
	
	float* re;
	float* im;
	
	int channels;
	
	
	
private:
	SpectrumReverb(const SpectrumReverb& other) = delete;
	SpectrumReverb& operator=(const SpectrumReverb& other) = delete;
	
};

#endif /* SpectrumReverb_h */
