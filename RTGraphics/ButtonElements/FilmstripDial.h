//
//  DialKnob.hpp
//  Tremelon
//
//  Created by Isaac Roberts on 5/30/20.
//

#ifndef DialKnob_hpp
#define DialKnob_hpp

#include <stdio.h>
#include <JuceHeader.h>

#include <math.h>

#include "RTGraphics/CommonLAF.h"
#include "Filmstrip.h"

template<typename Base = juce::LookAndFeel_V4>
class FilmstripDial : public Base, public Filmstrip
{
public:
	FilmstripDial(int64 hashCode, int numSpritesOnFilmstrip)
	: Base(), Filmstrip(hashCode, numSpritesOnFilmstrip) {}
	
	FilmstripDial(const char* imgData, int imgDataSize, int numSprites)
	: Base(), Filmstrip(imgData, imgDataSize, numSprites) {}
	
	bool imagesVertical() override {return true;}
	
	virtual ~FilmstripDial()
	{}
	
	void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &) override
	{
		int frameNo = int (sliderPosProportional * Filmstrip::num_sprites_on_filmstrip) ;
		frameNo = frameNo >= Filmstrip::num_sprites_on_filmstrip ? Filmstrip::num_sprites_on_filmstrip-1 : frameNo;

		drawFrame(g, frameNo, Filmstrip::xOffset+x, Filmstrip::yOffset+y);
	}

//	void createSliderTextBox(Slider &s);
	
//	void setLayout(Slider& s, Slider::SliderLayout set);
//	Slider::SliderLayout getSliderLayout(Slider& s) override;

	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilmstripDial);
	
	int num_sprites_on_filmstrip;
	
	int64 imgHash;
    Image img;//Image is already a pointer-like reference to an image

//	std::map<Slider*, Slider::SliderLayout> layouts;
	
};

#endif /* DialKnob_hpp */
