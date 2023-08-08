//
//  InstrData.h
//  
//
//  Created by Isaac Roberts on 3/29/22.
//

#ifndef InstrData_h
#define InstrData_h

#include <JuceHeader.h>

struct InstrData {
	InstrData() : midiCollector(), keyboardState(), bufferPtr(NULL) {
		keyboardState.addListener(&midiCollector);
	}
	~InstrData() {}
	void prepare(double sampleRate)
	{
		
		midiCollector.reset(sampleRate);
	}

	juce::MidiMessageCollector midiCollector;
	juce::MidiKeyboardState keyboardState;
	juce::MidiBuffer* bufferPtr;
};

#endif /* InstrData_h */
