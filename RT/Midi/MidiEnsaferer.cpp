//
//  MidiEnsaferer.cpp
//  KeyFinder - All
//
//  Created by Isaac Roberts on 8/3/21.
//  Copyright © 2021 Algorithmic Design. All rights reserved.
//

// Created by Isaac Roberts


#include "MidiEnsaferer.h"

//using namespace Scales;
using namespace juce;

/*
MidiEnsaferer::MidiEnsaferer()
:
note_counts(),
needsAllNotesOff(false)
{
	note_counts.setRange(0, NOTE_COUNT_BITS*128, false);
}
void MidiEnsaferer::prepare()
{
	needsAllNotesOff=false;
}
void MidiEnsaferer::allNotesOffReceived()
{
	note_counts.setRange(0, NOTE_COUNT_BITS*128, false);
}
void MidiEnsaferer::clearNotes()
{
	//TODO: figure out current midi channel
//	postMidiMessage(MidiMessage::allNotesOff(1));
	needsAllNotesOff=true;
}
void MidiEnsaferer::process(MidiBuffer& midi, int numSamples, bool useControllers)
{
	if (needsAllNotesOff)
	{
		midi.addEvent(MidiMessage::allNotesOff(1), 0);
		needsAllNotesOff=false;
		allNotesOffReceived();
	}
}
MidiEnsaferer::~MidiEnsaferer()
{
	
}

uint32 MidiEnsaferer::inc_note(note_t n)
{
	auto ct = note_counts.getBitRangeAsInt(n*NOTE_COUNT_BITS, NOTE_COUNT_BITS);
	if (ct < MAX_NOTE_COUNT)
	{
		note_counts.setBitRangeAsInt(n*NOTE_COUNT_BITS, NOTE_COUNT_BITS, ct+1);
	}
	return ct;
}
uint32 MidiEnsaferer::dec_note(note_t n)
{
	auto ct = note_counts.getBitRangeAsInt(n*NOTE_COUNT_BITS, NOTE_COUNT_BITS);
	if (ct > 0)
	{
		--ct;
		note_counts.setBitRangeAsInt(n*NOTE_COUNT_BITS, NOTE_COUNT_BITS, ct);
	}
	return ct;
}

void MidiEnsaferer::internal_note_on(juce::MidiBuffer& to, juce::MidiMessage& orig, int sample)
{
	if (newNote >=0  && newNote < 128)
	{
		auto ct = inc_note(newNote);

		if (ct == 0)
		{
			MidiMessage newMsg(orig);
			to.addEvent(newMsg, sample);
		}
		else
		{
			if (ct > 20)
			{
				
			}
			//Send off message for previous
			MidiMessage offMsg = MidiMessage::noteOff(orig.getChannel(), newNote, 1.0f);
			to.addEvent(offMsg, sample);
			//Send on message for current
			MidiMessage onMsg(orig);
			onMsg.setNoteNumber(newNote);
			to.addEvent(onMsg, sample+1);
		}
	}
//	else
//		jassertfalse;
}
void MidiEnsaferer::internal_note_off(juce::MidiBuffer& to, juce::MidiMessage& orig, int note_num, int sample)
{
	if (note_num >= 0 && note_num < 128)
	{
		auto ct = dec_note(note_num);

		if (ct > 20)
		{
			
		}
		
		if (ct == 0)
		{
			MidiMessage newMsg = MidiMessage::noteOff(orig.getChannel(), note_num, 1.0f);
			to.addEvent(newMsg, sample);
		}
	}
//	else
//		jassertfalse;
}
void MidiEnsaferer::internal_note_on(juce::MidiBuffer& to, int newNote, int sample, float velo)
{
	if (newNote >= 0 && newNote < 128)
	{
		auto ct = inc_note(newNote);
		const int c=1;
		if (ct == 0)
		{
			MidiMessage newMsg = MidiMessage::noteOn(c, newNote, velo);
			to.addEvent(newMsg, sample);
		}
		else
		{
			if (ct > 20)
			{
				
			}
			//Send off message for previous
			MidiMessage offMsg = MidiMessage::noteOff(c, newNote, 1.0f);
			to.addEvent(offMsg, sample);
			//Send on message for current
			MidiMessage onMsg = MidiMessage::noteOn(c, newNote, velo);
			to.addEvent(onMsg, sample+1);
		}
	}
//	else jassertfalse;
}
void MidiEnsaferer::internal_note_off(juce::MidiBuffer& to, int note_num, int sample)
{
	if (note_num >= 0 && note_num < MAX_NOTE)
	{
		const int c = 1;
		auto ct = dec_note(note_num);
		if (ct > 20)
		{
			
		}
		if (ct == 0)
		{
			MidiMessage newMsg = MidiMessage::noteOff(c, note_num, 1.0f);
			to.addEvent(newMsg, sample);
		}
	}
//	else jassertfalse;
}
*/
