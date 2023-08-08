//
//  RigSlider.cpp
//  
//
//  Created by Isaac Roberts on 9/15/21.
//

// Created by Isaac Roberts


#include "RigSlider.h"


RigSlider::RigSlider()
: Slider(), name("TestRig")
{
	setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
}
void RigSlider::setName(String set)
{
	name = set;
	setTooltip(name);
}
void RigSlider::paint(Graphics& g)
{
	Slider::paint(g);
	g.setColour(Colours::white);
	g.drawFittedText(name, 0, 0, getWidth(), 15, Justification::centredTop, 1);
}
void RigSlider::resized()
{
	setTextBoxStyle(Slider::TextBoxBelow, false, getWidth(), 20);
	Slider::resized();
}

RigSlider::~RigSlider()
{
	
}

