//
//  SliderIncDecBottom.cpp
//  
//
//  Created by Isaac Roberts on 8/10/21.
//

// Created by Isaac Roberts

/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 6 End-User License
   Agreement and JUCE Privacy Policy (both effective as of the 16th June 2020).

   End User License Agreement: www.juce.com/juce-6-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

class SliderIncDecBottom::Pimpl   : public AsyncUpdater, // this needs to be public otherwise it will cause an
											 // error when JUCE_DLL_BUILD=1
						private Value::Listener
{
public:
	Pimpl (SliderIncDecBottom& s, TextEntryBoxPosition textBoxPosition)
	  : owner (s),
		textBoxPos (textBoxPosition)
	{
	}

	~Pimpl() override
	{
		currentValue.removeListener (this);
	}

	//==============================================================================
	void registerListeners()
	{
		currentValue.addListener (this);
	}

	bool incDecDragDirectionIsHorizontal() const noexcept
	{
		return incDecButtonMode == incDecButtonsDraggable_Horizontal
				|| (incDecButtonMode == incDecButtonsDraggable_AutoDirection && incDecButtonsSideBySide);
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

		// keep the current values inside the new range.
		setValue (getValue(), dontSendNotification);

		updateText();
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
			if (valueBox != nullptr)
				valueBox->hideEditor (true);

			lastCurrentValue = newValue;

			// (need to do this comparison because the Value will use equalsWithSameType to compare
			// the new and old values, so will generate unwanted change events if the type changes)
			if (currentValue != newValue)
				currentValue = newValue;

			updateText();
			owner.repaint();

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
		listeners.callChecked (checker, [&] (SliderIncDecBottom::Listener& l) { l.incDecSliderValueChanged (&owner); });

		if (checker.shouldBailOut())
			return;

		if (owner.onValueChange != nullptr)
			owner.onValueChange();
	}

	void sendDragStart()
	{
		owner.startedDragging();

		Component::BailOutChecker checker (&owner);
		listeners.callChecked (checker, [&] (SliderIncDecBottom::Listener& l) { l.incDecSliderDragStarted (&owner); });

		if (checker.shouldBailOut())
			return;

		if (owner.onDragStart != nullptr)
			owner.onDragStart();
	}

	void sendDragEnd()
	{
		owner.stoppedDragging();
		sliderBeingDragged = -1;

		Component::BailOutChecker checker (&owner);
		listeners.callChecked (checker, [&] (SliderIncDecBottom::Listener& l) { l.incDecSliderDragEnded (&owner); });

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
		auto newValue = owner.snapValue (getValue() + delta, notDragging);
		
		if (currentDrag != nullptr)
		{
			setValue (newValue, sendNotificationSync);
		}
		else
		{
			DragInProgress drag (*this);
			setValue (newValue, sendNotificationSync);
		}
		
	}

	void valueChanged (Value& value) override
	{
		if (value.refersToSameSourceAs (currentValue))
		{
			setValue (currentValue.getValue(), dontSendNotification);
		}
	}

	void textChanged()
	{
		auto newValue = owner.snapValue (owner.getValueFromText (valueBox->getText()), notDragging);

		if (newValue != static_cast<double> (currentValue.getValue()))
		{
			DragInProgress drag (*this);
			setValue (newValue, sendNotificationSync);
		}

		updateText(); // force a clean-up of the text, needed in case setValue() hasn't done this.
	}

	void updateText()
	{
		if (valueBox != nullptr)
		{
			auto newValue = owner.getTextFromValue (currentValue.getValue());

			if (newValue != valueBox->getText())
				valueBox->setText (newValue, dontSendNotification);
		}
	}

	double constrainedValue (double value) const
	{
		return normRange.snapToLegalValue (value);
	}

	void setVelocityModeParameters (double sensitivity, int threshold,
									double offset, bool userCanPressKeyToSwapMode,
									ModifierKeys::Flags newModifierToSwapModes)
	{
		velocityModeSensitivity = sensitivity;
		velocityModeOffset = offset;
		velocityModeThreshold = threshold;
		userKeyOverridesVelocity = userCanPressKeyToSwapMode;
		modifierToSwapModes = newModifierToSwapModes;
	}

	void setIncDecButtonsMode (IncDecButtonMode mode)
	{
		if (incDecButtonMode != mode)
		{
			incDecButtonMode = mode;
			owner.lookAndFeelChanged();
		}
	}

	void setTextBoxStyle (TextEntryBoxPosition newPosition,
						  bool isReadOnly,
						  int textEntryBoxWidth,
						  int textEntryBoxHeight)
	{
		if (textBoxPos != newPosition
			 || editableText != (! isReadOnly)
			 || textBoxWidth != textEntryBoxWidth
			 || textBoxHeight != textEntryBoxHeight)
		{
			textBoxPos = newPosition;
			editableText = ! isReadOnly;
			textBoxWidth = textEntryBoxWidth;
			textBoxHeight = textEntryBoxHeight;

			owner.repaint();
			owner.lookAndFeelChanged();
		}
	}

	void setTextBoxIsEditable (bool shouldBeEditable)
	{
		editableText = shouldBeEditable;
		updateTextBoxEnablement();
	}

	void showTextBox()
	{
		jassert (editableText); // this should probably be avoided in read-only sliders.

		if (valueBox != nullptr)
			valueBox->showEditor();
	}

	void hideTextBox (bool discardCurrentEditorContents)
	{
		if (valueBox != nullptr)
		{
			valueBox->hideEditor (discardCurrentEditorContents);

			if (discardCurrentEditorContents)
				updateText();
		}
	}

	void setTextValueSuffix (const String& suffix)
	{
		if (textSuffix != suffix)
		{
			textSuffix = suffix;
			updateText();
		}
	}

	void updateTextBoxEnablement()
	{
		if (valueBox != nullptr)
		{
			bool shouldBeEditable = editableText && owner.isEnabled();

			if (valueBox->isEditable() != shouldBeEditable) // (to avoid changing the single/double click flags unless we need to)
				valueBox->setEditable (shouldBeEditable);
		}
	}

	void lookAndFeelChanged (LookAndFeel& lf)
	{
		if (textBoxPos != NoTextBox)
		{
			auto previousTextBoxContent = (valueBox != nullptr ? valueBox->getText()
															   : owner.getTextFromValue (currentValue.getValue()));

			valueBox.reset();
			valueBox.reset (lf.createIncDecSliderTextBox (owner));
			owner.addAndMakeVisible (valueBox.get());

			valueBox->setWantsKeyboardFocus (false);
			valueBox->setText (previousTextBoxContent, dontSendNotification);
			valueBox->setTooltip (owner.getTooltip());
			updateTextBoxEnablement();
			valueBox->onTextChange = [this] { textChanged(); };

		}
		else
		{
			valueBox.reset();
		}

		incButton.reset (lf.createIncDecSliderButton (owner, true));
		decButton.reset (lf.createIncDecSliderButton (owner, false));

		owner.addAndMakeVisible (incButton.get());
		owner.addAndMakeVisible (decButton.get());

		incButton->onClick = [this] { incrementOrDecrement (normRange.interval); };
		decButton->onClick = [this] { incrementOrDecrement (-normRange.interval); };

		if (incDecButtonMode != incDecButtonsNotDraggable)
		{
			incButton->addMouseListener (&owner, false);
			decButton->addMouseListener (&owner, false);
		}
		else
		{
			incButton->setRepeatSpeed (300, 100, 20);
			decButton->setRepeatSpeed (300, 100, 20);
		}

		auto tooltip = owner.getTooltip();
		incButton->setTooltip (tooltip);
		decButton->setTooltip (tooltip);
	
		owner.setComponentEffect (lf.getIncDecSliderEffect (owner));

		owner.resized();
		owner.repaint();
	}

	void showPopupMenu()
	{
		PopupMenu m;
		m.setLookAndFeel (&owner.getLookAndFeel());
		m.addItem (1, TRANS ("Velocity-sensitive mode"), true, isVelocityBased);
		m.addSeparator();

		m.showMenuAsync (PopupMenu::Options(),
						 ModalCallbackFunction::forComponent (sliderMenuCallback, &owner));
	}

	static void sliderMenuCallback (int result, SliderIncDecBottom* slider)
	{
		if (slider != nullptr)
		{
			switch (result)
			{
				case 1:   slider->setVelocityBasedMode (! slider->getVelocityBasedMode()); break;
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
		double newPos = 0;
		
		auto mouseDiff = incDecDragDirectionIsHorizontal() ?
						   e.position.x - mouseDragStartPos.x
						  : mouseDragStartPos.y - e.position.y;

		newPos = owner.valueToProportionOfLength (valueOnMouseDown)
				   + mouseDiff * (1.0 / pixelsForFullDragExtent);

		incButton->setState (mouseDiff < 0 ? Button::buttonNormal : Button::buttonDown);
		decButton->setState (mouseDiff > 0 ? Button::buttonNormal : Button::buttonDown);
	
		newPos = jlimit (0.0, 1.0, newPos);
		valueWhenLastDragged = owner.proportionOfLengthToValue (newPos);
	}

	void handleVelocityDrag (const MouseEvent& e)
	{
		bool hasHorizontalStyle = incDecDragDirectionIsHorizontal();

		auto mouseDiff = (hasHorizontalStyle ? e.position.x - mousePosWhenLastDragged.x
												  : e.position.y - mousePosWhenLastDragged.y);

		auto maxSpeed = jmax (200.0, (double) sliderRegionSize);
		auto speed = jlimit (0.0, maxSpeed, (double) std::abs (mouseDiff));

		if (speed != 0.0)
		{
			speed = 0.2 * velocityModeSensitivity
					  * (1.0 + std::sin (MathConstants<double>::pi * (1.5 + jmin (0.5, velocityModeOffset
																						 + jmax (0.0, (double) (speed - velocityModeThreshold))
																							/ maxSpeed))));

			if (mouseDiff < 0)
				speed = -speed;

			if (! incDecDragDirectionIsHorizontal())
				speed = -speed;

			auto newPos = owner.valueToProportionOfLength (valueWhenLastDragged) + speed;
			newPos = jlimit (0.0, 1.0, newPos);
			valueWhenLastDragged = owner.proportionOfLengthToValue (newPos);

			e.source.enableUnboundedMouseMovement (true, false);
		}
	}

	void mouseDown (const MouseEvent& e)
	{
		incDecDragged = false;
		useDragEvents = false;
		mouseDragStartPos = mousePosWhenLastDragged = e.position;
		currentDrag.reset();

		if (owner.isEnabled())
		{
			if (e.mods.isPopupMenu() && menuEnabled)
			{
				showPopupMenu();
			}
			else if (normRange.end > normRange.start)
			{
				useDragEvents = true;

				if (valueBox != nullptr)
					valueBox->hideEditor (true);

				sliderBeingDragged = 0;
				
				valueWhenLastDragged = currentValue.getValue();
				
				valueOnMouseDown = valueWhenLastDragged;

				currentDrag.reset (new DragInProgress (*this));
				mouseDrag (e);
			}
		}
	}

	void mouseDrag (const MouseEvent& e)
	{
		if (useDragEvents && normRange.end > normRange.start)
		{
			DragMode dragMode = notDragging;

			if (e.getDistanceFromDragStart() < 10 || ! e.mouseWasDraggedSinceMouseDown())
				return;

			incDecDragged = true;
			mouseDragStartPos = e.position;
		
			if (isAbsoluteDragMode (e.mods) || (normRange.end - normRange.start) / sliderRegionSize < normRange.interval)
			{
				dragMode = absoluteDrag;
				handleAbsoluteDrag (e);
			}
			else
			{
				dragMode = velocityDrag;
				handleVelocityDrag (e);
			}
		

			valueWhenLastDragged = jlimit (normRange.start, normRange.end, valueWhenLastDragged);

			if (sliderBeingDragged == 0)
			{
				setValue (owner.snapValue (valueWhenLastDragged, dragMode),
						  sendChangeOnlyOnRelease ? dontSendNotification : sendNotificationSync);
			}

			mousePosWhenLastDragged = e.position;
		}
	}

	void mouseUp()
	{
		if (owner.isEnabled()
			 && useDragEvents
			 && (normRange.end > normRange.start)
			 && (incDecDragged))
		{
			restoreMouseIfHidden();

			if (sendChangeOnlyOnRelease && valueOnMouseDown != static_cast<double> (currentValue.getValue()))
				triggerChangeMessage (sendNotificationAsync);

			currentDrag.reset();

			
			incButton->setState (Button::buttonNormal);
			decButton->setState (Button::buttonNormal);
		}

		currentDrag.reset();
	}


//	void showPopupDisplay()
//	{
//			return;
//	}

//	bool canDoubleClickToValue() const
//	{
//		return false;
//	}

//	void mouseDoubleClick()
//	{
//		if (false)
//		{
//		}
//	}

	double getMouseWheelDelta (double value, double wheelAmount)
	{
		return normRange.interval * wheelAmount;
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
					if (valueBox != nullptr)
						valueBox->hideEditor (false);

					auto value = static_cast<double> (currentValue.getValue());
					auto delta = getMouseWheelDelta (value, (std::abs (wheel.deltaX) > std::abs (wheel.deltaY)
																  ? -wheel.deltaX : wheel.deltaY)
															   * (wheel.isReversed ? -1.0f : 1.0f));
					if (delta != 0.0)
					{
						auto newValue = value + jmax (normRange.interval, std::abs (delta)) * (delta < 0 ? -1.0 : 1.0);

						DragInProgress drag (*this);
						setValue (owner.snapValue (newValue, notDragging), sendNotificationSync);
					}
				}
			}

			return true;
		}

		return false;
	}

	bool isAbsoluteDragMode (ModifierKeys mods) const
	{
		return isVelocityBased == (userKeyOverridesVelocity && mods.testFlags (modifierToSwapModes));
	}

	void restoreMouseIfHidden()
	{
		for (auto& ms : Desktop::getInstance().getMouseSources())
		{
			if (ms.isUnboundedMouseMovementEnabled())
			{
				ms.enableUnboundedMouseMovement (false);

				Point<float> mousePos;

				mousePos = owner.localPointToGlobal (Point<float> (((float) owner.getWidth()  / 2.0f),
																   ((float) owner.getHeight() / 2.0f)));
				

				const_cast <MouseInputSource&> (ms).setScreenPosition (mousePos);
			}
		}
	}

	//==============================================================================
	void paint (Graphics& g, LookAndFeel& lf)
	{
	}

	//==============================================================================
	void resized (LookAndFeel& lf)
	{
		auto layout = lf.getIncDecSliderLayout (owner);
		sliderRect = layout.sliderBounds;

		if (valueBox != nullptr)
			valueBox->setBounds (layout.textBoxBounds);
		
		resizeIncDecButtons();
		
	}

	//==============================================================================

	void resizeIncDecButtons()
	{
		auto buttonRect = sliderRect;

		if (textBoxPos == TextBoxLeft || textBoxPos == TextBoxRight)
			buttonRect.expand (-2, 0);
		else
			buttonRect.expand (0, -2);

		incDecButtonsSideBySide = buttonRect.getWidth() > buttonRect.getHeight();

		if (incDecButtonsSideBySide)
		{
			decButton->setBounds (buttonRect.removeFromLeft (buttonRect.getWidth() / 2));
			decButton->setConnectedEdges (Button::ConnectedOnRight);
			incButton->setConnectedEdges (Button::ConnectedOnLeft);
		}
		else
		{
			decButton->setBounds (buttonRect.removeFromBottom (buttonRect.getHeight() / 2));
			decButton->setConnectedEdges (Button::ConnectedOnTop);
			incButton->setConnectedEdges (Button::ConnectedOnBottom);
		}

		incButton->setBounds (buttonRect);
	}

	//==============================================================================
	SliderIncDecBottom& owner;

	ListenerList<SliderIncDecBottom::Listener> listeners;
	Value currentValue;
	double lastCurrentValue = 0;
	NormalisableRange<double> normRange { 0.0, 10.0 };
	double valueWhenLastDragged = 0, valueOnMouseDown = 0;
	double velocityModeSensitivity = 1.0, velocityModeOffset = 0;
	int velocityModeThreshold = 1;
	Point<float> mouseDragStartPos, mousePosWhenLastDragged;
	int sliderRegionSize = 1;
	int sliderBeingDragged = -1;
	int pixelsForFullDragExtent = 250;
	Time lastMouseWheelTime;
	Rectangle<int> sliderRect;
	std::unique_ptr<DragInProgress> currentDrag;

	TextEntryBoxPosition textBoxPos;
	String textSuffix;
	int numDecimalPlaces = 7;
	int textBoxWidth = 80, textBoxHeight = 20;
	IncDecButtonMode incDecButtonMode = incDecButtonsNotDraggable;
	ModifierKeys::Flags modifierToSwapModes = ModifierKeys::ctrlAltCommandModifiers;

	bool editableText = true;
	bool isVelocityBased = false;
	bool userKeyOverridesVelocity = true;
	bool incDecButtonsSideBySide = false;
	bool sendChangeOnlyOnRelease = false;
	bool showPopupOnDrag = false;
	bool showPopupOnHover = false;
	bool menuEnabled = false;
	bool useDragEvents = false;
	bool incDecDragged = false;
	bool scrollWheelEnabled = true;
	bool snapsToMousePos = true;

	std::unique_ptr<Label> valueBox;
	std::unique_ptr<Button> incButton, decButton;

	//==============================================================================
	static double smallestAngleBetween (double a1, double a2) noexcept
	{
		return jmin (std::abs (a1 - a2),
					 std::abs (a1 + MathConstants<double>::twoPi - a2),
					 std::abs (a2 + MathConstants<double>::twoPi - a1));
	}
};


//==============================================================================
SliderIncDecBottom::SliderIncDecBottom()
{
	init (TextBoxLeft);
}

SliderIncDecBottom::SliderIncDecBottom (const String& name)  : Component (name)
{
	init (TextBoxLeft);
}

SliderIncDecBottom::SliderIncDecBottom (TextEntryBoxPosition textBoxPos)
{
	init (textBoxPos);
}

void SliderIncDecBottom::init (TextEntryBoxPosition textBoxPos)
{
	setWantsKeyboardFocus (false);
	setRepaintsOnMouseActivity (true);

	pimpl.reset (new Pimpl (*this, textBoxPos));

	SliderIncDecBottom::lookAndFeelChanged();
	updateText();

	pimpl->registerListeners();
}

SliderIncDecBottom::~SliderIncDecBottom() {}

//==============================================================================
void SliderIncDecBottom::addListener (Listener* l)       { pimpl->listeners.add (l); }
void SliderIncDecBottom::removeListener (Listener* l)    { pimpl->listeners.remove (l); }

//==============================================================================


void SliderIncDecBottom::setVelocityBasedMode (bool vb)                 { pimpl->isVelocityBased = vb; }
bool SliderIncDecBottom::getVelocityBasedMode() const noexcept          { return pimpl->isVelocityBased; }
bool SliderIncDecBottom::getVelocityModeIsSwappable() const noexcept    { return pimpl->userKeyOverridesVelocity; }
int SliderIncDecBottom::getVelocityThreshold() const noexcept           { return pimpl->velocityModeThreshold; }
double SliderIncDecBottom::getVelocitySensitivity() const noexcept      { return pimpl->velocityModeSensitivity; }
double SliderIncDecBottom::getVelocityOffset() const noexcept           { return pimpl->velocityModeOffset; }

void SliderIncDecBottom::setVelocityModeParameters (double sensitivity, int threshold,
										double offset, bool userCanPressKeyToSwapMode,
										ModifierKeys::Flags modifierToSwapModes)
{
	jassert (threshold >= 0);
	jassert (sensitivity > 0);
	jassert (offset >= 0);

	pimpl->setVelocityModeParameters (sensitivity, threshold, offset,
									  userCanPressKeyToSwapMode, modifierToSwapModes);
}

double SliderIncDecBottom::getSkewFactor() const noexcept               { return pimpl->normRange.skew; }
bool SliderIncDecBottom::isSymmetricSkew() const noexcept               { return pimpl->normRange.symmetricSkew; }

void SliderIncDecBottom::setSkewFactor (double factor, bool symmetricSkew)
{
	pimpl->normRange.skew = factor;
	pimpl->normRange.symmetricSkew = symmetricSkew;
}

void SliderIncDecBottom::setSkewFactorFromMidPoint (double sliderValueToShowAtMidPoint)
{
	pimpl->normRange.setSkewForCentre (sliderValueToShowAtMidPoint);
}

int SliderIncDecBottom::getMouseDragSensitivity() const noexcept        { return pimpl->pixelsForFullDragExtent; }

void SliderIncDecBottom::setMouseDragSensitivity (int distanceForFullScaleDrag)
{
	jassert (distanceForFullScaleDrag > 0);

	pimpl->pixelsForFullDragExtent = distanceForFullScaleDrag;
}

void SliderIncDecBottom::setIncDecButtonsMode (IncDecButtonMode mode)                   { pimpl->setIncDecButtonsMode (mode); }

SliderIncDecBottom::TextEntryBoxPosition SliderIncDecBottom::getTextBoxPosition() const noexcept    { return pimpl->textBoxPos; }
int SliderIncDecBottom::getTextBoxWidth() const noexcept                                { return pimpl->textBoxWidth; }
int SliderIncDecBottom::getTextBoxHeight() const noexcept                               { return pimpl->textBoxHeight; }

void SliderIncDecBottom::setTextBoxStyle (TextEntryBoxPosition newPosition, bool isReadOnly, int textEntryBoxWidth, int textEntryBoxHeight)
{
	pimpl->setTextBoxStyle (newPosition, isReadOnly, textEntryBoxWidth, textEntryBoxHeight);
}

bool SliderIncDecBottom::isTextBoxEditable() const noexcept                     { return pimpl->editableText; }
void SliderIncDecBottom::setTextBoxIsEditable (const bool shouldBeEditable)     { pimpl->setTextBoxIsEditable (shouldBeEditable); }
void SliderIncDecBottom::showTextBox()                                          { pimpl->showTextBox(); }
void SliderIncDecBottom::hideTextBox (bool discardCurrentEditorContents)        { pimpl->hideTextBox (discardCurrentEditorContents); }

void SliderIncDecBottom::setChangeNotificationOnlyOnRelease (bool onlyNotifyOnRelease)
{
	pimpl->sendChangeOnlyOnRelease = onlyNotifyOnRelease;
}

bool SliderIncDecBottom::getSliderSnapsToMousePosition() const noexcept           { return pimpl->snapsToMousePos; }
void SliderIncDecBottom::setSliderSnapsToMousePosition (bool shouldSnapToMouse)   { pimpl->snapsToMousePos = shouldSnapToMouse; }

//==============================================================================
void SliderIncDecBottom::colourChanged()        { lookAndFeelChanged(); }
void SliderIncDecBottom::lookAndFeelChanged()   { pimpl->lookAndFeelChanged (getLookAndFeel()); }
void SliderIncDecBottom::enablementChanged()    { repaint(); pimpl->updateTextBoxEnablement(); }

//==============================================================================
Range<double> SliderIncDecBottom::getRange() const noexcept  { return { pimpl->normRange.start, pimpl->normRange.end }; }
double SliderIncDecBottom::getMaximum() const noexcept       { return pimpl->normRange.end; }
double SliderIncDecBottom::getMinimum() const noexcept       { return pimpl->normRange.start; }
double SliderIncDecBottom::getInterval() const noexcept      { return pimpl->normRange.interval; }

void SliderIncDecBottom::setRange (double newMin, double newMax, double newInt)      { pimpl->setRange (newMin, newMax, newInt); }
void SliderIncDecBottom::setRange (Range<double> newRange, double newInt)            { pimpl->setRange (newRange.getStart(), newRange.getEnd(), newInt); }
void SliderIncDecBottom::setNormalisableRange (NormalisableRange<double> newRange)   { pimpl->setNormalisableRange (newRange); }

double SliderIncDecBottom::getValue() const                  { return pimpl->getValue(); }
Value& SliderIncDecBottom::getValueObject() noexcept         { return pimpl->currentValue; }

void SliderIncDecBottom::setValue (double newValue, NotificationType notification)
{
	pimpl->setValue (newValue, notification);
}

void SliderIncDecBottom::updateText()
{
	pimpl->updateText();
}

void SliderIncDecBottom::setTextValueSuffix (const String& suffix)
{
	pimpl->setTextValueSuffix (suffix);
}

String SliderIncDecBottom::getTextValueSuffix() const
{
	return pimpl->textSuffix;
}

String SliderIncDecBottom::getTextFromValue (double v)
{
	auto getText = [this] (double val)
	{
		if (textFromValueFunction != nullptr)
			return textFromValueFunction (val);

		if (getNumDecimalPlacesToDisplay() > 0)
			return String (val, getNumDecimalPlacesToDisplay());

		return String (roundToInt (val));
	};

	return getText (v) + getTextValueSuffix();
}

double SliderIncDecBottom::getValueFromText (const String& text)
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

double SliderIncDecBottom::proportionOfLengthToValue (double proportion)
{
	return pimpl->normRange.convertFrom0to1 (proportion);
}

double SliderIncDecBottom::valueToProportionOfLength (double value)
{
	return pimpl->normRange.convertTo0to1 (value);
}

double SliderIncDecBottom::snapValue (double attemptedValue, DragMode)
{
	return attemptedValue;
}

int SliderIncDecBottom::getNumDecimalPlacesToDisplay() const noexcept   { return pimpl->numDecimalPlaces; }

void SliderIncDecBottom::setNumDecimalPlacesToDisplay (int decimalPlacesToDisplay)
{
	pimpl->numDecimalPlaces = decimalPlacesToDisplay;
	updateText();
}

//==============================================================================
int SliderIncDecBottom::getThumbBeingDragged() const noexcept           { return pimpl->sliderBeingDragged; }
void SliderIncDecBottom::startedDragging() {}
void SliderIncDecBottom::stoppedDragging() {}
void SliderIncDecBottom::valueChanged() {}

//==============================================================================
void SliderIncDecBottom::setScrollWheelEnabled (bool enabled)           { pimpl->scrollWheelEnabled = enabled; }

//==============================================================================
void SliderIncDecBottom::paint (Graphics& g)        { pimpl->paint (g, getLookAndFeel()); }
void SliderIncDecBottom::resized()                  { pimpl->resized (getLookAndFeel()); }

void SliderIncDecBottom::focusOfChildComponentChanged (FocusChangeType)     { repaint(); }

void SliderIncDecBottom::mouseDown (const MouseEvent& e)    { pimpl->mouseDown (e); }
void SliderIncDecBottom::mouseUp   (const MouseEvent&)      { pimpl->mouseUp(); }
void SliderIncDecBottom::mouseMove (const MouseEvent&)      { }
void SliderIncDecBottom::mouseExit (const MouseEvent&)      { }

// If popup display is enabled and set to show on mouse hover, this makes sure
// it is shown when dragging the mouse over a slider and releasing
void SliderIncDecBottom::mouseEnter (const MouseEvent&)     { }

void SliderIncDecBottom::modifierKeysChanged (const ModifierKeys& modifiers)
{
}

void SliderIncDecBottom::mouseDrag (const MouseEvent& e)
{
	if (isEnabled())
		pimpl->mouseDrag (e);
}

void SliderIncDecBottom::mouseDoubleClick (const MouseEvent&)
{
}

void SliderIncDecBottom::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
	if (! (isEnabled() && pimpl->mouseWheelMove (e, wheel)))
		Component::mouseWheelMove (e, wheel);
}

} // namespace juce
