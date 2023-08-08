//
//  RTGain.h
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/6/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "FX.h"

class RTGain: public FX
{
public:
	RTGain();
	RTGain(float gain);
	
	enum GainParams {
		Gain=0
	};
	
	//Abstract Gettors & Setters
//	virtual void set(int n, float set) override {
//		gain = set;
//	}
//	virtual float get(int n) override {
//		#ifdef DEBUG
//			if (n>=1)
//				return extraGet();
//		#endif
//		return gain;
//	}
//	virtual int paramAmt() override {
//		return 1;
//	}
	void setDecibels(float db) {
		gain = pow(10, db/20);
	}
	void setDb(float db) {
		gain = pow(10, db/20);
	}
	void setAmplitude(float set) {
		gain = set;
	}
	void setAmp(float set) {
		gain = set;
	}
	
	void process(ContextR& context) ;
	void process(ContextNR& context) ;
	
	template <class Context>
	void bypassProcess(Context& c, bypassParams)
	{}
	void sidechain(InputBlock& levels, ContextR&  c) { process(c); }
	void sidechain(InputBlock& levels, ContextNR& c) { process(c); }

	
	virtual ~RTGain();
	
protected:
	float gain;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTGain);

};
