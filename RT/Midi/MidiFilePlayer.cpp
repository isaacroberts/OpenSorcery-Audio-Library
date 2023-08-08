//
//  MidiFilePlayer.cpp
//  
//
//  Created by Isaac Roberts on 7/19/21.
//

// Created by Isaac Roberts


#include "MidiFilePlayer.h"


MidiFilePlayer::MidiFilePlayer()
: seq(NULL),
startTime(0),
stopTime(0),
positionTime(0),
sampleRate(1000),
playing(false),
ended(false),
needsAllNotesOff(false),
loopOnEnd(false),
clearIncoming(false)
{
}
void MidiFilePlayer::setSampleRate(float set)
{
	sampleRate = set;
}
void MidiFilePlayer::loadFile(const char* data, size_t dataSize, int trackNum)
{
	MemoryInputStream dataStream((const void*)data, dataSize, false);
	MidiFile file;
	file.readFrom(dataStream, true);
	loadFile(file, trackNum);
}
void MidiFilePlayer::loadFile(File f, int trackNum)
{
	if (!f.existsAsFile())
	{
		jassertfalse;
		return;
	}
	FileInputStream myStream (f);
	if (!myStream.openedOk())
	{
		jassertfalse;
		return;
	}
	MidiFile midi;
	bool msxs = midi.readFrom (myStream);
	if (msxs)
	{
		loadFile(midi, trackNum);
	}
	else
	{
		jassertfalse;
	}
}
void MidiFilePlayer::loadBestTrack(File f)
{
	if (!f.existsAsFile())
	{
		jassertfalse;
		return;
	}
	FileInputStream myStream (f);
	if (!myStream.openedOk())
	{
		jassertfalse;
		return;
	}
	MidiFile midi;
	bool msxs = midi.readFrom (myStream);
	if (msxs)
	{
		loadBestTrack(midi);
	}
	else
	{
		jassertfalse;
	}
}

int MidiFilePlayer::getNumNoteOnEvents(const MidiMessageSequence* s)
{
	int amt=0;
	for (int n=0; n < s->getNumEvents(); ++n)
	{
		if (s->getEventPointer(n)->message.isNoteOn())
			amt++;
	}
	return amt;
}
const MidiMessageSequence* MidiFilePlayer::testFile(MidiFile& file, int track)
{
	//TODO: add all tracks if trackNum == -1
	auto* tmpSeq = file.getTrack(track);
//	if (tmpSeq->getNumEvents()==0)
//	{
////		jassertfalse;
//		return NULL;
//	}
//
//	auto start = seq->getStartTime();
//	auto stop = seq->getEndTime();
//
//	std::cout<<start<<" - "<<stop<<std::endl;
//
//	if (stop<=start)
//	{
////		jassertfalse;
//		return NULL;
//	}
	if (getNumNoteOnEvents(tmpSeq)==0)
		return NULL;
	return tmpSeq;
}
void MidiFilePlayer::loadMMS(const MidiMessageSequence* tmpSeq)
{
	
//	auto start = tmpSeq->getStartTime();
	float start=0;
	auto stop = tmpSeq->getEndTime();
	
	//Finds first non-meta event instead of interpreting tempo events
	for (int n = 0; n < tmpSeq->getNumEvents(); ++n)
	{
		auto* event = tmpSeq->getEventPointer(n);
		if (!event->message.isMetaEvent())
		{
			start = tmpSeq->getEventTime(n);
			break;
		}
	}
	
	renderingLock.lock();

	reset();
	
	delete seq;
	seq = new MidiMessageSequence(*tmpSeq);
	startTime = start;
	stopTime = stop;
	positionTime = startTime;
	renderingLock.unlock();
}
void MidiFilePlayer::loadFile(MidiFile file, int trackNum)
{
	if (file.getNumTracks() <= trackNum)
		jassertfalse;
	
	file.convertTimestampTicksToSeconds();

	auto* tmpSeq = testFile(file, trackNum);
	if (tmpSeq)
	{
		//Copy to remove const-ness
		loadMMS(tmpSeq);
	}
}
void MidiFilePlayer::loadBestTrack(MidiFile file)
{
	int N = file.getNumTracks();
	if (N==0) return;

	file.convertTimestampTicksToSeconds();
//	int max = 0;
//	int best = 0;
	
	MidiMessageSequence* seq = new MidiMessageSequence();
	
	for (int n=0; n < N; ++n)
	{
		seq->addSequence(*file.getTrack(n), 0);
//		int amt = getNumNoteOnEvents(file.getTrack(n));
//		if (amt > max)
//		{
//			max  = amt;
//			best = n;
//		}
	}
	loadMMS(seq);
	delete seq;
}
void MidiFilePlayer::shiftNotes(int shift)
{
	renderingLock.lock();
	for (int n=0; n < seq->getNumEvents(); ++n)
	{
		auto* event = seq->getEventPointer(n);
		if (event->message.isNoteOnOrOff())
		{
			event->message.setNoteNumber(event->message.getNoteNumber()+shift);
		}
	}
	renderingLock.unlock();
}
void MidiFilePlayer::setStartTime(float set)
{
	startTime = set;
}
void MidiFilePlayer::setStopTime(float set)
{
	stopTime = set;
}
void MidiFilePlayer::freeFile()
{
	delete seq;
	seq = NULL;
}
bool MidiFilePlayer::handleEOF()
{
	needsAllNotesOff=true;
	if (loopOnEnd)
	{
		positionTime = startTime;
		playing=true;
		ended=false;
	}
	else
	{
		playing = false;
		ended = true;
	}
	return !ended;
}
void MidiFilePlayer::renderToBuffer(MidiBuffer& buffer, int numSamples)
{
	if (needsAllNotesOff)
	{
		for (int n=1; n <= 16; ++n ) {
			buffer.addEvent(MidiMessage::allNotesOff(n), 0);
		}
		needsAllNotesOff=false;
	}
	if (seq)
	{
		if (playing && !ended)
		{
			renderingLock.lock();
			//Prevent multithreading errors
			float posTime = positionTime;
			float endTime = posTime + float(numSamples) / sampleRate;
			positionTime = endTime;
			
			if (posTime < stopTime)
			{
				if (clearIncoming)
					buffer.clear();
								
				for (int n = seq->getNextIndexAtTime(posTime); n < seq->getNumEvents(); ++n)
				{
					float time = seq->getEventTime(n);
					auto* event = seq->getEventPointer(n);
					
					if (time >= stopTime || time >= endTime) {
						break;
					}
					else
					{
						int sample = (time-posTime) * sampleRate;
						buffer.addEvent(event->message, sample);
					}
				}
			}
			else {
				bool willContinue = handleEOF();
				if (willContinue)
				{
					if (clearIncoming)
						buffer.clear();
				}
				
			}
			renderingLock.unlock();
		}
	}
}
void MidiFilePlayer::reset()
{
	positionTime = startTime;
	if (playing || clearIncoming)
		needsAllNotesOff=true;
	ended=false;
}
void MidiFilePlayer::restart()
{
	ended = false;
	positionTime = startTime;
	if (playing || clearIncoming)
		needsAllNotesOff=true;
	playing = true;
}
void MidiFilePlayer::setPlaying(bool set)
{
	if (set != playing)
	{
		if (set)
			play();
		else
			pause();
	}
}
void MidiFilePlayer::play()
{
	if (ended) {
		positionTime = startTime;
		ended = false;
	}
	if (!playing && clearIncoming)
		needsAllNotesOff = true;
	playing=true;
}
void MidiFilePlayer::pause()
{
	if (playing)
		needsAllNotesOff=true;
	playing=false;
}
void MidiFilePlayer::stop()
{
	ended=true;
	positionTime=startTime;
	if (playing || clearIncoming)
		needsAllNotesOff=true;
	playing=false;
}
void MidiFilePlayer::start()
{
	if (playing || clearIncoming)
		needsAllNotesOff=true;
	
	positionTime = startTime;
	ended = false;
	playing=true;
}
void MidiFilePlayer::togglePlay()
{
	if (playing) pause();
	else play();
}
void MidiFilePlayer::toggleReset()
{
	if (playing) stop();
	else start();
}
MidiFilePlayer::~MidiFilePlayer()
{
	delete seq;
}

