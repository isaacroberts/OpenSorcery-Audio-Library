//
//  UniqueHashCoder.h
//  
//
//  Created by Isaac Roberts on 8/8/20.
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

class UniqueHashCoder {
public:
	
	static int getUniqueHashCode() {
		return nextHashCode++;
	}
	
	static int nextHashCode;
};

int UniqueHashCoder::nextHashCode = 0 ;
