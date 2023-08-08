//
//  FilmstripMeter.h
//  
//
//  Created by Isaac Roberts on 8/9/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

//#include "../RTMeter.h"

#include "Filmstrip.h"

using namespace juce;

class FilmstripMeter : public RTMeter::LAF, public Filmstrip
{
public:
	FilmstripMeter(int64 hashCode, int numSprites);
	FilmstripMeter(const char* imgData, int imgSize, int numSprites);

	bool imagesVertical() override {return false;}
	
	void drawMeter (juce::Graphics&,
			int x, int y, float rel,
			float level, float min, float max,
			int width, int height,
			const RTMeter* meter) override;

	
	
	virtual ~FilmstripMeter();
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilmstripMeter);

};
