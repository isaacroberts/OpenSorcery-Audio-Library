//
//  Wavetable_Env.h
//  Library - Shared Code
//
//  Created by Isaac Roberts on 7/8/20.
//  Copyright © 2020 RobertsAudio. All rights reserved.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "Instrument.h"

/*
	Wavetable is different than RTSampler because it uses a TableEnv.
		The TableEnv has:
			 sourceLength : Total length of original sample
			 sourceFade	: Fade % of sample (by sourceLength)
			 attack : Attack time of note
			 noteHold : Length of each note
			 noteFade : Fade (%) of note (by noteHold)
		Todo: Consider moving TableEnv to a struct instead of 5 variables
			  Consider combining RTSampler & Wavetable
 
 */

class Wavetable_Env : public Instrument {
public:
	Wavetable_Env();
	
	void setBaseNote(int midiNote);
	void setBaseNote(int n, int o) { setBaseNote(n + o * 12); }
	void setBaseFreq(float sampleFreq);
	void setTable(float sourceRate, float sampleFreq, AudioBuffer<float> data);
	void setTable(float sourceRate, int n, int o, AudioBuffer<float> data);

	void clearTable();
	
	const AudioBuffer<float>& getTable() { return table; }
	float getSourceSampleRate() {
		return sourceRate;
	}
	float getBaseFreq() {
		return baseFreq;
	}
	virtual Note* getAssociatedVoice() override;
	
	virtual ~Wavetable_Env();
	
	enum Params {
		//Fade are in Pct (0-1)
		SourceLength, SourceFade,
		Attack, Hold, Fade
	};
	
	void setParam(int param, float set);
	float getParam(int param);
	
protected:
	AudioBuffer<float> table;
	float baseFreq;
	float sourceRate;
	
	//All in seconds
	float sourceLength;
	float sourceFade;
	
	float attack, noteHold, noteFade;
	
	friend class WaveTableNote;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wavetable_Env);

public:
	class Voice : public Note
	{
	public:
		Voice() : Note ()
		{
		}
		
		void onStart() override;
		void onStop(float velo, bool allowTail) override;
		
		virtual void playNote(ContextR& c, int startIx, int numSamples) override;
		virtual bool canPlaySound (RTSynthSound*) override;

	protected:
	//	Wavetable_Env* instr;
		float floatPhase;
		float time=0;
	//	float remainder=0;

		float getEnv(Wavetable_Env* instr, int sample, float timeRatio, float rate);
		float copyData(const float* input, float* data, float ratio, int endIx, float tablePhase, int tableSize, float volume, Wavetable_Env* instr);
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Voice);
	
	};
};
