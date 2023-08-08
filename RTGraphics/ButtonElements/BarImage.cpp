//
//  BarImage.cpp
//  Tremelon - Shared Code
//
//  Created by Isaac Roberts on 6/13/20.
//

// Created by Isaac Roberts

#include "BarImage.h"



BarImage::BarImage(int64 hashCode)
:
CommonLAF()
{
	imgHash = hashCode;
}

//Destructor
BarImage::~BarImage()
{
	
}
void BarImage::drawLinearSlider(Graphics& g,  int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider & slider)
{
//	std::cout<<sliderPos<<" / ("<<minSliderPos<<", "<<maxSliderPos<<std::endl;
	int px = sliderPos - img.getWidth()/2;
//	std::cout<<px<<std::endl;
    g.drawImageAt( img, px, y );

}

void BarImage::resize(float scale)
{
	//Re-get from cache to prevent low resolution from multiple rescales
	img = ImageCache::getFromHashCode(imgHash);
	//Rescale to size * scale
	int width = int(scale * img.getWidth());
	int height = int(scale * img.getHeight());
	img = img.rescaled(width,height);
}

