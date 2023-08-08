//
//  RadioButtonSet.h
//  
//
//  Created by Isaac Roberts on 1/24/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class RadioButtonSet : public juce::Component
{
	enum Orientation {
		Vertical, Horizontal
	};
public:
	RadioButtonSet(int amt);
	RadioButtonSet();
	void setAmt(int set);
	
	void acceptButtons(int groupId=2983);
	
	virtual void paint(Graphics&) override;
	virtual void resized() override;
	
	virtual ~RadioButtonSet();
	
	void setOrientation(Orientation set) {orientation = set;}
	
	Button* getButton(int n) {return buttons[n];}
	Label* getLabel(int n) { return labels[n];}
	
	void setButton(int n, Button* set) {buttons[n] = set;}
	void setLabel(int n, Label* set) {labels[n] = set;}
	
	void setButtonSizeAndLabelWidth(int button_size, int label_width) {buttonSize = button_size; labelWidth = label_width;}
	
private:
	Button** buttons;
	Label** labels;
	int amt;
	Orientation orientation;
	int buttonSize=25, labelWidth=30;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RadioButtonSet);

};
