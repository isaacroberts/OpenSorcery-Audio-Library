//
//  RigSlider.h
//  
//
//  Created by Isaac Roberts on 9/15/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class RigSlider : public Slider
{
public:
	RigSlider();
	void setName(String name);
	
	void paint(Graphics& g) override;
	void resized() override;
	
	virtual ~RigSlider();
	
public:
	String name;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RigSlider);

};
