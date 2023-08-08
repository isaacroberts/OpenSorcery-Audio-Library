//
//  NoteModel.cpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 16/08/2019.
//

//#include "NoteModel.hpp"
//#ifndef LIB_VERSION
//#include "StaticCounter.h"
//#include "DataLoggerRoot.h"
//#endif

NoteModel::NoteModel ()
{
    note = 0;
    velocity = 0;
    startTime = 0;
    noteLegnth = 0;
#ifndef LIB_VERSION
    uniqueId = StaticCounter::count();
#endif
}
NoteModel::NoteModel (u8 n, u8 v, st_int st, st_int nl, Flags f)
{
    note = n;
    velocity = v;
    startTime = st;
    noteLegnth = nl;
    flags = f;
#ifndef LIB_VERSION
    uniqueId = StaticCounter::count();
#endif
}
NoteModel::NoteModel (const NoteModel & other)
{
    note = other.note;
    velocity = other.velocity;
    startTime = other.startTime;
    noteLegnth = other.noteLegnth;
    flags = other.flags;
#ifndef LIB_VERSION
    uniqueId = other.uniqueId;
#endif
    sendChange = other.sendChange;
}

PRESequence::PRESequence(PRESequence&& other) noexcept
	: events(std::move(other.events)),
	tsLow(other.tsLow), tsHigh(other.tsHigh),
	lowNote(other.lowNote), highNote(other.highNote)
{
	
}
// move assignment
PRESequence& PRESequence::operator=(PRESequence&& other) noexcept
{
	events = std::move(other.events);
	tsLow = other.tsLow;
	tsHigh = other.tsHigh;
	lowNote = other.lowNote;
	highNote = other.highNote;
	return *this;
}

PRESequence PRESequence::fromMidiMessageSequence(juce::MidiMessageSequence& seq, float timeFactor, int ticks)
{
	seq.updateMatchedPairs();
	
	PRESequence pre(ticks);
	timeFactor *= ticks;
	for (int n=0; n < seq.getNumEvents(); ++n)
	{
		auto event = seq.getEventPointer(n);
		if (event->message.isNoteOn())
		{
			auto& msg = event->message;
			
			int st_ticks = msg.getTimeStamp() * timeFactor;
			int l_ticks;
			if (event->noteOffObject)
			{
				l_ticks =
					(event->noteOffObject->message.getTimeStamp() - event->message.getTimeStamp())
					 * timeFactor;
			}
			else
				l_ticks = 5 * ticks;

			// NoteModel (u8 n, u8 v, st_int st, st_int nl, Flags flags);
			NoteModel note(msg.getNoteNumber(), msg.getVelocity(), st_ticks, l_ticks, NoteModel::Flags());
			pre.events.push_back(note);
		}
	}
	return pre;
}

juce::MidiMessageSequence PRESequence::getMidiMessageSequence(float timeFactor)
{
	using namespace juce;
	MidiMessageSequence seq;
	
	const int c = 1;
	
	for (int n=0; n < events.size(); ++n)
	{
		auto& note = events[n];
		
		float vel = note.getVelocity()/128.f;
		
		float startTime = note.getStartTime() / timeFactor;
		float endTime = (note.getStartTime() + note.getNoteLength()) / timeFactor;
		seq.addEvent(MidiMessage::noteOn (c, note.getNote(), vel ), startTime);
		seq.addEvent(MidiMessage::noteOff(c, note.getNote(), 1.0f), endTime);
	}
	seq.updateMatchedPairs();
	return seq;
}

void PRESequence::render(juce::MidiBuffer& buff, int numSamples, double sampleRate, float tickTimeFactor) {
	render(buff, numSamples, sampleRate, tickTimeFactor, tsHigh);
}
void PRESequence::render(juce::MidiBuffer& buff, int numSamples, double sampleRate, float timeFactor, float loopLength)
{
	if (!playing)
		return;
		
	using namespace juce;
	
	int startSmp = curTime*sampleRate;
	int endSmp = startSmp + numSamples;
	
	double toSmp = sampleRate/timeFactor;
	
	const int c = 1;
	
	for (int n=0; n < events.size(); ++n)
	{
		int start = events[n].getStartTime() * toSmp;
		int end = start + events[n].getNoteLength() * toSmp;
				
		if (start >= startSmp && start < endSmp)
			buff.addEvent(MidiMessage::noteOn (c, events[n].getNote(), events[n].getVelocity() / 128.f), (int)(start));
		
		if ( end >= startSmp  && end < endSmp)
			buff.addEvent(MidiMessage::noteOff(c, events[n].getNote(), events[n].getVelocity() / 128.f), (int)(end));
	}
	
	curTime = endSmp/sampleRate;

	if (curTime >= loopLength)
	{
		if (looping)
		{
			curTime -= loopLength;
		}
		else
			playing=false;
	}

}
void PRESequence::play()
{
	if (!playing)
	{
		curTime=0;
		playing=true;
	}
	else
	{
		playing=false;
	}
	looping=false;
}
void PRESequence::loop()
{
	if (!playing)
	{
		playing=true;
		curTime=0;
	}
	looping=true;
}


/*
 Quantising to say 1/8 notes would require an input value of 240 (or 0.5 * the default resolution)
 */
void NoteModel::quantiseModel (int qValue, bool qStartTime, bool qNoteLegnth)
{
    auto quantiseValue = [&](int qDiv, int valueToQuantise) -> int
    {
        const int simpleQLow = (valueToQuantise / qDiv) * qDiv; //use lossey int division
        const int simpleQHigh = ((valueToQuantise / qDiv)+1) * qDiv;
        
        if (valueToQuantise-simpleQLow < simpleQHigh-valueToQuantise) { //lower boundry
            return simpleQLow;
        }
        return simpleQHigh;
    };
    
    if (qStartTime) {
        startTime = quantiseValue(qValue, startTime);
    }
    if (qNoteLegnth) {
        noteLegnth = quantiseValue(qValue, noteLegnth);
        if (noteLegnth == 0) {
            noteLegnth = qValue; //
        }
    }
}

//bool NoteModel::compare (const NoteModel & other, bool compareUIDs)
//{
//#define RETURN_IF(A) if(A){return false;}
//    
//    RETURN_IF(note                  != other.note)
//    RETURN_IF(velocity              != other.velocity)
//    RETURN_IF(startTime             != other.startTime)
//    RETURN_IF(noteLegnth            != other.noteLegnth)
//    RETURN_IF(flags.state           != other.flags.state)
//    RETURN_IF(flags.isGenerative    != other.flags.isGenerative)
//    
//#ifndef LIB_VERSION
//    if (compareUIDs) {
//        RETURN_IF(uniqueId != other.uniqueId)
//    }
//#endif
//    return true;
//}


void NoteModel::setNote (u8 _note)
{
    note = _note;
    trigger();
#ifndef LIB_VERSION
    LOG_NOTE_EDITED_PR(note, velocity, startTime, noteLegnth);
#endif
}
void NoteModel::setVelocity (u8 _velocity)
{
    velocity = _velocity;
    trigger();
#ifndef LIB_VERSION
    LOG_NOTE_EDITED_PR(note, velocity, startTime, noteLegnth);
#endif
}
void NoteModel::setStartTime (st_int _time)
{
    startTime = _time;
	//TODO: Autobound
}
void NoteModel::setNoteLength (st_int _len)
{
    noteLegnth = _len;
	//TODO: Autobound
}

void NoteModel::trigger()
{
    if (sendChange != nullptr) {
        sendChange(note, velocity);
    }
}
void NoteModel::trigger(const u8 n, const u8 vel)
{
    if (sendChange != nullptr) {
        sendChange(n, vel);
    }
}
void PRESequence::autobound()
{
	//unneccesary
	for (auto n : events)
	{
		int note = n.getNote();
		jassert(note >=0 && note < 128);
		lowNote = std::min(lowNote, note);
		highNote = std::max(highNote, note);
		int tsStart = n.getStartTime();
		int tsEnd = n.getStartTime() + n.getNoteLength();
		tsLow = std::min(tsLow, tsStart);
		tsHigh = std::max(tsHigh, tsEnd);
	}
}
void PRESequence::print ()
{
    for (auto nm : events) {
        std::cout << (int) nm.getNote() << " - " << (int) nm.getVelocity() << " : " << nm.getStartTime() << " - " << nm.getNoteLength() << "\n";
    }
}

