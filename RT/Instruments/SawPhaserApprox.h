//
//  SawPhaserApprox.h
//  
//
//  Created by Isaac Roberts on 5/25/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "OscInstrument.h"
#include "LegatoNote.h"
#include "../FX/Filters/RTButterworth.h"

using namespace juce;

//#define unisonAmt 7
class SawPhaser : public Instrument
{
//	static constexpr int unisonAmt=100;
public:
	SawPhaser();
	virtual ~SawPhaser() {}
	
	RTSynthVoice* getAssociatedVoice() override;
	int polyphony() override {return 2;}
	
	bool appliesToNote(int n) override
//	{return true;}
	{return n>=20;}
	virtual bool appliesToChannel(int c) override
	{return true;}
//	{return c==ch;}


//	void setChorusMix(float set) { chorusMix = set; }
	void setSpread(float set) { spread = set; }
	void setFreq(float set) { topFreq = set; }
//	void setUnisonNoteShift(int n);
//	void setGateThresh(float set) { gateThresh = set; }
	friend class SawPhaserNote;
	
	void setType(Osc::Type set) {
		type = set;
	}
	Osc::Type getType() { return type; }
protected:
	Osc::Type type;

public:
	float spread;
	float topFreq=22000;
	
	ADSR::Parameters adsrParams;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SawPhaser);

};

class SawPhaserNote : public Note
{
public:
	SawPhaserNote();
	
	virtual bool canPlaySound (RTSynthSound* s) override {
		return (dynamic_cast<SawPhaser*>(s)!=nullptr);
	}
//	void clearCurrentNote() override;
	
//	static int instrsInUse;
	
	void onStart() override;
	void onStop(float velo, bool allowTailoff) override;
	
	void playNote(ContextR& c, int startIx, int numSamples) override;
	virtual ~SawPhaserNote() {}
	
	float pitchWheelSemitones() override {return 2.f;}
	void pitchWheelMoved(float value) override {
		pitchWheel = value;
	}
	
protected:
	ADSR adsr;
	float t;
	float pureT;
	float noteOffEnv;
	float lastX;
//	float falloffRate;
//	float pos[unisonAmt];
//	double fPhase[unisonAmt];
//	float freq2[unisonAmt];
	float pitchWheel;
	
//	float rollingDb;
//	float alpha;
	
protected:
};
