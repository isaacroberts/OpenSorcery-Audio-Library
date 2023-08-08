//
//  FixedTooltip.h
//  
//
//  Created by Isaac Roberts on 5/2/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class JUCE_API FixedTooltip  : public Component,
								 private Timer
{
public:
	//==============================================================================
	/** Creates a tooltip window.

		Make sure your app only creates one instance of this class, otherwise you'll
		get multiple overlaid tooltips appearing. The window will initially be invisible
		and will make itself visible when it needs to display a tip.

		To change the style of tooltips, see the LookAndFeel class for its tooltip
		methods.

		@param parentComponent  if set to nullptr, the FixedTooltip will appear on the desktop,
								otherwise the tooltip will be added to the given parent
								component.
		@param millisecondsBeforeTipAppears     the time for which the mouse has to stay still
												before a tooltip will be shown

		@see TooltipClient, LookAndFeel::drawTooltip, LookAndFeel::getTooltipBounds
	*/
	explicit FixedTooltip (Component* parentComponent = nullptr,
							int millisecondsBeforeTipAppears = 700);

	/** Destructor. */
	~FixedTooltip() override;

	//==============================================================================
	/** Changes the time before the tip appears.
		This lets you change the value that was set in the constructor.
	*/
	void setMillisecondsBeforeTipDisappears (int newTimeMs = 700) noexcept;

	/** Can be called to manually force a tip to be shown at a particular location. */
	void displayTip (Point<int> screenPosition, const String& text);

	/** Can be called to manually hide the tip if it's showing. */
	void hideTip();

	/** Asks a component for its tooltip.
		This can be overridden if you need custom lookup behaviour or to modify the strings.
	*/
	virtual String getTipFor (Component&);

	//==============================================================================
	/** A set of colour IDs to use to change the colour of various aspects of the tooltip.

		These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
		methods.

		@see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
	*/
	enum ColourIds
	{
		backgroundColourId      = 0x1001b00,    /**< The colour to fill the background with. */
		textColourId            = 0x1001c00,    /**< The colour to use for the text. */
		outlineColourId         = 0x1001c10     /**< The colour to use to draw an outline around the tooltip. */
	};

	//==============================================================================
	/** This abstract base class is implemented by LookAndFeel classes to provide
		window drawing functionality.
	*/

private:
	//==============================================================================
	Point<float> lastMousePos;
	Component* lastComponentUnderMouse = nullptr;
	String tipShowing, lastTipUnderMouse;
	int millisecondHoldTime;
	int mouseClicks = 0, mouseWheelMoves = 0;
	unsigned int lastCompChangeTime = 0, lastHideTime = 0;
	bool reentrant = false;

	void paint (Graphics&) override;
	void mouseEnter (const MouseEvent&) override;
	void timerCallback() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FixedTooltip)
};

