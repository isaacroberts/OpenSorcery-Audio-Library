//
//  MultiInstrumentProcessor.h
//  Lo-Fi Sampler - Shared Code
//
//  Created by Isaac Roberts on 7/8/20.
//  Copyright © 2020 New Nation LLC. All rights reserved.
//

// Created by Isaac Roberts

#pragma once

#if 0

#include <JuceHeader.h>

using namespace juce;

#include "Instruments/Instrument.h"
#include "InstrumentProcessor.h"

class MultiInstrumentProcessor : public InstrumentProcessor//, public MidiKeyboardState::Listener
{
	/*
	 It just overrides InstrumentProcessor
	 
	 That means there's an unnecessary Synth object hanging around but it's okay 
	 
	 */
	
public:
	//==============================================================================
	MultiInstrumentProcessor(int synthAmt);
	~MultiInstrumentProcessor() override;
	
	void setInstrument(int n, Instrument* i);
	
//	virtual int getInstrumentAmt()=0;
//	virtual Instrument* getInstrument(int n)=0;
	
	void setSynth(int ix);
	int getSynthAmt() {
		return synthAmt;
	}
	int getCurrentInstrument() {
		return curSynth;
	}
	void next() { setSynth((curSynth+1) % synthAmt);}
	void prev() { setSynth(((curSynth-1)%synthAmt+synthAmt) % synthAmt);}/*safe modulus for negatives*/
	
	//==============================================================================
	virtual void releaseResources() override;


    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
//	virtual void prepare(double sampleRate, int samplesPerBlock) {}
	
	void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
//	virtual void postProcess(AudioBuffer<float> &buffer) {}
	
	

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int index) override;
//	const String getProgramName (int index) override;
//	void changeProgramName (int index, const String& newName) override;


protected:
	//==============================================================================
	
//	AudioDeviceManager deviceManager;
	
	// this collects real-time midi messages from the midi input device, and
	// turns them into blocks that we can process in our audio callback
	MidiMessageCollector midiCollector;

	// this represents the state of which keys on our on-screen keyboard are held
	// down. When the mouse is clicked on the keyboard component, this object also
	// generates midi messages for this, which we can pass on to our synth.
	MidiKeyboardState keyboardState;

	int synthAmt;
	int curSynth;
	//Allows tailing for previous synth
	int lastSynth;
private:
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiInstrumentProcessor)
protected:
	Synthesiser* synths;
};
#endif
