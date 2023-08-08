//
//  DbSlider.h
//  RTGraphics - Shared Code
//
//  Created by Isaac Roberts on 6/17/20.
//  Copyright © 2020 Isaac Roberts. All rights reserved.
//


#ifndef DbSlider_h
#define DbSlider_h

#include <stdio.h>

#include <math.h>

#include <JuceHeader.h>

class DbSlider : public Slider
{
public:
	DbSlider() :
		Slider()
	{}
	
	String getTextFromValue(double value) override
	{
		if (value<=.0000001) //epsilon
		{
			return "-INF";
		}
		else {
			double exp = log10(value);
			String str = String(exp, 2, false);
			str += " dB";
			return str;
		}
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DbSlider);
	
	
};

#endif /* DbSlider_h */
