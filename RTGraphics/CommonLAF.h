//
//  CommonLAF.h
//  Tremelon - Shared Code
//
//  Created by Isaac Roberts on 6/17/20.
//  Copyright © 2020 F-AudioLabs. All rights reserved.
//

// Created by Isaac Roberts 

#ifndef CommonLAF_h
#define CommonLAF_h

#include <stdio.h>

#include <JuceHeader.h>

class CommonLAF : public juce::LookAndFeel_V3
{
public:
	CommonLAF();
	virtual ~CommonLAF();
	
	virtual juce::Font getLabelFont(juce::Label& l)override;
	virtual juce::Font getSliderPopupFont(juce::Slider& s)override;
	
//	void drawBubble (Graphics &g, BubbleComponent& b, const Point<float> &position, const Rectangle<float> & body)override;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CommonLAF);
	
protected:
	
	static juce::Font* textBoxFont;
	static int fontReferences;
};

#endif /* CommonLAF_h */
