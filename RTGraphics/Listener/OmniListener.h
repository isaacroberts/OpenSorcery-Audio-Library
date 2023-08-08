//
//  OmniListener.h
//  
//
//  Created by Isaac Roberts on 2/18/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

//#include "../Sliders/TwoValueSlider.h"

using namespace juce;

class OmniListener : public Slider::Listener, public Button::Listener, public TextEditor::Listener, public TwoValue::TwoValueSlider::Listener
{
public:
	OmniListener()
	: callback([&]() {})
	{}
	
	void sliderValueChanged (Slider *s) override {callback();}
	void sliderDragStarted (Slider *s) override {callback();}
	void sliderDragEnded (Slider *s) override {callback();}
	
	void sliderLowValueChanged (TwoValue::TwoValueSlider* slider) override {callback(); }
	void sliderHighValueChanged (TwoValue::TwoValueSlider* slider) override {callback(); }
	
	void buttonClicked (Button *b) override {callback();}
	void buttonStateChanged (Button *b) override {callback();}
	
	void textEditorTextChanged (TextEditor &e) override {callback();}
	void textEditorReturnKeyPressed (TextEditor &e) override {callback();}
	void textEditorEscapeKeyPressed (TextEditor &e) override {callback();}
	void textEditorFocusLost (TextEditor &e) override {callback();}
	
	virtual ~OmniListener() {}
	
	std::function<void()> callback;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OmniListener);

};
