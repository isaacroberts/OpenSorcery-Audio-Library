//
//  CommonLAF.cpp
//  Tremelon - Shared Code
//
//  Created by Isaac Roberts on 6/17/20.
//  Copyright © 2020 F-AudioLabs. All rights reserved.
//

// Created by Isaac Roberts

#include "CommonLAF.h"

using namespace juce;

Font* CommonLAF::textBoxFont = NULL;
int CommonLAF::fontReferences = 0;

CommonLAF::CommonLAF()
: LookAndFeel_V3()
{
	if (textBoxFont == NULL)
	{
//		textBoxFont = new Font(Typeface::createSystemTypefaceFor(BinaryData::led_font_ttf, BinaryData::led_font_ttfSize));
		textBoxFont = new Font("Arial", 15., Font::plain);
		textBoxFont->setHeight(15.);
		textBoxFont->setStyleFlags(0);
		fontReferences = 1;
	}
	else
		fontReferences++;

	setColour (BubbleComponent::backgroundColourId, Colour (0,0,0));
//	setColour (BubbleComponent::outlineColourId, Colour(255,255,255));
	setColour (TooltipWindow::textColourId, Colour(255,255,255));
	
}

Font CommonLAF::getLabelFont(Label& l)
{
	return *textBoxFont;
}
Font CommonLAF::getSliderPopupFont(Slider& s)
{
	return *textBoxFont;
}

//void CommonLAF::drawBubble (Graphics &g, BubbleComponent& b, const Point<float> &position, const Rectangle<float> & body)
//{
//
//}

//Destructor
CommonLAF::~CommonLAF()
{
	fontReferences--;
	if (fontReferences <= 0)
	{
		delete textBoxFont;
		textBoxFont = NULL;
	}
}

