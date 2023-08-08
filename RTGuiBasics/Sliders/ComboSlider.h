//
//  ComboSlider.h
//  
//
//  Created by Isaac Roberts on 5/11/21.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>

//using namespace juce;

class  ComboSlider  : public juce::Component,
					  public juce::SettableTooltipClient
{
public:
	//==============================================================================
	/** The types of slider available.

		@see setSliderStyle, setRotaryParameters
	*/
	enum DragDirection
	{
		LinearBar,                      /**< A horizontal bar slider with the text label drawn on top of it. */
		LinearBarVertical,              /**< A vertical bar slider with the text label drawn on top of it. */
		LinearBarVerticalInverse,
		None
	};

	/** The position of the slider's text-entry box.
		@see setTextBoxStyle
	*/
	const int NoTextBox= 0;
	const int absoluteDrag = 0;
	//==============================================================================
	/** Creates a slider.
		When created, you can set up the slider's style and range with setSliderStyle(), setRange(), etc.
	*/
	ComboSlider();

	/** Creates a slider.
		When created, you can set up the slider's style and range with setSliderStyle(), setRange(), etc.
	*/
	explicit ComboSlider (const juce::String& componentName);

	/** Creates a slider with some explicit options. */
	ComboSlider (DragDirection style);

	/** Destructor. */
	~ComboSlider() override;

	//==============================================================================
	/** Changes the type of slider interface being used.

		@param newStyle         the type of interface
		@see setRotaryParameters, setVelocityBasedMode
	*/
	void setSliderStyle (DragDirection newStyle);

	/** Returns the slider's current style.
		@see setSliderStyle
	*/
	DragDirection getSliderStyle() const noexcept;

	/** Sets the distance the mouse has to move to drag the slider across
		the full extent of its range.

		This only applies when in modes like RotaryHorizontalDrag, where it's using
		relative mouse movements to adjust the slider.
	*/
	void setMouseDragSensitivity (int distanceForFullScaleDrag);

	/** Returns the current sensitivity value set by setMouseDragSensitivity(). */
	int getMouseDragSensitivity() const noexcept;

	//==============================================================================
	/** Changes the slider's current value.

		This will trigger a callback to ComboSlider::Listener::sliderValueChanged() for any listeners
		that are registered, and will synchronously call the valueChanged() method in case subclasses
		want to handle it.

		@param newValue         the new value to set - this will be restricted by the
								minimum and maximum range, and will be snapped to the
								nearest interval if one has been set
		@param notification     can be one of the NotificationType values, to request
								a synchronous or asynchronous call to the valueChanged() method
								of any ComboSlider::Listeners that are registered. A notification will
								only be sent if the ComboSlider's value has changed.
	*/
	void setValue (double newValue, juce::NotificationType notification = juce::sendNotificationAsync);

	/** Returns the slider's current value. */
	double getValue() const;

	/** Returns the Value object that represents the slider's current position.
		You can use this Value object to connect the slider's position to external values or setters,
		either by taking a copy of the Value, or by using Value::referTo() to make it point to
		your own Value object.
		@see Value, getMaxValue, getMinValueObject
	*/
	juce::Value& getValueObject() noexcept;

	//==============================================================================
	/** Sets the limits that the slider's value can take.

		@param newMinimum   the lowest value allowed
		@param newMaximum   the highest value allowed
		@param newInterval  the steps in which the value is allowed to increase - if this
							is not zero, the value will always be (newMinimum + (newInterval * an integer)).
	*/
	void setRange (double newMinimum,
				   double newMaximum,
				   double newInterval = 0);

	/** Sets the limits that the slider's value can take.

		@param newRange     the range to allow
		@param newInterval  the steps in which the value is allowed to increase - if this
							is not zero, the value will always be (newMinimum + (newInterval * an integer)).
	*/
	void setRange (juce::Range<double> newRange, double newInterval);

	/** Sets a NormalisableRange to use for the ComboSlider values.

		@param newNormalisableRange     the NormalisableRange to use
	*/
	void setNormalisableRange (juce::NormalisableRange<double> newNormalisableRange);

	/** Returns the slider's range. */
	juce::Range<double> getRange() const noexcept;

	/** Returns the current maximum value.
		@see setRange, getRange
	*/
	double getMaximum() const noexcept;

	/** Returns the current minimum value.
		@see setRange, getRange
	*/
	double getMinimum() const noexcept;

	/** Returns the current step-size for values.
		@see setRange, getRange
	*/
	double getInterval() const noexcept;

	
	//==============================================================================
	/** Used by setIncDecButtonsMode().
	 */
	enum IncDecButtonMode
	{
		incDecButtonsNotDraggable,
		incDecButtonsDraggable_AutoDirection,
		incDecButtonsDraggable_Horizontal,
		incDecButtonsDraggable_Vertical
	};
	
	/** When the style is IncDecButtons, this lets you turn on a mode where the mouse
	 can be dragged on the buttons to drag the values.
	 
	 By default this is turned off. When enabled, clicking on the buttons still works
	 them as normal, but by holding down the mouse on a button and dragging it a little
	 distance, it flips into a mode where the value can be dragged. The drag direction can
	 either be set explicitly to be vertical or horizontal, or can be set to
	 incDecButtonsDraggable_AutoDirection so that it depends on whether the buttons
	 are side-by-side or above each other.
	 */
	void setIncDecButtonsMode (IncDecButtonMode mode);
	
	//==============================================================================
	/** Changes the location and properties of the text-entry box.
	 
	 @param newPosition          where it should go (or NoTextBox to not have one at all)
	 @param isReadOnly           if true, it's a read-only display
	 @param textEntryBoxWidth    the width of the text-box in pixels. Make sure this leaves enough
	 room for the slider as well!
	 @param textEntryBoxHeight   the height of the text-box in pixels. Make sure this leaves enough
	 room for the slider as well!
	 
	 @see setTextBoxIsEditable, getValueFromText, getTextFromValue
	 */
	
	//==============================================================================
	/** A class for receiving callbacks from a ComboSlider.

		To be told when a slider's value changes, you can register a ComboSlider::Listener
		object using ComboSlider::addListener().

		@see ComboSlider::addListener, ComboSlider::removeListener
	*/
	class JUCE_API  Listener
	{
	public:
		//==============================================================================
		/** Destructor. */
		virtual ~Listener() = default;

		//==============================================================================
		/** Called when the slider's value is changed.

			This may be caused by dragging it, or by typing in its text entry box,
			or by a call to ComboSlider::setValue().

			You can find out the new value using ComboSlider::getValue().

			@see ComboSlider::valueChanged
		*/
		virtual void sliderValueChanged (ComboSlider* slider) = 0;

		//==============================================================================
		/** Called when the slider is about to be dragged.

			This is called when a drag begins, then it's followed by multiple calls
			to sliderValueChanged(), and then sliderDragEnded() is called after the
			user lets go.

			@see sliderDragEnded, ComboSlider::startedDragging
		*/
		virtual void sliderDragStarted (ComboSlider*) {}

		/** Called after a drag operation has finished.
			@see sliderDragStarted, ComboSlider::stoppedDragging
		*/
		virtual void sliderDragEnded (ComboSlider*) {}
	};

	/** Adds a listener to be called when this slider's value changes. */
	void addListener (Listener* listener);

	/** Removes a previously-registered listener. */
	void removeListener (Listener* listener);

	//==============================================================================
	/** You can assign a lambda to this callback object to have it called when the slider value is changed. */
	std::function<void()> onValueChange;

	/** You can assign a lambda to this callback object to have it called when the slider's drag begins. */
	std::function<void()> onDragStart;

	/** You can assign a lambda to this callback object to have it called when the slider's drag ends. */
	std::function<void()> onDragEnd;

	/** You can assign a lambda that will be used to convert textual values to the slider's normalised position. */
	std::function<double (const juce::String&)> valueFromTextFunction;

	/** You can assign a lambda that will be used to convert the slider's normalised position to a textual value. */
	std::function<juce::String (double)> textFromValueFunction;

	//==============================================================================
	/** This lets you choose whether double-clicking or single-clicking with a specified
		key modifier moves the slider to a given position.

		By default this is turned off, but it's handy if you want either of these actions
		to act as a quick way of resetting a slider. Just pass in the value you want it to
		go to when double-clicked. By default the key modifier is the alt key but you can
		pass in another key modifier, or none to disable this behaviour.

		@see getDoubleClickReturnValue
	*/
	void setDoubleClickReturnValue (bool shouldDoubleClickBeEnabled,
									double valueToSetOnDoubleClick,
									juce::ModifierKeys singleClickModifiers = juce::ModifierKeys::altModifier);

	/** Returns the values last set by setDoubleClickReturnValue() method.
		@see setDoubleClickReturnValue
	*/
	double getDoubleClickReturnValue() const noexcept;

	/** Returns true if double-clicking to reset to a default value is enabled.
		@see setDoubleClickReturnValue
	*/
	bool isDoubleClickReturnEnabled() const noexcept;

	//==============================================================================
	/** Tells the slider whether to keep sending change messages while the user
		is dragging the slider.

		If set to true, a change message will only be sent when the user has
		dragged the slider and let go. If set to false (the default), then messages
		will be continuously sent as they drag it while the mouse button is still
		held down.
	*/
	void setChangeNotificationOnlyOnRelease (bool onlyNotifyOnRelease);

	/** This lets you change whether the slider thumb jumps to the mouse position
		when you click.

		By default, this is true. If it's false, then the slider moves with relative
		motion when you drag it.

		This only applies to linear bars, and won't affect two- or three- value
		sliders.
	*/
	void setSliderSnapsToMousePosition (bool shouldSnapToMouse);

	/** Returns true if setSliderSnapsToMousePosition() has been enabled. */
	bool getSliderSnapsToMousePosition() const noexcept;

	/** If enabled, this gives the slider a pop-up bubble which appears while the
		slider is being dragged or hovered-over.

		This can be handy if your slider doesn't have a text-box, so that users can
		see the value just when they're changing it.

		If you pass a component as the parentComponentToUse parameter, the pop-up
		bubble will be added as a child of that component when it's needed. If you
		pass nullptr, the pop-up will be placed on the desktop instead (note that it's a
		transparent window, so if you're using an OS that can't do transparent windows
		you'll have to add it to a parent component instead).

		By default the popup display shown when hovering will remain visible for 2 seconds,
		but it is possible to change this by passing a different hoverTimeout value. A
		value of -1 will cause the popup to remain until a mouseExit() occurs on the slider.
	*/
	void setPopupDisplayEnabled (bool shouldShowOnMouseDrag,
								 bool shouldShowOnMouseHover,
								 Component* parentComponentToUse,
								 int hoverTimeout = 2000);

	/** If a popup display is enabled and is currently visible, this returns the component
		that is being shown, or nullptr if none is currently in use.
		@see setPopupDisplayEnabled
	*/
	Component* getCurrentPopupDisplay() const noexcept;

	/** If this is set to true, then right-clicking on the slider will pop-up
		a menu to let the user change the way it works.

		By default this is turned off, but when turned on, the menu will include
		things like velocity sensitivity, and for rotary sliders, whether they
		use a linear or rotary mouse-drag to move them.
	*/
	void setPopupMenuEnabled (bool menuEnabled);

	/** This can be used to stop the mouse scroll-wheel from moving the slider.
		By default it's enabled.
	*/
	void setScrollWheelEnabled (bool enabled);

	/** Returns a number to indicate which thumb is currently being dragged by the mouse.

		This will return 0 for the main thumb, 1 for the minimum-value thumb, 2 for
		the maximum-value thumb, or -1 if none is currently down.
	*/
	int getThumbBeingDragged() const noexcept;

	//==============================================================================
	/** Callback to indicate that the user is about to start dragging the slider.
		@see ComboSlider::Listener::sliderDragStarted
	*/
	virtual void startedDragging();

	/** Callback to indicate that the user has just stopped dragging the slider.
		@see ComboSlider::Listener::sliderDragEnded
	*/
	virtual void stoppedDragging();

	/** Callback to indicate that the user has just moved the slider.
		@see ComboSlider::Listener::sliderValueChanged
	*/
	virtual void valueChanged();

	//==============================================================================
	/** Subclasses can override this to convert a text string to a value.

		When the user enters something into the text-entry box, this method is
		called to convert it to a value.
		The default implementation just tries to convert it to a double.

		@see getTextFromValue
	*/
	virtual double getValueFromText (const juce::String& text);

	/** Turns the slider's current value into a text string.

		Subclasses can override this to customise the formatting of the text-entry box.

		The default implementation just turns the value into a string, using
		a number of decimal places based on the range interval. If a suffix string
		has been set using setTextValueSuffix(), this will be appended to the text.

		@see getValueFromText
	*/
	virtual juce::String getTextFromValue (double value);

	/** Sets a suffix to append to the end of the numeric value when it's displayed as
		a string.

		This is used by the default implementation of getTextFromValue(), and is just
		appended to the numeric value. For more advanced formatting, you can override
		getTextFromValue() and do something else.
	*/
	void setTextValueSuffix (const juce::String& suffix);

	/** Returns the suffix that was set by setTextValueSuffix(). */
	juce::String getTextValueSuffix() const;

	//==============================================================================
	/** Allows a user-defined mapping of distance along the slider to its value.

		The default implementation for this performs the skewing operation that
		can be set up in the setSkewFactor() method. Override it if you need
		some kind of custom mapping instead, but make sure you also implement the
		inverse function in valueToProportionOfLength().

		@param proportion       a value 0 to 1.0, indicating a distance along the slider
		@returns                the slider value that is represented by this position
		@see valueToProportionOfLength
	*/
	virtual double proportionOfLengthToValue (double proportion);

	/** Allows a user-defined mapping of value to the position of the slider along its length.

		The default implementation for this performs the skewing operation that
		can be set up in the setSkewFactor() method. Override it if you need
		some kind of custom mapping instead, but make sure you also implement the
		inverse function in proportionOfLengthToValue().

		@param value            a valid slider value, between the range of values specified in
								setRange()
		@returns                a value 0 to 1.0 indicating the distance along the slider that
								represents this value
		@see proportionOfLengthToValue
	*/
	virtual double valueToProportionOfLength (double value);

	/** Returns the X or Y coordinate of a value along the slider's length.

		If the slider is horizontal, this will be the X coordinate of the given
		value, relative to the left of the slider. If it's vertical, then this will
		be the Y coordinate, relative to the top of the slider.

		If the slider is rotary, this will throw an assertion and return 0. If the
		value is out-of-range, it will be constrained to the length of the slider.
	*/
	float getPositionOfValue (double value) const;

	//==============================================================================
	/** This can be overridden to allow the slider to snap to user-definable values.

		If overridden, it will be called when the user tries to move the slider to
		a given position, and allows a subclass to sanity-check this value, possibly
		returning a different value to use instead.

		@param attemptedValue   the value the user is trying to enter
		@param dragMode         indicates whether the user is dragging with
								the mouse; notDragging if they are entering the value
								using the text box or other non-dragging interaction
		@returns                the value to use instead
	*/
	virtual double snapValue (double attemptedValue);


	//==============================================================================
	/** This can be called to force the text box to update its contents.
		(Not normally needed, as this is done automatically).
	*/
	void updateText();

	/** True if the slider moves horizontally. */
	bool isHorizontal() const noexcept;
	/** True if the slider moves vertically. */
	bool isVertical() const noexcept;
	/** True if the slider is in a rotary mode. */
	bool isRotary() const noexcept;
	/** True if the slider is in a linear bar mode. */
	bool isBar() const noexcept;
	/** True if the slider has two thumbs. */
	bool isTwoValue() const noexcept;
	/** True if the slider has three thumbs. */
	bool isThreeValue() const noexcept;

	//==============================================================================
	/** A set of colour IDs to use to change the colour of various aspects of the slider.

		These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
		methods.

		@see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
	*/
	enum ColourIds
	{
		backgroundColourId          = 0x1001200,  /**< A colour to use to fill the slider's background. */

		textColourId   = 0x1001500,  /**< The background colour for the text-editor box. */
//		textColourId    = 0x1001600,  /**< The text highlight colour for the text-editor box. */
		outlineColourId  = 0x1001700   /**< The colour to use for a border around the text-editor box. */
	};

	//==============================================================================
	/** A struct defining the placement of the slider area and the text box area
		relative to the bounds of the whole ComboSlider component.
	 */
	//==============================================================================
	/** This abstract base class is implemented by LookAndFeel classes to provide
		slider drawing functionality.
	*/
	struct JUCE_API  LookAndFeelMethods
	{
		virtual ~LookAndFeelMethods() = default;

		//==============================================================================
		virtual void drawComboSlider (juce::Graphics&,
									   int x, int y, int width, int height,
									   const juce::String& text,
									   float pos, int itemId,
									   const ComboSlider::DragDirection,
									   ComboSlider&);

		virtual void drawComboSliderBackground (juce::Graphics&,
												 int x, int y, int width, int height,
												 const juce::String& text,
												 float pos, int itemId,
												 const ComboSlider::DragDirection style,
												 ComboSlider&);

//		virtual Button* createSliderButton (ComboSlider&, bool isIncrement) = 0;

		virtual juce::ImageEffectFilter* getComboSliderEffect (ComboSlider&) { return NULL; }

		virtual juce::Font getFontForComboSlider (ComboSlider& c) { return juce::Font(18); }
		virtual juce::Font getComboSliderPopupFont (ComboSlider& c) { return juce::Font(18); }
		virtual int getComboSliderPopupPlacement (ComboSlider& c);

	};

	//==============================================================================
	/** @internal */
	void paint (juce::Graphics&) override;
	/** @internal */
	void resized() override;
	/** @internal */
	void mouseDown (const juce::MouseEvent&) override;
	/** @internal */
	void mouseUp (const juce::MouseEvent&) override;
	/** @internal */
	void mouseDrag (const juce::MouseEvent&) override;
	/** @internal */
	void mouseDoubleClick (const juce::MouseEvent&) override;
	/** @internal */
	void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;
	/** @internal */
	void modifierKeysChanged (const juce::ModifierKeys&) override;
	/** @internal */
	void lookAndFeelChanged() override;
	/** @internal */
	void focusOfChildComponentChanged (FocusChangeType) override;
	/** @internal */
	void colourChanged() override;
	/** @internal */
	void mouseMove (const juce::MouseEvent&) override;
	/** @internal */
	void mouseExit (const juce::MouseEvent&) override;
	/** @internal */
	void mouseEnter (const juce::MouseEvent&) override;

private:
	//==============================================================================
	JUCE_PUBLIC_IN_DLL_BUILD (class Pimpl)
	std::unique_ptr<Pimpl> pimpl;

	void init (DragDirection);

   #if JUCE_CATCH_DEPRECATED_CODE_MISUSE
	// These methods' bool parameters have changed: see the new method signature.
	JUCE_DEPRECATED (void setValue (double, bool));
	JUCE_DEPRECATED (void setValue (double, bool, bool));
	JUCE_DEPRECATED (void setMinValue (double, bool, bool, bool));
	JUCE_DEPRECATED (void setMinValue (double, bool, bool));
	JUCE_DEPRECATED (void setMinValue (double, bool));
	JUCE_DEPRECATED (void setMaxValue (double, bool, bool, bool));
	JUCE_DEPRECATED (void setMaxValue (double, bool, bool));
	JUCE_DEPRECATED (void setMaxValue (double, bool));
	JUCE_DEPRECATED (void setMinAndMaxValues (double, double, bool, bool));
	JUCE_DEPRECATED (void setMinAndMaxValues (double, double, bool));
   #endif

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComboSlider)
};

