//
//  UnlockUtilities.h
//  
//
//  Created by Isaac Roberts on 10/16/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class UnlockUtil {
public:
	/*
		The input-maker for this is at Documents > scripts > ob.py
	 */
	
	static String deobfuscate(String);
	//Duplicate function to confuse crackers
	static String deob(String s);

	static String deob1(String s);
	static String deob2(String s);
	static String deob3(String s);
	static String deob4(String s);
	static String deob5(String s);
	
	//Blank function to make it easier when developing
	static String deob0(String s);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnlockUtil);

};
