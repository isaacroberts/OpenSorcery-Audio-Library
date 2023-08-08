//
//  MsgFlasher.h
//  
//
//  Created by Isaac Roberts on 5/9/23.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class MsgFlasher : public juce::Component, juce::Timer
{
public:
	MsgFlasher();
	virtual ~MsgFlasher();
	
	void paint(Graphics& g) override;
	void resized() override;
	
	void flash(String msg, float time);

	void stop();
	
	void setMaxTime(float set) { maxTime = set; }
	void setTextColor(Colour set) {
		textColor = set;
	}
	void setFont(Font set) {
		font = set;
	}
	
protected:
	void timerCallback() override;
	float getElapsedTime();
	void _startOnMessageThread();
	
	virtual float opacityCurve(float tPct);
	
protected:
	String curMsg;
	
	float maxTime;
	float curMaxTime;
	int64 startTime;

	bool flashing;

	float lastOpacity;
	float nextOpacity;
	
public:
	Font font;
	Colour textColor;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MsgFlasher);

};
