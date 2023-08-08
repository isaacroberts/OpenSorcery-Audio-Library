//
//  RTMeter.h
//  
//
//  Created by Isaac Roberts on 8/9/20.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>

/**
 ----- Timer Meter Class -----
 
	callback should be set to a function [float f()] in an audioprocessor
		function should read the current level on the main loop and then store that variable
		get function should only return the stored level
 
 
 */

using namespace juce;
class MeterGaugeBase;

class RTMeter : public juce::Component, private juce::Timer
{
public:
	RTMeter();
	
	void startTimer();
	
	void setMin(float set) {min = set;}
	void setMax(float set) { max = set;}
	void setRange(float mn, float mx) { min=mn; max=mx; }
	//void setMinDisplay()
	void setCallbackInRMS(bool set) {callbackInRMS = set;}
	
	//Member Variable
	std::function<float()> callback;
	
	//Implementation is in MeterGaugeBase to keep includes sane 
	void connectGauge(MeterGaugeBase& g);
	
	void timerCallback() override;
	void paint(juce::Graphics& g) override;
	
	virtual ~RTMeter();
	
	class LAF {
	 public:
		 virtual ~LAF() {}

		 /** This is called to draw the actual numbers and bars on top of the static background */
		 virtual void drawMeter (juce::Graphics&,
									 int x, int y, float relLevel,
									 float level, float min, float max,
									int width, int height,
									const RTMeter* meter) = 0;

	 };
	
//	void setRTMeterLAF(RTMeter::LAF* set) {
//		laf = set;
//	}
//	void setRTMeterLAF(RTMeter::LAF& set) {
//		laf = &set;
//	}


protected:
	float level;
	float min, max;
	bool callbackInRMS;
	
//	RTMeter::LAF *laf;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTMeter);

	
public:
	

};

//Simple default meter
