//
//  MidiEnsaferer.h
//  KeyFinder - All
//
//  Created by Isaac Roberts on 8/3/21.
//  Copyright © 2021 Algorithmic Design. All rights reserved.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>
	
#include "MidiCommon.h"

/*
class MidiEnsaferer {
public:
	MidiEnsaferer();
	
	virtual ~MidiEnsaferer();
	
public:
	
	void prepare();
	void clearNotes();
	void allNotesOffReceived();
	
	void reset();
	
	void process(juce::MidiBuffer &midi, int numSamples, bool useControllers);
	
	void internal_note_on(juce::MidiBuffer& to, juce::MidiMessage& orig, int note, int sample);
	void internal_note_off(juce::MidiBuffer& to, juce::MidiMessage& orig, int note, int sample);

	void internal_note_on(juce::MidiBuffer& to, int note, int sample, float velo);
	void internal_note_off(juce::MidiBuffer& to, int note, int sample);
	
//	void unchecked_note_off(juce::MidiBuffer& to, int noteOff, int sample);
	
	juce::uint32 inc_note(note_t n);
	juce::uint32 dec_note(note_t n);
	

protected:
	void turn_prev_off(juce::MidiBuffer& midi, int orig_note, int c, int sample);
	
	juce::BigInteger note_counts;
	int ch;
	
#define NOTE_COUNT_BITS 2
#define MAX_NOTE_COUNT 3
#define TOTAL_COUNT_BITS (NOTE_COUNT_BITS*128)
	
	bool needsAllNotesOff=false;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiEnsaferer);

};
*/
