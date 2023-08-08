//
//  DebugWindow.cpp
//  
//
//  Created by Isaac Roberts on 8/21/20.
//

// Created by Isaac Roberts


#include "DebugWindow.h"


#if DEBUG_WINDOW

DebugWindow::DebugWindow()
:
currentLine(""),
editLock()
{
	instances=0;
}
DebugWindow::DebugInstance::DebugInstance()
: TextEditor(), Timer()
{
	DebugWindow::addInstance();
	
	setMultiLine(true, true);
	setReadOnly(true);
	setCaretVisible(false);
	setScrollbarsShown(true);
	setFont(Font("Courier", 12, 0));
	setScrollBarThickness(5);
	setTextToShowWhenEmpty("Debug Window", Colour(128,128,128));
	setScrollToShowCursor(true);
	setJustification(Justification::left);
	setLineSpacing(1.0);
	setTabKeyUsedAsCharacter (true);
	
	setColour(TextEditor::backgroundColourId, Colour(255,252,240));
	setColour(TextEditor::textColourId, Colour(0,0,0));
	setColour(TextEditor::highlightColourId, Colour(255, 255, 0));
	setColour(TextEditor::highlightedTextColourId, Colour(0,0,0));
	
	timerCallback();
}
DebugWindow::DebugInstance::~DebugInstance()
{
	DebugWindow::removeInstance();
}
void DebugWindow::DebugInstance::start()
{
	startTimerHz(2);
}
void DebugWindow::DebugInstance::timerCallback()
{
	//flush first
	
	if (getTotalNumChars() > 10000)
	{
		//flush long text
		String text = getText();
		//substring to 6000 to prevent flushing every tick
		text = text.substring(6000);
		setText(text, false);
	}
	
	String addedText = DebugWindow::getInst()->addedText();

	if (addedText.length() > 0)
	{
		moveCaretToEnd();
		insertTextAtCaret(addedText);
	}
}
String DebugWindow::addedText()
{
	editLock.lock();
	int size = lines.size();
	editLock.unlock();
	
	if (size > 0)
	{
		editLock.lock();
		String all_lines = lines.joinIntoString("");
		lines.clear();
		editLock.unlock();
		
		all_lines += "\n";
		
		return all_lines;
	}
	else
		return "";
}
void DebugWindow::flush()
{
	editLock.lock();
//	std::cout<<"debug::flush"<<std::endl;
	lines.add(currentLine);
//	std::cout<<"successfully flushed"<<std::endl;
	editLock.unlock();
	
	currentLine = "";
}

void DebugWindow::_println()
{
	currentLine += "\n";
	flush();
}

DebugWindow::~DebugWindow()
{
}

#endif
