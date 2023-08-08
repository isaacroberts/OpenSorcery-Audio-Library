//
//  SVGButton.h
//  
//
//  Created by Isaac Roberts on 1/24/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class SVGButton : public juce::DrawableButton
{
public:
	SVGButton();
	SVGButton(String name);
	SVGButton(String name, DrawableButton::ButtonStyle style);
	
	void construct();
	
	void loadSVG(const char* data, size_t size);
	
	void loadSVG(const char* normal, 		size_t normal_size,
				 const char* down,	 		size_t down_size,
				 const char* hover=NULL, 	size_t hover_size=0,
				 const char* disabled=NULL, size_t disabled_size=0);
	
	void loadToggleSVG(const char* normal, 	size_t normal_size,
					   const char* down,	size_t down_size);
//				 const char* hover=NULL, 	size_t hover_size=0,
//				 const char* disabled=NULL, size_t disabled_size=0);

	
	void loadToggleSVG(const char* off, 		size_t offSize,
					   const char* on,			size_t onSize,
					   const char* hoverOff,	size_t hoverOffSize,
					   const char* hoverOn,		size_t hoverOnSize,
					   const char* pressedOff,	size_t pressedOffSize,
					   const char* pressedOn,	size_t pressedOnSize,
					   const char* disabledOff,	size_t disabledOffSize,
					   const char* disabledOn,	size_t disabledOnSize
					   );
	

	void loadSVGToggles(const char* off, size_t off_size,
						const char*  on, size_t on_size) {
		loadToggleSVG(off, off_size, on, on_size);
	}

	
	void setToggleImages(Drawable* up, Drawable* down);
	
	virtual ~SVGButton();
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVGButton);

};

struct SVGButtonLeftAligned : public SVGButton {
	Rectangle<float> getImageBounds() const override;
};
