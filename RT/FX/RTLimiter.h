//
//  RTBendCompressor.h
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/6/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "FX.h"

class RTLimiter : public FX
{
public:
	RTLimiter();
	RTLimiter(float gain, float thresh, float ratio);
	
	int paramAmt() override {
		return 3;
	}
	
	enum RTLimiterParams : int {
		Gain, Thresh, Ratio
	};
	
	//Getters & Setters
	void set(int n, float set) override {
		if (n==Gain)
			gain = set;
		else if (n==Thresh)
			thresh = set;
		else if (n==Ratio)
			ratio = set;
	}
	float get(int n) override {
		if (n==Gain)
			return gain;
		if (n==Thresh)
			return thresh;
		if (n==Ratio)
			return ratio;
		else return extraGet();
	}
	//Amp
	void setThresh(float set) {
		thresh = set;
	}
	
	void setRatio(float set) {
		ratio = set;
	}
	//Amp
	void setGain(float set) {
		gain = set;
	}

	void process(ContextR& context);
	void process(ContextNR& context) {
		jassertfalse;
	}
	
	void sidechain(InputBlock& levels, ContextR& context) {
		jassertfalse;
	}
	void sidechain(InputBlock& levels, ContextNR& context) {
		jassertfalse;
	}

	
	template <class Context>
	void bypassProcess(Context& c, bypassParams) {
	}
	
	virtual ~RTLimiter();
protected:
	float gain, thresh, ratio;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTLimiter);

};
