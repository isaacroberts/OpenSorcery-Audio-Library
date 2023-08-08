//
//  LayeredSVGButton.cpp
//  
//
//  Created by Isaac Roberts on 6/6/22.
//

// Created by Isaac Roberts


#include "LayeredSVGButton.h"





void LayeredSVGButton::loadBelow(const char* data, size_t size)
{
	below = juce::ImageFileFormat::loadFrom(data, size);
}
void LayeredSVGButton::loadAbove(const char* data, size_t size)
{
	above = juce::ImageFileFormat::loadFrom(data, size);
}
void LayeredSVGButton::loadBelowDown(const char* data, size_t size)
{
	belowDown = juce::ImageFileFormat::loadFrom(data, size);
}
void LayeredSVGButton::loadAboveDown(const char* data, size_t size)
{
	aboveDown = juce::ImageFileFormat::loadFrom(data, size);
}
void LayeredSVGButton::drawIm(Graphics& g, Image& i)
{
	//TODO: This will squash the mf
	g.drawImage(i, 0, 0, getWidth(), getHeight(), 0, 0, i.getWidth(), i.getHeight());
}
void LayeredSVGButton::paint(Graphics& g)
{
	bool down = isDown();
	
	if (down && belowDown.isValid())
		drawIm(g, belowDown);
	else if (below.isValid())
		drawIm(g, below);

	Button::paint(g);
	
	if (down && aboveDown.isValid())
		drawIm(g, aboveDown);
	else if (above.isValid())
		drawIm(g, above);
}


Rectangle<float> LayeredSVGButtonLeftAligned::getImageBounds() const
{
	auto r = getLocalBounds();
	
	if (getStyle() != ImageStretched)
	{
		auto indentX = jmin (getEdgeIndent(), proportionOfWidth  (0.3f));
		auto indentY = jmin (getEdgeIndent(), proportionOfHeight (0.3f));
		
		if (getStyle() == ImageOnButtonBackground || getStyle() == ImageOnButtonBackgroundOriginalSize)
		{
			indentX = jmax (getWidth()  / 4, indentX);
			indentY = jmax (getHeight() / 4, indentY);
		}
		else if (getStyle() == ImageAboveTextLabel)
		{
			r = r.withTrimmedBottom (jmin (16, proportionOfHeight (0.25f)));
		}
		
		r = r.reduced (indentX, indentY);
	}
	r.setWidth(r.getHeight());
	
	return r.toFloat();
	
}
