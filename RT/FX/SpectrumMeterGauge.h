//
//  SpectrumMeterGauge.h
//  
//
//  Created by Isaac Roberts on 9/17/22.
//

// Created by Isaac Roberts 

#ifndef SpectrumMeterGauge_h
#define SpectrumMeterGauge_h

#include "FX.h"


class SpectrumMeterGauge : public FX
{
public:
	SpectrumMeterGauge();
	
	virtual ~SpectrumMeterGauge();
	
	//Default order = 11
	void setOrder(int order);
	void setScopeSize(int n);
//	//FX Stuff
	void prepare2(Spec& spec) override;
	
	void process(ContextR&);
	void process(ContextNR&);
	void reset2() override;
	
	void bypassProcess(ContextR& c, bool allowTail, bool isInputSilent) {
		commonProcess(c.getOutput());
	}
	void bypassProcess(ContextNR& c, bool allowTail, bool isInputSilent) {
		commonProcess(c.getInput());
	}
	
	void sidechain(InputBlock& levels, ContextR& c) {process(c); }
	void sidechain(InputBlock& levels, ContextNR& c) {process(c); }
	
	int getNumFreqs();
	float getValue(int n);
	float getValueDb(int n, float min=-100, float max=10);
	float getScaledDb(int n, float min=-100, float max=10);

	inline float operator [](int n) { return getValue(n); }
	
	
	float getFrequency(int n);
	//Log2 of Freq
	float getL2Freq(int n);
	//Log2 of Freq, scaled from 0 and 1
	float getScaledL2Freq(int n, float minF=10);
	bool isDrawReady();
	void startDraw();
	void markDrawCompleted();
	
	
protected:
	void pushNextSampleIntoFifo (float sample) noexcept;
	void commonProcess(InputBlock&);

protected:
	int fftOrder = 11;
//	int scopeSize = 512;
	int fftSize;
	
	juce::dsp::FFT forwardFFT;                      // [4]
	juce::dsp::WindowingFunction<float> window;     // [5]

	float* fifo = NULL;                           // [6]
	float* fftData= NULL;                    // [7]
	int fifoIndex = 0;                              // [8]
	bool nextFFTBlockReady = false;                 // [9]
//	float* scopeData=NULL;                    // [10]

private:
	SpectrumMeterGauge(const SpectrumMeterGauge& other)=delete;
	SpectrumMeterGauge& operator=(const SpectrumMeterGauge& other)=delete;
	
};

#endif /* SpectrumMeterGauge_h */
