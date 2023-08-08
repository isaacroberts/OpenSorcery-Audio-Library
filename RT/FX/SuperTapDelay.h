//
//  SuperTapDelay.h
//  MagicVocals - Shared Code
//
//  Created by Isaac Roberts on 7/9/20.
//  Copyright © 2020 Sweetwater. All rights reserved.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "FX.h"
//#include "Filters/RTFilters.h"
#include "Filters/RTButterworth.h"

#include "RTPan.h"
#include "MinorPitchShifter.h"
#include "../BPMMonitor.h"

typedef dsp::DelayLine<float, DelayLineInterpolationTypes::Linear> DspDelayLine;

// -- Single Element --
class SingleTapDelay : public FX
{
public:
	SingleTapDelay(float delay, float shift, float gain, float freq, float pan);
	SingleTapDelay();
	
	enum SuperTapParams {
		Delay, Shift, Gain, Freq, Pan
	};

	void setBPM(double bpm);
	void setNoBPM();
	
	//Getters & Setters
	int paramAmt() override {return 5;}
	void set(int n, float set) override;
	float get(int n) override;
	
	void setDelay(float set);
	
	void prepare2(Spec& spec) override;
	void process (ContextR&);
	void process(ContextNR& context) { jassertfalse; }//TODO:
	void reset2() override;
	
	void clear();
	
	inline float getGain() { return gain; }
	
	void enableShift() { allocate_shift = true; }
	
	~SingleTapDelay();
	
protected:
	dsp::ProcessSpec savedSpec;
	RTButterLowPassNoSmooth filter;
	RTPan pan;
	MinorPitchShifter shifter;
	DspDelayLine delayLine;
	
	float delay, gain;
		
	bool allocate_shift;
	float bpmFactor;
	
};

// -- Batch Store --
class SuperTapDelay : public FX, public BPMMonitor::Listener
{
public:
	SuperTapDelay();
	
//	void setBPM(double set) override;
	void bpmChanged(double bpm, int num, int den) override;
	void setNoBPM();

	//Getters & Setters
	int paramAmt() override {return delayAmt * 5;}
	void set(int n, float set) override;
	float get(int n) override;
	
	void prepare2(Spec& spec) override;
	void process(ContextR& context);
	void process(ContextNR& context) { jassertfalse; }//TODO:
	void reset2() override;
	
	void clear();
	
	void bypassProcess(ContextR& c, bypassParams);
	void bypassProcess(ContextNR& c, bypassParams);
	
	float getTotalGain() {
		float total = 0;
		for (int n=0; n < delayAmt; ++n)
			total += delays[n]->getGain();
		return total;
	}
	
	virtual ~SuperTapDelay();
	
	void setDelayAmt(int n);
	void addDelay(SingleTapDelay* add);
	
protected:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SuperTapDelay);

	int delayAmt;
	SingleTapDelay** delays;
	bool hasCleared;
};

