//
//  TwoValueSlider.cpp
//  
//
//  Created by Isaac Roberts on 2/28/21.
//

// Created by Isaac Roberts


//#include "TwoValueSlider.h"
//#include "RTLAF/TwoValueCommon.h"

namespace TwoValue
{

class TwoValueSlider::Pimpl   : public AsyncUpdater, // this needs to be public otherwise it will cause an
											 // error when JUCE_DLL_BUILD=1
						private Value::Listener
{
public:
	Pimpl (TwoValueSlider& s, SliderStyle sliderStyle, TextEntryBoxPosition textBoxPosition)
	  : owner (s),
		style (sliderStyle),
		textBoxPos (textBoxPosition)
	{
		rotaryParams.startAngleRadians = MathConstants<float>::pi * 1.2f;
		rotaryParams.endAngleRadians   = MathConstants<float>::pi * 2.8f;
		rotaryParams.stopAtEnd = true;
	}

	~Pimpl() override
	{
		valueMin.removeListener (this);
		valueMax.removeListener (this);
		popupDisplay.reset();
	}

	//==============================================================================
	void registerListeners()
	{
		valueMin.addListener (this);
		valueMax.addListener (this);
	}

	bool isHorizontal() const noexcept
	{
		return style == TwoValueHorizontal;
	}

	bool isVertical() const noexcept
	{
		return style == TwoValueVertical;
	}

	bool isRotary() const noexcept
	{
		return style == Rotary;
	}

	bool isBar() const noexcept
	{
		return false;
	}

	bool isTwoValue() const noexcept
	{
		return true;
	}

	bool isThreeValue() const noexcept
	{
		return false;
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

	void updateRange()
	{
		// figure out the number of DPs needed to display all values at this
		// interval setting.
		numDecimalPlaces = 3;

		if (normRange.interval != 0.0)
		{
			int v = std::abs (roundToInt (normRange.interval * 10000000));

			while ((v % 10) == 0 && numDecimalPlaces > 0)
			{
				--numDecimalPlaces;
				v /= 10;
			}
		}

		setMinValue (getMinValue(), dontSendNotification, false);
		setMaxValue (getMaxValue(), dontSendNotification, false);
		
		updateMinText();
		updateMaxText();
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

	void setMinValue (double newValue, NotificationType notification, bool allowNudgingOfOtherValues)
	{
		if (minValueBox != nullptr)
			minValueBox->hideEditor (true);

		// The minimum value only applies to sliders that are in two- or three-value mode.
		newValue = constrainedValue (newValue);

		if (allowNudgingOfOtherValues && newValue > static_cast<double> (valueMax.getValue()))
			setMaxValue (newValue, notification, false);

		newValue = jmin (static_cast<double> (valueMax.getValue()), newValue);
	
		
		if (lastValueMin != newValue)
		{
			lastValueMin = newValue;
			valueMin = newValue;
			updateMinText();
			owner.repaint();
			updatePopupDisplay (newValue);

			triggerChangeMessage (notification);
		}
	}

	void setMaxValue (double newValue, NotificationType notification, bool allowNudgingOfOtherValues)
	{
		// The maximum value only applies to sliders that are in two- or three-value mode.
		if (maxValueBox != nullptr)
			maxValueBox->hideEditor (true);

		newValue = constrainedValue (newValue);

		if (allowNudgingOfOtherValues && newValue < static_cast<double> (valueMin.getValue()))
			setMinValue (newValue, notification, false);

		newValue = jmax (static_cast<double> (valueMin.getValue()), newValue);
		
		if (lastValueMax != newValue)
		{
			lastValueMax = newValue;
			valueMax = newValue;
			updateMaxText();
			owner.repaint();
			updatePopupDisplay (valueMax.getValue());

			triggerChangeMessage (notification);
		}
	}

	void setMinAndMaxValues (double newMinValue, double newMaxValue, NotificationType notification)
	{
		// The maximum value only applies to sliders that are in two- or three-value mode.
		if (newMaxValue < newMinValue)
			std::swap (newMaxValue, newMinValue);
		
		if (minValueBox != nullptr)
			minValueBox->hideEditor (true);
		if (maxValueBox != nullptr)
			maxValueBox->hideEditor (true);

		newMinValue = constrainedValue (newMinValue);
		newMaxValue = constrainedValue (newMaxValue);

		if (lastValueMax != newMaxValue || lastValueMin != newMinValue)
		{
			lastValueMax = newMaxValue;
			lastValueMin = newMinValue;
			valueMin = newMinValue;
			valueMax = newMaxValue;
			updateText();
			owner.repaint();

			triggerChangeMessage (notification);
		}
	}

	double getMinValue() const
	{
		return valueMin.getValue();
	}

	double getMaxValue() const
	{
		return valueMax.getValue();
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
		listeners.callChecked (checker, [&] (TwoValueSlider::Listener& l) { l.sliderLowValueChanged (&owner); });
		listeners.callChecked (checker, [&] (TwoValueSlider::Listener& l) { l.sliderHighValueChanged (&owner); });

		if (checker.shouldBailOut())
			return;

		if (owner.onLowValueChange != nullptr)
			owner.onLowValueChange();
		if (owner.onHighValueChange != nullptr)
			owner.onHighValueChange();
	}
	
	void handleAsyncLowUpdate()
	{
		cancelPendingUpdate();

		Component::BailOutChecker checker (&owner);
		listeners.callChecked (checker, [&] (TwoValueSlider::Listener& l) { l.sliderLowValueChanged (&owner); });

		if (checker.shouldBailOut())
			return;

		if (owner.onLowValueChange != nullptr)
			owner.onLowValueChange();
	}
	void handleAsyncHighUpdate()
	{
		cancelPendingUpdate();

		Component::BailOutChecker checker (&owner);
		listeners.callChecked (checker, [&] (TwoValueSlider::Listener& l) { l.sliderHighValueChanged (&owner); });

		if (checker.shouldBailOut())
			return;

		if (owner.onHighValueChange != nullptr)
			owner.onHighValueChange();
	}

	void sendDragStart()
	{
		owner.startedDragging();

		Component::BailOutChecker checker (&owner);
		listeners.callChecked (checker, [&] (TwoValueSlider::Listener& l) { l.sliderDragStarted (&owner); });

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
		listeners.callChecked (checker, [&] (TwoValueSlider::Listener& l) { l.sliderDragEnded (&owner); });

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
	}

	void valueChanged (Value& value) override
	{
		if (value.refersToSameSourceAs (valueMin))
			setMinValue (valueMin.getValue(), dontSendNotification, true);
		else if (value.refersToSameSourceAs (valueMax))
			setMaxValue (valueMax.getValue(), dontSendNotification, true);
	}

	void minTextChanged()
	{
		auto newMinValue = owner.snapValue (owner.getValueFromText (minValueBox->getText()), notDragging);

		if (newMinValue != static_cast<double> (valueMin.getValue()))
		{
			DragInProgress drag (*this);
			setMinValue (newMinValue, sendNotificationSync, true);
		}

		updateMinText(); // force a clean-up of the text, needed in case setValue() hasn't done this.
	}
	
	void maxTextChanged()
	{
		auto newMaxValue = owner.snapValue (owner.getValueFromText (maxValueBox->getText()), notDragging);

		if (newMaxValue != static_cast<double> (valueMax.getValue()))
		{
			DragInProgress drag (*this);
			setMaxValue (newMaxValue, sendNotificationSync, true);
		}

		updateMaxText(); // force a clean-up of the text, needed in case setValue() hasn't done this.
	}

	std::shared_ptr<Label> getTextBox(bool isMax) {
		if (isMax)
		{
			if (maxValueBox)
				return maxValueBox;
			else return NULL;
		}
		else
		{
			if (minValueBox)
				return minValueBox;
			else return NULL;
		}
	}
	
	void updateText()
	{
		updateMinText();
		updateMaxText();
	}
	void updateMinText()
	{
		if (minValueBox != nullptr)
		{
			auto newValue = owner.getTextFromValue (valueMin.getValue());

			if (newValue != minValueBox->getText())
				minValueBox->setText (newValue, dontSendNotification);
		}
	}
	void updateMaxText()
	{
		if (maxValueBox != nullptr)
		{
			auto newValue = owner.getTextFromValue (valueMax.getValue());

			if (newValue != maxValueBox->getText())
				maxValueBox->setText (newValue, dontSendNotification);
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

		if (isVertical())
			pos = 1.0 - pos;

		jassert (pos >= 0 && pos <= 1.0);
		return (float) (sliderRegionStart + pos * sliderRegionSize);
	}

	void setSliderStyle (SliderStyle newStyle)
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

		if (minValueBox != nullptr)
			minValueBox->showEditor();
		if (maxValueBox != nullptr)
			maxValueBox->showEditor();
	}

	void hideTextBox (bool discardCurrentEditorContents)
	{
		if (minValueBox != nullptr)
		{
			minValueBox->hideEditor (discardCurrentEditorContents);

			if (discardCurrentEditorContents)
				updateMinText();
		}
		if (maxValueBox != nullptr)
		{
			maxValueBox->hideEditor (discardCurrentEditorContents);

			if (discardCurrentEditorContents)
				updateMaxText();
		}
	}

	void setTextValueSuffix (const String& suffix)
	{
		if (textSuffix != suffix)
		{
			textSuffix = suffix;
			updateMinText();
			updateMaxText();
		}
	}

	void updateTextBoxEnablement()
	{
		if (minValueBox != nullptr)
		{
			bool shouldBeEditable = editableText && owner.isEnabled();

			if (minValueBox->isEditable() != shouldBeEditable) // (to avoid changing the single/double click flags unless we need to)
				minValueBox->setEditable (shouldBeEditable);
		}
		if (maxValueBox != nullptr)
		{
			bool shouldBeEditable = editableText && owner.isEnabled();

			if (maxValueBox->isEditable() != shouldBeEditable) // (to avoid changing the single/double click flags unless we need to)
				maxValueBox->setEditable (shouldBeEditable);
		}
	}

	void lookAndFeelChanged (LookAndFeel* lafm)
	{
		if (textBoxPos != NoTextBox)
		{
			auto previousMinTextBoxContent = (minValueBox != nullptr ? minValueBox->getText()
															   : owner.getTextFromValue (valueMin.getValue()));
			
			minValueBox.reset();
			minValueBox.reset (lafm->createTVSliderTextBox (owner));
			owner.addAndMakeVisible (minValueBox.get());

			minValueBox->setWantsKeyboardFocus (false);
			minValueBox->setText (previousMinTextBoxContent, dontSendNotification);
			minValueBox->setTooltip (owner.getTooltip());
			minValueBox->onTextChange = [this] { minTextChanged(); };

			
			auto previousMaxTextBoxContent = (maxValueBox != nullptr ? maxValueBox->getText()
															   : owner.getTextFromValue (valueMax.getValue()));
			
			maxValueBox.reset();
			maxValueBox.reset (lafm->createTVSliderTextBox (owner));
			owner.addAndMakeVisible (maxValueBox.get());

			maxValueBox->setWantsKeyboardFocus (false);
			maxValueBox->setText (previousMaxTextBoxContent, dontSendNotification);
			maxValueBox->setTooltip (owner.getTooltip());
			maxValueBox->onTextChange = [this] { maxTextChanged(); };

			
			updateTextBoxEnablement();
		}
		else
		{
			minValueBox.reset();
			maxValueBox.reset();
		}

//		owner.setComponentEffect (lf.getSliderEffect (owner));

		owner.resized();
		owner.repaint();
	}

	void showPopupMenu()
	{
//		PopupMenu m;
//		m.setLookAndFeel (&owner.getLookAndFeel());
//		m.addSeparator();

//		if (isRotary())
//		{
//			PopupMenu rotaryMenu;
//			rotaryMenu.addItem (2, TRANS ("Use circular dragging"),           true, style == Rotary);
//			rotaryMenu.addItem (3, TRANS ("Use left-right dragging"),         true, style == RotaryHorizontalDrag);
//			rotaryMenu.addItem (4, TRANS ("Use up-down dragging"),            true, style == RotaryVerticalDrag);
//			rotaryMenu.addItem (5, TRANS ("Use left-right/up-down dragging"), true, style == RotaryHorizontalVerticalDrag);
//
//			m.addSubMenu (TRANS ("Rotary mode"), rotaryMenu);
//		}

//		m.showMenuAsync (PopupMenu::Options(),
//						 ModalCallbackFunction::forComponent (sliderMenuCallback, &owner));
	}

//	static void sliderMenuCallback (int result, TwoValueSlider* slider)
//	{
//		if (slider != nullptr)
//		{
//			switch (result)
//			{
//				case 2:   slider->setSliderStyle (Rotary); break;
//				case 3:   slider->setSliderStyle (RotaryHorizontalDrag); break;
//				case 4:   slider->setSliderStyle (RotaryVerticalDrag); break;
//				case 5:   slider->setSliderStyle (RotaryHorizontalVerticalDrag); break;
//				default:  break;
//			}
//		}
//	}

	bool thumbsVisible() {
		return minVisible || maxVisible;
	}
	
	int getThumbIndexAt (const MouseEvent& e)
	{
		if (minVisible && maxVisible)
		{
			auto mousePos = isVertical() ? e.position.y : e.position.x;

			auto minPosDistance    = std::abs (getLinearSliderPos (valueMin.getValue()) + (isVertical() ? 0.1f : -0.1f) - mousePos);
			auto maxPosDistance    = std::abs (getLinearSliderPos (valueMax.getValue()) + (isVertical() ? -0.1f : 0.1f) - mousePos);

			return maxPosDistance <= minPosDistance ? 2 : 1;
		}
		else if (minVisible)
			return 1;
		else if (maxVisible)
			return 2;
		else
			return 0;
	}

	//==============================================================================
	/*
	void handleRotaryDrag (const MouseEvent& e)
	{
		auto dx = e.position.x - (float) sliderRect.getCentreX();
		auto dy = e.position.y - (float) sliderRect.getCentreY();

		if (dx * dx + dy * dy > 25.0f)
		{
			auto angle = std::atan2 ((double) dx, (double) -dy);

			while (angle < 0.0)
				angle += MathConstants<double>::twoPi;

			if (rotaryParams.stopAtEnd && e.mouseWasDraggedSinceMouseDown())
			{
				if (std::abs (angle - lastAngle) > MathConstants<double>::pi)
				{
					if (angle >= lastAngle)
						angle -= MathConstants<double>::twoPi;
					else
						angle += MathConstants<double>::twoPi;
				}

				if (angle >= lastAngle)
					angle = jmin (angle, (double) jmax (rotaryParams.startAngleRadians, rotaryParams.endAngleRadians));
				else
					angle = jmax (angle, (double) jmin (rotaryParams.startAngleRadians, rotaryParams.endAngleRadians));
			}
			else
			{
				while (angle < rotaryParams.startAngleRadians)
					angle += MathConstants<double>::twoPi;

				if (angle > rotaryParams.endAngleRadians)
				{
					if (smallestAngleBetween (angle, rotaryParams.startAngleRadians)
						 <= smallestAngleBetween (angle, rotaryParams.endAngleRadians))
						angle = rotaryParams.startAngleRadians;
					else
						angle = rotaryParams.endAngleRadians;
				}
			}

			auto proportion = (angle - rotaryParams.startAngleRadians) / (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians);
			valueWhenLastDragged = owner.proportionOfLengthToValue (jlimit (0.0, 1.0, proportion));
			lastAngle = angle;
		}
	}*/

	void handleAbsoluteDrag (const MouseEvent& e)
	{
		auto mousePos = isHorizontal() ? e.position.x : e.position.y;
		double newPos = 0;

		newPos = (mousePos - (float) sliderRegionStart) / (double) sliderRegionSize;

		if (isVertical())
			newPos = 1.0 - newPos;
		

		newPos = (isRotary() && ! rotaryParams.stopAtEnd) ? newPos - std::floor (newPos)
														  : jlimit (0.0, 1.0, newPos);
		valueWhenLastDragged = owner.proportionOfLengthToValue (newPos);
	}


	void mouseDown (const MouseEvent& e)
	{
		incDecDragged = false;
		useDragEvents = false;
		mouseDragStartPos = mousePosWhenLastDragged = e.position;
		currentDrag.reset();
		popupDisplay.reset();

		if (owner.isEnabled() && thumbsVisible())
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

				if (minValueBox != nullptr)
					minValueBox->hideEditor (true);
				if (maxValueBox != nullptr)
					maxValueBox->hideEditor (true);

				sliderBeingDragged = getThumbIndexAt (e);

				minMaxDiff = static_cast<double> (valueMax.getValue()) - static_cast<double> (valueMin.getValue());

				valueWhenLastDragged = (sliderBeingDragged == 2 ? valueMax
									 :  valueMin ).getValue();
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
		if (useDragEvents && normRange.end > normRange.start
				)//&& e.mouseWasClicked() )
			//&& valueBox != nullptr && valueBox->isEditable())
		{
			DragMode dragMode = notDragging;

			if (style == Rotary)
			{
//				handleRotaryDrag (e);
			}
			else
			{

				//if ((normRange.end - normRange.start) / sliderRegionSize < normRange.interval)
				{
					dragMode = absoluteDrag;
					handleAbsoluteDrag (e);
				}
			}

			valueWhenLastDragged = jlimit (normRange.start, normRange.end, valueWhenLastDragged);

			if (sliderBeingDragged == 1)
			{
				setMinValue (owner.snapValue (valueWhenLastDragged, dragMode),
							 sendChangeOnlyOnRelease ? dontSendNotification : sendNotificationAsync, true);

				if (e.mods.isShiftDown())
					setMaxValue (getMinValue() + minMaxDiff, dontSendNotification, true);
				else
					minMaxDiff = static_cast<double> (valueMax.getValue()) - static_cast<double> (valueMin.getValue());
			}
			else if (sliderBeingDragged == 2)
			{
				setMaxValue (owner.snapValue (valueWhenLastDragged, dragMode),
							 sendChangeOnlyOnRelease ? dontSendNotification : sendNotificationAsync, true);

				if (e.mods.isShiftDown())
					setMinValue (getMaxValue() - minMaxDiff, dontSendNotification, true);
				else
					minMaxDiff = static_cast<double> (valueMax.getValue()) - static_cast<double> (valueMin.getValue());
			}

			mousePosWhenLastDragged = e.position;
		}
	}

	void mouseUp()
	{
		if (owner.isEnabled() && thumbsVisible()
			 && useDragEvents
			 && (normRange.end > normRange.start) )
		{
			restoreMouseIfHidden();

			if (sendChangeOnlyOnRelease)
				triggerChangeMessage (sendNotificationAsync);

			currentDrag.reset();
			popupDisplay.reset();

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
//		auto shouldShowPopup = showPopupOnHover
//								&& (Time::getMillisecondCounterHiRes() - lastPopupDismissal) > 250;

//		if (shouldShowPopup
//			 && ! isTwoValue())
//		{
//			if (owner.isMouseOver (true))
//			{
//				if (popupDisplay == nullptr)
//					showPopupDisplay();
//
//				if (popupDisplay != nullptr && popupHoverTimeout != -1)
//					popupDisplay->startTimer (popupHoverTimeout);
//			}
//		}
	}

	void mouseExit()
	{
		popupDisplay.reset();
	}

	void showPopupDisplay()
	{
		if (popupDisplay == nullptr)
		{
			popupDisplay.reset (new PopupDisplayComponent (owner, parentForPopupDisplay == nullptr));

			if (parentForPopupDisplay != nullptr)
				parentForPopupDisplay->addChildComponent (popupDisplay.get());
			else
				popupDisplay->addToDesktop (ComponentPeer::windowIsTemporary
											| ComponentPeer::windowIgnoresKeyPresses
											| ComponentPeer::windowIgnoresMouseClicks);

			
			updatePopupDisplay (sliderBeingDragged == 2 ? getMaxValue()
															: getMinValue());

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
		return doubleClickToValue;
	}

	void mouseDoubleClick()
	{
		if (canDoubleClickToValue())
		{
			DragInProgress drag (*this);
			setMinValue (minDoubleClickReturnValue, sendNotificationSync, false);
			setMaxValue (maxDoubleClickReturnValue, sendNotificationSync, false);
		}
	}

	double getMouseWheelDelta (double value, double wheelAmount)
	{
		auto proportionDelta = wheelAmount * 0.15;
		auto currentPos = owner.valueToProportionOfLength (value);
		auto newPos = currentPos + proportionDelta;
		newPos = (isRotary() && ! rotaryParams.stopAtEnd) ? newPos - std::floor (newPos)
														  : jlimit (0.0, 1.0, newPos);
		return owner.proportionOfLengthToValue (newPos) - value;
	}

	bool mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
	{
//		if (scrollWheelEnabled
//			 && style != TwoValueHorizontal
//			 && style != TwoValueVertical)
//		{
//			// sometimes duplicate wheel events seem to be sent, so since we're going to
//			// bump the value by a minimum of the interval, avoid doing this twice..
//			if (e.eventTime != lastMouseWheelTime)
//			{
//				lastMouseWheelTime = e.eventTime;
//
//				if (normRange.end > normRange.start && ! e.mods.isAnyMouseButtonDown())
//				{
//					if (valueBox != nullptr)
//						valueBox->hideEditor (false);
//
//					auto value = static_cast<double> (currentValue.getValue());
//					auto delta = getMouseWheelDelta (value, (std::abs (wheel.deltaX) > std::abs (wheel.deltaY)
//																  ? -wheel.deltaX : wheel.deltaY)
//															   * (wheel.isReversed ? -1.0f : 1.0f));
//					if (delta != 0.0)
//					{
//						auto newValue = value + jmax (normRange.interval, std::abs (delta)) * (delta < 0 ? -1.0 : 1.0);
//
//						DragInProgress drag (*this);
//						setValue (owner.snapValue (newValue, notDragging), sendNotificationSync);
//					}
//				}
//			}
//
//			return true;
//		}
		return false;
	}

	void modifierKeysChanged (const ModifierKeys& modifiers)
	{
		if (style != Rotary)
			restoreMouseIfHidden();
	}

	void restoreMouseIfHidden()
	{
		for (auto& ms : Desktop::getInstance().getMouseSources())
		{
			if (ms.isUnboundedMouseMovementEnabled())
			{
				ms.enableUnboundedMouseMovement (false);

				auto pos = sliderBeingDragged == 2 ? getMaxValue() :  getMinValue() ;
				juce::Point<float> mousePos;

				if (isRotary())
				{
//					mousePos = ms.getLastMouseDownPosition();
//
//					auto delta = (float) (pixelsForFullDragExtent * (owner.valueToProportionOfLength (valueOnMouseDown)
//																	   - owner.valueToProportionOfLength (pos)));
//
//					if (style == RotaryHorizontalDrag)      mousePos += Point<float> (-delta, 0.0f);
//					else if (style == RotaryVerticalDrag)   mousePos += Point<float> (0.0f, delta);
//					else                                    mousePos += Point<float> (delta / -2.0f, delta / 2.0f);
//
//					mousePos = owner.getScreenBounds().reduced (4).toFloat().getConstrainedPoint (mousePos);
//					mouseDragStartPos = mousePosWhenLastDragged = owner.getLocalPoint (nullptr, mousePos);
//					valueOnMouseDown = valueWhenLastDragged;
				}
				else
				{
					auto pixelPos = (float) getLinearSliderPos (pos);

					mousePos = owner.localPointToGlobal (juce::Point<float> (isHorizontal() ? pixelPos : ((float) owner.getWidth()  / 2.0f),
																	   isVertical()   ? pixelPos : ((float) owner.getHeight() / 2.0f)));
				}

				const_cast <MouseInputSource&> (ms).setScreenPosition (mousePos);
			}
		}
	}

	//==============================================================================
	void paint (Graphics& g, LookAndFeel* lf)
	{
		
		if (isRotary())
		{
//			auto sliderPos = (float) owner.valueToProportionOfLength (lastCurrentValue);
//			jassert (sliderPos >= 0 && sliderPos <= 1.0f);
//
//			lf.drawRotarySlider (g,
//								 sliderRect.getX(), sliderRect.getY(),
//								 sliderRect.getWidth(), sliderRect.getHeight(),
//								 sliderPos, rotaryParams.startAngleRadians,
//								 rotaryParams.endAngleRadians, owner);
		}
		else
		{
			lf->drawTwoValueSlider (g,
								 sliderRect.getX(), sliderRect.getY(),
								 sliderRect.getWidth(), sliderRect.getHeight(),
								 getLinearSliderPos (lastValueMin),
								 getLinearSliderPos (lastValueMax),
								 minVisible, maxVisible,
								 style, owner);
		}
		
	}

	void setMinSliderVisible(bool set)
	{
		if (set != minVisible)
		{
			minVisible = set;
			if (minValueBox != nullptr)
			{
				minValueBox->setEnabled(minVisible);
			}
			owner.repaint();
		}
	}
	void setMaxSliderVisible(bool set)
	{
		if (set != maxVisible)
		{
			maxVisible = set;
			if (maxValueBox != nullptr)
			{
				maxValueBox->setEnabled(maxVisible);
			}
			owner.repaint();
		}
	}
	
	//==============================================================================
	void resized (SliderLookAndFeelMethods* lf)
	{
		auto layout = lf->getTVSliderLayout (owner);
		sliderRect = layout.sliderBounds;

		
		if (textBoxPos != TextBoxDetached)
		{
			if (minValueBox != nullptr)
				minValueBox->setBounds (layout.minTextBoxBounds);
			if (maxValueBox != nullptr)
				maxValueBox->setBounds (layout.maxTextBoxBounds);
		}
		
		if (isHorizontal())
		{
			sliderRegionStart = layout.sliderBounds.getX();
			sliderRegionSize = layout.sliderBounds.getWidth();
		}
		else if (isVertical())
		{
			sliderRegionStart = layout.sliderBounds.getY();
			sliderRegionSize = layout.sliderBounds.getHeight();
		}
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
	TwoValueSlider& owner;
	SliderStyle style;

	ListenerList<TwoValueSlider::Listener> listeners;
	Value valueMin, valueMax;
	double lastValueMin = 0, lastValueMax = 0;
	NormalisableRange<double> normRange { 0.0, 10.0 };
	double minDoubleClickReturnValue = 0, maxDoubleClickReturnValue = 0;
	double valueWhenLastDragged = 0, valueOnMouseDown = 0;
	double minMaxDiff=0;
//	double lastAngle = 0;
	RotaryParameters rotaryParams;
	juce::Point<float> mouseDragStartPos, mousePosWhenLastDragged;
	int sliderRegionStart = 0, sliderRegionSize = 1;
	int sliderBeingDragged = -1;
	int pixelsForFullDragExtent = 250;
	Time lastMouseWheelTime;
	juce::Rectangle<int> sliderRect;
	std::unique_ptr<DragInProgress> currentDrag;

	TextEntryBoxPosition textBoxPos;
	String textSuffix;
	int numDecimalPlaces = 7;
	int textBoxWidth = 80, textBoxHeight = 20;
	IncDecButtonMode incDecButtonMode = incDecButtonsNotDraggable;
	ModifierKeys::Flags modifierToSwapModes = ModifierKeys::ctrlAltCommandModifiers;

	bool minVisible=true, maxVisible=true;
	
	bool editableText = true;
	bool doubleClickToValue = false;
	bool incDecButtonsSideBySide = false;
	bool sendChangeOnlyOnRelease = false;
	bool showPopupOnDrag = false;
	bool showPopupOnHover = false;
	bool menuEnabled = false;
	bool useDragEvents = false;
	bool incDecDragged = false;
	bool scrollWheelEnabled = true;
	bool snapsToMousePos = true;
	bool autoResizeTextBoxes = true;

	int popupHoverTimeout = 2000;
	double lastPopupDismissal = 0.0;

	ModifierKeys singleClickModifiers;

	std::shared_ptr<Label> minValueBox;
	std::shared_ptr<Label> maxValueBox;
	
	std::unique_ptr<Button> incButton, decButton;

	//==============================================================================
	struct PopupDisplayComponent  : public BubbleComponent,
									public Timer
	{
		PopupDisplayComponent (TwoValueSlider& s, bool isOnDesktop)
			: owner (s),
			  font (owner.getLookAndFeel().getTVSliderPopupFont (s))
		{
			if (isOnDesktop)
				setTransform (AffineTransform::scale (Component::getApproximateScaleFactorForComponent (&s)));

			setAlwaysOnTop (true);
			setAllowedPlacement (owner.getLookAndFeel().getTVSliderPopupPlacement (s));
//			setLookAndFeel (&s.getLookAndFeel());
		}

		~PopupDisplayComponent() override
		{
			if (owner.pimpl != nullptr)
				owner.pimpl->lastPopupDismissal = Time::getMillisecondCounterHiRes();
		}
		//TODO: three line
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
			if (twoLine())
			{
				w = std::max(font.getStringWidth(text),
							 font.getStringWidth(owner.getName()));
				w += 18;
				h = (int) (font.getHeight() *  2.6f );
			}
			else
			{
				w = font.getStringWidth (text) + 18;
				h = (int) (font.getHeight() * 1.6f );
			}
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
		TwoValueSlider& owner;
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
TwoValueSlider::TwoValueSlider()
{
	init (TwoValueHorizontal, TextBoxLeft);
}

TwoValueSlider::TwoValueSlider (const String& name)  : Component (name)
{
	init (TwoValueHorizontal, TextBoxLeft);
}

TwoValueSlider::TwoValueSlider (SliderStyle style, TextEntryBoxPosition textBoxPos)
{
	init (style, textBoxPos);
}

void TwoValueSlider::init (SliderStyle style, TextEntryBoxPosition textBoxPos)
{
	setWantsKeyboardFocus (false);
	setRepaintsOnMouseActivity (true);

	pimpl.reset (new Pimpl (*this, style, textBoxPos));

	TwoValueSlider::lookAndFeelChanged();
	updateText();

	pimpl->registerListeners();
}

TwoValueSlider::~TwoValueSlider() {}

//==============================================================================
void TwoValueSlider::addListener (Listener* l)       { pimpl->listeners.add (l); }
void TwoValueSlider::removeListener (Listener* l)    { pimpl->listeners.remove (l); }

//==============================================================================
TwoValue::SliderStyle TwoValueSlider::getSliderStyle() const noexcept     { return pimpl->style; }
void TwoValueSlider::setSliderStyle (SliderStyle newStyle)              { pimpl->setSliderStyle (newStyle); }

void TwoValueSlider::setRotaryParameters (RotaryParameters p) noexcept
{
	// make sure the values are sensible..
	jassert (p.startAngleRadians >= 0 && p.endAngleRadians >= 0);
	jassert (p.startAngleRadians < MathConstants<float>::pi * 4.0f
			  && p.endAngleRadians < MathConstants<float>::pi * 4.0f);

	pimpl->rotaryParams = p;
}

void TwoValueSlider::setRotaryParameters (float startAngleRadians, float endAngleRadians, bool stopAtEnd) noexcept
{
	setRotaryParameters ({ startAngleRadians, endAngleRadians, stopAtEnd });
}

TwoValueSlider::RotaryParameters TwoValueSlider::getRotaryParameters() const noexcept
{
	return pimpl->rotaryParams;
}


double TwoValueSlider::getSkewFactor() const noexcept               { return pimpl->normRange.skew; }
bool TwoValueSlider::isSymmetricSkew() const noexcept               { return pimpl->normRange.symmetricSkew; }

void TwoValueSlider::setSkewFactor (double factor, bool symmetricSkew)
{
	pimpl->normRange.skew = factor;
	pimpl->normRange.symmetricSkew = symmetricSkew;
}

void TwoValueSlider::setSkewFactorFromMidPoint (double sliderValueToShowAtMidPoint)
{
	pimpl->normRange.setSkewForCentre (sliderValueToShowAtMidPoint);
}

int TwoValueSlider::getMouseDragSensitivity() const noexcept        { return pimpl->pixelsForFullDragExtent; }

void TwoValueSlider::setMouseDragSensitivity (int distanceForFullScaleDrag)
{
	jassert (distanceForFullScaleDrag > 0);

	pimpl->pixelsForFullDragExtent = distanceForFullScaleDrag;
}

void TwoValueSlider::setIncDecButtonsMode (IncDecButtonMode mode)                   { pimpl->setIncDecButtonsMode (mode); }

TwoValueSlider::TextEntryBoxPosition TwoValueSlider::getTextBoxPosition() const noexcept    { return pimpl->textBoxPos; }
int TwoValueSlider::getTextBoxWidth() const noexcept                                { return pimpl->textBoxWidth; }
int TwoValueSlider::getTextBoxHeight() const noexcept                               { return pimpl->textBoxHeight; }

void TwoValueSlider::setTextBoxStyle (TextEntryBoxPosition newPosition, bool isReadOnly, int textEntryBoxWidth, int textEntryBoxHeight)
{
	pimpl->setTextBoxStyle (newPosition, isReadOnly, textEntryBoxWidth, textEntryBoxHeight);
}

bool TwoValueSlider::isTextBoxEditable() const noexcept                     { return pimpl->editableText; }
void TwoValueSlider::setTextBoxIsEditable (const bool shouldBeEditable)     { pimpl->setTextBoxIsEditable (shouldBeEditable); }
void TwoValueSlider::showTextBox()                                          { pimpl->showTextBox(); }
void TwoValueSlider::hideTextBox (bool discardCurrentEditorContents)        { pimpl->hideTextBox (discardCurrentEditorContents); }

void TwoValueSlider::setChangeNotificationOnlyOnRelease (bool onlyNotifyOnRelease)
{
	pimpl->sendChangeOnlyOnRelease = onlyNotifyOnRelease;
}

bool TwoValueSlider::getSliderSnapsToMousePosition() const noexcept           { return pimpl->snapsToMousePos; }
void TwoValueSlider::setSliderSnapsToMousePosition (bool shouldSnapToMouse)   { pimpl->snapsToMousePos = shouldSnapToMouse; }

void TwoValueSlider::setPopupDisplayEnabled (bool showOnDrag, bool showOnHover, Component* parent, int hoverTimeout)
{
	pimpl->showPopupOnDrag = showOnDrag;
	pimpl->showPopupOnHover = showOnHover;
	pimpl->parentForPopupDisplay = parent;
	pimpl->popupHoverTimeout = hoverTimeout;
}

juce::Component* TwoValueSlider::getCurrentPopupDisplay() const noexcept      { return pimpl->popupDisplay.get(); }

//==============================================================================
void TwoValueSlider::colourChanged()        { lookAndFeelChanged(); }
	void TwoValueSlider::lookAndFeelChanged()   {  pimpl->lookAndFeelChanged(&getLookAndFeel()); }
void TwoValueSlider::enablementChanged()    { repaint(); pimpl->updateTextBoxEnablement(); }

//void TwoValueSlider::setLookAndFeelMethods(LookAndFeelMethods* lm) {
//	lafm= lm;
//	pimpl->lookAndFeelChanged (lafm);
//}
//==============================================================================
Range<double> TwoValueSlider::getRange() const noexcept  { return { pimpl->normRange.start, pimpl->normRange.end }; }
double TwoValueSlider::getMaximum() const noexcept       { return pimpl->normRange.end; }
double TwoValueSlider::getMinimum() const noexcept       { return pimpl->normRange.start; }
double TwoValueSlider::getInterval() const noexcept      { return pimpl->normRange.interval; }

void TwoValueSlider::setRange (double newMin, double newMax, double newInt)      { pimpl->setRange (newMin, newMax, newInt); }
void TwoValueSlider::setRange (Range<double> newRange, double newInt)            { pimpl->setRange (newRange.getStart(), newRange.getEnd(), newInt); }
void TwoValueSlider::setNormalisableRange (NormalisableRange<double> newRange)   { pimpl->setNormalisableRange (newRange); }

Value& TwoValueSlider::getMinValueObject() noexcept      { return pimpl->valueMin; }
Value& TwoValueSlider::getMaxValueObject() noexcept      { return pimpl->valueMax; }

double TwoValueSlider::getMinValue() const      { return pimpl->getMinValue(); }
double TwoValueSlider::getMaxValue() const      { return pimpl->getMaxValue(); }

void TwoValueSlider::setMinValue (double newValue, NotificationType notification, bool allowNudgingOfOtherValues)
{
	pimpl->setMinValue (newValue, notification, allowNudgingOfOtherValues);
}

void TwoValueSlider::setMaxValue (double newValue, NotificationType notification, bool allowNudgingOfOtherValues)
{
	pimpl->setMaxValue (newValue, notification, allowNudgingOfOtherValues);
}

void TwoValueSlider::setMinAndMaxValues (double newMinValue, double newMaxValue, NotificationType notification)
{
	pimpl->setMinAndMaxValues (newMinValue, newMaxValue, notification);
}

void TwoValueSlider::setDoubleClickReturnValue (bool isDoubleClickEnabled,  double minDefault, double maxDefault, ModifierKeys mods)
{
	pimpl->doubleClickToValue = isDoubleClickEnabled;
	pimpl->minDoubleClickReturnValue = minDefault;
	pimpl->maxDoubleClickReturnValue = maxDefault;
	pimpl->singleClickModifiers = mods;
}

std::shared_ptr<Label> TwoValueSlider::getTextBox(bool max) { return pimpl->getTextBox(max);}

double TwoValueSlider::getLowDoubleClickReturnValue() const noexcept 	{ return pimpl->minDoubleClickReturnValue; }
double TwoValueSlider::getHighDoubleClickReturnValue() const noexcept 	{ return pimpl->maxDoubleClickReturnValue; }
bool TwoValueSlider::isDoubleClickReturnEnabled() const noexcept        { return pimpl->doubleClickToValue; }

void TwoValueSlider::updateText()
{
	pimpl->updateText();
}

void TwoValueSlider::setTextValueSuffix (const String& suffix)
{
	pimpl->setTextValueSuffix (suffix);
}

String TwoValueSlider::getTextValueSuffix() const
{
	return pimpl->textSuffix;
}

String TwoValueSlider::getTextFromValue (double v)
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

double TwoValueSlider::getValueFromText (const String& text)
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

double TwoValueSlider::proportionOfLengthToValue (double proportion)
{
	return pimpl->normRange.convertFrom0to1 (proportion);
}

double TwoValueSlider::valueToProportionOfLength (double value)
{
	return pimpl->normRange.convertTo0to1 (value);
}

double TwoValueSlider::snapValue (double attemptedValue, DragMode)
{
	if (snapFunction != nullptr)
		return snapFunction(attemptedValue);
	else
		return attemptedValue;
}

int TwoValueSlider::getNumDecimalPlacesToDisplay() const noexcept   { return pimpl->numDecimalPlaces; }

void TwoValueSlider::setNumDecimalPlacesToDisplay (int decimalPlacesToDisplay)
{
	pimpl->numDecimalPlaces = decimalPlacesToDisplay;
	updateText();
}


void TwoValueSlider::setMinSliderVisible(bool set) { pimpl->setMinSliderVisible(set); }
void TwoValueSlider::setMaxSliderVisible(bool set) { pimpl->setMaxSliderVisible(set); }
   

//==============================================================================
int TwoValueSlider::getThumbBeingDragged() const noexcept           { return pimpl->sliderBeingDragged; }
void TwoValueSlider::startedDragging() {}
void TwoValueSlider::stoppedDragging() {}
void TwoValueSlider::valueChanged() {}

//==============================================================================
void TwoValueSlider::setPopupMenuEnabled (bool menuEnabled)         { pimpl->menuEnabled = menuEnabled; }
void TwoValueSlider::setScrollWheelEnabled (bool enabled)           { pimpl->scrollWheelEnabled = enabled; }

bool TwoValueSlider::isHorizontal() const noexcept                  { return pimpl->isHorizontal(); }
bool TwoValueSlider::isVertical() const noexcept                    { return pimpl->isVertical(); }
bool TwoValueSlider::isRotary() const noexcept                      { return pimpl->isRotary(); }
bool TwoValueSlider::isBar() const noexcept                         { return pimpl->isBar(); }
bool TwoValueSlider::isTwoValue() const noexcept                    { return pimpl->isTwoValue(); }
bool TwoValueSlider::isThreeValue() const noexcept                  { return pimpl->isThreeValue(); }

float TwoValueSlider::getPositionOfValue (double value) const       { return pimpl->getPositionOfValue (value); }

//==============================================================================
void TwoValueSlider::paint (Graphics& g)        { pimpl->paint (g, &getLookAndFeel()); }
void TwoValueSlider::resized()                  { pimpl->resized (&getLookAndFeel()); }

void TwoValueSlider::focusOfChildComponentChanged (FocusChangeType)     { repaint(); }

void TwoValueSlider::mouseDown (const MouseEvent& e)    { pimpl->mouseDown (e); }
void TwoValueSlider::mouseUp   (const MouseEvent&)      { pimpl->mouseUp(); }
void TwoValueSlider::mouseMove (const MouseEvent&)      { pimpl->mouseMove(); }
void TwoValueSlider::mouseExit (const MouseEvent&)      { pimpl->mouseExit(); }

// If popup display is enabled and set to show on mouse hover, this makes sure
// it is shown when dragging the mouse over a slider and releasing
void TwoValueSlider::mouseEnter (const MouseEvent&)     { pimpl->mouseMove(); }

void TwoValueSlider::modifierKeysChanged (const ModifierKeys& modifiers)
{
	if (isEnabled())
		pimpl->modifierKeysChanged (modifiers);
}

void TwoValueSlider::mouseDrag (const MouseEvent& e)
{
	if (isEnabled())
		pimpl->mouseDrag (e);
}

void TwoValueSlider::mouseDoubleClick (const MouseEvent&)
{
	if (isEnabled())
		pimpl->mouseDoubleClick();
}

void TwoValueSlider::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
	if (! (isEnabled() && pimpl->mouseWheelMove (e, wheel)))
		Component::mouseWheelMove (e, wheel);
}


TwoValue::SliderLayout SliderLookAndFeelMethods::getTVSliderLayout (TwoValueSlider& slider)
{
	int minXSpace = 0;
	int minYSpace = 0;
	
	TwoValueSlider::TextEntryBoxPosition textBoxPos = slider.getTextBoxPosition();
	
	if (textBoxPos == TwoValueSlider::TextBoxLeft || textBoxPos == TwoValueSlider::TextBoxRight)
		minXSpace = 30;
	else
		minYSpace = 15;
	
	auto localBounds = slider.getLocalBounds();
	
	int pad = getTwoValueSliderThumbRadius(slider);
	
	auto textBoxWidth  = jmax (0, jmin (slider.getTextBoxWidth(),  localBounds.getWidth() - minXSpace));
	auto textBoxHeight = jmax (0, jmin (slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));
	
	int textGap = slider.isHorizontal() ? 3 : 2;
	
	TwoValue::SliderLayout layout;
	
	// 2. set the textBox bounds
	
	if (textBoxPos != TwoValueSlider::NoTextBox && textBoxPos != TwoValueSlider::TextBoxDetached)
	{
		
		if (slider.isBar())
		{
			layout.minTextBoxBounds = localBounds;
			layout.maxTextBoxBounds = localBounds;
		}
		else
		{
			layout.minTextBoxBounds.setWidth (textBoxWidth);
			layout.maxTextBoxBounds.setWidth (textBoxWidth);
			
			layout.minTextBoxBounds.setHeight (textBoxHeight);
			layout.maxTextBoxBounds.setHeight (textBoxHeight);
			
			if (textBoxPos == TwoValueSlider::TextBoxLeft)
			{
				layout.minTextBoxBounds.setX (0);
				layout.maxTextBoxBounds.setX (textBoxWidth + textGap);
			}
			else if (textBoxPos == TwoValueSlider::TextBoxRight)
			{
				layout.minTextBoxBounds.setX (localBounds.getWidth() - textBoxWidth);
				layout.maxTextBoxBounds.setX (localBounds.getWidth() - textBoxWidth*2 - textGap);
			}
			else /* above or below -> centre horizontally */
			{
				layout.minTextBoxBounds.setX ((localBounds.getWidth() - textBoxWidth) / 2);
				layout.maxTextBoxBounds.setX ((localBounds.getWidth() - textBoxWidth) / 2);
			}
			
			if (textBoxPos == TwoValueSlider::TextBoxAbove)
			{
				layout.minTextBoxBounds.setY (0);
				layout.maxTextBoxBounds.setY (textBoxWidth + textGap);
			}
			else if (textBoxPos == TwoValueSlider::TextBoxBelow)
			{
				layout.minTextBoxBounds.setY (localBounds.getHeight() - textBoxHeight);
				layout.maxTextBoxBounds.setY (localBounds.getHeight() - textBoxHeight*2 - textGap);
			}
			else /* left or right -> centre vertically */
			{
				layout.minTextBoxBounds.setY ((localBounds.getHeight() - textBoxHeight) / 2);
				layout.maxTextBoxBounds.setY ((localBounds.getHeight() - textBoxHeight) / 2);
			}
		}
	}
	
	// 3. set the slider bounds
	
	layout.sliderBounds = localBounds;
	
	if (slider.isBar())
	{
		layout.sliderBounds.reduce (1, 1);   // bar border
	}
	else
	{
		if (textBoxPos == TwoValueSlider::TextBoxLeft)       layout.sliderBounds.removeFromLeft (textBoxWidth*2+textGap - pad);
		else if (textBoxPos == TwoValueSlider::TextBoxRight) layout.sliderBounds.removeFromRight (textBoxWidth*2+textGap - pad);
		else if (textBoxPos == TwoValueSlider::TextBoxAbove) layout.sliderBounds.removeFromTop (textBoxHeight*2+textGap - pad);
		else if (textBoxPos == TwoValueSlider::TextBoxBelow) layout.sliderBounds.removeFromBottom (textBoxHeight*2+textGap - pad);
		
		const int thumbIndent = getTwoValueSliderThumbRadius (slider);
		
		if (slider.isHorizontal())    layout.sliderBounds.reduce (thumbIndent*2, 0);
		else if (slider.isVertical()) layout.sliderBounds.reduce (0, thumbIndent*2);
	}
	
	return layout;
}
struct SliderLabelComp  : public Label
{
	SliderLabelComp() : Label ({}, {}) {}
	void mouseWheelMove (const MouseEvent&, const MouseWheelDetails&) override {}
};
Label* SliderLookAndFeelMethods::createTVSliderTextBox (TwoValueSlider& slider)
{
   auto l = new SliderLabelComp();
   
   l->setJustificationType (Justification::centred);
   l->setKeyboardType (TextInputTarget::decimalKeyboard);
   
   l->setColour (Label::textColourId, slider.findColour (Slider::textBoxTextColourId));
   l->setColour (Label::backgroundColourId, slider.findColour (Slider::textBoxBackgroundColourId));
   l->setColour (Label::outlineColourId, slider.findColour (Slider::textBoxOutlineColourId));
   l->setColour (TextEditor::textColourId, slider.findColour (Slider::textBoxTextColourId));
   l->setColour (TextEditor::backgroundColourId,
				 slider.findColour (Slider::textBoxBackgroundColourId)
				 .withAlpha (1.0f));
   l->setColour (TextEditor::outlineColourId, slider.findColour (Slider::textBoxOutlineColourId));
   l->setColour (TextEditor::highlightColourId, slider.findColour (Slider::textBoxHighlightColourId));
   
   return l;
}

Button* SliderLookAndFeelMethods::createTVSliderButton (TwoValueSlider&, bool isIncrement)
{
	return new TextButton (isIncrement ? "+" : "-", String());
}
Font SliderLookAndFeelMethods::getTVSliderPopupFont (TwoValueSlider& slider) {
	return Font (15.0f, Font::bold);
}
int SliderLookAndFeelMethods::getTVSliderPopupPlacement (TwoValueSlider& slider)  {
	return BubbleComponent::above
	| BubbleComponent::below
	| BubbleComponent::left
	| BubbleComponent::right;
}



}
