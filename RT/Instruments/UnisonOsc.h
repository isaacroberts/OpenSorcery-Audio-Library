//
//  UnisonOsc.h
//  
//
//  Created by Isaac Roberts on 7/16/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "OscInstrument.h"

using namespace juce;

class UnisonOsc : public OscInstr
{
public:
	UnisonOsc();
	virtual ~UnisonOsc() {}
	
	Note* getAssociatedVoice() override;
	int polyphony() override {return 8;}
	
	void setChorusMix(float set) { chorusMix = set; }
	void setSpread(float set) { spread = set; }
	void setUnisonNoteShift(int n);
	friend class UnisonOscNote;
private:
	static constexpr int unisonAmt=9;
	float rPhase[unisonAmt];
	float chorusMix;
	float unisonFactor;
	float spread;
	
	ADSR::Parameters adsrParams;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnisonOsc);

};

class UnisonOscNote : public OscNote
{
public:
	UnisonOscNote();
	
	void onStart() override;
	void onStop(float velo, bool allowTailoff) override;
	
	void playNote(ContextR& c, int startIx, int numSamples) override;
	virtual ~UnisonOscNote() {}
	
	float pitchWheelSemitones() override {return .5f;}
	void pitchWheelMoved(float value) override {
		pitchWheel = Util::wheelToFreqFactor(value);
	}
	
protected:
//	float savedSpread;
	ADSR adsr;
	float fPhase;
	float pitchWheel;
	
protected:
	static constexpr int unisonAmt=UnisonOsc::unisonAmt;

	float getPos(int u, int unAmt)
	{
		float range = (unAmt-1)/2.f;
		return (u-range)/range;
	}
	float getPan(int u, int unAmt)
	{
		//Starts at zero, ends at 1
		return (u) / float(unAmt-1);
	}
};
