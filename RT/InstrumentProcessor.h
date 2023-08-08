//
//  InstrumentProcessor.h
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

#include "Instruments/RTSynth.h"
#include "Instruments/Instrument.h"

class InstrumentProcessor : public AudioProcessor//, public MidiKeyboardState::Listener
{
public:
	//==============================================================================
	InstrumentProcessor();
	~InstrumentProcessor() override;

	//============ Abstract ========================================================
	virtual String getNameForPreset() const = 0;
	
	virtual int getInstrumentAmt() const =0;
	virtual Instrument* getInstrument(int n)=0;

	//============ Callables ========================================================

	void addInstrumentAndPolyphony(Instrument* instr, int poly);
	
	//==============================================================================
	virtual void releaseResources() override;


	void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	virtual void prepare(double sampleRate, int samplesPerBlock) {}
	
	void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
	virtual void postProcess(AudioBuffer<float> &buffer) {}
	
	
	//==============================================================================
//	AudioProcessorEditor* createEditor() override;
//	bool hasEditor() const override;

	const String getName() const override;

	
	bool acceptsMidi() const override {return true;}
	bool producesMidi() const override {return false;}
	bool isMidiEffect() const override {return false;}
	
//	void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
//	void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

	
	virtual double getTailLengthSeconds() const override {return 0;}

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int index) override;
	const String getProgramName (int index) override;
	void changeProgramName (int index, const String& newName) override;


	
	//==============================================================================
	virtual void getStateInformation (MemoryBlock& destData) override;
	virtual void setStateInformation (const void* data, int sizeInBytes) override;
	
	virtual void loadProcessorChainPresets (XmlElement& data) {}
	virtual void saveProcessorChainPreset (XmlElement& xml) {}
	
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

	// the synth itself!
	RTSynth synth;
private:
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentProcessor)
};

#endif
