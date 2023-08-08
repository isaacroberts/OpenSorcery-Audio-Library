//
//  RTReverb.h
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/4/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "../FX.h"


class RTJuceReverb : public FX
{
public:
	RTJuceReverb();
//	RTJuceReverb(dsp::Convolution&& conv);

//	RTJuceReverb(const void* data, size_t dataSize, bool normalize=true);
	
//	Getters & Setters
	int paramAmt() override {return 0;}
	void set(int n, float set) override {}
	float get(int n) override { return extraGet(); }
		
	void loadImpulseResponse(const void* data, size_t dataSize, bool normalize);
	
	inline void setImpulseFromBinary(const char* binaryData, size_t dataLen, bool normalize=true) {
		loadImpulseResponse(binaryData, dataLen, normalize);
	}
//	void setImpulse(const float* data, size_t len);
	void loadImpulseFromFile(File f, bool stereo=true, bool normalize=true);
//	void setImpulse(const float** data, size_t len, int numChannels);
//	void setImpulseChannel(const float* data, size_t len, int c, int totalC);

	
	void process(ContextR& context) ;
	void process(ContextNR& context) ;
	
	void prepare2(const ProcessSpec& spec)override;
	
	
	void bypassProcess(ContextR& , bypassParams) ;
	void bypassProcess(ContextNR&, bypassParams) ;
	
	void sidechain(InputBlock& levels, ContextR& c) {process(c); }
	void sidechain(InputBlock& levels, ContextNR& c) {process(c); }

	
	virtual ~RTJuceReverb();
	
protected:
	dsp::Convolution conv;
	bool hasReset;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTJuceReverb);

};



//class JuceReverbWithThread : public RTJuceReverb
//{
//public:
//	JuceReverbWithThread()
//	:
//
//
//};
