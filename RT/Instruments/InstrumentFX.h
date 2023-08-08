//
//  InstrumentFX.h
//  
//
//  Created by Isaac Roberts on 3/29/22.
//

// Created by Isaac Roberts 

#pragma once

#include "InstrFXBase.h"

using namespace juce;

template <class Instr, class SynthType=RTSynth>
class InstrumentFX : public InstrFXBase
{
public:
	InstrumentFX()
	: InstrFXBase(), instr(NULL)
	{
	}
	template<class... Args>
	void createInstr(Args&&... args)
	{
		instr = new Instr(args...);
		synth.addSound(instr);
		for (int n=0; n < instr->polyphony(); ++n)
		{
			synth.addVoice(instr->getAssociatedVoice());
		}
	}
	template<class... Args>
	void createInstrument(Args&&... args) { createInstr(args...); }

	template<class... Args>
	void createInstrWithPolyphony(int polyphony, Args&&... args)
	{
		instr = new Instr(args...);
		synth.addSound(instr);
		for (int n=0; n < polyphony; ++n)
		{
			synth.addVoice(instr->getAssociatedVoice());
		}
	}

	
//	Override
	//FX Stuff
	void prepare2(Spec& spec) override
	{
		synth.prepare(spec);
	}
	void process (ContextR& c)
	{
		fx_proc_start(c);
		synth.process(c, getMidiBuffer());
	}
	void process(ContextNR& c)
	{
		fx_proc_start(c);
		c.bypassProcess();
		synth.process(c, getMidiBuffer());
	}
	void bypassProcess(ContextR& c, bypassParams)
	{
		if (allowTail)
			synth.processTail(c, getMidiBuffer());
		else
			synth.processSilence(c, getMidiBuffer());
	}
	void bypassProcess(ContextNR& c, bypassParams) 
	{
		if (allowTail)
			synth.processTail(c, getMidiBuffer());
		else
			synth.processSilence(c, getMidiBuffer());
	}
	
	void sidechain(InputBlock& levels, ContextR& c) {process(c); }
	void sidechain(InputBlock& levels, ContextNR& c) {process(c); }
	
	void allNotesOff() override
	{
		synth.allNotesOff(-1, true);
	}
	void allSoundsOff() override
	{
		synth.allNotesOff(-1, false);
	}
	
	Instr& getInstr() { return *instr; }
	Instr& operator*() { return *instr; }
	Instr* operator->() { return instr; }
	
	SynthType& getSynth() { return synth; }
	
	virtual ~InstrumentFX()
	{
		synth.clearVoices();
		synth.clearSounds();
	}
protected:
	SynthType synth;
	Instr* instr;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentFX);

};
