//
//  TwoPartSVGBarSlider.h
//  
//
//  Created by Isaac Roberts on 2/22/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "../Sliders/TwoValueSlider.h"

using namespace juce;

template <typename Base>
class SVGBarSlider : public Base
{
public:
	SVGBarSlider(const char* baseData, const char* headData)
	: Base()
	{
		if (baseData!=NULL)
		{
			std::unique_ptr<XmlElement> svg_xml_1(XmlDocument::parse(baseData)); // GET THE SVG AS A XML
			base = Drawable::createFromSVG(*svg_xml_1); // GET THIS AS DRAWABLE
		}
		else {
			base = std::unique_ptr<Drawable>();
		}
		
		if (headData != NULL)
		{
			std::unique_ptr<XmlElement> svg_xml_3(XmlDocument::parse(headData)); // GET THE SVG AS A XML
			head = Drawable::createFromSVG(*svg_xml_3); // GET THIS AS DRAWABLE
		}
		else {
			head = std::unique_ptr<Drawable>();
		}
	}

	
	void drawLinearSlider(Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider &slider)
	{
		base->drawAt(g, 0, -1, 1.0);
		
		if (slider.isVertical())
		{
			int rad = getSliderThumbRadius(slider);
			if (slider.isTwoValue())
			{
				minSliderPos -= rad;
				maxSliderPos -= rad;
				int diff = minSliderPos - maxSliderPos;
//				std::cout<<minSliderPos<<" -> "<<maxSliderPos<<", diff="<<diff<<"rad = "<<rad<<std::endl;
				if (diff < rad*2)
				{
					int space = rad*2-diff;
					minSliderPos += space/2;
					maxSliderPos -= space/2;
					if (maxSliderPos < 0)
					{
						minSliderPos -= maxSliderPos;
						maxSliderPos=0;
					}
				}
				head->drawAt(g, 0, maxSliderPos, 1.0f);
				head->drawAt(g, 0, minSliderPos, 1.0f);
			}
			else
			{
				head->drawAt(g, 0, sliderPos - rad, 1.0f);
				
				if (slider.isThreeValue())
				{
					
				}
			}
		}
		else
		{
			
		}
	 }
	
	void drawTwoValueSlider(Graphics &g, int x, int y, int width, int height, float minSliderPos, float maxSliderPos, bool minVisible, bool maxVisible, const TwoValueSlider::SliderStyle style, TwoValueSlider &slider)
	{
		
//		RectanglePlacement placement(RectanglePlacement::yBottom | RectanglePlacement::xMid | RectanglePlacement::stretchToFit);
//		Rectangle<float> rect(0, 0, width, height);
//
//		base->drawWithin(g, rect, placement, 1.0);
		base->drawAt(g, 0, 0, 1);
		
		if (slider.isVertical())
		{
			int rad = getTwoValueSliderThumbRadius(slider);
		
//			minSliderPos -= rad;
			maxSliderPos -= rad*2;
			
//			int diff = minSliderPos - maxSliderPos;
////				std::cout<<minSliderPos<<" -> "<<maxSliderPos<<", diff="<<diff<<"rad = "<<rad<<std::endl;
//			if (diff < rad*2)
//			{
//				int space = rad*2-diff;
//				minSliderPos += space/2;
//				maxSliderPos -= space/2;
//				if (maxSliderPos < 0)
//				{
//					minSliderPos -= maxSliderPos;
//					maxSliderPos=0;
//				}
//			}
			if (maxVisible)
				head->drawAt(g, 0, maxSliderPos, 1.0f);
			if (minVisible)
				head->drawAt(g, 0, minSliderPos, 1.0f);
		}
		else
		{
			
		}
	 }

	void resize(int px, int py)
	{
		int rad = 10;

		if (!head)
		{
			headHeight = 0;
			if (!base)
				return;
			else
			{
				Rectangle<float> bounds(0, -rad, px, py);
				RectanglePlacement placement(RectanglePlacement::yBottom | RectanglePlacement::xMid);
				base->setTransformToFit(bounds, placement);
				
				auto dim = base->getDrawableBounds();
				
				float ratio = px / dim.getWidth();
				
				dim *= ratio;
				
				Rectangle<float> clipRect(dim.getX(), dim.getY() + dim.getHeight()-py, dim.getWidth(), py);
				DrawableRectangle clipBounds;
				clipBounds.setRectangle(Parallelogram<float>(bounds));
				base->setClipPath(std::make_unique<DrawableRectangle>(clipBounds));
				
			}
		}
		
		Rectangle<float> headDim = head->getDrawableBounds();
//		headDim.setY(0);
		
//		std::cout<<"headDim "<<headDim.getX()<<", "<<headDim.getY()<<", "<<headDim.getWidth()<<", "<<headDim.getHeight()<<std::endl;
		
		float ratio = px / headDim.getWidth();
		
		
		headDim *= ratio;
		
//		headDim.setWidth(ratio * headDim.getWidth());
//		headDim.setHeight (ratio * headDim.getHeight());
		headHeight = headDim.getHeight() - headDim.getY();
		

		if (base)
		{
			Rectangle<float> baseDim = base->getDrawableBounds();
			baseDim *= ratio;
			float offset = headDim.getWidth() - baseDim.getWidth();
			
			Rectangle<float> bounds(offset/2, 0, baseDim.getWidth(), py);
			
			
			RectanglePlacement placement(RectanglePlacement::fillDestination);
		
			base->setTransformToFit(bounds, placement);
		}
		
			
//		float headY = headDim.py / headDim.px * px;
		
//		Rectangle<float> bounds2(0, 0, px, headY);
		RectanglePlacement placement2(RectanglePlacement::stretchToFit);
	
		
		head->setTransformToFit(headDim, placement2);
	}
	
	int getSliderThumbRadius (Slider &s)  {
		return (headHeight * (1-headShadowPercentage))/2;
	}
	
	int getTwoValueSliderThumbRadius (TwoValueSlider &s)  {
		return (headHeight * (1-headShadowPercentage))/2;
	}
	
	virtual ~SVGBarSlider()
	{
		
	}
	
	void setHeadShadowPercentage(float set) { headShadowPercentage = set; }
	
private:
	std::unique_ptr<Drawable> base, head;
	float headHeight;
	float headShadowPercentage;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVGBarSlider);

};
