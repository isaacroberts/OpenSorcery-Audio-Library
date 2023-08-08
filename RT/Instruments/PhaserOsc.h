//
//  PhaserOsc.h
//  
//
//  Created by Isaac Roberts on 7/16/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "OscInstrument.h"
#include "LegatoNote.h"

using namespace juce;

//#define unisonAmt 7
class PhaserOsc : public OscInstr
{
	static constexpr int unisonAmt=100;
public:
	PhaserOsc(int ch);
	virtual ~PhaserOsc() {}
	
	RTSynthVoice* getAssociatedVoice() override;
	int polyphony() override {return 10;}
	
	bool appliesToNote(int n) override
//	{return true;}
	{return n>=48;}
	virtual bool appliesToChannel(int c) override
	{return true;}
//	{return c==ch;}


	void setChorusMix(float set) { chorusMix = set; }
	void setSpread(float set) { spread = set; }
	void setUnisonNoteShift(int n);
	void setGateThresh(float set) { gateThresh = set; }
	friend class PhaserOscNote;
private:
	int ch;
	float rPhase[unisonAmt];
	float chorusMix;
	float unisonFactor;
	float spread;
	float gateThresh;
	
	ADSR::Parameters adsrParams;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaserOsc);

};

class PhaserOscNote : public Note
{
public:
	PhaserOscNote();
	
	virtual bool canPlaySound (RTSynthSound* s) override {
		return (dynamic_cast<PhaserOsc*>(s)!=nullptr);
	}
	void clearCurrentNote() override;
	
	static int instrsInUse;
	
	void onStart() override;
	void onStop(float velo, bool allowTailoff) override;
	
	void playNote(ContextR& c, int startIx, int numSamples) override;
	virtual ~PhaserOscNote() {}
	
	float pitchWheelSemitones() override {return 2.f;}
	void pitchWheelMoved(float value) override {
		pitchWheel = value;
	}
	
	Osc::Type getType() {
		return static_cast<PhaserOsc*>(instrument)->getType();
	}

	
protected:
//	float savedSpread;
//#define unisonAmt 9
	static constexpr int unisonAmt = PhaserOsc::unisonAmt;
	ADSR adsr;
	float pos[unisonAmt];
	double fPhase[unisonAmt];
//	float freq2[unisonAmt];
	float pitchWheel;
	
//	float rollingDb;
	float alpha;
	
protected:
	float getPos(int u, int unAmt)
	{
//		float range = (unAmt-1)/2.f;
//		return (u-range)/range;
		return pos[u];
	}
	
	float getPan(int u, int unAmt)
	{
		//Starts at zero, ends at 1
		return (u) / float(unAmt-1);
	}
};
