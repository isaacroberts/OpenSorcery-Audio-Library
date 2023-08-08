//
//  MultiLegatoSynth.h
//  
//
//  Created by Isaac Roberts on 5/24/22.
//

// Created by Isaac Roberts 

#pragma once

#include "RTSynth.h"
//using namespace juce;

class MultiLegatoSynth : public RTSynth
{
	class MLSVoice : public RTSynthVoice
	{
	public:
		MLSVoice() : RTSynthVoice(), lastNote(-1) {}
		~MLSVoice() {}
		void setLastNote(int set) {
			lastNote = set;
		}
		int getLastPlayingNote() {
			return lastNote;
		}

	protected:
		int lastNote;
	};
	
public:
	MultiLegatoSynth() : RTSynth() {
//		setNoteStealingEnabled(true);
	}
	virtual ~MultiLegatoSynth() {}
	
	void startVoice (RTSynthVoice* const voice,
								  RTSynthSound* const sound,
								  const int midiChannel,
								  const int midiNoteNumber,
							  const float velocity) override;

protected:
	
	/** Searches through the voices to find one that's not currently playing, and
		which can play the given sound.

		Returns nullptr if all voices are busy and stealing isn't enabled.

		To implement a custom note-stealing algorithm, you can either override this
		method, or (preferably) override findVoiceToSteal().
	*/
	RTSynthVoice* findFreeVoice (RTSynthSound* soundToPlay,
											 int midiChannel,
											 int midiNoteNumber,
											 bool stealIfNoneAvailable,
								 ContextR&, int, int) override;

	/** Chooses a voice that is most suitable for being re-used.
		The default method will attempt to find the oldest voice that isn't the
		bottom or top note being played. If that's not suitable for your synth,
		you can override this method and do something more cunning instead.
	*/
	//TODO: Add gatherVoiceTaper
	RTSynthVoice* findVoiceToSteal (RTSynthSound* soundToPlay,
												int midiChannel,
												int midiNoteNumber) const override;


	
	
	
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiLegatoSynth);

};
