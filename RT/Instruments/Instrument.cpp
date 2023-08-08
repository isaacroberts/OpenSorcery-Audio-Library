//
//  Instrument.cpp
//  Library - Shared Code
//
//  Created by Isaac Roberts on 7/8/20.
//  Copyright © 2020 RobertsAudio. All rights reserved.
//

// Created by Isaac Roberts


#include "Instrument.h"

Instrument::Instrument()
: RTSynthSound(),
active(true)
//preset(NULL)
{
	
}

Instrument::~Instrument()
{
//	delete preset;
}

Note::Note()
: RTSynthVoice(),
noteNum(69),//nice
velo(1.0f),
freq(440.),
phase(0),
time(0),
instrument(NULL)
{
}

//float Instrument::getParam(int n)
//{
//	return preset->get(n);
//}
void Note::setInstrument(Instrument* i)
{
	instrument = i;
}
void Note::startNote(int n, float velocity, RTSynthSound* synth, int pitchWheel)
{
	setInstrument((Instrument*)synth);
	
	if (instrument->isActive())
	{
		phase = 0;
		time = 0;
		noteNum = n;
		velo = velocity;

		freq = Utility::midiToFreq(noteNum);
		
		onStart();

		if (usePitchWheel())
			pitchWheelMoved(pitchWheel);
	}
	else
	{
		clearCurrentNote();
	}
}


void Note::pitchWheelMoved(int value)
{
	pitchWheelMoved(pitchWheelValueToFactor(value));
}

void Note::pitchWheelMoved(float value)
{
	if (usePitchWheel())
	{
		freq = Utility::midiAndWheelToFreq(noteNum, value);
	}
}

void Note::stopNote(float velo, bool allowTail)
{
	if (!allowTail)
	{
		clearCurrentNote();
	}
	onStop(velo, allowTail);
}

void Note::renderNextBlock(ContextR& c, int startIx, int numSamples)
{
	if ( isVoiceActive())
	{
		int prePhase = phase;
		playNote(c, startIx, numSamples);
		// just handle it here
		if (phase == prePhase)
			phase += numSamples;
		time += ((float)numSamples) / getSampleRate();
	}
}
void Note::renderTaper(ContextR& c, int startIx, int numSamples, int taperIx, int totalTaperLength)
{
	if ( isVoiceActive())
	{
		int prePhase = phase;
		playTaper(c, startIx, numSamples, taperIx, totalTaperLength);
		// just handle it here
		if (phase == prePhase)
			phase += numSamples;
		time += ((float)numSamples) / getSampleRate();
	}
}


void Note::afterTouchChanged(float value)
{
	velo = value;
}
/*
void Instrument::save(XmlElement* parent)
{
	XmlElement* element =new XmlElement(getNameForPreset());
	parent->addChildElement(element);
	preset->save(element);
}
void Instrument::load(XmlElement* parent)
{
	XmlElement* element = parent->getChildByName(getNameForPreset());
	if (element != NULL)
	{
		preset->load(element);
	}
}
*/
