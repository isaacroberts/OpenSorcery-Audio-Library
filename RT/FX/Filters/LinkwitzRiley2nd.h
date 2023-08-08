//
//  LinkwitzRiley2nd.h
//  
//
//  Created by Isaac Roberts on 4/19/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "LinkwitzRiley.h"
#include "GenericBiquad.h"

class LinkwitzRiley2nd : public LinkwitzRiley
{
public:
	LinkwitzRiley2nd() : LinkwitzRiley2nd(false, 440) {}
	LinkwitzRiley2nd(bool high) : LinkwitzRiley2nd(high, 440) {}
	LinkwitzRiley2nd(bool high, float f)
	  : LinkwitzRiley(high, f),
		filter(),
		hasReset(false)
	{
	}
	
	//==============================================================================
	/** Initialises the filter. */
	void prepare2 (const juce::dsp::ProcessSpec& spec) override
	{
		filter.prepare(spec.numChannels);
		setFreq(freq);
	}
	
	/** Resets the internal state variables of the filter. */
	void reset2() override
	{
		if (!hasReset)
		{
			filter.reset();
			hasReset=true;
		}
	}
	
	void process(ContextR& c) override
	{
		fx_proc_start(c);
		if (status == Filter::Active)
		{
			filter.process(c.getOutput());
			//TODO: I'm not sure if this is covered by the filter code
//			if (isHigh) {
//				//Polarity of high and low are reversed
//				c.getOutput() *= -1;
//			}
			filter.sanCheck();
			hasReset=false;
		}
		else if (status == Filter::Silence)
		{
			c.getOutput().clear();
			reset2();
		}
		else
		{
			c.bypassProcess();
			reset2();
		}
	}
	void process(ContextNR& c) override
	{
		fx_proc_start(c);

		if (status == Filter::Active)
		{
			filter.process(c.getInput(), c.getOutput());
			//TODO: I'm not sure if this is covered by the filter code
//			if (isHigh) {
//				//Polarity of high and low are reversed
//				c.getOutput() *= -1;
//			}
			filter.sanCheck();
			hasReset=false;
		}
		else if (status == Filter::Silence)
		{
			c.getOutput().clear();
			reset2();
		}
		else
		{
			c.bypassProcess();
			reset2();
		}
	}
	
	void bypassProcess(ContextR& c, bypassParams) override
	{
		//TODO: Track input if not isInputSilent
		filter.reset();
	}
	void bypassProcess(ContextNR& c, bypassParams) override
	{
		//TODO: Track input if not isInputSilent
		filter.reset();
	}

	virtual ~LinkwitzRiley2nd()
	{}
	
protected:
	void makeFilter() override {
		Filter::GenericBiquad::Coeffs c;
		
		if (isHigh)
			Filter::GenericBiquad::make_2nd_Order_LR_HiPass(freq, rate, c);
		else
			Filter::GenericBiquad::make_2nd_Order_LR_LoPass(freq, rate, c);
		
		filter.setCoeffs(c);
		filter.sanCheck();
	}
	
	
protected:
	Filter::GenericBiquad filter;
	bool hasReset;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LinkwitzRiley2nd);

};



class LinkwitzRiley2ndHigh : public LinkwitzRiley2nd {
public:
	LinkwitzRiley2ndHigh() : LinkwitzRiley2nd(true) {}
	LinkwitzRiley2ndHigh(float freq) : LinkwitzRiley2nd(true, freq) {}
};
class LinkwitzRiley2ndLow : public LinkwitzRiley2nd {
public:
	LinkwitzRiley2ndLow() : LinkwitzRiley2nd(false) {}
	LinkwitzRiley2ndLow(float freq) : LinkwitzRiley2nd(false, freq) {}
};

typedef LinkwitzRiley2ndHigh LinkwitzRiley2ndHi;
typedef LinkwitzRiley2ndLow  LinkwitzRiley2ndLo;

