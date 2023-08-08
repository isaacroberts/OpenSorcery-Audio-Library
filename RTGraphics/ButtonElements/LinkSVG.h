//
//  LinkSVG.h
//  
//
//  Created by Isaac Roberts on 3/3/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "SVGButton.h"

using namespace juce;

class LinkSVG : public SVGButton
{
public:
	LinkSVG() : SVGButton() {}
	LinkSVG(String name) : SVGButton(name) {}
	LinkSVG(String name, DrawableButton::ButtonStyle style) : SVGButton(name, style) {}

	void setURL(String set, bool setAsTooltip=true)
	{
		url = URL(set);
		setMouseCursor (MouseCursor::PointingHandCursor);
		if (setAsTooltip)
			setTooltip(set);
		else
			setTooltip("");
	}
	inline void setUrl(String set, bool tooltip=true) { setURL(set, tooltip); }

	void launch() {
		if (url.isWellFormed())
		{
			url.launchInDefaultBrowser();
		}
	}
	void clicked() override {
		//Prevents threads from hanging
		MessageManager::callAsync([&]() { launch(); });
	}
	
	virtual ~LinkSVG() {}
	
protected:
	URL url;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LinkSVG);
};


class OpacityLinkSVG : public Button
{
public:
	OpacityLinkSVG(const char* data)
	: Button("svgbutton"),
	opacity(1),
	url()
	{
		std::unique_ptr<XmlElement> svg_xml(XmlDocument::parse(data));
		image = Drawable::createFromSVG(*svg_xml);
	}
	OpacityLinkSVG()
	: Button("svgbutton"),
	image(nullptr),
	opacity(1),
	url()
	{
//		setMouseCursor (MouseCursor::PointingHandCursor);
	}
	
//	void paint(Graphics& g) override
//	{
//		if (image != nullptr)
//			image->draw(g, opacity);
//	}
	void paintButton (Graphics& g,
							  bool shouldDrawButtonAsHighlighted,
							  bool shouldDrawButtonAsDown) override
	{
		if (image != nullptr && opacity > 0)
			image->draw(g, opacity);
	}

	void resized() override
	{
		RectanglePlacement p(
							RectanglePlacement::centred);
		Rectangle<float> rect(0, 0, getWidth(), getHeight());
		image->setTransformToFit(rect, p);
	}
	
	void setOpacity(float set)
	{
		set = std::max(0.f, std::min(1.f, set));
		if (set != opacity)
		{
			opacity = set;
			repaint();
		}
	}
	void setImage(const char* data)
	{
		std::unique_ptr<XmlElement> svg_xml(XmlDocument::parse(data));
		image.reset();
		image = Drawable::createFromSVG(*svg_xml);
	}
	void loadSVG(const char* data) {
		setImage(data);
	}
	
	void setURL(String set, bool setAsTooltip=true)
	{
		url = URL(set);
		setMouseCursor (MouseCursor::PointingHandCursor);
		if (setAsTooltip)
			setTooltip(set);
		else
			setTooltip("");
	}
	inline void setUrl(String set, bool tooltip=true) { setURL(set, tooltip); }
	
	void launch() {
		if (url.isWellFormed())
		{
			url.launchInDefaultBrowser();
		}
	}
	
	void clicked() override {
		//Prevents threads from hanging
		MessageManager::callAsync([&]() { launch(); });
	}
	
	Rectangle<float> getDrawableBounds() { return image->getDrawableBounds(); }
	
	virtual ~OpacityLinkSVG() {}
	
protected:
	std::unique_ptr<Drawable> image;
	float opacity;
	URL url;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpacityLinkSVG);

};
