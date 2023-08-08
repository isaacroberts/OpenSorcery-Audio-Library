//
//  ParameterValue.h
//  
//
//  Created by Isaac Roberts on 10/24/20.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>
//#include "../DebugWindow.h"

//#include "TypeHandling.h"
//#include "PresetDefinitions.h"

using namespace juce;



template<class Type>
class ParameterValue : public RangedAudioParameter
{
public:
	/* ---- AudioProcessorParameter overrides ---- */
	
	float getValue() const override {
		return flatValue;
	}
	void setValue(float nrm) override {
		
		flatValue = nrm;
		value = type->denormalize(flatValue);
		type->callCallback(value);
//		Debug::print("calling ui callback");
		type->callUiCallback(value);
//		notify();
	}
	float getDefaultValue() const override {
		return type->normalize(type->getDefault());
	}
	
	/* ---- Internal ---- */
	Type get() const {return value;}
	Type getVal() const {return value;}
	
	operator Type() const { return value; }
	
	void set(Type set) {
		value = type->bound(set);
		flatValue = type->normalize(value);
		type->callCallback(value);
		type->callUiCallback(value);
		notify();
	}
	void setFromGraphics(Type set) {
		value = type->bound(set);
		flatValue = type->normalize(value);
		type->callCallback(value);
		notify();
	}
	void setDefault() {
		value = type->getDefault();
		flatValue = type->normalize(value);
		type->callCallback(value);
		type->callUiCallback(value);
		notify();
	}
	void setDefaultFromGraphics() {
		value = type->getDefault();
		flatValue = type->normalize(value);
		type->callCallback(value);
		notify();
	}
	
	
	float getFlatValue() { return flatValue; }
	float getFlatValue(float v) {
		return type->normalize(v);
	}
	
	void recognizeSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod, double interval)
	{
		/*
		 This applies default behavior for range, interval, onValueChange, and uiCallback
		 */
		int numDec = slider->getNumDecimalPlacesToDisplay();
		slider->setRange(type->getLow(), type->getHigh(), interval);
		RTPreset::setSliderResetMethod(slider, resetMethod, type->getDefault());
		slider->setNumDecimalPlacesToDisplay(numDec);
		slider->onValueChange = [=]() {this->setFromGraphics(slider->getValue());};
		type->setUiCallback( [=](Type v) {slider->setValue(v, dontSendNotification);} );
		type->callUiCallback(value);
	}
	void recognizeSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod)
	{
		/*
		 This does the same as above but uses the parameter's default interval based on data type and range
		 */
		recognizeSlider(slider, resetMethod, type->getDefaultInterval());
	}
	
	void recognizeSlider(Slider* slider, double interval)
	{
		/*
		 This does the same as above but uses the parameter's default interval based on data type and range
		 */
		recognizeSlider(slider, RTPreset::SliderResetMethod::None, interval);
	}
	void recognizeSlider(Slider* slider)
	{
		/*
		 This does the same as above but uses the parameter's default interval based on data type and range
		 */
		recognizeSlider(slider, RTPreset::SliderResetMethod::None, type->getDefaultInterval());
	}
	void recognizeSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod, int interval) {
		recognizeSlider(slider, resetMethod, (double)interval);
		slider->setNumDecimalPlacesToDisplay(0);
	}
	void recognizeSlider(Slider& slider, RTPreset::SliderResetMethod resetMethod, int interval) {
		recognizeSlider(&slider, resetMethod, (double)interval);
		slider.setNumDecimalPlacesToDisplay(0);
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
			interval = type->getDefaultInterval();
		}
		int numDec = slider->getNumDecimalPlacesToDisplay();
		slider->setRange(type->getLow(), type->getHigh(), interval);
		slider->setNumDecimalPlacesToDisplay(numDec);
		
		slider->onValueChange = [=]() {this->setFromGraphics(slider->getMinValue());};
		type->setUiCallback( [=](Type v) {slider->setMinValue(v, dontSendNotification, true);} );
		type->callUiCallback(value);
	}
	void recognizeSliderMax(Slider* slider)
	{
		/*
		 For twoValue slider max
		 This applies default behavior for range, interval, onValueChange, and uiCallback
		 */
		
		slider->onValueChange = [=]() {this->setFromGraphics(slider->getMaxValue());};
		type->setUiCallback( [=](Type v) {slider->setMaxValue(v, dontSendNotification, true);} );
		type->callUiCallback(value);
	}
	void recognizeSliderMin(Slider& slider, double interval=-1) { recognizeSliderMin(&slider,interval); }
	void recognizeSliderMax(Slider& slider) { recognizeSliderMax(&slider); }
	
	void recognizeButton(Button* button)
	{
		button->setClickingTogglesState(true);
		button->onClick = [=]() {this->setFromGraphics(button->getToggleState());};
		type->setUiCallback( [=](Type v) {
			button->setToggleState(v, dontSendNotification);
		} );
		type->callUiCallback(value);
	}
	void recognizeButton(Button& button) { recognizeButton(&button); }
	 
	void recognizeTextEditor(TextEditor* editor)
	{
		type->setUiCallback([=](int amt) {
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
		type->callUiCallback(value);
	}
	void recognizeTextEditor(TextEditor& editor) { recognizeTextEditor(&editor); }

	void setUiCallback(std::function<void(Type)> set)
	{
		type->setUiCallback( set);
		type->callUiCallback(value);
	}
	/* ---- More AudioProcessorParam utilities ---- */
	
	
	String getName(int maxLength) const override {
		return type->getName().substring(0, maxLength);
	}
	
	//TODO: Consider replacing this with return "" and saving the variable space
	// - I did it in LinkedParameter, I just don't want to change this before shipping 
	virtual String getLabel() const override { return label; }
	
	const NormalisableRange<float>& getNormalisableRange() const override {
		return type->getRange();
	}
	
	//	int getNumSteps() const override { return 500; }
//	inline float convertTo0to1(float v) { return type->normalize(v);}
//	inline float convertFrom0to1(float v) {return type->denormalize(v);}
	
	bool isDiscrete() const override { return TypeHandling::isDiscrete<Type>();}
	bool isBoolean() const override { return TypeHandling::isBoolean<Type>(); };
	bool isAutomatable() { return !TypeHandling::isBoolean<Type>(); }

	float getValueForText(const String& text) const override {
		return TypeHandling::getValueForText<Type>(text);
	}
	
	/* ---- Constructor ---- */
	ParameterValue(Parameter<Type>* type_)
	: RangedAudioParameter(RTPreset::paramId(type_->getID()), type_->getName()),
	type(type_),
	label("")
	{
		jassert (type->getID()!="");
		value = type->getDefault();
		flatValue = type->normalize(value);
		type->callCallback(value);
		notify();
	}
	void copyValue( ParameterValue& o)
	{
		//Assume parameterValue has same type
		value = o.value;
		flatValue = o.flatValue;
		type->callCallback(value);
		type->callUiCallback(value);
		notify();
	}
	void updateParamType(Parameter<Type>* type_)
	{
		type = type_;
		type->callCallback(value);
		type->callUiCallback(value);
		notify();
	}
	void updateParamTypeKeepUi(Parameter<Type>* type_)
	{
		std::function<void(Type)> uiCallback= type_->getUiCallback();
		type = type_;
		type->callCallback(value);
		type->setUiCallback(uiCallback);
		
		type->callUiCallback(value);
		notify();
	}
//	ParameterValue()
//	: RangedAudioParameter("parmm", ""),
//	label("")
//	{}
//	void setType(Parameter<Type>* type_) {
//		type = type_;
//		this->paramID = String(type->getID());
//		this->name = String(type->getName());
//		value = type->getDefault();
//		flatValue = type->normalize(value);
//		type->callCallback(value);
//		notify();
//	}
	
	virtual ~ParameterValue() {}
	
	/* ---- Internal Utility ---- */
	
	void update() {
		type->callCallback(value);
	}
	void callUiCallback() {
		type->callUiCallback(value);
	}
	
	void save(XmlElement* element, bool isPreset) const
	{
		if (isPreset && !savesToPreset())
			return;

		element->setAttribute(type->getName(), value);
	}
	void load(XmlElement* element, bool isPreset)
	{		
		if (isPreset && !savesToPreset())
			return;

		loadVal(element, type->getName(), type->getDefault());
//		std::cout<<"val loaded "<<value<<std::endl;
		type->bound(&value);
		flatValue = type->normalize(value);
		type->callCallback(value);
		type->callUiCallback(value);
		notify();
	}
	bool savesToPreset() const { return type->savesToPreset();}

	auto* getType() { return type; }
	
	void setCallback(std::function<void(Type)> set) {
		type->setCallback(set);
	}
	
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
	Parameter<Type>* type;
	Type value;
	float flatValue;
	String label;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterValue);
};
