//
//  MultiLegatoSynth.cpp
//  
//
//  Created by Isaac Roberts on 5/24/22.
//

// Created by Isaac Roberts


#include "MultiLegatoSynth.h"


//void MultiLegatoSynth::MLSVoice::clearCurrentNote()
//{
//	isPlaying=false;
//}
//bool MultiLegatoSynth::MLSVoice::isVoiceActive() const
//{
//	return isPlaying;
//}

void MultiLegatoSynth::startVoice (RTSynthVoice* const voice,
								  RTSynthSound* const sound,
								  const int midiChannel,
								  const int midiNoteNumber,
								  const float velocity)
{
	
	if (voice != nullptr && sound != nullptr)
	{
		voice->currentlyPlayingNote = midiNoteNumber;
//		voice->setLastNote(midiNoteNumber);
		voice->currentPlayingMidiChannel = midiChannel;
		voice->noteOnTime = ++lastNoteOnCounter;
		voice->currentlyPlayingSound = sound;
		voice->setKeyDown (true);
		voice->setSostenutoPedalDown (false);
		voice->setSustainPedalDown (sustainPedalsDown[midiChannel]);

		voice->startNote (midiNoteNumber, velocity, sound,
						  lastPitchWheelValues [midiChannel - 1]);
	}
}
//==============================================================================
RTSynthVoice* MultiLegatoSynth::findFreeVoice (RTSynthSound* soundToPlay,
											  int midiChannel, int midiNoteNumber,
											  const bool stealIfNoneAvailable,
											   ContextR&, int, int) 
{
	const ScopedLock sl (lock);

	for (auto* voice : voices)
		if ((! voice->isVoiceActive()) && voice->canPlaySound (soundToPlay))
			return voice;

	auto voice= findVoiceToSteal (soundToPlay, midiChannel, midiNoteNumber);
	return voice;
//	return nullptr;
}

RTSynthVoice* MultiLegatoSynth::findVoiceToSteal (RTSynthSound* soundToPlay,
												 int midiChannel, int midiNoteNumber) const
{
	// This voice-stealing algorithm applies the following heuristics:
	// - Re-use the oldest notes first
	// - Protect the lowest & topmost notes, even if sustained, but not if they've been released.

	// apparently you are trying to render audio without having any voices...
	jassert (! voices.isEmpty());

	RTSynthVoice* closest=nullptr;
	int minDist = 256;//More than possible
	
	for (auto* voice : voices)
	{
		if (voice->canPlaySound (soundToPlay))
		{
//			int n2 = voice->getLastNote();
			int n2 = voice->getCurrentlyPlayingNote();

			int dist = abs(midiNoteNumber - n2);
			if (dist < minDist)
			{
				minDist = dist;
				closest = voice;
			}
		}
	}
	return closest;
}
