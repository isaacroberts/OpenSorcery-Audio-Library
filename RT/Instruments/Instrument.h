//
//  Instrument.h
//  Library - Shared Code
//
//  Created by Isaac Roberts on 7/8/20.
//  Copyright © 2020 RobertsAudio. All rights reserved.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

//#include "../Presets/Preset.h"

#include "RTSynth.h"

//class Note;

using namespace juce;
using namespace dsp;

class Instrument : public RTSynthSound
{
public:
	//Will be passed to the note on start
	Instrument();
	
	/*
	 
	 //----------Synth Definitions-------------
	 int polyphony() override {return 16;}
	 Note* getAssociatedVoice() override {
		return new Note();
	 }

	 bool appliesToNote(int num) override {return true;}
	 bool appliesToChannel(int c) override {return true;}

	 
	 */
	
	
	//---------- Overrides --------------
	virtual int getParamAmt() {
		return 0;
	}
	virtual void set(int n, float set) {}
	virtual float get(int n) {return 0;}
	
	//----------Synth Definitions-------------
	virtual int polyphony() {return 16;}
	virtual RTSynthVoice* getAssociatedVoice()=0;
	
	virtual bool appliesToNote(int num) override {return true;}
	virtual bool appliesToChannel(int c) override {return true;}
	
	virtual ~Instrument();
	
	void setActive(bool set) { active = set; }
	bool isActive() { return active; }
	
protected:
	bool active;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Instrument);
};

class Note : public RTSynthVoice
{
public:
	Note();
	
	virtual void setInstrument(Instrument* instr);
	
	
	/*
	 void onStart() override;
	 void onStop(float velo, bool allowTail) override;
	 void playNote(ContextR& c, int startIx, int numSamples) override;
	 bool canPlaySound (RTSynthSound* instr) override {
		return dynamic_cast<const InstrumentClassName*> (instr) != nullptr;
	 }
	 //Instrument Definitions
	 bool usePitchWheel() override { return true; }
	 float pitchWheelSemitones() override {return 2.f;}
	 //Controls
	 void pitchWheelMoved(float value) override;
	 void controllerMoved(int cNum, float value) override {}
	 //void afterTouchChanged(float value) override;
	 void channelPressureChanged(float value) override {}
	 
	 */
	
// --------- Overrides ---------
	virtual void onStart() {}
	virtual void onStop(float velo, bool allowTail) {}
	//Should return
	virtual bool canPlaySound (RTSynthSound*) override = 0;
	virtual void playNote(ContextR& c, int startIx, int numSamples) =0;
	virtual void playTaper(ContextR& c, int startIx, int numSamples, int taperIx, int totalTaperLength) {
		//implement this!
		jassertfalse;
	}

	
	//Can be overridden
	virtual void pitchWheelMoved(float value);
	
	
	/* Controllers: (Unmentioned are undefined)
		0: Bank Select
		1: Modulation wheel
		2: Breath
		4: Foot pedal
		5: Portamento time
		6: Data entry (?)
		7: Volume
		8: Balance
		10: Pan
		11: Expression
		12: Effect Controller 1
		13: Effect controller 2
		64: Damper pedal on/off
		65: Portamento on/off
		66: Sostenuto on/off
		67: Soft pedal on/off
		68: Legato Footswitch on/off
		69: Hold 2
		70-79: Sound controller 1-10
		80-83: General purpose Midi CC
		84: Portamento amount
		88: High resolution velocity prefix
		91-95: Effect N depth
	 */
	virtual void controllerMoved(int cNum, float value) {}
	virtual void afterTouchChanged(float value);
	virtual void channelPressureChanged(float value) {}
	//Instrument Definitions
	virtual bool usePitchWheel() { return true; }
	virtual float pitchWheelSemitones() {return 2.f;}
	
	//Controls
	void pitchWheelMoved(int value) override;
	inline void controllerMoved(int c, int value) override { controllerMoved(c, (value/127.f));}
	inline void aftertouchChanged(int value) override { afterTouchChanged(value/127.f);}
	inline void channelPressureChanged(int value) override { channelPressureChanged(value/127.f);}
	
	
	//Should not be overridden
	void startNote(int n, float velo, RTSynthSound* sound, int pitchWheel) override;
	void stopNote(float velo, bool allowTail) override;
	void renderNextBlock(ContextR&, int startIx, int numSamples) override;
	void renderTaper(ContextR&, int startIx, int numSamples, int taperIx, int totalTaperLength) override;

	
protected:
	int noteNum;
	float velo;
	float freq;
	int phase;
	float time;
	
	Instrument* instrument;
	
	inline float pitchWheelValueToFactor(int value) {
		return (value-8192)/16383.f * pitchWheelSemitones()/12.f;
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Note);
};



