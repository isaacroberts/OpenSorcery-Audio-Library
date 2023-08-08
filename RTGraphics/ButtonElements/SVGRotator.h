//
//  SVGRotator.h
//  
//
//  Created by Isaac Roberts on 7/15/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include <stdio.h>
#include <JuceHeader.h>

#include <math.h>

#include "RTGraphics/CommonLAF.h"

class SVGRotator : public CommonLAF
{
public:
	SVGRotator(const char* fixedTop, const char* rotator, const char* shadow, float startAngle=0);
	
	
	virtual ~SVGRotator();
	
	static Slider::RotaryParameters getAngleSpread(float factorNormalized);
	
	void drawRotarySlider (Graphics &, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &)override;
	

	void resize(float px);
	
//	void setLayout(Slider& s, Slider::SliderLayout set);
//	Slider::SliderLayout getSliderLayout(Slider& s) override;

	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVGRotator);
	
//	int64 imgHash;
//    Image img;//Image is already a pointer-like reference to an image

	std::unique_ptr<Drawable> fixedTop, rotator, fixedShadow;
public:
	float cx, cy;
	float startAngle;
private:
	int scale;
	
//	std::map<Slider*, Slider::SliderLayout> layouts;
	
};
