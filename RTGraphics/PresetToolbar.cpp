//
//  PresetToolbar.cpp
//  Tremelon
//
//  Created by Isaac Roberts on 5/31/20.
//

#include "PresetToolbar.h"

#include <memory>

//#include "ButtonElements/HyperLinkToolbarButton.h"

PresetToolbar::PresetToolbar(int buttonAmt_)
//	: Toolbar(),
:
	handler(NULL),
	buttonAmt(buttonAmt_)
{
//	setVertical(false);
//	setStyle(Toolbar::ToolbarItemStyle::iconsOnly);
	
	button = new Button*[buttonAmt];
	
	for (int n=0; n < buttonAmt; ++n)
	{
		button[n] = createButton(n);
		addAndMakeVisible(button[n]);
	}
	
//	addAndMakeVisible(presetBox);
	
}
void PresetToolbar::setPresetHandler(PresetHandlerBase* set)
{
	handler = set;
	
	//		Folder=0, Prev, Next, Save, Trash
	
	button[Folder]->onClick	= [&] () { handler->select_folder();};
	button[Prev]->onClick   = [&] () { handler->previous();};
	button[Next]->onClick 	= [&] () { handler->next();};
	button[Save]->onClick 	= [&] () { handler->save_preset();};
	button[Trash]->onClick	= [&] () { handler->trash_preset();};
	
	addAndMakeVisible(handler);
}

Button* PresetToolbar::createButton(int id)
{
	String name[5] = {"...", "<", ">", "Save", "Del"};
	String tooltip[5] = {"Select Preset Folder", "Previous", "Next", "Save", "Delete"};
	
	return new TextButton(name[id], tooltip[id]);
}

void PresetToolbar::resized()
{
	int comboBoxButtonsW = 5;
	
	int buttonW = getWidth() / (buttonAmt + comboBoxButtonsW);
	
	
	int x = 0;
	
	int buttonSplitPt = 2;
	
	for (int n=0;n<buttonSplitPt; ++n)
	{
		button[n]->setBounds( x + n * buttonW , 0, buttonW, getHeight());
	}
	handler->setBounds(buttonSplitPt * buttonW, 0, comboBoxButtonsW * buttonW, getHeight());
	
	x = comboBoxButtonsW * buttonW;
	
	for (int n=buttonSplitPt; n < buttonAmt; ++n)
	{
		button[n]->setBounds( x + n * buttonW , 0, buttonW, getHeight());
	}
}

PresetToolbar::~PresetToolbar()
{
	for (int n=0;n<buttonAmt; ++n)
	{
		button[n]->setLookAndFeel(NULL);
		delete button[n];
	}
		
	delete[] button;
	
	delete handler;
}

/*
void PresetToolbar::swap_presets()
{
	//this swaps the buttons states, which sends a notification thru the button manager
	if (button[a_]->getToggleState())
	{
		button[a_]->setToggleState(false, NotificationType::dontSendNotification);
		button[b_]->setToggleState(true, NotificationType::sendNotification);
	}
	else
	{
		button[a_]->setToggleState(true, NotificationType::sendNotification);
		button[b_]->setToggleState(false, NotificationType::dontSendNotification);
	}
}*/

 /*
 
 
//button[9]->onClick = [&] () { handler->set_a_preset();};
//button[10]->onClick = [&] () { swap_presets();};
//button[11]->onClick = [&] () { handler->set_b_preset();};

 
//	button[a_]->setClickingTogglesState(true);
//	button[b_]->setClickingTogglesState(true);
//
//	button[a_]->setRadioGroupId(10);
//	button[b_]->setRadioGroupId(10);

//	if (handler->is_A_Active())
//	{
//		button[a_]->setToggleState(true, NotificationType::dontSendNotification);
//	}
//	else
//		button[b_]->setToggleState(true, NotificationType::dontSendNotification);

 
*/
