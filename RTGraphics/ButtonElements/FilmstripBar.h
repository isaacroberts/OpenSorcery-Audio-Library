//
//  VerticalFilmstrip.h
//  
//
//  Created by Isaac Roberts on 8/8/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "Filmstrip.h"

class FilmstripBar : public CommonLAF, public Filmstrip
{
public:
	FilmstripBar(int64 hashCode, int numSprites, bool horiz);
	FilmstripBar(const char* imgData, int imgSize, int numSprites, bool horiz);
	
	bool imagesVertical() override {return horizontal;}
	
	virtual ~FilmstripBar();
	
	void drawLinearSlider (Graphics &, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider &) override;
	
	
protected:
	bool horizontal;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilmstripBar);

};
