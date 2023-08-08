//
//  RTMobileReverb.h
//  
//
//  Created by Isaac Roberts on 1/16/21.
//

// Created by Isaac Roberts 

#pragma once


#include <JuceHeader.h>


#include "RT/FX/FX.h"

#include "../MobileConvolver.h"

#include "ReverbCommon.h"
//#include "../Utilities/RTWaitEvent.h"
//#include "../Utilities/RTJobQueue.h"

using namespace juce;

class RTMobileReverb : public FX
{
public:
	RTMobileReverb() : FX(),
		impulse(NULL),
		impulse_len(0),
		convolver(),
		channels(-1),
		blockSize(-1),
		hasCleared(true)
	{}
	
//	Getters & Setters
	int paramAmt() override {return 0;}
	void set(int n, float set) override {}
	float get(int n) override { return extraGet(); }
	
	void setImpulseLen(size_t set) {
		if (impulse_len != set)
		{
			jassert(!hasBeenPrepared());
			impulse_len = set;
		}
	}

	void createBlankImpulse(float sec);
	void createInterestingImpulse();
	
	void setImpulse(float* data, size_t len);
	void setImpulseFromBinary(const char* binaryData, size_t dataLen);
	void loadImpulseFromFile(File f);
	
	void process(ContextR& context) ;
	void process(ContextNR& context) ;
	
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
	
	virtual ~RTMobileReverb();
	
protected:
	
	void loadImpulse();
	void reloadImpulse();
	
	void normalize();
	
	float* impulse;
	size_t impulse_len;
//	int impulse_channels;
//	ImpulseChannelLayerMode layerMode;
	fftconvolver::MobileConvolver convolver;
	int channels;
	int blockSize;
	bool threaded;
	bool hasCleared;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTMobileReverb);

};
