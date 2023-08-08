//
//  FilmstripBar.cpp
//  
//
//  Created by Isaac Roberts on 8/8/20.
//

// Created by Isaac Roberts


#include "FilmstripBar.h"

#include "UniqueHashCoder.h"

using namespace juce;

FilmstripBar::FilmstripBar(int64 hashCode, int numSprites, bool horiz)
	: CommonLAF(), Filmstrip(hashCode, numSprites),
	horizontal(horiz)
{
}
FilmstripBar::FilmstripBar(const char* imgData, int imgSize, int numSprites, bool horiz)
: CommonLAF(), Filmstrip(imgData, imgSize, numSprites),
horizontal(horiz)
{
}
FilmstripBar::~FilmstripBar()
{}
void FilmstripBar::drawLinearSlider (Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle orientation, Slider &s)
{
	int frameNo;
	
	jassert (horizontal == (orientation == Slider::LinearHorizontal || orientation == Slider::LinearBar));
	
	if (horizontal)
		frameNo = int(((width-(sliderPos-x)) / width) * (num_sprites_on_filmstrip-1));
	else
		frameNo = int(((height-(sliderPos-y)) / height) * (num_sprites_on_filmstrip-1));
	
//	std::cout<<"horiz"<<horizontal<<" "<<frameNo<<std::endl;
	
	drawFrame(g, frameNo);
}
