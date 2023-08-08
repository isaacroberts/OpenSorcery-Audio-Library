//
//  LEDIndicator.h
//  
//
//  Created by Isaac Roberts on 3/30/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

//Creates a circular ShapeButton that toggles between green and grey

class LEDIndicator : public juce::ShapeButton
{
public:
	LEDIndicator()
	: ShapeButton("LED", Colours::green, Colours::lightgreen, Colours::darkgreen)
	{
		Colour bypassOnGreen(0, 153, 0);
		
		setColours(Colours::grey, Colour(120, 120, 120), Colours::darkgrey);
		setOnColours(bypassOnGreen, bypassOnGreen, Colours::darkgreen);
		
		Path circle;
		circle.addEllipse(0, 0, 10, 10);
		setShape(circle, false, false, true);
		
		setClickingTogglesState(true);
		shouldUseOnColours(true);
		setOutline(Colours::black, 1.2f);
	}
	
	virtual ~LEDIndicator()
	{
		
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LEDIndicator);

};
