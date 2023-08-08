//
//  FlagBox.h
//  KeyFinder
//
//  Created by Isaac Roberts on 10/4/21.
//  Copyright © 2021 Algorithmic Design. All rights reserved.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>
//
//using namespace juce;

class FlagBox {
public:
	FlagBox();
	
	void setFlags(int set) {}
	int getFlags() { return flags; }
	
	virtual ~FlagBox();
	
	//lambda functions onChange
	std::function<void(int flags)> onChange;
protected:
	virtual int getNumFlagsSet() { return 0; }
	
	
	
protected:
	int flags;
	//Scroller/Marquee - no viewport
	//Scrollbar
	//Button
	juce::PopupMenu popup;
	juce::Justification justification;
	
	bool allowScroll, useMarquee;
	
	int maxFlags=-1;
	
	//Optional button to clear
	
	
	
	
public:
	struct LookAndFeelMethods {
		virtual ~LookAndFeelMethods() {}
		//LookAndFeel Methods
		virtual void drawFlagBoxBackground(juce::Rectangle<float> bounds, int numFlagsSet, FlagBox& f) {}
		virtual void drawFlagBox(int flags, juce::Rectangle<float> bounds, float offset, int numSet, FlagBox& f) {}
		virtual void drawFlagBoxPopupBackground() {}
		virtual void drawFlagBoxPopupItem() {}
	};
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlagBox);

};
