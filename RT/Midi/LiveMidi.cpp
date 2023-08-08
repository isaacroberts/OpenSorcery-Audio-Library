//
//  LiveEditMidiSequence.cpp
//  
//
//  Created by Isaac Roberts on 5/17/22.
//

// Created by Isaac Roberts


#include "LiveMidi.h"
#include "RTGraphics/Utilities.h"
//#include "RTGraphics/DebugWindow.h"

LiveMidi::LiveMidi()
 : PlayheadMonitor::Listener(false),
	nextStepMessages(10),
	messages(),
	listeners(),
	fixedLength(-1),
	sequenceLength(0),
	enabled(true),
	looping(false)
{
}
LiveMidi::LiveMidi(PlayheadMonitor* p)
: PlayheadMonitor::Listener(false, p),
	nextStepMessages(10),
	messages(),
	listeners(),
	fixedLength(-1),
	sequenceLength(0),
	enabled(true),
	looping(false)
{
	
}
/*
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
 */
void LiveMidi::swapData(LiveMidi& o)
{
	std::swap(messages, o.messages);
	std::swap(sequenceLength, o.sequenceLength);
	allNotesOff();
	updateAll(NULL);
}
void LiveMidi::copyData(LiveMidi& o)
{
	messages = o.messages;
	sequenceLength = o.sequenceLength;
	
	allNotesOff();
	updateAll(NULL);
}

/*
	TODO: Update this class to use the MidiScheme - AFTER releasing the lemonaid
 
 */


LiveMidi::~LiveMidi()
{
	for (int n=0; n < messages.size(); ++n)
	{
		//Removes hanging sequence ptr
		messages[n]->turnOffIfOn();
		messages[n]->clear();
	}
}

void LiveMidi::setFixedLength(float set)
{
	fixedLength = set;
}
void LiveMidi::setChannel(int set)
{
	allNotesOff();
	defaultChannel = set;
}
bool LiveMidi::wasNoteOnWithin(float time)
{
	float rangeStart = getPosition() - time;
	float rangeEnd = getPosition();
	for (int n=0; n < messages.size(); ++n)
	{
		float s = messages[n]->getStart();
		if (s > rangeStart && s <= rangeEnd)
			return true;
	}
	return false;
}
MidiMessageSequence* LiveMidi::getMidiMessageSequence(int ticksPerQn)
{
	MidiMessageSequence* seq = new MidiMessageSequence();
	float factor = getMonitor()->getMidiFactor(ticksPerQn);
	for (int n=0; n < messages.size(); ++n)
	{
		auto msg = messages[n];
		seq->addEvent(msg->toMidiMessageOn (factor));
		seq->addEvent(msg->toMidiMessageOff(factor));
	}
	seq->updateMatchedPairs();
	return seq;
}
MidiMessageSequence* LiveMidi::getMidiMessageSequenceForChannel(int ticksPerQn, int ch, bool moveToChannelOne)
{
	MidiMessageSequence* seq=NULL;
		
	float factor = getMonitor()->getMidiFactor(ticksPerQn);
	for (int n=0; n < messages.size(); ++n)
	{
		auto msg = messages[n];
		if (msg->getChannel() == ch)
		{
			if (seq==NULL)
				seq = new MidiMessageSequence();
			
			auto m1 = msg->toMidiMessageOn (factor);
			auto m2 = msg->toMidiMessageOff(factor);
			if (moveToChannelOne)
			{
				m1.setChannel(1);
				m2.setChannel(1);
			}
			seq->addEvent(m1);
			seq->addEvent(m2);
		}
	}
	if (seq)
	{
		seq->updateMatchedPairs();
	}
	return seq;
}


MidiMessageSequence* LiveMidi::getMidiMessageSequenceOneChannel(int ticksPerQn, int channel)
{
	MidiMessageSequence* seq = new MidiMessageSequence();
	float factor = getMonitor()->getMidiFactor(ticksPerQn);
	for (int n=0; n < messages.size(); ++n)
	{
		auto msg = messages[n];
		auto m1 = msg->toMidiMessageOn (factor);
		m1.setChannel(channel);
		seq->addEvent(m1);
		auto m2 = msg->toMidiMessageOff(factor);
		m2.setChannel(channel);
		seq->addEvent(m2);
	}
	seq->updateMatchedPairs();
	return seq;
}

MidiMessageSequence* LiveMidi::getMidiMessageSequenceNoTimeSig(int ticksPerQn)
{
	MidiMessageSequence* seq = new MidiMessageSequence();
	float factor = ticksPerQn * 4;
	for (int n=0; n < messages.size(); ++n)
	{
		auto msg = messages[n];
		seq->addEvent(msg->toMidiMessageOn (factor));
		seq->addEvent(msg->toMidiMessageOff(factor));
	}
	seq->updateMatchedPairs();
	return seq;
}
MidiMessageSequence* LiveMidi::getMidiMessageSequenceForReload()
{
	MidiMessageSequence* seq = new MidiMessageSequence();
	for (int n=0; n < messages.size(); ++n)
	{
		auto msg = messages[n];
		seq->addEvent(msg->toMidiMessageOn (4.f));
		seq->addEvent(msg->toMidiMessageOff(4.f));
	}
	seq->updateMatchedPairs();
	return seq;
}

void LiveMidi::loadSequence(const MidiMessageSequence* seq, Listener* src)
{
	resetPosition();
	clearNextStep();
	allNotesOff();
	messages.clear();
	
	for (int n=0; n < seq->getNumEvents(); ++n)
	{
		auto event = seq->getEventPointer(n);
		if (event->message.isNoteOn())
		{
			messages.add(new Message(this, event));
		}
	}
	updateSequenceLength();
	updateAll(src);
}
void LiveMidi::loadSequence(const MidiMessageSequence* seq, int ticksPerQN, Listener* src)
{
	resetPosition();
	clearNextStep();
	allNotesOff();
	messages.clear();
	
	int ticksPerBar = ticksPerQN*4;
	
	for (int n=0; n < seq->getNumEvents(); ++n)
	{
		auto event = seq->getEventPointer(n);
		if (event->message.isNoteOn())
		{
			messages.add(new Message(this, event, ticksPerBar));
		}
	}
	updateSequenceLength();
	updateAll(src);
}

void LiveMidi::loadFile(MidiFile file, Listener* src)
{
	int ticksPerQn;
	MidiMessageSequence* all = MidiUtil::mergeAndConvertToBPM(file, ticksPerQn);
	//else barLength =
	if (all->getNumEvents() > 0)
	{
		loadSequence(all, ticksPerQn);
	}
	delete all;
}

void LiveMidi::clear(Listener* src)
{
	sequenceLength=0;
	clearNextStep();
	allNotesOff();
	messages.clear();
	updateAll(src);
}
void LiveMidi::updateSequenceLength()
{
	sequenceLength=0;
	for (int i=0; i < messages.size(); ++i)
	{
		float msgEnd = messages[i]->getEnd();
		if (msgEnd > sequenceLength)
			sequenceLength = msgEnd;
	}
}

void LiveMidi::renderMidi(MidiBuffer& buff, int numSamples)
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
	
	if (messages.size()==0)
		return;
	
	if (!playing() || !enabled)
		return;
	
	float step = getBarsPerSample();
	
	float startBar = monitor->getPositionInBars();
	float endBar = startBar + numSamples * step;
	
//	float endBar = monitor->getPositionInBars();
//	float startBar = endBar - numSamples * step;
	
//	static float prevEnd = 0;
//	
//	float diff = startBar - prevEnd;
//	int sdif = int(diff/step);
//	if (sdif !=0)
//	{
////		std::cout<<"diff "<<sdif<<" samples / "<<numSamples<<  std::endl;
//		//Diff = NumSamples - previous samples
//		
//	}
//	prevEnd = endBar;
	
	
	int patternLengthSp = std::round(monitor->getSamplesPerBar() * fixedLength);
	
	//Wraparound
	float startOffset = 0;
//	float endOffset = 0;
	if (looping) // Consider: Looping OR DAW-playing
	{
		float length = fixedLength > 0 ? fixedLength : sequenceLength;

		
		startOffset = int(startBar/length) * length;

	}
	
	//This locks for multithreading purposes
	renderingStartBar = startBar-startOffset - numSamples*step;
	renderingEndBar   = endBar-startOffset;
	
	for (int i=0; i < messages.size(); ++i)
	{
		Message::Ptr msg = messages[i];
		if (!msg->isOn())
		{
			//TODO: Consider changing this back to use the slightly faster original comparisons
			auto msgStart = msg->getStart();
			int st = (msgStart+startOffset-startBar)/step;
			if (st<0)
				st += patternLengthSp;
			if (st >=0 &&  st < numSamples)
	//		if ((msgStart+startOffset) >= startBar && (msgStart+endOffset) < endBar)
			{
				//beat / (beats/second) * samples/Second
				//second * samples/Second
				//Samples
	//			int t = (msgStart+startOffset-startBar) / step;
				buff.addEvent(msg->liveMessageOn(), st);
			}
		}
		else
		{
			//When length is longer than fixedLength, it seems to not turn the note on, or immediately turn it off
			float msgEnd = msg->getEnd();
			int et = (msgEnd + startOffset - startBar) / step;
			//if note is on and past its off point, it needs to be turned off
			if (et < 0)
			{
				et = 0;
			}
			if (et >= 0 && et < numSamples)
			{
				buff.addEvent(msg->liveMessageOff(), et);
			}
		}
	}
}

int LiveMidi::getNoteIndex(Message::Ptr note)
{
	for (int n=0; n < messages.size(); ++n) {
		if (messages[n]==note)
			return n;
	}
	return -1;
}
LiveMidi::Message::Ptr LiveMidi::_addNote(int n, int ch, float start, float length, float velo, Listener* src)
{
	jassert(isPositiveAndBelow(ch, 17));
	
	int ix = messages.size();
	if (fixedLength>0)
	{
		jassert(start < fixedLength);
		if (start + length > fixedLength)
			length = fixedLength - start;
	}
	auto note = new Message(this, (note_t)(n), start, length, (note_t)(velo*127), (note_t)ch);
	messages.add(note);
	
	if (shouldNoteBeOn(start, length))
	{
		//Create Note On

	}
	
	updateAdded(src, messages[ix]);
	return messages[ix];
}
void LiveMidi::removeNote(Message::Ptr note, Listener* src)
{
	//Notes are stored by reference-counted ptrs, so must be alerted about its deletion
	if (isNoteCurrentlyPlaying(note))
	{
		//Create note off
		noteOff(note);
	}
	note->clear();
	int ix = getNoteIndex(note);
	messages.remove(ix);
	updateRemoved(src, ix, note);
}
void LiveMidi::noteStartChanged	(float tFrom, Message::Ptr msg, Listener* src)
{
	//TODO: Determine whether note is now out of playing range
	if (msg->isOn())
	{
		//TODO: Determine whether note is now out of playing range
		noteOff(msg);
	}
	
	//TODO: Make this more efficient
	updateSequenceLength();
	updateOne(src, msg, true);
}
void LiveMidi::noteLengthChanged (float tFrom, Message::Ptr msg, Listener* src)
{
	//TODO: Determine whether note is now out of playing range
	if (msg->isOn())
	{
		//TODO: Determine whether note is now out of playing range
		noteOff(msg);
	}

	//TODO: Make this more efficient
	updateSequenceLength();
	updateOne(src, msg, true);
}
void LiveMidi::noteStartAndLengthChanged (float startFrom, float lengthFrom, Message::Ptr msg, Listener* src)
{
	if (msg->isOn())
	{
		//TODO: Determine whether note is now out of playing range
		noteOff(msg);
	}
	
	//TODO: Make this more efficient
	updateSequenceLength();
	
	updateOne(src, msg, true);
}
void LiveMidi::noteNumChanged (note_t from, Message::Ptr msg, Listener* src)
{
	if (isNoteCurrentlyPlaying(msg))
	{
		//Create note off and new note on
		noteOff(from, msg->getChannel());
		msg->turnOff();
		noteOn (msg);
	}
	updateOne(src, msg, true);
}
void LiveMidi::noteChannelChanged (note_t from, Message::Ptr msg, Listener* src)
{
	if (isNoteCurrentlyPlaying(msg))
	{
		//Create note off and new note on
		noteOff(msg->getNote(), from);
		msg->turnOff();
		noteOn (msg);
	}
	updateOne(src, msg, false);
}

bool LiveMidi::shouldNoteBeOn(float start, float length)
{
	if (!playing() || !enabled) return false;
	
	float pos = monitor->getPositionInBars();
	
	if (looping) // Consider: Looping OR DAW-playing
	{
		float pl = 0;
		float startOffset = 0;
		pl = fixedLength > 0 ? fixedLength : sequenceLength;
		startOffset = int(pos/pl) * pl;
		pos -= startOffset;
	}

	return (start) < pos && (start+length) >= pos;
}
bool LiveMidi::isNoteCurrentlyPlaying(Message::Ptr msg)
{
	return msg->isOn();
//	return isNoteCurrentlyPlaying(msg->start, msg->length);
}

void LiveMidi::noteOn(note_t n, note_t v, note_t c)
{
	nextStepMessages.push(TrivialMessage(n, v, c, true));
}
void LiveMidi::noteOff(note_t n, note_t c)
{
	nextStepMessages.push(TrivialMessage(n, 127, c, false));
}

void LiveMidi::noteOn(Message::Ptr msg)
{
	nextStepMessages.push(msg->trivMessageOn());
}
void LiveMidi::noteOff(Message::Ptr msg)
{
	nextStepMessages.push(msg->trivMessageOff());
}

void LiveMidi::allNotesOff()
{
	for (int n=0; n < messages.size(); ++n)
	{
		if (messages[n]->isOn())
			noteOff(messages[n]);
//		messages[n]->turnOffIfOn();
	}
}

void LiveMidi::clearNextStep()
{
//	nextStepMessages.clear();
	//TODO: Remove notes on
}

float LiveMidi::getPosition()
{
	if (!looping)
		return getPositionInBars();
	else if (fixedLength > 0)
		return getPositionInBars(fixedLength);
	else
		return getPositionInBars(sequenceLength);
}
void LiveMidi::setEnabled(bool set)
{
	if (enabled != set)
	{
		enabled = set;
		if (!enabled)
			allNotesOff();
		
	}
}
void LiveMidi::togglePlay()
{
	if (playing())
		allNotesOff();
	setPlaying(!playing(), true, true);
}
void LiveMidi::setLooping(bool set)
{
	looping=set;
}

void LiveMidi::positionJumped(float from, float to)
{
	if (playing() && enabled)
	{
//		DebugWindow::print("LM jumped", from, to);
		//All notes off for only notes that shouldn't be on anymore
		for (int n=0; n < messages.size(); ++n)
		{
			bool isOn = messages[n]->isOn();
			bool shouldBe = shouldNoteBeOn(messages[n]->getStart(), messages[n]->getLength());
			
//			if (isOn && !shouldBe)
//				noteOff(messages[n]);
//			else if (!isOn && shouldBe)
//				noteOn(messages[n]);
			
			if (isOn)
				noteOff(messages[n]);
			if (shouldBe)
				noteOn(messages[n]);
			// isOn && shouldBe
			//	    retrigger
			// !isOn && !shouldBe
			// 		nothing
		}
	}
}
void LiveMidi::pauseChanged(bool isNowPlaying)
{
//	DebugWindow::print("LM", isNowPlaying ? "played" : "paused");
	if (!isNowPlaying && enabled)
		allNotesOff();
	
}
void LiveMidi::bpmChanged(float set, int timeSigNum, int timeSigDen)
{
	
}
//void LiveMidi::play()
//{
//	if (paused())
//	{
//		play(true, true);
//	}
//	looping=false;
//}
void LiveMidi::loop()
{
	if (paused())
	{
		play(true, false);
	}
	looping=true;
}
//void LiveMidi::pause()
//{
//	pause(false, true);
//	allNotesOff();
//}
//void LiveMidi::stop()
//{
//	pause(true, true);
//	allNotesOff();
//}

void LiveMidi::addListener(Listener* l, bool callUpdate)
{
	listeners.add(l);
	if (callUpdate)
	{
		l->allNotesUpdated(this);
	}
}
void LiveMidi::removeListener(Listener* l)
{
	listeners.remove(l);
}

void LiveMidi::updateOne(Listener* src, Message::Ptr msg, bool position) {
	if (listeners.size() > (src==NULL ? 0 : 1))
	{
		MessageManager::callAsync([&, src, position, msg]() { inthread_updateOne(src, position, msg); });
	}
}
void LiveMidi::updateAdded(Listener* src, Message::Ptr msg) {
	if (listeners.size() > (src==NULL ? 0 : 1))
	{
		MessageManager::callAsync([&, src, msg]() { inthread_updateAdded(src, msg); });
	}
}
void LiveMidi::updateRemoved(Listener* src, int ix, Message::Ptr msg) {
	if (listeners.size() > (src==NULL ? 0 : 1))
	{
		MessageManager::callAsync([&, src, ix, msg]() { inthread_updateRemoved(src, ix, msg); });
	}
}
void LiveMidi::updateAll(Listener* src)
{
	if (listeners.size() > (src==NULL ? 0 : 1))
	{
		MessageManager::callAsync([&, src]() { inthread_updateAll(src); });
	}
}
void LiveMidi::inthread_updateOne(Listener* src, bool position, Message::Ptr msg)
{
	if (src==NULL)
	{
		//if crashes: consider callChecked
		listeners.call([this, position, msg] (Listener& l) { l.noteUpdated(msg, position, this); });
	}
	else
	{
		listeners.callExcluding(src, [this, position, msg] (Listener& l) { l.noteUpdated(msg, position, this); });
	}
}
void LiveMidi::inthread_updateAdded(Listener* src, Message::Ptr msg)
{
	if (src==NULL)
	{
		//if crashes: consider callChecked
		listeners.call([this, msg] (Listener& l) { l.noteAdded(msg, this); });
	}
	else
	{
		listeners.callExcluding(src, [this, msg] (Listener& l) { l.noteAdded(msg, this); });
	}
}
void LiveMidi::inthread_updateRemoved(Listener* src, int ix, Message::Ptr msg)
{
	if (src==NULL)
	{
		//if crashes: consider callChecked
		listeners.call([this, ix, msg] (Listener& l) { l.noteRemoved(ix, msg, this); });
	}
	else
	{
		listeners.callExcluding(src, [this, ix, msg] (Listener& l) { l.noteRemoved(ix, msg, this); });
	}
}
void LiveMidi::inthread_updateAll(Listener* src)
{
	if (src==NULL)
	{
		//if crashes: consider callChecked
		listeners.call([this] (Listener& l) { l.allNotesUpdated(this); });
	}
	else
	{
		listeners.callExcluding(src, [this] (Listener& l) { l.allNotesUpdated(this); });
	}
}
