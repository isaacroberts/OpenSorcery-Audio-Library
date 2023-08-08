//
//  ListenerParam.h
//  
//
//  Created by Isaac Roberts on 10/14/21.
//

//#pragma once

//#include <JuceHeader.h>
//
//#include "TypeHandling.h"
//#include "PresetDefinitions.h"

using namespace juce;


template<class Type>
class LoneParam : public RangedAudioParameter
{
public:
	// From Parameter
	LoneParam(String id_, String name_)
	: RangedAudioParameter(RTPreset::paramId(id_), sanName(name_)),
	id(id_), name(sanName(name_)),
	low(0), high(1), def(1),
	range(0, 1),
	saveToPreset(true)
	{
		value = def;
		flatValue = normalize(value);
		callback = [&](Type s) {};
		uiCallback = [&](Type s, void*) {};
		range.interval = getDefaultInterval();
//		notify();
	}

	LoneParam(String id_, String name_, Type low_, Type high_, Type default_)
	: RangedAudioParameter(RTPreset::paramId(id_), sanName(name_)),
	id(id_), name(sanName(name_)),
	low(low_), high(high_), def(default_),
	range(low, high),
	saveToPreset(true)
	{
		value = def;
		flatValue = normalize(value);
		callback = [&](Type s) {};
		uiCallback = [&](Type s, void*) {};
		range.interval = getDefaultInterval();
		notify();
	}
	
	
	LoneParam(ParameterID id_, String name_, Type low_, Type high_, Type default_)
	: RangedAudioParameter(id_, sanName(name_)),
	id(id_.getParamID()), name(sanName(name_)),
	low(low_), high(high_), def(default_),
	range(low, high),
	saveToPreset(true)
	{
		value = def;
		flatValue = normalize(value);
		callback = [&](Type s) {};
		uiCallback = [&](Type s, void*) {};
		range.interval = getDefaultInterval();
		notify();
	}
	
	void setInfo(Type low_, Type high_, Type default_) {
		low = low_;
		high = high_;
		def = default_;
		value = def;
		flatValue = normalize(value);
		range.start=low;
		range.end=high;
		range.interval = getDefaultInterval();
	}
	void setInfo(bool default_) {
		low = 0;
		high = 1;
		def = default_;
		value = def;
		flatValue = normalize(value);
		range.start=0;
		range.end=1;
		range.interval = 1;
	}
	
	void clearCallback() {
		callback = [&](Type s) {};
	}
	void clearUiCallback() {
		uiCallback = [&](Type s, void*) {} ;
	}
	Type bound(Type value) const
	{
		if (value < low)
			return low;
		if (value > high)
			return high;
		return value;
	}
	Type* bound(Type* value) const
	{
		if (*value < low)
			*value = low;
		else if (*value > high)
			*value = high;

		return value;
	}
	
	float normalize(Type value) const { return TypeHandling::normalize<Type>(value, low, high);}
	Type denormalize(float flat_value) const { return TypeHandling::denormalize<Type>(flat_value, low, high);}
	
	inline Type getLow() const {return low;}
	inline Type getHigh() const {return high;}
	inline Type getDefault() const {return def;}

	inline void setRange(Type from, Type to) {
		low = from;
		high = to;
	}
	inline void setDefault(Type set_) { def = set_; }

public:

	
	void setCallback(std::function<void(Type)> set)
	{
		callback = set;
		jassert (callback);
//		callCallback();
	}
	void setUiCallback(std::function<void(Type, void*)> set)
	{
		uiCallback = set;
		jassert(uiCallback);
	}
	
//	void bindCallback(Type& bind)
//	{
//		callback = [&](Type set) { bind = set; };
////		callCallback();
//	}
	void bindCallback(Type* bind)
	{
		callback = [=](Type set){ *bind = set; };
//		callCallback();
	}
		
	String getName() const {return name;}
	
private:
	static String sanName(String name) {
		return name.replace(" ", "_");
	}
public:

	const NormalisableRange<float>& getRange() const { return range; }
	
	virtual void setRate(float set) {}
	
	bool savesToPreset() { return saveToPreset;}
	void setSavesToPreset(bool set) {saveToPreset = set;}
	
	double getDefaultInterval() {return TypeHandling::getDefaultInterval<Type>(low, high);}
	
	
	/* ---- AudioProcessorParameter overrides ---- */
	
	float getValue() const override {
		return flatValue;
	}
	void setValue(float nrm) override {
		if (flatValue != nrm)
		{
			flatValue = nrm;
			value = denormalize(flatValue);
			callCallback();
			callUiCallback(NULL);
	//		notify();
		}
	}
	float getDefaultValue() const override {
		return normalize(getDefault());
	}
	
	/* ---- Internal ---- */
	Type get() const {return value;}
	Type getVal() const {return value;}
	
	void set(Type set, void* source=NULL) {
		if (value != set)
		{
			value = bound(set);
			flatValue = normalize(value);
			callCallback();
			callUiCallback(source);
			notify();
		}
	}
	void setFromGraphics(Type set, void* source=NULL) {
		if (value != set)
		{
			value = bound(set);
			flatValue = normalize(value);
			callCallback();
			notify();
		}
	}
	void setDefault() {
		if (value != getDefault())
		{
			value = getDefault();
			flatValue = normalize(value);
			callCallback();
			callUiCallback(NULL);
			notify();
		}
	}
	void setDefaultFromGraphics() {
		if (value != getDefault())
		{
			value = getDefault();
			flatValue = normalize(value);
			callCallback(NULL);
			notify();
		}
	}
	
	float getFlatValue() { return flatValue; }
	float getFlatValue(float v) {
		return normalize(v);
	}
	
	/* ---- More AudioProcessorParam utilities ---- */
	
	
	String getName(int maxLength) const override {
		return getName().substring(0, maxLength);
	}
	
	virtual String getLabel() const override { return ""; }
	
	const NormalisableRange<float>& getNormalisableRange() const override {
		return getRange();
	}
	
	//	int getNumSteps() const override { return 500; }
//	inline float convertTo0to1(float v) { return normalize(v);}
//	inline float convertFrom0to1(float v) {return denormalize(v);}
	
	bool isDiscrete() const override { return TypeHandling::isDiscrete<Type>();}
	bool isBoolean() const override { return TypeHandling::isBoolean<Type>(); };
	bool isAutomatable() { return !TypeHandling::isBoolean<Type>(); }

	float getValueForText(const String& text) const override {
		return TypeHandling::getValueForText<Type>(text);
	}
//	ListenerParam()
//	: RangedAudioParameter("parmm", ""),
//	label("")
//	{}
//	void setType(Parameter<Type>* type_) {
//		type = type_;
//		this->paramID = String(getID());
//		this->name = String(getName());
//		value = getDefault();
//		flatValue = normalize(value);
//		callCallback(value);
//		notify();
//	}
	
	virtual ~LoneParam() {}
	
	/* ---- Internal Utility ---- */
	
	void update() {
		callCallback();
	}
	virtual void callCallback() {
		callback(value);
	}
	virtual void callUiCallback(void* source) {
		auto* messageManager = juce::MessageManager::getInstanceWithoutCreating();
		if (messageManager)
		{
			if (messageManager->isThisTheMessageThread())
			{
				uiCallback(value, source);
			}
			else
			{
//				safeCaller->reset(value);
//				auto dupeCallback = uiCallback;
				messageManager->callAsync([=](){uiCallback(value, source);});
			}
		}
	}
	void save(XmlElement* element, bool isPreset) const
	{
		if (isPreset && !saveToPreset)
			return;
		element->setAttribute(getName(), value);
	}
	void load(XmlElement* element, bool isPreset)
	{
		if (isPreset && !saveToPreset)
			return;
		loadVal(element, getName(), getDefault());
//		std::cout<<"val loaded "<<value<<std::endl;
		bound(&value);
		flatValue = normalize(value);
		callCallback();
		callUiCallback(NULL);
		notify();
	}
public:
	void recognizeSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod, double interval, void* associatedSource=NULL)
	{
		/*
		 This applies default behavior for range, interval, onValueChange, and uiCallback
		 */
		int numDec = slider->getNumDecimalPlacesToDisplay();
		slider->setRange(getLow(), getHigh(), interval);
		RTPreset::setSliderResetMethod(slider, resetMethod, getDefault());
		slider->setNumDecimalPlacesToDisplay(numDec);
		if (associatedSource==NULL)
			slider->onValueChange = [=]() {this->setFromGraphics(slider->getValue(), associatedSource);};
		else
			slider->onValueChange = [=]() {this->set(slider->getValue(), associatedSource);};

		if (associatedSource==NULL)
		{
			//Otherwise its a listener
			setUiCallback( [=](Type v, void*) {slider->setValue(v, dontSendNotification);} );
			callUiCallback(NULL);
		}
		else
			slider->setValue(value, dontSendNotification);
	}
	void recognizeSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod)
	{
		/*
		 This does the same as above but uses the parameter's default interval based on data type and range
		 */
		recognizeSlider(slider, resetMethod, getDefaultInterval());
	}
	
	void recognizeSlider(Slider* slider, double interval)
	{
		/*
		 This does the same as above but uses the parameter's default interval based on data type and range
		 */
		recognizeSlider(slider, RTPreset::SliderResetMethod::None, interval);
	}
	void recognizeSlider(Slider* slider, void* associatedSource) {
		recognizeSlider(slider, RTPreset::SliderResetMethod::None, getDefaultInterval(), associatedSource);
	}

	void recognizeSlider(Slider* slider)
	{
		/*
		 This does the same as above but uses the parameter's default interval based on data type and range
		 */
		recognizeSlider(slider, RTPreset::SliderResetMethod::None, getDefaultInterval());
	}
	void recognizeSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod, int interval) {
		recognizeSlider(slider, resetMethod, (double)interval);
		slider->setNumDecimalPlacesToDisplay(0);
	}
	void recognizeSlider(Slider& slider, RTPreset::SliderResetMethod resetMethod, int interval) {
		recognizeSlider(&slider, resetMethod, (double)interval);
		slider.setNumDecimalPlacesToDisplay(0);
	}
	void recognizeSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod, int interval, void* source) {
		recognizeSlider(slider, resetMethod, (double)interval, source);
		slider->setNumDecimalPlacesToDisplay(0);
	}

	void recognizeSlider(Slider& slider, RTPreset::SliderResetMethod resetMethod, double interval) {
		recognizeSlider(&slider, resetMethod, interval); }
	void recognizeSlider(Slider& slider, RTPreset::SliderResetMethod resetMethod) { recognizeSlider(&slider, resetMethod); }
	void recognizeSlider(Slider& slider, double interval) { recognizeSlider(&slider, interval); }
	
	void recognizeSlider(Slider& slider) { recognizeSlider(&slider); }
	
	void recognizeSliderMin(Slider* slider, double interval=-1)
	{
		/*
		 For twoValue slider min
		 This applies default behavior for range, interval, onValueChange, and uiCallback
		 
		 This sets the slider range - max does not
		 */
		
		if (interval < 0)
		{
			interval = getDefaultInterval();
		}
		int numDec = slider->getNumDecimalPlacesToDisplay();
		slider->setRange(getLow(), getHigh(), interval);
		slider->setNumDecimalPlacesToDisplay(numDec);
		
		slider->onValueChange = [=]() {this->setFromGraphics(slider->getMinValue());};
		setUiCallback( [=](Type v, void*) {slider->setMinValue(v, dontSendNotification, true);} );
		callUiCallback(NULL);
	}
	void recognizeSliderMax(Slider* slider)
	{
		/*
		 For twoValue slider max
		 This applies default behavior for range, interval, onValueChange, and uiCallback
		 */
		
		slider->onValueChange = [=]() {this->setFromGraphics(slider->getMaxValue());};
		setUiCallback( [=](Type v, void*) {slider->setMaxValue(v, dontSendNotification, true);} );
		callUiCallback(NULL);
	}
	void recognizeSliderMin(Slider& slider, double interval=-1) { recognizeSliderMin(&slider,interval); }
	void recognizeSliderMax(Slider& slider) { recognizeSliderMax(&slider); }
	
	void recognizeButton(Button* button)
	{
		button->setClickingTogglesState(true);
		button->onClick = [=]() {this->setFromGraphics(button->getToggleState());};
		setUiCallback( [=](Type v, void*) {
			button->setToggleState(v, dontSendNotification);
		} );
		callUiCallback(NULL);
	}
	void recognizeButton(Button& button) { recognizeButton(&button); }
	 
	void recognizeTextEditor(TextEditor* editor)
	{
		setUiCallback([=](int amt) {
			editor->setText(TypeHandling::valueToString<Type>(value), false);
		});
		editor->onReturnKey = [=]() {
			if (editor->isEmpty()) {
				setDefaultFromGraphics();
				editor->setText(TypeHandling::valueToString<Type>(value), false);
			}
			else
				set(TypeHandling::getValueForText<Type>(editor->getText()));
			editor->moveKeyboardFocusToSibling(true);
		};
		editor->onFocusLost = [=]() {
			editor->setText(TypeHandling::valueToString<Type>(value), false);
		};
		editor->onEscapeKey = [=]() {
			//triggers onFocusLost command
			editor->moveKeyboardFocusToSibling(true);
		};
		callUiCallback(NULL);
	}
	void recognizeTextEditor(TextEditor& editor) { recognizeTextEditor(&editor); }

	
	
	
protected:
	//Private utility functions
	
	void loadVal(XmlElement* element, String name, Type def) {
		value = TypeHandling::loadVal(element, name, def);
	}
	//uses type specific
	inline void notify() {
		sendValueChangedMessageToListeners(flatValue);
	}
	
	
protected:
	String id;
	String name;
	std::function<void(Type)> callback;
	std::function<void(Type, void*)> uiCallback;//The void* is the listener source
	Type low, high;
	Type def;
//	int interval = 100;
	
	NormalisableRange<float> range;
	bool saveToPreset=true;
	Type value;
	float flatValue;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoneParam);
};
