//
//  MidiScheme.cpp
//  
//
//  Created by Isaac Roberts on 7/5/22.
//

// Created by Isaac Roberts


#include "MidiScheme.h"


MidiScheme::MidiScheme()
: PlayheadMonitor::Listener(false),
   nextStepMessages(10),
looping(true)
{
}

MidiScheme::MidiScheme(PlayheadMonitor* p)
: PlayheadMonitor::Listener(false, p),
   nextStepMessages(10),
looping(true)
{
	
}
MidiScheme::~MidiScheme()
{
	
}

void MidiScheme::renderNextStep(MidiBuffer& buff, int numSamples)
{
	while (!nextStepMessages.empty())
	{
		TrivialMessage msg;
		bool sxs = nextStepMessages.pop(msg);
		if (sxs)
		{
			//Puts note-offs before note-ons. This ensures that if it jumps from one note to the same note, it will retrigger correctly
			int smp = msg.isNoteOn ? 1 : 0;
			buff.addEvent(msg.getMidiMessage(), smp);
		}
	}
}
void MidiScheme::getRenderTimes(float length, int numSamples, float& start, float& step, int& loopRoll)
{
	step = getBarsPerSample();
	
	start = monitor->getPositionInBars();
	if (looping)
	{
		//Wraparound
		float startOffset = int(start/length) * length;
		start -= startOffset;
		
		//This locks for multithreading purposes
		renderingStartBar = start - numSamples*step;
		renderingEndBar   = start + numSamples * step;
				
		loopRoll = std::round(monitor->getSamplesPerBar() * length);
	}
	else
	{
		//This locks for multithreading purposes
		renderingStartBar = start;
		renderingEndBar   = start + numSamples * step;

		loopRoll=0;
	}
//	end = endBar - endOffset;
}
int MidiScheme::getRenderFrame(float t, int numSamples, float start, float step, int loopRoll)
{
	int st = (t-start)/step;
	
	if (st<0)
		st += loopRoll;

	if (st >=0 &&  st < numSamples)
	{
		return st;
	}
	return -1;
}
int MidiScheme::getRenderFrameIfLate(float t, int numSamples, float start, float step, int pl_)
{
	if (t < start)
		return 0;
	
	int st = (t-start)/step;
	
	if (st<0)
		return 0;

	if (st < numSamples)
	{
		return st;
	}
	return -1;
}


bool MidiScheme::shouldNoteBeOn(float start, float noteLength)
{
	if (!playing()) return false;
	
	float pos = monitor->getPositionInBars();
	
	if (looping) // Consider: Looping OR DAW-playing
	{
		jassertfalse;
		float midiLength = 1;
		float startOffset = 0;
		startOffset = int(pos/midiLength) * midiLength;
		pos -= startOffset;
	}

	return (start) < pos && (start+noteLength) >= pos;
}

void MidiScheme::pauseChanged(bool isNowPlaying)
{
	if (!isNowPlaying)
		allNotesOff();
}

void MidiScheme::noteOn(note_t n, note_t v, note_t c)
{
	nextStepMessages.push(TrivialMessage(n, v, c, true));
}
void MidiScheme::noteOff(note_t n, note_t c)
{
	nextStepMessages.push(TrivialMessage(n, 127, c, false));
}
void MidiScheme::lazyAllNotesOff(note_t c)
{
	nextStepMessages.push(TrivialMessage::allNotesOff(c));
}
void MidiScheme::lazyAllNotesOff()
{
	for (note_t c =1; c <= 16; ++c)
		nextStepMessages.push(TrivialMessage::allNotesOff(c));
}

MidiMessageSequence* MidiScheme::getMidiMessageSequence(int ticksPerQN)
{
	MidiMessageSequence* seq = new MidiMessageSequence();
	float factor = ticksPerQN * 4;
	loadMidiMessageSequence(seq, factor);
	seq->updateMatchedPairs();
	return seq;
}
MidiMessageSequence* MidiScheme::getMidiMessageSequenceNoTimeSig(int ticksPerQN)
{
	MidiMessageSequence* seq = new MidiMessageSequence();
	float factor = ticksPerQN * 4;
	loadMidiMessageSequence(seq, factor);
	seq->updateMatchedPairs();
	return seq;
}

bool MidiScheme::teq(float t1, float t2)
{
	return fabs(t1-t2) < .001f;
}







MidiScheme::TrivialMessage::TrivialMessage()
: n(0), v(0), c(0),
isNoteOn(false), isAllNotesOff(false)
{
	
}
MidiScheme::TrivialMessage::TrivialMessage(note_t note, note_t velo, note_t ch, bool isOn)
: n(note), v(velo), c(ch),
isNoteOn(isOn), isAllNotesOff(false)
{
	
}
MidiScheme::TrivialMessage MidiScheme::TrivialMessage::allNotesOff(note_t c) {
	TrivialMessage t(0,0,c,false);
	t.isAllNotesOff=true;
	return t;
}
juce::MidiMessage MidiScheme::TrivialMessage::getMidiMessage()
{
	if (isAllNotesOff)
		return MidiMessage::allNotesOff(c);
	else if (isNoteOn)
		return MidiMessage::noteOn(c, n, v);
	else
		return MidiMessage::noteOff(c, n, v);
}
