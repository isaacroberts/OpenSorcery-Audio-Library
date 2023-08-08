//
//  LiveMessage.cpp
//  
//
//  Created by Isaac Roberts on 5/17/22.
//

//Midi Messages

void LiveMidi::Message::turnOn()
{
	jassert(!liveIsOn);
	liveIsOn=true;
}
void LiveMidi::Message::turnOff()
{
	jassert(liveIsOn);
	liveIsOn=false;
}
void LiveMidi::Message::turnOffIfOn()
{
	liveIsOn=false;
}

juce::MidiMessage LiveMidi::Message::liveMessageOn()
{
	jassert(!liveIsOn);
	liveIsOn=true;
	return juce::MidiMessage::noteOn(ch, note, (uint8)velo);
}
juce::MidiMessage LiveMidi::Message::liveMessageOff()
{
	jassert(liveIsOn);
	liveIsOn=false;
	return juce::MidiMessage::noteOff(ch, note, (uint8)velo);
}

LiveMidi::TrivialMessage LiveMidi::Message::trivMessageOn()
{
	jassert(!liveIsOn);
	liveIsOn=true;
	return TrivialMessage(note, velo, ch, true);
}
LiveMidi::TrivialMessage LiveMidi::Message::trivMessageOff()
{
	jassert(liveIsOn);
	liveIsOn=false;
	return TrivialMessage(note, velo, ch, false);
}

juce::MidiMessage LiveMidi::Message::toMidiMessageOn()
{
	return juce::MidiMessage::noteOn(ch, note, (uint8)velo);
}
juce::MidiMessage LiveMidi::Message::toMidiMessageOff()
{
	return juce::MidiMessage::noteOff(ch, note, (uint8)velo);
}
juce::MidiMessage LiveMidi::Message::toMidiMessageOn(float factor, int blockStart)
{
	auto msg = toMidiMessageOn();
	// *4 because this is in bars
	msg.setTimeStamp(start * factor - blockStart);
	return msg;
}
juce::MidiMessage LiveMidi::Message::toMidiMessageOff(float factor, int blockStart)
{
	auto msg = toMidiMessageOff();
	msg.setTimeStamp((start+length) * factor - blockStart);
	return msg;
}


void LiveMidi::Message::quantize (int qValue, Listener* src)
{
	//TODO: QDiv not same in this
	auto quantiseValue = [&](int qDiv, float valueToQuantise) -> float
	{
		int qV = valueToQuantise * qDiv;
		const float simpleQLow = qV / float(qDiv); //use lossey int division
		const float simpleQHigh = (qV+1) / float(qDiv);
		
		if (valueToQuantise-simpleQLow < simpleQHigh-valueToQuantise) { //lower boundry
			return simpleQLow;
		}
		return simpleQHigh;
	};
	
	float nStart = quantiseValue(qValue, start);
	float nLength = quantiseValue(qValue, length);
	if (nLength < 1.f/qValue)
		nLength = 1.f/qValue;
	setStartAndLength(nStart, nLength, src);
}

void LiveMidi::Message::setStartAndLength(float sStart, float sLength, Listener* src) {
	jassert(sLength > 0);
	
	bool sDif = !teq(sStart, start);
	bool lDif = !teq(sLength, length);
	if (sDif && lDif)
	{
		std::swap(start, sStart);
		std::swap(length, sLength);
		sequence->noteStartAndLengthChanged(sStart, sLength, thisPtr(), src);
	}
	else if (sDif)
	{
		std::swap(start, sStart);
		sequence->noteStartChanged(sStart, thisPtr(), src);
	}
	else if (lDif)
	{
		std::swap(length, sLength);
		sequence->noteLengthChanged(sLength, thisPtr(), src);
	}
}

void LiveMidi::Message::moveT(float by, Listener* src)
{
	if (by==0) return;
	
	float orig = start;
	if (by < 0 && by+start < 0)
	{
		start=0;
	}
	else
		start+=by;
	sequence->noteStartChanged(orig, thisPtr(), src);
}
void LiveMidi::Message::moveN(int by, Listener* src)
{
	if (by==0) return;
	int orig = note;
	if (by+note < 0) note = 0;
	else if (by+note > 127) note=127;
	sequence->noteNumChanged(orig, thisPtr(), src);
}
void LiveMidi::Message::setStart(float set, Listener* src) {
	set = std::max(0.f, set);
	if (!teq(set,start))
	{
		std::swap(start, set);
		sequence->noteStartChanged(set, thisPtr(), src);
	}
}
void LiveMidi::Message::setLength(float set, Listener* src) {
	jassert(set > 0);
	set = std::max(0.f, set);
	if (!teq(set,length))
	{
		std::swap(length, set);
		sequence->noteLengthChanged(set, thisPtr(), src);
	}
}
void LiveMidi::Message::setNote(note_t set, Listener* src) {
	if (set >128)
		set = 128;
	if (set!=note)
	{
		std::swap(note, set);
		sequence->noteNumChanged(set, thisPtr(), src);
	}
}
void LiveMidi::Message::setVelocity(note_t set, Listener* src) {
	if (set!=velo)
	{
		std::swap(velo, set);
		//Don't notify velo - it won't change anything during playback
	}
}
void LiveMidi::Message::setChannel(note_t set, Listener* src) {
	jassert(set <= 16);
	if (set!=ch)
	{
		std::swap(ch, set);
		sequence->noteChannelChanged(set, thisPtr(), src);
	}
}
void LiveMidi::Message::setStartAndEnd(float start, float end, Listener* src) {
	jassert(end >= start);
	if (end < start)
		std::swap(end, start);
	setStartAndLength(start, end-start, src);
}
void LiveMidi::Message::setEnd(float set, Listener* src) {
	setLength(set-start, src);
}
void LiveMidi::Message::setNote(int set, Listener* src) {
	set = jlimit(0, 127, set);
	setNote((note_t)(set), src);
}
void LiveMidi::Message::setVelocity(float set, Listener* src) {
	set = jlimit(0.f, 1.f, set);
	setVelocity((note_t)(set*127), src);
}
void LiveMidi::Message::setVelocity(int set, Listener* src) {
	set = jlimit(0, 127, set);
	setVelocity((note_t)(set), src);
}
void LiveMidi::Message::setChannel(int set, Listener* src) {
	set = jlimit(0, 16, set);
	setChannel((note_t)(set), src);
}
void LiveMidi::Message::deleteMessage(Listener* src)
{
	sequence->removeNote(thisPtr(), src);
}


LiveMidi::Message::
Message(LiveMidi* seq, note_t note_, float start_, float length_, note_t velo_, note_t ch_)
	: sequence(seq),
	note(note_),
	start(start_),
	length(length_),
	velo(velo_),
	ch(ch_)
{}

LiveMidi::Message::
Message(LiveMidi* seq, MidiMessageSequence::MidiEventHolder* event)
: sequence(seq),
note(event->message.getNoteNumber()), velo(event->message.getVelocity()), ch(event->message.getChannel())
{	
	start = event->message.getTimeStamp()/4.f;
	if (event->noteOffObject)
	{
		length = event->noteOffObject->message.getTimeStamp()/4.f - start;
		if (length < .0625) length = .0625;
	}
	else
		length = .25f;
}
LiveMidi::Message::
Message(LiveMidi* seq, MidiMessageSequence::MidiEventHolder* event, int ticksPerBar)
: sequence(seq),
note(event->message.getNoteNumber()), velo(event->message.getVelocity()), ch(event->message.getChannel())
{
	start = event->message.getTimeStamp() / float(ticksPerBar);
	if (event->noteOffObject)
	{
		float endStamp = event->noteOffObject->message.getTimeStamp();
		length = endStamp/ticksPerBar - start;
		if (length < .0625) length = .0625;
	}
	else
		length = .25f;
}

LiveMidi::Message::Message(Message&& other) noexcept // move constructor
	: sequence(other.sequence),
	note(other.note),
	start(other.start),
	length(other.length),
	velo(other.velo),
	ch(other.ch)
{}

LiveMidi::Message&
LiveMidi::Message::operator=(Message&& other) noexcept // move assignment
{
	//sequence=other.sequence;//?
	note=other.note;
	start=other.start;
	length=other.length;
	velo=other.velo;
	ch=other.ch;
	return *this;
}

LiveMidi::Message::~Message()
{}

bool LiveMidi::Message::operator==(const Message& o) const //equality operator
{
	return note==o.note && velo==o.velo && ch==o.ch && teq(start, o.start) && teq(length, o.length);
}
bool LiveMidi::teq(float t1, float t2)
{
	return fabs(t1-t2) < .001f;
}




LiveMidi::TrivialMessage::TrivialMessage()
: n(0), v(0), c(0),
isNoteOn(false), isAllNotesOff(false)
{
	
}
LiveMidi::TrivialMessage::TrivialMessage(note_t note, note_t velo, note_t ch, bool isOn)
: n(note), v(velo), c(ch),
isNoteOn(isOn), isAllNotesOff(false)
{
	
}
LiveMidi::TrivialMessage LiveMidi::TrivialMessage::allNotesOff(note_t c) {
	TrivialMessage t(0,0,c,false);
	t.isAllNotesOff=true;
	return t;
}
juce::MidiMessage LiveMidi::TrivialMessage::getMidiMessage()
{
	if (isAllNotesOff)
		return MidiMessage::allNotesOff(c);
	else if (isNoteOn)
		return MidiMessage::noteOn(c, n, v);
	else
		return MidiMessage::noteOff(c, n, v);
}
