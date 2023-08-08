//
//  OscInstrument.h
//  
//
//  Created by Isaac Roberts on 7/16/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "Instrument.h"

class OscInstr : public Instrument
{
public:
	
	OscInstr() : OscInstr(Osc::Saw) {}
	OscInstr(Osc::Type t);

	void setNoAdsr();
	void setOnOffTesterAdsr();
	
	int polyphony() override {return 16;}
	
	RTSynthVoice* getAssociatedVoice() override;
	
	virtual ~OscInstr() {}
	
	void setType(Osc::Type set) {
		type = set;
	}
	Osc::Type getType() { return type; }
	auto& getADSR() { return adsr;}
protected:
	Osc::Type type;
	ADSR::Parameters adsr;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscInstr);

};

class OscNote : public Note
{
public:
	OscNote() : Note() {}
	
	void onStart() override {
		if (OscInstr* si = dynamic_cast<OscInstr*>(instrument)) {
			adsr.setParameters(si->getADSR());
			adsr.noteOn();
		}
	}
	
	void onStop(float vel, bool allowTail) override {
		if (allowTail)
			adsr.noteOff();
		else
			clearCurrentNote();
	}
	
	bool canPlaySound (RTSynthSound*) override;
	
	void playNote(ContextR& c, int startIx, int numSamples) override;

	~OscNote() {}
	
protected:
	Osc::Type getType();
	juce::ADSR adsr;
//	float env(float);
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscNote);

};
