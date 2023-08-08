//
//  TwoValueCommon.h
//  
//
//  Created by Isaac Roberts on 5/2/21.
//

//#pragma once

//#include <JuceHeader.h>

//using namespace juce;

namespace TwoValue
{
	
	enum SliderStyle
	{
		Rotary,                         /**< A rotary control that you move by dragging the mouse in a circular motion, like a knob.
										 @see setRotaryParameters */
		TwoValueHorizontal,             /**< A horizontal slider that has two thumbs instead of one, so it can show a minimum and maximum value.
										 @see setMinValue, setMaxValue */
		TwoValueVertical,               /**< A vertical slider that has two thumbs instead of one, so it can show a minimum and maximum value.
										 @see setMinValue, setMaxValue */
	};
	
	
	//==============================================================================
	/** A struct defining the placement of the slider area and the text box area
	 relative to the bounds of the whole TwoValueSlider component.
	 */
	struct SliderLayout
	{
		juce::Rectangle<int> sliderBounds;
		juce::Rectangle<int> minTextBoxBounds;
		juce::Rectangle<int> maxTextBoxBounds;
	};
}
