//
//  BasicMeterLAF.cpp
//  
//
//  Created by Isaac Roberts on 11/21/22.
//

// Created by Isaac Roberts

#include "BasicMeterLAF.h"



BasicMeterLAF::BasicMeterLAF(Colour color, bool isVert)
: LookAndFeel_V4(),
col(color),
bg(Colours::black),
outline(Colours::black),
vertical(isVert)
{
}
BasicMeterLAF::BasicMeterLAF(bool vert)
: LookAndFeel_V4(),
col(Colours::green),
bg(Colours::black),
outline(Colours::black),
vertical(vert)
{
}
BasicMeterLAF::BasicMeterLAF()
: LookAndFeel_V4(),
col(Colours::green),
bg(Colours::black),
outline(Colours::black),
vertical(true)
{
}

void BasicMeterLAF::drawMeter (juce::Graphics& g, int x, int y, float percent,
					float level, float min, float max,
					int width, int height, const RTMeter* meter)
{
		
	if (vertical)
	{
		int hh = int(height * percent);
		g.setColour(bg);
		g.fillRect(0, hh, width, height-hh);

		g.setColour(col);
		g.fillRect(0, height-hh,  width, hh);
	}
	else
	{
		int ww = int(width * percent);
		g.setColour(bg);
		g.fillRect(ww, 0, width-ww, height);
		g.setColour(col);
		g.fillRect(0, 0, ww, height);
	}
	g.setColour(outline);
	g.drawRect(0, 0, width, height);
}
BasicMeterLAF::~BasicMeterLAF()
{
	
}
