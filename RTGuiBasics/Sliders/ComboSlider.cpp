//
//  ComboSlider.cpp
//  
//
//  Created by Isaac Roberts on 5/11/21.
//

// Created by Isaac Roberts


//#include "ComboSlider.h"

using namespace juce;

class ComboSlider::Pimpl   : public AsyncUpdater, // this needs to be public otherwise it will cause an
											 // error when JUCE_DLL_BUILD=1
						private Value::Listener
{
public:
	Pimpl (ComboSlider& s, DragDirection sliderStyle)
	  : owner (s),
		style (sliderStyle)
	{
	}

	~Pimpl() override
	{
		currentValue.removeListener (this);
		popupDisplay.reset();
	}

	//==============================================================================
	void registerListeners()
	{
		currentValue.addListener (this);
	}

	bool isHorizontal() const noexcept
	{
		return style == LinearBar;
	}

	bool isVertical() const noexcept
	{
		return style == LinearBarVertical || style==LinearBarVerticalInverse;
	}

	bool incDecDragDirectionIsHorizontal() const noexcept
	{
		return incDecButtonMode == incDecButtonsDraggable_Horizontal
				|| (incDecButtonMode == incDecButtonsDraggable_AutoDirection && incDecButtonsSideBySide);
	}

	float getPositionOfValue (double value) const
	{
		if (isHorizontal() || isVertical())
			return getLinearSliderPos (value);

		jassertfalse; // not a valid call on a slider that doesn't work linearly!
		return 0.0f;
	}
	
	void setTextValueSuffix (const String& suffix)
	{
		if (textSuffix != suffix)
		{
			textSuffix = suffix;
//			updateText();
		}
	}

	void updateRange()
	{
		// figure out the number of DPs needed to display all values at this
		// interval setting.
		numDecimalPlaces = 7;

		if (normRange.interval != 0.0)
		{
			int v = std::abs (roundToInt (normRange.interval * 10000000));

			while ((v % 10) == 0 && numDecimalPlaces > 0)
			{
				--numDecimalPlaces;
				v /= 10;
			}
		}

		// keep the current values inside the new range..
		
		setValue (getValue(), dontSendNotification);
	}

	void setRange (double newMin, double newMax, double newInt)
	{
		normRange = NormalisableRange<double> (newMin, newMax, newInt,
											   normRange.skew, normRange.symmetricSkew);
		updateRange();
	}

	void setNormalisableRange (NormalisableRange<double> newRange)
	{
		normRange = newRange;
		updateRange();
	}

	double getValue() const
	{
		// for a two-value style slider, you should use the getMinValue() and getMaxValue()
		// methods to get the two values.
		return currentValue.getValue();
	}

	void setValue (double newValue, NotificationType notification)
	{
		// for a two-value style slider, you should use the setMinValue() and setMaxValue()
		// methods to set the two values.

		newValue = constrainedValue (newValue);

		if (newValue != lastCurrentValue)
		{
			lastCurrentValue = newValue;

			// (need to do this comparison because the Value will use equalsWithSameType to compare
			// the new and old values, so will generate unwanted change events if the type changes)
			if (currentValue != newValue)
				currentValue = newValue;

			owner.repaint();
			updatePopupDisplay (newValue);

			triggerChangeMessage (notification);
		}
	}

	void triggerChangeMessage (NotificationType notification)
	{
		if (notification != dontSendNotification)
		{
			owner.valueChanged();

			if (notification == sendNotificationSync)
				handleAsyncUpdate();
			else
				triggerAsyncUpdate();
		}
	}

	void handleAsyncUpdate() override
	{
		cancelPendingUpdate();

		Component::BailOutChecker checker (&owner);
		listeners.callChecked (checker, [&] (ComboSlider::Listener& l) { l.sliderValueChanged (&owner); });

		if (checker.shouldBailOut())
			return;

		if (owner.onValueChange != nullptr)
			owner.onValueChange();
	}

	void sendDragStart()
	{
		owner.startedDragging();

		Component::BailOutChecker checker (&owner);
		listeners.callChecked (checker, [&] (ComboSlider::Listener& l) { l.sliderDragStarted (&owner); });

		if (checker.shouldBailOut())
			return;

		if (owner.onDragStart != nullptr)
			owner.onDragStart();
	}

	void sendDragEnd()
	{
		owner.stoppedDragging();

		Component::BailOutChecker checker (&owner);
		listeners.callChecked (checker, [&] (ComboSlider::Listener& l) { l.sliderDragEnded (&owner); });

		if (checker.shouldBailOut())
			return;

		if (owner.onDragEnd != nullptr)
			owner.onDragEnd();
	}

	struct DragInProgress
	{
		DragInProgress (Pimpl& p)  : owner (p)      { owner.sendDragStart(); }
		~DragInProgress()                           { owner.sendDragEnd(); }

		Pimpl& owner;

		JUCE_DECLARE_NON_COPYABLE (DragInProgress)
	};

	void incrementOrDecrement (double delta)
	{
//		if (style == IncDecButtons)
//		{
//			auto newValue = owner.snapValue (getValue() + delta, notDragging);
//
//			if (currentDrag != nullptr)
//			{
//				setValue (newValue, sendNotificationSync);
//			}
//			else
//			{
//				DragInProgress drag (*this);
//				setValue (newValue, sendNotificationSync);
//			}
//		}
	}

	void valueChanged (Value& value) override
	{
		if (value.refersToSameSourceAs (currentValue))
		{
			setValue (currentValue.getValue(), dontSendNotification);
		}
	}

	double constrainedValue (double value) const
	{
		return normRange.snapToLegalValue (value);
	}

	float getLinearSliderPos (double value) const
	{
		double pos;

		if (normRange.end <= normRange.start)
			pos = 0.5;
		else if (value < normRange.start)
			pos = 0.0;
		else if (value > normRange.end)
			pos = 1.0;
		else
			pos = owner.valueToProportionOfLength (value);

		if (isVertical() )
			pos = 1.0 - pos;

		jassert (pos >= 0 && pos <= 1.0);
		return (float) (sliderRegionStart + pos * sliderRegionSize);
	}

	void setSliderStyle (DragDirection newStyle)
	{
		if (style != newStyle)
		{
			style = newStyle;
			owner.repaint();
			owner.lookAndFeelChanged();
		}
	}

	void setIncDecButtonsMode (IncDecButtonMode mode)
	{
		if (incDecButtonMode != mode)
		{
			incDecButtonMode = mode;
			owner.lookAndFeelChanged();
		}
	}

	void lookAndFeelChanged (LookAndFeel& lf)
	{
//		if (style == IncDecButtons)
//		{
//			incButton.reset (lf.createSliderButton (owner, true));
//			decButton.reset (lf.createSliderButton (owner, false));
//
//			owner.addAndMakeVisible (incButton.get());
//			owner.addAndMakeVisible (decButton.get());
//
//			incButton->onClick = [this] { incrementOrDecrement (normRange.interval); };
//			decButton->onClick = [this] { incrementOrDecrement (-normRange.interval); };
//
//			if (incDecButtonMode != incDecButtonsNotDraggable)
//			{
//				incButton->addMouseListener (&owner, false);
//				decButton->addMouseListener (&owner, false);
//			}
//			else
//			{
//				incButton->setRepeatSpeed (300, 100, 20);
//				decButton->setRepeatSpeed (300, 100, 20);
//			}
//
//			auto tooltip = owner.getTooltip();
//			incButton->setTooltip (tooltip);
//			decButton->setTooltip (tooltip);
//		}
//		else
//		{
//			incButton.reset();
//			decButton.reset();
//		}

		owner.setComponentEffect (lf.getComboSliderEffect (owner));

		owner.resized();
		owner.repaint();
	}

	void showPopupMenu()
	{
		PopupMenu m;
		m.setLookAndFeel (&owner.getLookAndFeel());
//		m.addItem (1, TRANS ("Velocity-sensitive mode"), true, isVelocityBased);
		m.addSeparator();

		m.showMenuAsync (PopupMenu::Options(),
						 ModalCallbackFunction::forComponent (sliderMenuCallback, &owner));
	}

	static void sliderMenuCallback (int result, ComboSlider* slider)
	{
		if (slider != nullptr)
		{
			switch (result)
			{
//				case 1:   slider->setVelocityBasedMode (! slider->getVelocityBasedMode()); break;
//				case 2:   slider->setSliderStyle (Rotary); break;
//				case 3:   slider->setSliderStyle (RotaryHorizontalDrag); break;
//				case 4:   slider->setSliderStyle (RotaryVerticalDrag); break;
//				case 5:   slider->setSliderStyle (RotaryHorizontalVerticalDrag); break;
				default:  break;
			}
		}
	}

//	int getThumbIndexAt (const MouseEvent& e)
//	{
//		return 0;
//	}

	void handleAbsoluteDrag (const MouseEvent& e)
	{
		auto mousePos = (isHorizontal()) ? e.position.x : e.position.y;
		double newPos = 0;

		if (! snapsToMousePos)
		{
			auto mouseDiff = (style == LinearBar
								|| (incDecDragDirectionIsHorizontal()))
							  ? e.position.x - mouseDragStartPos.x
							  : mouseDragStartPos.y - e.position.y;

			if (style == LinearBarVertical)
				mouseDiff = - mouseDiff;

			newPos = owner.valueToProportionOfLength (valueOnMouseDown)
					   + mouseDiff * (1.0 / pixelsForFullDragExtent);

//			if (style == IncDecButtons)
//			{
//				incButton->setState (mouseDiff < 0 ? Button::buttonNormal : Button::buttonDown);
//				decButton->setState (mouseDiff > 0 ? Button::buttonNormal : Button::buttonDown);
//			}
		}
		else
		{
			newPos = (mousePos - (float) sliderRegionStart) / (double) sliderRegionSize;

			
			if (isVertical())
			{
				if (style == LinearBarVerticalInverse)
					newPos = 1.0 - newPos;
			}
		}

		newPos = jlimit (0.0, 1.0, newPos);
		valueWhenLastDragged = owner.proportionOfLengthToValue (newPos);
	}
	
	void mouseDown (const MouseEvent& e)
	{
		incDecDragged = false;
		useDragEvents = false;
		mouseDragStartPos = mousePosWhenLastDragged = e.position;
		currentDrag.reset();
		popupDisplay.reset();

		if (owner.isEnabled())
		{
			if (e.mods.isPopupMenu() && menuEnabled)
			{
				showPopupMenu();
			}
			else if (canDoubleClickToValue()
					 && (singleClickModifiers != ModifierKeys() && e.mods.withoutMouseButtons() == singleClickModifiers))
			{
				mouseDoubleClick();
			}
			else if (normRange.end > normRange.start)
			{
				useDragEvents = true;

				lastAngle = owner.valueToProportionOfLength (currentValue.getValue());

				valueWhenLastDragged = currentValue.getValue();
				valueOnMouseDown = valueWhenLastDragged;

				if (showPopupOnDrag || showPopupOnHover)
				{
					showPopupDisplay();

					if (popupDisplay != nullptr)
						popupDisplay->stopTimer();
				}

				currentDrag.reset (new DragInProgress (*this));
				mouseDrag (e);
			}
		}
	}

	void mouseDrag (const MouseEvent& e)
	{
		if (useDragEvents && normRange.end > normRange.start)
		{
//			DragMode dragMode = notDragging;

//			if (style == IncDecButtons && ! incDecDragged)
//			{
//				if (e.getDistanceFromDragStart() < 10 || ! e.mouseWasDraggedSinceMouseDown())
//					return;
//
//				incDecDragged = true;
//				mouseDragStartPos = e.position;
//			}

			if ((normRange.end - normRange.start) / sliderRegionSize < normRange.interval)
			{
//				dragMode = absoluteDrag;
				handleAbsoluteDrag (e);
			}
			
			valueWhenLastDragged = jlimit (normRange.start, normRange.end, valueWhenLastDragged);

			setValue (owner.snapValue (valueWhenLastDragged),
					  sendChangeOnlyOnRelease ? dontSendNotification : sendNotificationSync);
				
			mousePosWhenLastDragged = e.position;
		}
	}

	void mouseUp()
	{
		if (owner.isEnabled()
			 && useDragEvents
			 && (normRange.end > normRange.start) )
//			 && (style != IncDecButtons || incDecDragged))
		{
			restoreMouseIfHidden();

			if (sendChangeOnlyOnRelease && valueOnMouseDown != static_cast<double> (currentValue.getValue()))
				triggerChangeMessage (sendNotificationAsync);

			currentDrag.reset();
			popupDisplay.reset();

//			if (style == IncDecButtons)
//			{
//				incButton->setState (Button::buttonNormal);
//				decButton->setState (Button::buttonNormal);
//			}
		}
		else if (popupDisplay != nullptr)
		{
			popupDisplay->startTimer (200);
		}

		currentDrag.reset();
	}

	void mouseMove()
	{
		// this is a workaround for a bug where the popup display being dismissed triggers
		// a mouse move causing it to never be hidden
		auto shouldShowPopup = showPopupOnHover
								&& (Time::getMillisecondCounterHiRes() - lastPopupDismissal) > 250;

		if (shouldShowPopup)
		{
			if (owner.isMouseOver (true))
			{
				if (popupDisplay == nullptr)
					showPopupDisplay();

				if (popupDisplay != nullptr && popupHoverTimeout != -1)
					popupDisplay->startTimer (popupHoverTimeout);
			}
		}
	}

	void mouseExit()
	{
		popupDisplay.reset();
	}

	void showPopupDisplay()
	{
//		if (style == IncDecButtons)
//			return;

		if (popupDisplay == nullptr)
		{
			popupDisplay.reset (new PopupDisplayComponent (owner, parentForPopupDisplay == nullptr));

			if (parentForPopupDisplay != nullptr)
				parentForPopupDisplay->addChildComponent (popupDisplay.get());
			else
				popupDisplay->addToDesktop (ComponentPeer::windowIsTemporary
											| ComponentPeer::windowIgnoresKeyPresses
											| ComponentPeer::windowIgnoresMouseClicks);

			updatePopupDisplay (getValue());

			popupDisplay->setVisible (true);
		}
	}

	void updatePopupDisplay (double valueToShow)
	{
		if (popupDisplay != nullptr)
			popupDisplay->updatePosition (owner.getTextFromValue (valueToShow));
	}

	bool canDoubleClickToValue() const
	{
		return doubleClickToValue
//				&& style != IncDecButtons
				&& normRange.start <= doubleClickReturnValue
				&& normRange.end >= doubleClickReturnValue;
	}

	void mouseDoubleClick()
	{
		if (canDoubleClickToValue())
		{
			DragInProgress drag (*this);
			setValue (doubleClickReturnValue, sendNotificationSync);
		}
	}

	double getMouseWheelDelta (double value, double wheelAmount)
	{
//		if (style == IncDecButtons)
//			return normRange.interval * wheelAmount;

		auto proportionDelta = wheelAmount * 0.15;
		auto currentPos = owner.valueToProportionOfLength (value);
		auto newPos = currentPos + proportionDelta;
		newPos = jlimit (0.0, 1.0, newPos);
		return owner.proportionOfLengthToValue (newPos) - value;
	}

	bool mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
	{
		if (scrollWheelEnabled)
		{
			// sometimes duplicate wheel events seem to be sent, so since we're going to
			// bump the value by a minimum of the interval, avoid doing this twice..
			if (e.eventTime != lastMouseWheelTime)
			{
				lastMouseWheelTime = e.eventTime;

				if (normRange.end > normRange.start && ! e.mods.isAnyMouseButtonDown())
				{
					auto value = static_cast<double> (currentValue.getValue());
					auto wheelDelta = (std::abs (wheel.deltaX) > std::abs (wheel.deltaY)
									   ? -wheel.deltaX : wheel.deltaY);
					if (wheel.isReversed)
						wheelDelta = -wheelDelta;
					if (style==LinearBarVertical)
						wheelDelta = -wheelDelta;
					
					auto delta = getMouseWheelDelta (value, wheelDelta);
					
					if (delta != 0.0)
					{
						auto newValue = value + delta;

//						DragInProgress drag (*this);
						
						currentValue = newValue;
						int newIntValue = (int)(newValue);
						if (newIntValue != intValue)
						{
							intValue = newIntValue;
							setValue (owner.snapValue (intValue), sendNotificationSync);
						}
					}
				}
			}

			return true;
		}

		return false;
	}

	void modifierKeysChanged (const ModifierKeys& modifiers)
	{
//		if (style != IncDecButtons)
		restoreMouseIfHidden();
	}

	void restoreMouseIfHidden()
	{
		for (auto& ms : Desktop::getInstance().getMouseSources())
		{
			if (ms.isUnboundedMouseMovementEnabled())
			{
				ms.enableUnboundedMouseMovement (false);

				auto pos = currentValue.getValue();
				juce::Point<float> mousePos;

				
				auto pixelPos = (float) getLinearSliderPos (pos);

				mousePos = owner.localPointToGlobal (juce::Point<float> (isHorizontal() ? pixelPos : ((float) owner.getWidth()  / 2.0f),
																   isVertical()   ? pixelPos : ((float) owner.getHeight() / 2.0f)));
			

				const_cast <MouseInputSource&> (ms).setScreenPosition (mousePos);
			}
		}
	}

	//==============================================================================
	void paint (Graphics& g, LookAndFeel& lf)
	{
		String text = owner.getTextFromValue(lastCurrentValue);
		lf.drawComboSlider (g,
							 owner.getX(), owner.getY(),
							 owner.getWidth(), owner.getHeight(),
							 text, getLinearSliderPos (lastCurrentValue),
							 lastCurrentValue,
							 style, owner);
	}

	//==============================================================================
	void resized (LookAndFeel& laf)
	{
		if (isHorizontal())
		{
			sliderRegionStart = owner.getX();
			sliderRegionSize = owner.getWidth();
		}
		else if (isVertical())
		{
			sliderRegionStart = owner.getY();
			sliderRegionSize = owner.getHeight();
		}
//		else if (style == IncDecButtons)
//		{
//			resizeIncDecButtons();
//		}
	}

	//==============================================================================

//	void resizeIncDecButtons()
//	{
//		auto buttonRect = sliderRect;
//
//		if (textBoxPos == TextBoxLeft || textBoxPos == TextBoxRight)
//			buttonRect.expand (-2, 0);
//		else
//			buttonRect.expand (0, -2);
//
//		incDecButtonsSideBySide = buttonRect.getWidth() > buttonRect.getHeight();
//
//		if (incDecButtonsSideBySide)
//		{
//			decButton->setBounds (buttonRect.removeFromLeft (buttonRect.getWidth() / 2));
//			decButton->setConnectedEdges (Button::ConnectedOnRight);
//			incButton->setConnectedEdges (Button::ConnectedOnLeft);
//		}
//		else
//		{
//			decButton->setBounds (buttonRect.removeFromBottom (buttonRect.getHeight() / 2));
//			decButton->setConnectedEdges (Button::ConnectedOnTop);
//			incButton->setConnectedEdges (Button::ConnectedOnBottom);
//		}
//
//		incButton->setBounds (buttonRect);
//	}

	//==============================================================================
	ComboSlider& owner;
	DragDirection style;

	ListenerList<ComboSlider::Listener> listeners;
	Value currentValue;
	int intValue;
	double lastCurrentValue = 0;
	NormalisableRange<double> normRange { 0.0, 10.0 };
	double doubleClickReturnValue = 0;
	double valueWhenLastDragged = 0, valueOnMouseDown = 0, lastAngle = 0;
//	double velocityModeSensitivity = 1.0, velocityModeOffset = 0, minMaxDiff = 0;
//	int velocityModeThreshold = 1;
	juce::Point<float> mouseDragStartPos, mousePosWhenLastDragged;
	int sliderRegionStart = 0, sliderRegionSize = 1;
//	int sliderBeingDragged = -1;
	int pixelsForFullDragExtent = 250;
	Time lastMouseWheelTime;
	std::unique_ptr<DragInProgress> currentDrag;

	String textSuffix;
	int numDecimalPlaces = 7;
	int textBoxWidth = 80, textBoxHeight = 20;
	IncDecButtonMode incDecButtonMode = incDecButtonsNotDraggable;
	ModifierKeys::Flags modifierToSwapModes = ModifierKeys::ctrlAltCommandModifiers;

	bool editableText = true;
	bool doubleClickToValue = false;
//	bool isVelocityBased = false;
	bool userKeyOverridesVelocity = true;
	bool incDecButtonsSideBySide = false;
	bool sendChangeOnlyOnRelease = false;
	bool showPopupOnDrag = false;
	bool showPopupOnHover = false;
	bool menuEnabled = false;
	bool useDragEvents = false;
	bool incDecDragged = false;
	bool scrollWheelEnabled = true;
	bool snapsToMousePos = false;

	int popupHoverTimeout = 2000;
	double lastPopupDismissal = 0.0;

	ModifierKeys singleClickModifiers;

//	std::unique_ptr<Button> incButton, decButton;

	//==============================================================================
	struct PopupDisplayComponent  : public BubbleComponent,
									public Timer
	{
		PopupDisplayComponent (ComboSlider& s, bool isOnDesktop)
			: owner (s),
			  font (s.getLookAndFeel().getComboSliderPopupFont (s))
		{
			if (isOnDesktop)
				setTransform (AffineTransform::scale (Component::getApproximateScaleFactorForComponent (&s)));

			setAlwaysOnTop (true);
			setAllowedPlacement (owner.getLookAndFeel().getComboSliderPopupPlacement (s));
			setLookAndFeel (&s.getLookAndFeel());
		}

		~PopupDisplayComponent() override
		{
			if (owner.pimpl != nullptr)
				owner.pimpl->lastPopupDismissal = Time::getMillisecondCounterHiRes();
		}
		
		bool twoLine() {
			return ! owner.getName().isEmpty();
		}
		void paintContent (Graphics& g, int w, int h) override
		{
			g.setFont (font);
			g.setColour (owner.findColour (TooltipWindow::textColourId, true));
			String drText = twoLine() ? owner.getName()+"\n"+text : text;
			g.drawFittedText (drText, juce::Rectangle<int> (w, h), Justification::centred, 2);
		}
		
		void getContentSize (int& w, int& h) override
		{
			w = std::max(font.getStringWidth (text), font.getStringWidth(owner.getName())) + 18;
			h = (int) (font.getHeight() * (twoLine() ? 2.6f : 1.6f) );
		}

		void updatePosition (const String& newText)
		{
			text = newText;
			BubbleComponent::setPosition (&owner);
			repaint();
		}

		void timerCallback() override
		{
			stopTimer();
			owner.pimpl->popupDisplay.reset();
		}

	private:
		//==============================================================================
		ComboSlider& owner;
		Font font;
		String text;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupDisplayComponent)
	};

	std::unique_ptr<PopupDisplayComponent> popupDisplay;
	Component* parentForPopupDisplay = nullptr;

	//==============================================================================
	static double smallestAngleBetween (double a1, double a2) noexcept
	{
		return jmin (std::abs (a1 - a2),
					 std::abs (a1 + MathConstants<double>::twoPi - a2),
					 std::abs (a2 + MathConstants<double>::twoPi - a1));
	}
};


//==============================================================================
ComboSlider::ComboSlider()
{
	init (LinearBarVertical);
}

ComboSlider::ComboSlider (const String& name)  : Component (name)
{
	init (LinearBarVertical);
}

ComboSlider::ComboSlider (DragDirection style)
{
	init (style);
}

void ComboSlider::init (DragDirection style)
{
	setWantsKeyboardFocus (false);
	setRepaintsOnMouseActivity (true);

	pimpl.reset (new Pimpl (*this, style));

	ComboSlider::lookAndFeelChanged();

	pimpl->registerListeners();
}

ComboSlider::~ComboSlider() {}

//==============================================================================
void ComboSlider::addListener (Listener* l)       { pimpl->listeners.add (l); }
void ComboSlider::removeListener (Listener* l)    { pimpl->listeners.remove (l); }

//==============================================================================
ComboSlider::DragDirection ComboSlider::getSliderStyle() const noexcept     { return pimpl->style; }
void ComboSlider::setSliderStyle (DragDirection newStyle)              { pimpl->setSliderStyle (newStyle); }

int ComboSlider::getMouseDragSensitivity() const noexcept        { return pimpl->pixelsForFullDragExtent; }

void ComboSlider::setMouseDragSensitivity (int distanceForFullScaleDrag)
{
	jassert (distanceForFullScaleDrag > 0);

	pimpl->pixelsForFullDragExtent = distanceForFullScaleDrag;
}

void ComboSlider::setIncDecButtonsMode (IncDecButtonMode mode)                   { pimpl->setIncDecButtonsMode (mode); }

void ComboSlider::setChangeNotificationOnlyOnRelease (bool onlyNotifyOnRelease)
{
	pimpl->sendChangeOnlyOnRelease = onlyNotifyOnRelease;
}

bool ComboSlider::getSliderSnapsToMousePosition() const noexcept           { return pimpl->snapsToMousePos; }
void ComboSlider::setSliderSnapsToMousePosition (bool shouldSnapToMouse)   { pimpl->snapsToMousePos = shouldSnapToMouse; }

void ComboSlider::setPopupDisplayEnabled (bool showOnDrag, bool showOnHover, Component* parent, int hoverTimeout)
{
	pimpl->showPopupOnDrag = showOnDrag;
	pimpl->showPopupOnHover = showOnHover;
	pimpl->parentForPopupDisplay = parent;
	pimpl->popupHoverTimeout = hoverTimeout;
}

juce::Component* ComboSlider::getCurrentPopupDisplay() const noexcept      { return pimpl->popupDisplay.get(); }

//==============================================================================
void ComboSlider::colourChanged()        { lookAndFeelChanged(); }
void ComboSlider::lookAndFeelChanged()   { pimpl->lookAndFeelChanged (getLookAndFeel()); }

//==============================================================================
Range<double> ComboSlider::getRange() const noexcept  { return { pimpl->normRange.start, pimpl->normRange.end }; }
double ComboSlider::getMaximum() const noexcept       { return pimpl->normRange.end; }
double ComboSlider::getMinimum() const noexcept       { return pimpl->normRange.start; }
double ComboSlider::getInterval() const noexcept      { return pimpl->normRange.interval; }

void ComboSlider::setRange (double newMin, double newMax, double newInt)      { pimpl->setRange (newMin, newMax, newInt); }
void ComboSlider::setRange (Range<double> newRange, double newInt)            { pimpl->setRange (newRange.getStart(), newRange.getEnd(), newInt); }
void ComboSlider::setNormalisableRange (NormalisableRange<double> newRange)   { pimpl->setNormalisableRange (newRange); }

double ComboSlider::getValue() const                  { return pimpl->getValue(); }
Value& ComboSlider::getValueObject() noexcept         { return pimpl->currentValue; }

void ComboSlider::setValue (double newValue, NotificationType notification)
{
	pimpl->setValue (newValue, notification);
}

void ComboSlider::setDoubleClickReturnValue (bool isDoubleClickEnabled,  double valueToSetOnDoubleClick, ModifierKeys mods)
{
	pimpl->doubleClickToValue = isDoubleClickEnabled;
	pimpl->doubleClickReturnValue = valueToSetOnDoubleClick;
	pimpl->singleClickModifiers = mods;
}

double ComboSlider::getDoubleClickReturnValue() const noexcept       { return pimpl->doubleClickReturnValue; }
bool ComboSlider::isDoubleClickReturnEnabled() const noexcept        { return pimpl->doubleClickToValue; }

void ComboSlider::setTextValueSuffix (const String& suffix)
{
	pimpl->setTextValueSuffix (suffix);
}

String ComboSlider::getTextValueSuffix() const
{
	return pimpl->textSuffix;
}

String ComboSlider::getTextFromValue (double v)
{
	auto getText = [this] (double val)
	{
		if (textFromValueFunction != nullptr)
			return textFromValueFunction (val);

//		if (getNumDecimalPlacesToDisplay() > 0)
//			return String (val, getNumDecimalPlacesToDisplay());

		return String (roundToInt (val));
	};

	return getText (v) + getTextValueSuffix();
}

double ComboSlider::getValueFromText (const String& text)
{
	auto t = text.trimStart();

	if (t.endsWith (getTextValueSuffix()))
		t = t.substring (0, t.length() - getTextValueSuffix().length());

	if (valueFromTextFunction != nullptr)
		return valueFromTextFunction (t);

	while (t.startsWithChar ('+'))
		t = t.substring (1).trimStart();

	return t.initialSectionContainingOnly ("0123456789.,-")
			.getDoubleValue();
}

double ComboSlider::proportionOfLengthToValue (double proportion)
{
	return pimpl->normRange.convertFrom0to1 (proportion);
}

double ComboSlider::valueToProportionOfLength (double value)
{
	return pimpl->normRange.convertTo0to1 (value);
}

double ComboSlider::snapValue (double attemptedValue)
{
	return attemptedValue;
}

//int ComboSlider::getNumDecimalPlacesToDisplay() const noexcept   { return pimpl->numDecimalPlaces; }
//
//void ComboSlider::setNumDecimalPlacesToDisplay (int decimalPlacesToDisplay)
//{
//	pimpl->numDecimalPlaces = decimalPlacesToDisplay;
//	updateText();
//}

//==============================================================================
void ComboSlider::startedDragging() {}
void ComboSlider::stoppedDragging() {}
void ComboSlider::valueChanged() {}

//==============================================================================
void ComboSlider::setPopupMenuEnabled (bool menuEnabled)         { pimpl->menuEnabled = menuEnabled; }
void ComboSlider::setScrollWheelEnabled (bool enabled)           { pimpl->scrollWheelEnabled = enabled; }

bool ComboSlider::isHorizontal() const noexcept                  { return pimpl->isHorizontal(); }
bool ComboSlider::isVertical() const noexcept                    { return pimpl->isVertical(); }

float ComboSlider::getPositionOfValue (double value) const       { return pimpl->getPositionOfValue (value); }

//==============================================================================
void ComboSlider::paint (Graphics& g)        { pimpl->paint (g, getLookAndFeel()); }
void ComboSlider::resized()                  { pimpl->resized (getLookAndFeel()); }

void ComboSlider::focusOfChildComponentChanged (FocusChangeType)     { repaint(); }

void ComboSlider::mouseDown (const MouseEvent& e)    { pimpl->mouseDown (e); }
void ComboSlider::mouseUp   (const MouseEvent&)      { pimpl->mouseUp(); }
void ComboSlider::mouseMove (const MouseEvent&)      { pimpl->mouseMove(); }
void ComboSlider::mouseExit (const MouseEvent&)      { pimpl->mouseExit(); }

// If popup display is enabled and set to show on mouse hover, this makes sure
// it is shown when dragging the mouse over a slider and releasing
void ComboSlider::mouseEnter (const MouseEvent&)     { pimpl->mouseMove(); }

void ComboSlider::modifierKeysChanged (const ModifierKeys& modifiers)
{
	if (isEnabled())
		pimpl->modifierKeysChanged (modifiers);
}

void ComboSlider::mouseDrag (const MouseEvent& e)
{
	if (isEnabled())
		pimpl->mouseDrag (e);
}

void ComboSlider::mouseDoubleClick (const MouseEvent&)
{
	if (isEnabled())
		pimpl->mouseDoubleClick();
}

void ComboSlider::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
	if (! (isEnabled() && pimpl->mouseWheelMove (e, wheel)))
		Component::mouseWheelMove (e, wheel);
}


void ComboSlider::LookAndFeelMethods::drawComboSlider (juce::Graphics& g,
				   int x, int y, int w, int h,
				   const String& text, float pos, int itemId,
				   const ComboSlider::DragDirection drag,
				   ComboSlider& c)
{
	drawComboSliderBackground(g, x, y, w, h, text, pos, itemId, drag, c);
//
	g.setFont(getFontForComboSlider(c));
	g.setColour(c.findColour(ComboSlider::textColourId));
	
//	g.setColour(Colours::white);
	
	g.drawFittedText(text, 0, 0, w, h, Justification::centred, 2);
}

void ComboSlider::LookAndFeelMethods::drawComboSliderBackground (juce::Graphics& g,
											   int x, int y, int w, int h,
											   const String& text,
											   float pos, int itemId,
											   const ComboSlider::DragDirection style,
											   ComboSlider& c)
{
	g.setColour(c.findColour(ComboSlider::backgroundColourId));
	g.fillRect(0, 0, w, h);
	
	g.setColour (c.findColour (ComboSlider::outlineColourId));
	g.drawRect (0, 0, w, h, 1);
}
int ComboSlider::LookAndFeelMethods::getComboSliderPopupPlacement (ComboSlider&)
{
	return BubbleComponent::above
			| BubbleComponent::below
			| BubbleComponent::left
			| BubbleComponent::right;
}
