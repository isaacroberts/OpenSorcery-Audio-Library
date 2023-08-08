//
//  RadioButtonSet.cpp
//  
//
//  Created by Isaac Roberts on 1/24/21.
//

// Created by Isaac Roberts


#include "RadioButtonSet.h"


RadioButtonSet::RadioButtonSet()
: Component(),
buttons(NULL), labels(NULL), amt(-1),
orientation(Vertical)
{
}
RadioButtonSet::RadioButtonSet(int amt_)
: Component(),
amt(amt_),
orientation(Vertical)
{
	buttons = new Button*[amt];
	labels = new Label*[amt];
}
void RadioButtonSet::setAmt(int set)
{
	if (amt == -1)
	{
		amt = set;
		buttons = new Button*[amt];
		labels = new Label*[amt];
	}
	else if (amt != set)
	{
		jassert (false);
	}
}

void RadioButtonSet::acceptButtons(int groupId)
{
	for (int n=0; n < amt; ++n)
	{
		buttons[n]->setClickingTogglesState(true);
		buttons[n]->setRadioGroupId(groupId, dontSendNotification);
		
		addAndMakeVisible(buttons[n]);
		addAndMakeVisible(labels[n]);
	}
}
void RadioButtonSet::paint(Graphics& g)
{
	g.fillAll(Colour(0.f, .0f, .4f));
}
void RadioButtonSet::resized()
{
	for (int n=0; n < amt; ++n)
	{
		buttons[n]->setBounds(0, n * buttonSize, buttonSize+5, buttonSize);
		labels[n]->setBounds(buttonSize+5, n*buttonSize, labelWidth, buttonSize);
	}
}

RadioButtonSet::~RadioButtonSet()
{
	for (int n=0; n < amt; ++n)
		delete buttons[n];
	delete[] buttons;
	
	for (int n=0; n < amt; ++n)
		delete labels[n];
	delete[] labels;
}

