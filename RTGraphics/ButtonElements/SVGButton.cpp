//
//  SVGButton.cpp
//  
//
//  Created by Isaac Roberts on 1/24/21.
//

// Created by Isaac Roberts


#include "SVGButton.h"


SVGButton::SVGButton()
: DrawableButton("SVGButton", DrawableButton::ImageFitted)
{
	construct();
}
SVGButton::SVGButton(String name)
: DrawableButton(name, DrawableButton::ImageFitted)
{
	construct();
}
SVGButton::SVGButton(String name, DrawableButton::ButtonStyle style)
: DrawableButton(name, style)
{
	construct();
}
void SVGButton::construct()
{
	setColour(DrawableButton::backgroundColourId,   Colours::transparentBlack);
	setColour(DrawableButton::backgroundOnColourId, Colours::transparentBlack);
}

Drawable* getDraw(const char* data, size_t size)
{
	if (data==NULL)
		return nullptr;
	return Drawable::createFromImageData(data, size).release();
}

void SVGButton::loadSVG(const char* data, size_t size)
{
	auto drawable = getDraw(data, size);
	setImages(drawable);
	delete drawable;
}

void SVGButton::loadSVG(const char* data,  size_t size,
						const char* down,  size_t down_size,
						const char* hover, size_t hover_size,
						const char* dis, size_t dis_size)
{
	auto dd = getDraw(data, size);
	auto dh = getDraw(hover, hover_size);
	auto dn = getDraw(down, down_size);
	auto ds = getDraw(dis, dis_size);
	setImages(dd, dh, dn, ds);
	delete dd;
	delete dh;
	delete dn;
	delete ds;
}

void SVGButton::loadToggleSVG(const char* data,  size_t size,
						const char* down_d,  size_t down_size)
{
	auto normal = getDraw(data, size);
	auto down   = getDraw(down_d, down_size);

	setClickingTogglesState(true);
			
	setImages(normal, normal, normal, normal,
			  down, down, down, down);
	delete normal;
	delete down;
}
void SVGButton::loadToggleSVG(const char* off, 		size_t offSize,
				   const char* on,			size_t onSize,
				   const char* hoverOff,	size_t hoverOffSize,
				   const char* hoverOn,		size_t hoverOnSize,
				   const char* pressedOff,	size_t pressedOffSize,
				   const char* pressedOn,	size_t pressedOnSize,
				   const char* disabledOff,	size_t disabledOffSize,
				   const char* disabledOn,	size_t disabledOnSize
				   )
{
	/*
	 void DrawableButton::setImages (const Drawable* normal,
	 const Drawable* over,
	 const Drawable* down,
	 const Drawable* disabled,
	 const Drawable* normalOn,
	 const Drawable* overOn,
	 const Drawable* downOn,
	 const Drawable* disabledOn)
	 */
	setClickingTogglesState(true);
	
	Drawable* offI 		= getDraw(off, offSize);
	Drawable* over 		= getDraw(hoverOff, hoverOffSize);
	Drawable* down 		= getDraw(pressedOff, pressedOffSize);
	Drawable* disabled 	= getDraw(disabledOff, disabledOffSize);
	Drawable* normalOn 	= getDraw(on, onSize);
	Drawable* overOn 	= getDraw(hoverOn, hoverOnSize);
	Drawable* downOn 	= getDraw(pressedOn, pressedOnSize);
	Drawable* disOn		= getDraw(disabledOn, disabledOnSize);
	
	setImages(offI, over, down, disabled,
			  normalOn, overOn, downOn, disOn);
	
	delete offI;
	delete over;
	delete down;
	delete disabled;
	delete normalOn;
	delete overOn;
	delete downOn;
	delete disOn;
}

void SVGButton::setToggleImages(Drawable* up, Drawable* down)
{
	setClickingTogglesState(true);
	setImages(up, up, up, up,
			  down, down, down, down);
}

SVGButton::~SVGButton()
{
	
}

Rectangle<float> SVGButtonLeftAligned::getImageBounds() const
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
