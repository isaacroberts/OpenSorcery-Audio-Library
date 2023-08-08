//
//  SVGRotator.cpp
//  
//
//  Created by Isaac Roberts on 7/15/20.
//

// Created by Isaac Roberts


#include "SVGRotator.h"

#include <cmath>

SVGRotator::SVGRotator(const char * topLayerData, const char* rotatorData, const char* shadowData, float startAngle_)
	: CommonLAF(),
startAngle(startAngle_)
{
	if (topLayerData!=NULL)
	{
		std::unique_ptr<XmlElement> svg_xml_1(XmlDocument::parse(topLayerData)); // GET THE SVG AS A XML
		fixedTop = Drawable::createFromSVG(*svg_xml_1); // GET THIS AS DRAWABLE
	}
	else {
		fixedTop = std::unique_ptr<Drawable>();
	}

	std::unique_ptr<XmlElement> svg_xml_2(XmlDocument::parse(rotatorData)); // GET THE SVG AS A XML
	rotator = Drawable::createFromSVG(*svg_xml_2); // GET THIS AS DRAWABLE

	
	if (shadowData != NULL)
	{
		std::unique_ptr<XmlElement> svg_xml_3(XmlDocument::parse(shadowData)); // GET THE SVG AS A XML
		fixedShadow = Drawable::createFromSVG(*svg_xml_3); // GET THIS AS DRAWABLE
	}
	else {
		fixedShadow = std::unique_ptr<Drawable>();
	}
	
}


SVGRotator::~SVGRotator()
{
//	delete rotator;
//	delete fixedTop;
//	delete fixedShadow;
}

Slider::RotaryParameters SVGRotator::getAngleSpread(float fac)
{
	Slider::RotaryParameters params;
	params.startAngleRadians	= (2-fac) * M_PI;
	params.endAngleRadians 		= (2+fac) * M_PI;
	params.stopAtEnd=true;
	return params;
}
void SVGRotator::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &s)
{
	if (fixedShadow)
	{
		fixedShadow->drawAt(g, 0, 0, 1.0);
	}
	
	float rot = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
	
//	rot -= startAngle;
	
//	AffineTransform affine = AffineTransform::rotation(rot, cx, cy);
	
	// box:
//	 juce::Rectangle<float> picBounds(0,0,31,31);
	auto picBounds = Rectangle<float>(0, 0, width, height);
//	g.drawRect(picBounds.expanded(1.f), 1.f);
	// svg file:

	//Code to rotate around original bounds
	juce::Rectangle<float> svgBounds =
		  ((DrawableComposite*) rotator.get())->getContentArea();
	juce::RectanglePlacement ourPlacement(RectanglePlacement::stretchToFit);
	juce::AffineTransform xform = ourPlacement.getTransformToFit (svgBounds, picBounds);
	xform = xform.rotated(rot, width/2.f, height/2.f);
	rotator->draw(g, 1.f, xform);

	
//	rotator->draw(g, 1.0, affine);
		
	if (fixedTop )
	{
		fixedTop->drawAt(g, 0, 0, 1.0);
	}
	
}

void SVGRotator::resize(float px)
{
	Rectangle<float> dim;
	
	if (fixedShadow)
	{
		dim = fixedShadow->getDrawableBounds();
	}
	else
	{
		dim = rotator->getDrawableBounds();
	}
	
	float scale = px/dim.getWidth();
	
	float py  = dim.getHeight() * scale;
	
	RectanglePlacement placement;
	
	if (fixedTop)
		fixedTop->setTransformToFit(fixedTop->getDrawableBounds() * scale, placement);
	
//	rotator->resetBoundingBoxToContentArea();
//	auto rdim = rotator->getDrawableBounds() * scale;
	
//	rotator->setTransformToFit(rdim, placement);
	if (fixedShadow)
		fixedShadow->setTransformToFit(fixedShadow->getDrawableBounds() * scale, placement);
	
	cx = px/2.0;
	cy = py/2.0;
}
/*
void SVGRotator::setLayout(Slider& s, Slider::SliderLayout set) {
//	set.sliderBounds.setWidth(s.getWidth());
//	set.sliderBounds.setHeight(s.getHeight());
	layouts[&s]=set;
}
Slider::SliderLayout SVGRotator::getSliderLayout(Slider& s)
{
	auto iter = layouts.find(&s);
	if (iter == layouts.end())
	{
		Slider::SliderLayout layout;
		layout.sliderBounds = s.getLocalBounds();
		return layout;
	}
	else
		iter->second.sliderBounds = s.getLocalBounds();
		return iter->second;
}
*/
