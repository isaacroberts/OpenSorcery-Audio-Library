//
//  MidiFilePlayer.h
//  
//
//  Created by Isaac Roberts on 7/19/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

/*
	Used for the Business Card, to play sweet home alabama
 */
class MidiFilePlayer
{
public:
	MidiFilePlayer();
	
	void loadFile(const char* sourceData, size_t dataSize, int trackNum=0);
//	void loadFile(String, int trackNum=0);
	void loadFile(File, int trackNum=0);
	void loadFile(MidiFile, int trackNum=0);
	
	void loadBestTrack(File);
	void loadBestTrack(MidiFile);
	
	void shiftNotes(int n);
	void setStartTime(float set);
	void setStopTime(float set);
	void freeFile();
	
	
	void setSampleRate(float set);
	
	void renderToBuffer(MidiBuffer& buffer, int numSamples);
	
	
	void reset();
	void restart();
	void play();
	void pause();
	void stop();
	void start();
	void togglePlay();
	void toggleReset();
	void setPlaying(bool set);
	
	virtual ~MidiFilePlayer();
	
	void setLoopOnEnd(bool set) {
		loopOnEnd = set;
	}
	
protected:
	bool handleEOF();//returns whether to continue
	const MidiMessageSequence* testFile(MidiFile&, int track);
	void loadMMS(const MidiMessageSequence* s);
	int getNumNoteOnEvents(const MidiMessageSequence* s);
	
protected:
	const MidiMessageSequence* seq;
	float startTime;
	float stopTime;

	float positionTime;
	float sampleRate;
	bool playing;
	bool ended;
	bool needsAllNotesOff;
	
	std::mutex renderingLock;
	
public:
	bool loopOnEnd;
	bool clearIncoming;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiFilePlayer);

};
