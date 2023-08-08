//
//  MidiScheme.h
//  
//
//  Created by Isaac Roberts on 7/5/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "PlayheadMonitor.h"
#include <boost/lockfree/queue.hpp>
#include "MidiCommon.h"

using namespace juce;

class MidiScheme : public PlayheadMonitor::Listener
{
private:
	struct TrivialMessage {
		TrivialMessage();
		TrivialMessage(note_t note, note_t velo, note_t ch, bool isOn);
		static TrivialMessage allNotesOff(note_t c);
		note_t n, v, c;
		bool isNoteOn;
		bool isAllNotesOff;
		juce::MidiMessage getMidiMessage();
	};

public:
	MidiScheme();
	MidiScheme(PlayheadMonitor*);
	MidiScheme(PlayheadMonitor& p) : MidiScheme(&p) {}

	MidiMessageSequence* getMidiMessageSequence(int ticksPerQN);
	MidiMessageSequence* getMidiMessageSequenceNoTimeSig(int ticksPerQn);

	virtual void renderMidi(MidiBuffer& buff, int numSamples) = 0;
protected:
	virtual void allNotesOff() = 0;
	virtual void loadMidiMessageSequence(MidiMessageSequence*, float factor) = 0;
public:
//	void positionJumped(float from, float to) override;
	void pauseChanged(bool isNowPlaying) override;
//	void bpmChanged(float set, int timeSigNum, int timeSigDen) override;


	
	void setLooping(bool set) { looping= set; }
	virtual ~MidiScheme();

protected:
	//Override
	
	//Utility
	void renderNextStep(MidiBuffer& buff, int numSamples);

	void getRenderTimes(float length, int numSamples, float& start, float& step, int& patternLengthSp);
	//Returns -1 if it shouldnt
	int  getRenderFrame (float t, int numSamples, float start, float step, int patternLengthSp);
	int  getRenderFrameIfLate (float t, int numSamples, float start, float step, int pl=0);

	bool shouldNoteBeOn(float start, float length);
	
	void noteOff(note_t n, note_t c);
	void noteOn (note_t n, note_t v, note_t c);
		
	void lazyAllNotesOff(note_t c);
	void lazyAllNotesOff();

	static bool teq(float t1, float t2);

protected:
	boost::lockfree::queue<TrivialMessage> nextStepMessages;
	float renderingStartBar=0;
	float renderingEndBar=0;

	bool looping=true;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiScheme);

};
