//
//  RigNumberInput.cpp
//  
//
//  Created by Isaac Roberts on 9/15/21.
//

// Created by Isaac Roberts


#include "RigNumberInput.h"


RigNumberInput::RigNumberInput()
: TextEditor(), name("NumberInput")
{
	setInputRestrictions(7, "0123456789");
	setJustification(Justification::centred);
}
void RigNumberInput::setName(String set)
{
	name = set;
	setTextToShowWhenEmpty(name, Colours::grey);
	setTooltip(name);
}
void RigNumberInput::paint(Graphics& g)
{
	TextEditor::paint(g);
	g.setColour(Colours::white);
	g.drawFittedText(name, 0, 0, getWidth(), 15, Justification::centredTop, 1);
}
void RigNumberInput::resized()
{
	TextEditor::resized();
}


RigNumberInput::~RigNumberInput()
{
	
}

