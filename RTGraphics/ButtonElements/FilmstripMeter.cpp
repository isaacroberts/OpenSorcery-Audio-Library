//
//  FilmstripMeter.cpp
//  
//
//  Created by Isaac Roberts on 8/9/20.
//

// Created by Isaac Roberts


#include "FilmstripMeter.h"


FilmstripMeter::FilmstripMeter(int64 hashCode, int numSprites)
	: Filmstrip(hashCode, numSprites)
{
}
FilmstripMeter::FilmstripMeter(const char* imgData, int imgSize, int numSprites)
	: Filmstrip(imgData, imgSize, numSprites)
{
}
FilmstripMeter::~FilmstripMeter()
{
}

void FilmstripMeter::drawMeter (juce::Graphics& g,
		int x, int y, float percent,
		float level, float min, float max,
		int width, int height,
		const RTMeter* meter)
{
	int frameNo = (percent * num_sprites_on_filmstrip);
	if (frameNo < 0)
		frameNo = 0;
	if (frameNo >= num_sprites_on_filmstrip)
		frameNo = num_sprites_on_filmstrip - 1;
	
//	std::cout<<level<<"/"<<max<<"  frameNo : "<<frameNo<<std::endl;
	
	drawFrame(g, frameNo);
}

