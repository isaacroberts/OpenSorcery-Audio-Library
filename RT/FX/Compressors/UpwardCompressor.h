//
//  UpwardCompressor.h
//  
//
//  Created by Isaac Roberts on 2/1/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "RTCompressor.h"

using namespace juce;

class UpwardCompressor : public RTCompressor {
public:
	UpwardCompressor();
	
	UpwardCompressor(float threshold, float ratio, float attack, float release, float kneewidth=.2, float pregain=0, float postgain=0);
	
	
	virtual void commonProcess(InputBlock &i, OutputBlock& b, BlockStorage&, bool isRep);
	
	virtual ~UpwardCompressor();
		
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpwardCompressor);
};
