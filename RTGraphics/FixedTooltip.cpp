//
//  FixedTooltip.cpp
//  
//
//  Created by Isaac Roberts on 5/2/21.
//

// Created by Isaac Roberts


#include "FixedTooltip.h"

//#include "RTLookAndFeel.h"

FixedTooltip::FixedTooltip (Component* parentComp, int delayMs)
	: Component ("Tooltip_Display"),
	  millisecondHoldTime (delayMs)
{
//	setAlwaysOnTop (false);
//	setOpaque (false);

//	if (parentComp != nullptr)
//		parentComp->addChildComponent (this);

	if (Desktop::getInstance().getMainMouseSource().canHover())
		startTimer (123);
}

FixedTooltip::~FixedTooltip()
{
	hideTip();
}

void FixedTooltip::setMillisecondsBeforeTipDisappears (const int newTimeMs) noexcept
{
	millisecondHoldTime = newTimeMs;
}

void FixedTooltip::paint (Graphics& g)
{
	getLookAndFeel().drawFixedTooltip (g, tipShowing, getWidth(), getHeight());
}

void FixedTooltip::mouseEnter (const MouseEvent&)
{
	hideTip();
}

void FixedTooltip::displayTip (Point<int> screenPos, const String& tip)
{
	jassert (tip.isNotEmpty());

	lastCompChangeTime = Time::getApproximateMillisecondCounter();
	
	if (! reentrant)
	{
		ScopedValueSetter<bool> setter (reentrant, true, false);

		if (tipShowing != tip)
		{
			tipShowing = tip;
			repaint();
		}
	}
}

String FixedTooltip::getTipFor (Component& c)
{
	if (Process::isForegroundProcess()
		 && ! ModifierKeys::currentModifiers.isAnyMouseButtonDown())
	{
		if (auto* ttc = dynamic_cast<TooltipClient*> (&c))
			if (! c.isCurrentlyBlockedByAnotherModalComponent())
				return ttc->getTooltip();
	}

	return {};
}

void FixedTooltip::hideTip()
{
	if (! reentrant)
	{
		tipShowing.clear();
		repaint();
	}
}

void FixedTooltip::timerCallback()
{
	if (!isVisible())
		return;
	
	auto& desktop = Desktop::getInstance();
	auto mouseSource = desktop.getMainMouseSource();
	auto now = Time::getApproximateMillisecondCounter();

	auto* newComp = mouseSource.isTouch() ? nullptr : mouseSource.getComponentUnderMouse();

	if (newComp == nullptr || getParentComponent() == nullptr || newComp->getPeer() == getPeer())
	{
		auto newTip = newComp != nullptr ? getTipFor (*newComp) : String();
		bool tipChanged = (newTip != lastTipUnderMouse || newComp != lastComponentUnderMouse);
		lastComponentUnderMouse = newComp;
		lastTipUnderMouse = newTip;

		auto clickCount = desktop.getMouseButtonClickCounter();
		auto wheelCount = desktop.getMouseWheelMoveCounter();
		bool mouseWasClicked = (clickCount > mouseClicks || wheelCount > mouseWheelMoves);
		mouseClicks = clickCount;
		mouseWheelMoves = wheelCount;

		auto mousePos = mouseSource.getScreenPosition();
		bool mouseMovedQuickly = mousePos.getDistanceFrom (lastMousePos) > 12;
		lastMousePos = mousePos;

		if (tipChanged || mouseWasClicked || mouseMovedQuickly)
			lastCompChangeTime = now;

		if (!newTip.isEmpty())
			lastHideTime = now;
		
		if (tipChanged && !newTip.isEmpty())
		{
			displayTip (mousePos.roundToInt(), newTip);
		}
		else if (newTip.isEmpty())
		{
			//TODO: Fade
			if (!tipShowing.isEmpty())
			{
				//Hold tip until timeout
				if (now > lastHideTime + millisecondHoldTime )
				{
					hideTip();
				}
			}
		}
	}
}
