//
//  BarImage.h
//  Tremelon - Shared Code
//
//  Created by Isaac Roberts on 6/13/20.
//

// Created by Isaac Roberts 

#ifndef BarImage_h
#define BarImage_h

#include <stdio.h>

#include <JuceHeader.h>

#include "RTGraphics/CommonLAF.h"

class BarImage : public CommonLAF
{
public:
	BarImage(int64 hashCode);
	
	void drawLinearSlider(Graphics&,  int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider &)override;

	virtual ~BarImage();
	
	void resize(float scale);
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BarImage);
	
	int64 imgHash;
    Image img;//Image is already a pointer-like reference to an image
};

#endif /* BarImage_h */
