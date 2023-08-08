//
//  RTDelay.h
//  
//
//  Created by Isaac Roberts on 5/20/21.
//

#pragma once

#include <JuceHeader.h>

//using namespace juce;

#include "FX.h"

class RTDelay : public FX
{
public:
	RTDelay(int maxDelay) : FX(),
	delay(maxDelay),
	delaySeconds(0)
	{
		
	}
	RTDelay() : FX(),
	delay(10000),
	delaySeconds(0)
	{
		
	}
	virtual ~RTDelay() {}
	
	//Override
	//FX Stuff
	void prepare2(Spec& spec) override {
		delay.prepare(spec);
		
		if (delaySeconds < 0) {
			delaySeconds = delay.getDelay() / rate;
		}
		else {
			setDelaySeconds(delaySeconds);
		}
	}
	template <class Context>
	void process (Context& c)  {
		fx_proc_start(c);
		delay.process(c.getContext());
	}
	
	template <class Context>
	void bypassProcess(Context& c, bypassParams)
	{
		//TODO: Actually propagate delayLine
		delay.reset();
	}

	
	
	void reset2() override {
		delay.reset();
	}
	
	//Getters & Setters
	int paramAmt() override { return 1; }
	void set(int n, float set) override {
		setDelaySeconds(set);
	}
	float get(int n) override {
		return delaySeconds;
	}
	
	void setDelaySamples(int set) {
		delay.setDelay(set);
		if (rate > 0)
			delaySeconds = set / rate;
		else {
			delaySeconds = -1;
		}
	}
	void setDelaySeconds(float set) {
		delaySeconds = set;
		if (rate > 0) {
			int samp =  delaySeconds * rate;
			delay.setDelay(samp);
		}
	}
	
protected:
	juce::dsp::DelayLine<float> delay;
	float delaySeconds;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTDelay);

};
