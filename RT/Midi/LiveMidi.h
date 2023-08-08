//
//  LiveEditMidiSequence.h
//  
//
//  Created by Isaac Roberts on 5/17/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "PlayheadMonitor.h"

#include <boost/lockfree/queue.hpp>

#include "MidiCommon.h"

using namespace juce;

class LiveMidi : public PlayheadMonitor::Listener
{
public:
	typedef LiveMidi* Ptr;
	class Listener;
	
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
	
	
	class Message : public juce::ReferenceCountedObject {
	public:
		typedef ReferenceCountedObjectPtr<Message> Ptr;
	private:
		//Rule of 5
		Message(LiveMidi* seq, note_t note, float start, float length, note_t velo, note_t ch);
		Message(LiveMidi* seq, MidiMessageSequence::MidiEventHolder*);
		Message(LiveMidi* seq, MidiMessageSequence::MidiEventHolder*, int ticksPerBar);

	public:
		Message(Message&& other) noexcept; // move constructor
		Message& operator=(Message&& other) noexcept; // move assignment
		bool operator==(const Message& other) const; //equality operator
		~Message();

		//Midi Messages
			//Keeps track of live-outputting midi
		juce::MidiMessage liveMessageOn();
		juce::MidiMessage liveMessageOff();
			//For boost lockfree
		TrivialMessage trivMessageOn();
		TrivialMessage trivMessageOff();
		
		juce::MidiMessage toMidiMessageOn();
		juce::MidiMessage toMidiMessageOff();
		
		juce::MidiMessage toMidiMessageOn(float factor, int blockStartSample=0);
		juce::MidiMessage toMidiMessageOff(float factor, int blockStartSample=0);
	
		bool isActive() { return sequence!=NULL; }
		bool isDead() { return sequence==NULL; }
		
		void deleteMessage(Listener* src=NULL);
		
		//Modifiers
		void quantize(int qValue, Listener* src=NULL);
//		void moveT(float to, Listener* source=NULL);
		void moveT(float by, Listener* source=NULL);
		void moveN(int by, Listener* source=NULL);

		void changeLength(float to, Listener* source=NULL);
	
		//Setters
		void setStartAndEnd(float start, float end, Listener* src=NULL);
		void setStartAndLength(float start, float length, Listener* src=NULL);
	
		void setStart(float set, Listener* src=NULL);
		void setLength(float set, Listener* src=NULL);
		void setEnd(float set, Listener* src=NULL);

		void setNote(int set, Listener* src=NULL);
		void setVelocity(float set, Listener* src=NULL);
		void setVelocity(int set, Listener* src=NULL);
		void setChannel(int set, Listener* src=NULL);
		
		void setNote(note_t set, Listener* src=NULL);
		void setVelocity(note_t set, Listener* src=NULL);
		void setChannel(note_t set, Listener* src=NULL);
		
		//Getters
		note_t getNote () {return note;}
		note_t getVelocity () {return velo;}
		float getFloatVelo() { return velo/127.f; }
		float getStartTime () {return start;}
		float getNoteLength () {return length;}
		note_t getChannel() { return ch; }
		
		float getStart() { return start; }
		float getLength() { return length; }
		float getEnd() { return start+length; }
		
		bool isNoteOn() { return liveIsOn; }
		bool isOn() { return liveIsOn; }

		void turnOn();
		void turnOff();
		void turnOffIfOn();
	protected:
		void clear() { jassert(!liveIsOn); sequence=NULL; }
		Ptr thisPtr() { return Ptr(this); }
	private:
		friend class LiveMidi;
		LiveMidi* sequence;
		note_t note;
		float start;
		float length;
		note_t velo;
		note_t ch;
		bool liveIsOn=false;
		
		/*
			TODO: You could calculate the sample position here to save cycles when rendering midi 
		 
		 */
		
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Message);
	};
	struct Listener {
		Listener() {}
		virtual ~Listener() {}
		/*
		 void noteUpdated(LiveMidi::Message::Ptr msg, bool position, LiveMidi*) override;
		 void noteAdded  (LiveMidi::Message::Ptr msg, LiveMidi*) override;
		 void noteRemoved(int ix, LiveMidi::Message::Ptr msg, LiveMidi*) override;
		 void allNotesUpdated(LiveMidi*) override;
		 */
		virtual void noteUpdated(Message::Ptr msg, bool position, LiveMidi*) = 0;
		virtual void noteAdded  (Message::Ptr msg, LiveMidi*) = 0;
		virtual void noteRemoved(int ix, Message::Ptr msg, LiveMidi*) = 0;

		virtual void allNotesUpdated(LiveMidi*) = 0;
	};
	
	LiveMidi();
	LiveMidi(PlayheadMonitor*);
	LiveMidi(PlayheadMonitor& p) : LiveMidi(&p) {}

	void swapData(LiveMidi& m);
	void copyData(LiveMidi& m);
	
	void renderMidi(MidiBuffer& buff, int numSamples);
	MidiMessageSequence* getMidiMessageSequence(int ticksPerQN);
	//Returns NULL if no notes on that channel 
	MidiMessageSequence* getMidiMessageSequenceForChannel(int ticksPerQN, int channel, bool moveToChannelOne=true);

	MidiMessageSequence* getMidiMessageSequenceOneChannel(int ticksPerQN, int channel=1);
	MidiMessageSequence* getMidiMessageSequenceDrumSep(int ticksPerQN);

	MidiMessageSequence* getMidiMessageSequenceNoTimeSig(int ticksPerQn);
	
	MidiMessageSequence* getMidiMessageSequenceForReload();

	int getNumEvents() { return messages.size(); }
	Message::Ptr getMessage(int n) { return messages[n]; }
	
	void setFixedLength(float set);
	void removeFixedLength() { setFixedLength(-1); }
	float getFixedLength() { return fixedLength;}
	void setChannel(int set);
	
	void setEnabled(bool set);
	bool isEnabled() { return enabled; }
	
	void togglePlay();
	void setLooping(bool set);
//	void play();
	void loop();
//	void pause();
	void stop();
	
	float getPosition();
	
	virtual ~LiveMidi();
	
	void addListener(Listener*, bool callUpdate);
	void removeListener(Listener*);
	
	Message::Ptr addNoteOnChannel(int note, int ch, float start, float length=.25, float velo=1.0f, Listener* src=NULL) {
		return _addNote(note, ch, start, length, velo, src);
	}
	Message::Ptr addNote(int note, float start, float length=.25, float velo=1.0f, Listener* src=NULL) {
		return _addNote(note, defaultChannel, start, length, velo, src);
	}
protected:
	Message::Ptr _addNote(int note, int ch, float start, float length, float velo, Listener* src);
public:
	void removeNote(Message::Ptr note, Listener* src);
	void loadSequence(const MidiMessageSequence* seq, Listener* src=NULL);
	void loadSequence(const MidiMessageSequence* seq, int ticksPerQuarterNote, Listener* src=NULL);

	void loadFile(MidiFile file, Listener* src=NULL);

	void clear(Listener* src=NULL);
	
	bool wasNoteOnWithin(float time);
	
protected:
	void positionJumped(float from, float to) override;
	void pauseChanged(bool isNowPlaying) override;
	void bpmChanged(float set, int timeSigNum, int timeSigDen) override;

	
protected:
	void addNote(Message::Ptr note, Listener* src);
	
	//Preconditions: Must have actually changed, new value must be set
	void noteStartChanged	(float tFrom, Message::Ptr msg, Listener* src);
	void noteLengthChanged	(float tFrom, Message::Ptr msg, Listener* src);
	void noteNumChanged		(note_t from, Message::Ptr msg, Listener* src);
	void noteChannelChanged	(note_t from, Message::Ptr msg, Listener* src);
	void noteStartAndLengthChanged(float startFrom, float lengthFrom, Message::Ptr msg, Listener* src);

	bool shouldNoteBeOn(float start, float length);
	bool isNoteCurrentlyPlaying(Message::Ptr note);

	int getNoteIndex(Message::Ptr note);
	
	void updateOne(Listener* src, Message::Ptr msg, bool position);
	void updateAdded(Listener* src, Message::Ptr msg);
	void updateRemoved(Listener* src, int ix, Message::Ptr msg);
	void updateAll(Listener* src);

	void inthread_updateOne(Listener* src, bool position, Message::Ptr msg);
	void inthread_updateAdded(Listener* src, Message::Ptr msg);
	void inthread_updateRemoved(Listener* src, int ix, Message::Ptr msg);

	void inthread_updateAll(Listener* src);
	
	void updateSequenceLength();
	void allNotesOff();
	void noteOff(Message::Ptr msg);
	void noteOn (Message::Ptr msg);

	void noteOff(note_t n, note_t c);
	void noteOn (note_t n, note_t v, note_t c);
	
	void clearNextStep();
	
	
	static bool teq(float t1, float t2);
protected:
	boost::lockfree::queue<TrivialMessage> nextStepMessages;
	ReferenceCountedArray<Message> messages;
	ListenerList<Listener> listeners;
//	float currentBar=0;
	//Keeps track of what's playing even when playhead is changed
	float renderingStartBar=0;
	float renderingEndBar=0;
	
	float fixedLength=-1;
	float sequenceLength = 0;
	
//	float bpm=120;
	int defaultChannel=1;
	
//	bool playing=false;
	bool enabled=true;
	bool looping=false;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveMidi);

};
