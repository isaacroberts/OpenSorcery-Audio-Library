//
//  LegatoNote.cpp
//  
//
//  Created by Isaac Roberts on 5/24/22.
//

// Created by Isaac Roberts


#include "LegatoNote.h"


LegatoNote::LegatoNote()
: RTSynthVoice(),
noteNum(69),//nice
targetFreq(440),
targetVelo(1.0f),
freqBeforePW(440.f),
pitchWheelFactor(1.0f),
alpha(0),
velo(1.0f),
freq(440.),
time(-1),
instrument(NULL)
{
//	setLegatoTime(.25f);
}

void LegatoNote::setInstrument(Instrument* i)
{
	instrument = i;
}
void LegatoNote::startNote(int n, float velocity, RTSynthSound* synth, int pitchWheel)
{
	setInstrument((Instrument*)synth);
	
	if (instrument->isActive())
	{
		bool wasSilent = getCurrentlyPlayingNote()==0 || alpha<=0;
		
		noteNum = n;

		targetVelo = velocity;
		targetFreq = Utility::midiToFreq(noteNum);
		
		if (time < 0)
		{
			//First time note is set, it should hard set to correct value
			wasSilent=true;
			velo = targetVelo;
			freqBeforePW = targetFreq;
			time = 0;
		}
		
		onStart(wasSilent);

		if (usePitchWheel())
			pitchWheelMoved(pitchWheel);
	}
	else
	{
		clearCurrentNote();
	}
}


void LegatoNote::stopNote(float velo, bool allowTail)
{
	if (!allowTail)
	{
//		time = -1;
		clearCurrentNote();
	}
	onStop(velo, allowTail);
}

void LegatoNote::pitchWheelMoved(int value)
{
	pitchWheelMoved(pitchWheelValueToFactor(value));
}

void LegatoNote::pitchWheelMoved(float value)
{
	if (usePitchWheel())
	{
		pitchWheelFactor = Utility::wheelToFreqFactor(value);
	}
}

void LegatoNote::renderNextBlock(ContextR& c, int startIx, int numSamples)
{
	if ( isVoiceActive())
	{
		
		playNote(c, startIx, numSamples);
		time += ((float)numSamples) / getSampleRate();
	}
}
void LegatoNote::updateLegatoSample()
{	
	bool mtchF = fabs(freqBeforePW-targetFreq)<.05;
	bool mtchV = fabs(velo-targetVelo)<.01;
	
	if (!mtchF || !mtchV)
	{
		if (!mtchF)
			freqBeforePW = timeStep(freqBeforePW, targetFreq, alpha);
		if (!mtchV)
			velo = timeStep(velo, targetVelo, alpha);
	}
	freq = freqBeforePW * pitchWheelFactor;
}

void LegatoNote::afterTouchChanged(float value)
{
	velo = value;
}
