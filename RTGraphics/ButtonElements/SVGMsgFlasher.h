//
//  SVGMsgFlasher.h
//  
//
//  Created by Isaac Roberts on 6/7/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class SVGMsgFlasher : public juce::Component, juce::Timer
{
public:
	struct SVGMsg {
		SVGMsg() : svg(nullptr), txt("") {}
		void load(juce::String msg, const char* data, size_t size);
		std::unique_ptr<juce::Drawable> svg=nullptr;
		juce::String txt;
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SVGMsg);
	};
	
	SVGMsgFlasher(int messageAmt);
	virtual ~SVGMsgFlasher();
	
	SVGMsg& getMsg(int n) {
		jassert (n < msgAmt);
		return msgs[n];
	}
	
	void paint(Graphics& g) override;
	void resized() override;
	
	void flashNext() {
		static int cur=0;
		flash(cur);
		cur = (cur+1) % msgAmt;
	}
	
	void flashRand() { flash(rand()%msgAmt); }
	void flash(int msg) { flash(msg, maxTime); }
	void flash(int msg, float time);

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
	int msgAmt;
	SVGMsg* msgs;
	float maxTime;
	float curMaxTime;
	int64 startTime;
//	float time;
	int curMsg;
	bool flashing;
	Rectangle<int> txtBounds;
	Rectangle<float> svgBounds;
	float lastOpacity;
	float nextOpacity;
	
public:
	Font font;
	Colour textColor;
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVGMsgFlasher);

};
