//
//  RTSingleReverb.h
//  
//
//  Created by Isaac Roberts on 3/19/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "RT/FX/FX.h"

#include "ReverbCommon.h"

#include "../TwoStageFFTConvolver.h"

using namespace juce;


template <bool MULTICHANNEL>
class RTSingleReverb : public FX
{
public:
	
	enum ImpulseChannelLayerMode {
		Repeat, RepeatLast
	};
	
	void setChannelLayerMode(ImpulseChannelLayerMode mode) {
		layerMode = mode;
	}
	
	RTSingleReverb() : FX(),
		impulse(NULL),
		impulse_len(0),
		impulse_channels(0),
		layerMode(Repeat),
		convolver(NULL),
		channels(-1),
		blockSize(-1),
		hasCleared(true)
	{}
	
//	Getters & Setters
	int paramAmt() override {return 0;}
	void set(int n, float set) override {}
	float get(int n) override { return extraGet(); }
	
	void setImpulseFromBinary(const char* binaryData, size_t dataLen);
	void setImpulse(const float* data, size_t len);
	void setImpulse(const float** data, size_t len, int numChannels);
	void setImpulseChannel(const float* data, size_t len, int c, int totalC);
	
	void process(ContextR& context) ;
	void process(ContextNR& context)  {
		jassertfalse;
	}
	
	void prepare2(const ProcessSpec& spec)override;
	
	void clear();
	
	//TODO: Allow tail
	void bypassProcess(ContextR&, bypassParams )  {
		if (!hasCleared)
			clear();
	}
	void bypassProcess(ContextNR&, bypassParams)  {
		if (!hasCleared)
			clear();
	}
	
	virtual ~RTSingleReverb();
	
protected:
	void loadImpulse();
	void reloadImpulse();
	
	const float** impulse;
	size_t impulse_len;
	int impulse_channels;
	ImpulseChannelLayerMode layerMode;
	fftconvolver::FFTConvolver* convolver;
	int num_threads;
	int channels;
	int blockSize;
	bool hasCleared;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTSingleReverb);

};
