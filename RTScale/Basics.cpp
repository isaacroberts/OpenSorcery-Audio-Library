//
//  Basics.cpp
//  
//
//  Created by Isaac Roberts on 5/15/22.
//

// Created by Isaac Roberts


//#include "Basics.h"

//#include <JuceHeader.h>

namespace Scale
{
	
	
//	constexpr char* note_names[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
	
	
	std::string getNoteName(int n)
	{
		int o = oct12(n);
		n = mod12(n);
		return std::string(note_names[n]) + std::to_string(o);
	}
	
	
	std::string getSpacedNoteName(int n)
	{
		int o = oct12(n);
		n = mod12(n);
		return std::string(note_names[n]) + " "+ std::to_string(o);
	}
	
	bool matchesNote(juce::String s, int i)
	{
		return s.startsWithIgnoreCase(note_names[i]);
	}
	
	int getNoteComponent(juce::String s)
	{
		for (int n=0; n < 5; ++n)
		{
			if (matchesNote(s, darkInterval[n])) return darkInterval[n];
		}
		for (int n=0; n < 7; ++n)
		{
			if (matchesNote(s, interval[n])) return interval[n];
		}
		return -1;
	}
	
	int int7Atoi(juce::String str)
	{
		int base=0;
		bool anyInt=false;
		
		for (int i=0; i < str.length(); ++i)
		{
			if (str[i]==' ')
			{}
			else if (str[i] >= '0' && str[i] <= '9')
			{
				base = 10 * base + (str[i] - '0');
				if (base > 127)
					return base;
				anyInt = true;
			}
			else
				return -1;
			++i;
		}
		if (!anyInt)
			return -1;
		return base ;

	}
	
	int getNoteNumber(juce::String name, bool allowPureNumber)
	{
		int nC = getNoteComponent(name);
		if (nC==-1)
		{
			if (allowPureNumber)
			{
				return int7Atoi(name);
			}
			else
				return -1;
		}
		else
		{
			int o =name.getTrailingIntValue();
			int n = o*12 + nC;
			if (n<0 || n>127)
				return -1;
			return n;
		}
	}
	
	
	
	bool isPitchWhite(int n)
	{
		n = mod12(n);
		return modeNum[n]>=0;
		/*
		// c c# d d# e f f# g g# a a# b
		// 1 0  1 0  1 1 0  1 0  1 0  1
		//101011010101
		//Reversed: 101010110101
		const int scale = 0b101010110101;
		return ((1<<n) & scale) > 0;
		 */
	}

	
	int getWhiteNote(int iv)
	{
		jassert(juce::isPositiveAndBelow(iv, 7));
		return interval[iv];
	}
	int getBlackNote(int iv)
	{
		jassert(juce::isPositiveAndBelow(iv, 5));
		return darkInterval[iv];
	}

	int intervalToNote(int iv)
	{
		int o = oct7(iv);;
		iv = mod7(iv);
		return interval[iv] + o * 12;
	}
	int darkIntervalToNote(int iv)
	{
		int o = oct5(iv);;
		iv = mod5(iv);
		return darkInterval[iv] + o * 12;
	}
	//Returns Iv + Oct*7
	int noteToIntervalOct(int note)
	{
		int o = oct12(note);
		note = mod12(note);
		return modeNum[note] + o * 7;
	}
	//Returns Iv, Oct
	int noteToInterval(int note, int& oct)
	{
		oct = oct12(note);
		note = mod12(note);
		return modeNum[note];
	}
	//Returns Iv
	int noteToInterval(int note)
	{
		note = mod12(note);
		return modeNum[note];
	}
	
	
	
	bool previousKeyWasWhite(int iv)
	{
		jassert(juce::isPositiveAndBelow(iv, 7));
		switch(iv) {
			case 0:
			case 3:
				return true;
		}
		return false;
	}
	bool nextKeyIsWhite(int iv)
	{
		jassert(juce::isPositiveAndBelow(iv, 7));
		switch (iv) {
			case 2:
			case 6:
				return true;
		}
		return false;
	}


}
