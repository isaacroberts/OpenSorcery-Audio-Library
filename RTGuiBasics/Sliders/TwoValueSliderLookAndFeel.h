//
//  TwoValueSliderLookAndFeel.h
//  
//
//  Created by Isaac Roberts on 5/2/21.
//

//#pragma once

//#include <JuceHeader.h>

//using namespace juce;

//#include "TwoValueCommon.h"

namespace TwoValue
{
	
	class TwoValueSlider;
//	class SliderLayout;
	
	struct SliderLookAndFeelMethods
	{
		
		virtual ~SliderLookAndFeelMethods() = default;
		
		//==============================================================================
		virtual void drawTwoValueSlider (juce::Graphics&,
										 int x, int y, int width, int height,
										 float minSliderPos,
										 float maxSliderPos, bool minVisible, bool maxVisible,
										 const SliderStyle,
										 TwoValueSlider&) {}//TODO: Add default
		
		//		virtual void drawLinearSliderBackground (Graphics&,
		//												 int x, int y, int width, int height,
		//												 float minSliderPos,
		//												 float maxSliderPos,
		//												 const TwoValueSlider::SliderStyle style,
		//												 TwoValueSlider&) = 0;
		//
		//		virtual void drawLinearSliderThumb (Graphics&,
		//											int x, int y, int width, int height,
		//											float minSliderPos,
		//											float maxSliderPos,
		//											const TwoValueSlider::SliderStyle,
		//											TwoValueSlider&) = 0;
		
		virtual int getTwoValueSliderThumbRadius (TwoValueSlider&) { return 2; }
		
		//		virtual void drawRotarySlider (Graphics&,
		//									   int x, int y, int width, int height,
		//									   float sliderPosProportional,
		//									   float rotaryStartAngle,
		//									   float rotaryEndAngle,
		//									   TwoValueSlider&) = 0;
		
		virtual juce::Button* createTVSliderButton (TwoValueSlider&, bool isIncrement);
		
		
		virtual juce::Label* createTVSliderTextBox (TwoValueSlider& slider);
		
//		virtual juce::ImageEffectFilter* getSliderEffect (TwoValueSlider& slider) { return nullptr; }
		
		juce::Font getTVSliderPopupFont (TwoValueSlider&);
		
		virtual int getTVSliderPopupPlacement (TwoValueSlider& slider);
		
		virtual SliderLayout getTVSliderLayout (TwoValueSlider& slider);
		 
	};
	
	
}
