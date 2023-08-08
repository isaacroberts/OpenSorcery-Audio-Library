//
//  SimpleSliderLAF.h
//  
//
//  Created by Isaac Roberts on 3/29/21.
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

class SimpleSliderLAF : public LookAndFeel_V4
{
public:
	SimpleSliderLAF(Colour bg, Colour handle, Colour outline, int sliderSize, float corners, float outlinePx)
	: LookAndFeel_V4(),
	backgroundColour(bg),
	handleColour(handle),
	outlineColour(outline),
	sliderDimension(sliderSize),
	outlineCornerSize(corners),
	outlineThickness(outlinePx)
	{
	}
	
	SimpleSliderLAF() : SimpleSliderLAF(Colours::black, Colours::white, Colours::grey) {}
	SimpleSliderLAF(Colour bg, Colour handle, Colour outline) : SimpleSliderLAF(bg, handle, outline, 6, 2, 1) {}
	
	void setColours(Colour bgCol, Colour handleCol, Colour outlineCol)
	{
		backgroundColour = bgCol;
		handleColour = handleCol;
		outlineColour = outlineCol;
	}
	void setCornerSize(float cornerSizePx)
	{
		outlineCornerSize = cornerSizePx;
	}
	void setNoOutline()
	{
		outlineThickness=0;
	}
	void setOutlineThickness(float lineThicknessPx)
	{
		outlineThickness = lineThicknessPx;
	}
	void setSliderSize(int px)
	{
		sliderDimension = px;
	}

   void drawTwoValueSlider (Graphics& g,
								  int x, int y, int width, int height,
								  float minSliderPos,
								  float maxSliderPos, bool minVisible, bool maxVisible,
								  const TwoValue::SliderStyle style,
								  TwoValueSlider& slider) override
	{
		int bgW = slider.getWidth();
		int bgH = slider.getHeight();
		
		
		if (outlineCornerSize > 0)
		{
			g.setColour(backgroundColour);
			g.fillRoundedRectangle(0, 0, bgW, bgH, outlineCornerSize);
		}
		else
			g.fillAll(backgroundColour);
		if (outlineThickness > 0)
		{
			g.setColour(outlineColour);
			g.drawRoundedRectangle(0, 0, bgW, bgH, outlineCornerSize, outlineThickness);
		}
		
		g.setColour(handleColour);
		
		if (slider.isVertical())
		{
			int rad = getTwoValueSliderThumbRadius(slider);
		
			maxSliderPos -= rad*2;
			minSliderPos -= .2;//Make handles overlap
//			minSliderPos -= rad*4;
			
			if (maxVisible)
			{
				g.fillRect(0.f, maxSliderPos, (float)width, (float)sliderDimension);
			}
			if (minVisible)
			{
				g.fillRect(0.f, minSliderPos, (float)width, (float)sliderDimension);
			}
		}
		else
		{
			
		}
	}
	
	int getSliderThumbRadius (Slider &s) override {
		return sliderDimension/2;
	}
	
	int getTwoValueSliderThumbRadius (TwoValueSlider &s) override {
		return sliderDimension/2;
	}
	
	virtual ~SimpleSliderLAF()
	{
		
	}
	
private:
	Colour backgroundColour;
	Colour handleColour;
	Colour outlineColour;
	int sliderDimension;
	float outlineCornerSize;
	float outlineThickness;
	
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleSliderLAF);

};
