//
//  Filmstrip.h
//  
//
//  Created by Isaac Roberts on 8/9/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class Filmstrip {
public:
	Filmstrip(int64 imgHash, int numSprites);
	Filmstrip(const char* imgData, int imgDataSize, int numSprites);
	
	virtual bool imagesVertical()=0;
	
	void drawFrame(Graphics& g, int frameNo, int x=0, int y=0);
	
	virtual void resize();
	virtual void resize(float scale);
	
	
	int64 getHash() {
		return hashCode;
	}
	
	virtual ~Filmstrip();
	
	void setOffset(int x, int y) {xOffset = x; yOffset = y; }
	
protected:
	int num_sprites_on_filmstrip;
	float dim;
	
	int xOffset=0;
	int yOffset=0;
	
	bool resizes=false;
	
	int64 hashCode;
	Image img;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Filmstrip);

};
