//
//  GUtilities.h
//  
//
//  Created by Isaac Roberts on 2/18/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>


using namespace juce;

namespace Utility
{
	inline bool isChar(const KeyPress& press, char c)
	{
		return press.isKeyCode(int(std::toupper(c)));
	}
	inline bool containsLetters(const KeyPress& press, String chars)
	{
		chars = chars.toUpperCase();
		for (int c=0; c < chars.length(); ++c)
		{
			int i = int(char(chars[c]));
			if (press.isKeyCode(i))
				return true;
		}
		return false;
	}
	inline bool containsRange(const KeyPress& press, int ix1, int ix2)
	{
		jassert(ix1 >= 0 && ix1 < 10);
		jassert(ix2 >= 0 && ix2 < 10);
		return press.getKeyCode() >= ix1 + 48 && press.getKeyCode() <= ix2 + 48;
	}
	inline int getNum(const KeyPress& press)
	{
		return press.getKeyCode() - 48;
	}
	String ratioTextFromValueFunction(double v);
	double ratioValueFromTextFunction(String t);
	
//	String reduceFraction(int num, int denom);
	//As above, but with only 2 and 3 prime factors in denominator
	String reduceFraction2sAnd3s(int num, int denom);

	
	static inline float clockToRad(int hours, int minutes=0)
	{
		if (hours < 6)
			hours += 12;
		
		float time = hours + minutes / 60.0;
		float rad = (time) / 12.0 * M_PI * 2;
		return rad;
	}
	
	static inline juce::Slider::RotaryParameters degFromBottom(float degStart, float degEnd)
	{
		juce::Slider::RotaryParameters param;
		
		while (degEnd < degStart)
		{
			degEnd += 360;
		}
		
		param.startAngleRadians = (degStart + 180) / 180 * M_PI;
		param.endAngleRadians	= (degEnd   + 180) / 180 * M_PI;
		param.stopAtEnd = true;
		return param;
	}
	
};
