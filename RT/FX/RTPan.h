//
//  RTPan.h
//  
//
//  Created by Isaac Roberts on 7/11/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "FX.h"

using namespace juce;

class RTPan : public FX
{
public:
	RTPan();
	RTPan(float pan);
	
	int paramAmt() override {
		return 1;
	}
	enum RTLimiterParams {
		Pan
	};

	//Getters & Setters
	void set(int n, float set) override;
	float get(int n) override;
		
	void prepare2(Spec& spec) override;
	void process(ContextR& context);
	void process(ContextNR& context);
	void reset2() override;
	

	
	template <class Context>
	void bypassProcess(Context& c, bypassParams) {
	}
	
	virtual ~RTPan();
protected:
	dsp::Panner<float> panner;
	float pan;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTPan);

};
