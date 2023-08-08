//
//  LinkwitzRiley.h
//  
//
//  Created by Isaac Roberts on 4/19/21.
//

#pragma once

#include <JuceHeader.h>

#include "FilterDefinitions.h"


class LinkwitzRiley : public FX
{
public:
	LinkwitzRiley() : LinkwitzRiley(false, 440) {}
	LinkwitzRiley(bool high) : LinkwitzRiley(high, 440) {}
	LinkwitzRiley(bool high, float f)
	  : freq(f),
		isHigh(high),
		status(Filter::Unprepared)
	{}
	

	virtual void process(ContextR& c) = 0;
	virtual void process(ContextNR& c) = 0;
	
	virtual void bypassProcess(ContextR& c, bypassParams) = 0;
	virtual void bypassProcess(ContextNR& c, bypassParams) = 0;
	
	
	void sidechain(InputBlock& level, ContextR& c) { process(c); }
	void sidechain(InputBlock& level, ContextNR& c) { process(c); }
	
	void setParameters(float f)  {
		setFreq(f);
	}
	
	
	void setFreq(float f) {
		freq = f;
		if (rate > 0)
		{
			if (freq <= 0)
				status = Filter::modeBelowZero(isHigh);
			else if (freq > rate/2)
				status = Filter::modeAboveNyquist(isHigh);
			else
				status = Filter::Active;
			
			safeMakeFilter();
		}
	}
	float getFreq() {
		return freq;
	}
	
	void setHigh() {
		setHigh(true);
	}
	void setLow() {
		setHigh(false);
	}
	void setHigh(bool set) {
		isHigh = set;
		updateType();
	}
	
	
	virtual ~LinkwitzRiley()
	{}
	
protected:
	void safeMakeFilter() {
		if (rate > 0 && status == Filter::Active)
		{
			makeFilter();
		}
	}
	virtual void makeFilter() = 0;
	virtual void updateType() { safeMakeFilter(); }
	
protected:
	float freq;
	bool isHigh;
	Filter::Mode status;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LinkwitzRiley);

};
