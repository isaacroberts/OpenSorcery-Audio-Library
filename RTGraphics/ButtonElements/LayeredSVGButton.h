//
//  LayeredSVGButton.h
//  
//
//  Created by Isaac Roberts on 6/6/22.
//

// Created by Isaac Roberts 

#pragma once


#include "SVGButton.h"


class LayeredSVGButton : public SVGButton
{
public:
	LayeredSVGButton() : SVGButton() {}
	LayeredSVGButton(String name) : SVGButton(name) {}
	LayeredSVGButton(String name, DrawableButton::ButtonStyle style) : SVGButton(name, style) {}
	virtual ~LayeredSVGButton() {}

	void loadBelow(const char* data, size_t size);
	void loadAbove(const char* data, size_t size);

	void loadBelowDown(const char* data, size_t size);
	void loadAboveDown(const char* data, size_t size);

	
	void setBelow(Image i) { below = i; }
	void setAbove(Image i) { above = i; }
	void setBelowDown(Image i) { belowDown = i; }
	void setAboveDown(Image i) { aboveDown = i; }

	
	void paint(Graphics& g) override;
	
protected:
	void drawIm(Graphics& g, Image& i);
	
protected:
	Image below, above;
	Image belowDown, aboveDown;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayeredSVGButton);

};

struct LayeredSVGButtonLeftAligned : public LayeredSVGButton {
	Rectangle<float> getImageBounds() const override;
};
