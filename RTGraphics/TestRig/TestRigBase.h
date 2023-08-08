//
//  TestRigBase.h
//  
//
//  Created by Isaac Roberts on 9/15/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class TestRigBase
{
public:
	/*
	 void connectKnobs();

	 int getRigDim() override 		{ return 50; }
	 bool isSplitHoriz() override 		{ return true;}
	 */
	
	virtual int getRigDim() = 0;
	virtual bool isSplitHoriz() { return true;}
	
	virtual void saveState (XmlElement* xml) {}
	virtual void loadState (XmlElement* xml) {}
	
	TestRigBase() {}
	virtual ~TestRigBase() {}

	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestRigBase);

};
