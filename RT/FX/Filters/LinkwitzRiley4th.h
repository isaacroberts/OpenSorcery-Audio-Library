//
//  LinkwitzRiley.h
//
//
//  Created by Isaac Roberts on 3/31/21.
//

// Created by Isaac Roberts

#pragma once

#include <JuceHeader.h>

#include "../FX.h"

#include "FilterDefinitions.h"
#include "LinkwitzRiley.h"

using namespace juce;

class LinkwitzRiley4th : public LinkwitzRiley
{
public:
	LinkwitzRiley4th() : LinkwitzRiley4th(false, 440) {}
	LinkwitzRiley4th(bool high) : LinkwitzRiley4th(high, 440) {}
	LinkwitzRiley4th(bool high, float f)
	  :	LinkwitzRiley(high, f),
		filter(),
		hasReset(false)
	{
		updateType();
	}

public:

	//==============================================================================
	/** Initialises the filter. */
	void prepare2 (const ProcessSpec& spec) override
	{
		filter.prepare(spec);
		updateType();
		setFreq(freq);
//		makeFilter();
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
	
	/*
	 JUCE UPDATE
	 goto juce_dsp > processors > juce_LinkwitzRileyFilter.h
	 Add this anywhere in public
	 
	 void sanCheckState() {
		 const float SMAX=1000;
		 for (auto s : { &s1, &s2, &s3, &s4 })
		   for (auto& e : *s)
		   {
			   if (isnan(e)) e=0;
			   else if (e > SMAX) e = SMAX;
			   else if (e <-SMAX) e =-SMAX;
		   }
	 }
	 */
	
	void process(ContextR& c) override
	{
		fx_proc_start(c);
		if (status == Filter::Active)
		{
			filter.process(c.getContext());
			filter.sanCheckState();
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
			filter.process(c.getContext());
			filter.sanCheckState();
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

	
	virtual ~LinkwitzRiley4th()
	{}
	
protected:
	void makeFilter() override {
		filter.setCutoffFrequency (freq);
		filter.sanCheckState();
	}
	void updateType() override {
		if (isHigh)
			filter.setType( juce::dsp::LinkwitzRileyFilterType::highpass );
		else
			filter.setType( juce::dsp::LinkwitzRileyFilterType::lowpass );
	}
	
protected:
	juce::dsp::LinkwitzRileyFilter<float> filter;
	bool hasReset;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LinkwitzRiley4th);

};

class LinkwitzRiley4thHigh : public LinkwitzRiley4th {
public:
	LinkwitzRiley4thHigh() : LinkwitzRiley4th(true) {}
	LinkwitzRiley4thHigh(float freq) : LinkwitzRiley4th(true, freq) {}
};
class LinkwitzRiley4thLow : public LinkwitzRiley4th {
public:
	LinkwitzRiley4thLow() : LinkwitzRiley4th(false) {}
	LinkwitzRiley4thLow(float freq) : LinkwitzRiley4th(false, freq) {}
};

typedef LinkwitzRiley4thHigh LinkwitzRiley4thHi;
typedef LinkwitzRiley4thLow  LinkwitzRiley4thLo;

