//
//  RTLookAndFeel.h
//  
//
//  Created by Isaac Roberts on 5/2/21.
//

//#pragma once

//#include "TwoValueSliderLookAndFeel.h"

namespace juce {

class JUCE_API FixedTooltipLookAndFeelMethods
{
public:
	FixedTooltipLookAndFeelMethods() {}
	virtual ~FixedTooltipLookAndFeelMethods() {};

	/** returns the bounds for a tooltip at the given screen coordinate, constrained within the given desktop area. */
	virtual void drawFixedTooltip (juce::Graphics&, const juce::String& text, int width, int height);
};


class JUCE_API RTLookAndFeel : 
				public TwoValue::SliderLookAndFeelMethods,
				public FixedTooltipLookAndFeelMethods,
				public ComboSlider::LookAndFeelMethods,
				public juce::SliderIncDecBottom::LookAndFeelMethods,
				public FlagBox::LookAndFeelMethods,
				public PopupSelector::LookAndFeelMethods,
				public RTMeter::LAF
{
public:
	virtual Button* createIncDecSliderButton (SliderIncDecBottom&, bool isIncrement) override
	{
		return new TextButton (isIncrement ? "+" : "-", String());
	}
private:
	class RTCP_LAF_SliderLabelComp  : public Label
	{
	public:
		RTCP_LAF_SliderLabelComp() : Label ({}, {}) {}
		void mouseWheelMove (const MouseEvent&, const MouseWheelDetails&) override {}
	};
public:
	virtual Label* createIncDecSliderTextBox (SliderIncDecBottom& slider) override
	{
		auto l = new RTCP_LAF_SliderLabelComp();
		//From 
		l->setJustificationType (Justification::centred);
		l->setKeyboardType (TextInputTarget::decimalKeyboard);

		l->setColour (Label::textColourId, slider.findColour (Slider::textBoxTextColourId));
		l->setColour (Label::backgroundColourId, slider.findColour (Slider::textBoxBackgroundColourId));
		l->setColour (Label::outlineColourId, slider.findColour (Slider::textBoxOutlineColourId));
		l->setColour (TextEditor::textColourId, slider.findColour (Slider::textBoxTextColourId));
		l->setColour (TextEditor::backgroundColourId, slider.findColour (Slider::textBoxBackgroundColourId));
		l->setColour (TextEditor::outlineColourId, slider.findColour (Slider::textBoxOutlineColourId));
		l->setColour (TextEditor::highlightColourId, slider.findColour (Slider::textBoxHighlightColourId));

		return l;
	}
	
	virtual ImageEffectFilter* getIncDecSliderEffect (SliderIncDecBottom&) override
	{
		return NULL;
	}
	
	virtual SliderIncDecBottom::SliderLayout getIncDecSliderLayout (SliderIncDecBottom& slider) override
	{
		int minXSpace = 0;
		int minYSpace = 0;
		
		auto textBoxPos = slider.getTextBoxPosition();
		
		if (textBoxPos == SliderIncDecBottom::TextBoxLeft || textBoxPos == SliderIncDecBottom::TextBoxRight)
			minXSpace = 30;
		else
			minYSpace = 15;
		
		auto localBounds = slider.getLocalBounds();
		
		auto textBoxWidth  = jmax (0, jmin (slider.getTextBoxWidth(),  localBounds.getWidth() - minXSpace));
		auto textBoxHeight = jmax (0, jmin (slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));
		
		SliderIncDecBottom::SliderLayout layout;
		
		// 2. set the textBox bounds
		if (textBoxPos != SliderIncDecBottom::NoTextBox)
		{
			layout.textBoxBounds.setWidth (textBoxWidth);
			layout.textBoxBounds.setHeight (textBoxHeight);
			
			if (textBoxPos == SliderIncDecBottom::TextBoxLeft)           layout.textBoxBounds.setX (0);
			else if (textBoxPos == SliderIncDecBottom::TextBoxRight)     layout.textBoxBounds.setX (localBounds.getWidth() - textBoxWidth);
			else /* above or below -> centre horizontally */ layout.textBoxBounds.setX ((localBounds.getWidth() - textBoxWidth) / 2);
			
			if (textBoxPos == SliderIncDecBottom::TextBoxAbove)          layout.textBoxBounds.setY (0);
			else if (textBoxPos == SliderIncDecBottom::TextBoxBelow)     layout.textBoxBounds.setY (localBounds.getHeight() - textBoxHeight);
			else /* left or right -> centre vertically */    layout.textBoxBounds.setY ((localBounds.getHeight() - textBoxHeight) / 2);
		
		}
		
		// 3. set the slider bounds
		
		layout.sliderBounds = localBounds;
		
		if (textBoxPos == SliderIncDecBottom::TextBoxLeft)       layout.sliderBounds.removeFromLeft (textBoxWidth);
		else if (textBoxPos == SliderIncDecBottom::TextBoxRight) layout.sliderBounds.removeFromRight (textBoxWidth);
		else if (textBoxPos == SliderIncDecBottom::TextBoxAbove) layout.sliderBounds.removeFromTop (textBoxHeight);
		else if (textBoxPos == SliderIncDecBottom::TextBoxBelow) layout.sliderBounds.removeFromBottom (textBoxHeight);
				
		return layout;
		
	}
	void drawPopupSelectorButton (Graphics& g, int width, int height, bool isButtonDown, bool isPopupShown,
										  PopupSelector& p) override {
		
		//TODO: Update this bad boi
		
		Rectangle<int> arrowZone (0, 0, width, height);
		Path path;
		path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
		path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
		path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);

		g.setColour (p.findColour (ComboBox::arrowColourId).withAlpha ((p.isEnabled() ? 0.9f : 0.2f)));
		g.strokePath (path, PathStrokeType (2.0f));

	}
	PopupMenu::Options getOptionsForPopupSelectorMenu (PopupSelector& box) override {
		return PopupMenu::Options().withMaximumNumColumns (1);
	}
	
	 void drawMeter (juce::Graphics& g, int x, int y, float relLevel,
					 float level, float min, float max,
					 int width, int height,
					 const RTMeter* meter) override;

};

}
