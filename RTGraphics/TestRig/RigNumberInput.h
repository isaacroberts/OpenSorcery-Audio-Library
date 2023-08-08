//
//  RigNumberInput.h
//  
//
//  Created by Isaac Roberts on 9/15/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class RigNumberInput : public TextEditor
{
public:
	RigNumberInput();
	
	void setName(String name);
	
	void paint(Graphics& g) override;
	void resized() override;

	
	virtual ~RigNumberInput();
	
protected:
	String name;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RigNumberInput);

};
