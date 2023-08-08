//
//  InstrumentArray.h
//  
//
//  Created by Isaac Roberts on 3/30/22.
//

#ifndef StackedInstrumentFX_h
#define StackedInstrumentFX_h


#include "InstrFXBase.h"

using namespace juce;

template <class Instr, int Amt, class SynthType=RTSynth>
class InstrumentArray : public InstrFXBase
{
public:
	InstrumentArray()
	: InstrFXBase(), instr()
	{
		for (int n=0; n < Amt; ++n)
		{
			instr[n] = NULL;
		}
	}
	template<class... Args>
	void createInstrs(Args... args)
	{
		for (int n=0; n < Amt; ++n)
		{
			createInstr(n, args...);
		}
	}
	template<class... Args>
	void createInstr(int n, Args&&... args)
	{
		jassert (!instr[n]);
		instr[n] = new Instr(args...);
		synth.addSound(instr[n]);
		for (int v=0; v < instr[n]->polyphony(); ++v)
		{
			synth.addVoice(instr[n]->getAssociatedVoice());
		}
	}
	
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
	
	Instr& getInstr(int n) { return *instr[n]; }
	
	Instr& operator[](int n) { return *instr[n]; }
	
	SynthType& getSynth() { return synth; }
	
	virtual ~InstrumentArray()
	{
		synth.clearVoices();
		synth.clearSounds();
	}
protected:
	SynthType synth;
	Instr* instr[Amt];
	//Allows tailing
//	bool isTailing[Amt];

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentArray);

};

#endif /* InstrumentArray.h */


/* With multiple synths
 
 using namespace juce;

 template <class Instr, int Amt, class SynthType=RTSynth>
 class InstrumentArray : public InstrFXBase
 {
 public:
	 InstrumentArray()
	 : InstrFXBase(), instr()
	 {
		 for (int n=0; n < Amt; ++n)
		 {
			 instr[n] = NULL;
		 }
	 }
	 template<class... Args>
	 void createInstrs(Args... args)
	 {
		 for (int n=0; n < Amt; ++n)
		 {
			 createInstr(n, args...);
		 }
	 }
	 template<class... Args>
	 void createInstr(int n, Args&&... args)
	 {
		 jassert (!instr[n]);
		 instr[n] = new Instr(args...);
		 synth[n].addSound(instr[n]);
		 for (int v=0; v < instr[n]->polyphony(); ++v)
		 {
			 synth[n].addVoice(instr[n]->getAssociatedVoice());
		 }
	 }
	 
	 //FX Stuff
	 void prepare2(Spec& spec) override
	 {
		 for (int n=0; n < Amt; ++n)
			 synth[n].prepare(spec);
	 }
	 void processR (ContextR& c) override
	 {
		 for (int n=0; n < Amt; ++n)
			 synth[n].process(c, getMidiBuffer());
	 }
	 void processNR(ContextNR& c) override
	 {
		 c.bypassProcess();
		 for (int n=0; n < Amt; ++n)
			 synth[n].process(c, getMidiBuffer());
	 }
	 void reset2() override
	 {
		 
	 }
	 
	 void allNotesOff() override
	 {
		 for (int n=0; n < Amt; ++n)
			 synth[n].allNotesOff(-1, true);
	 }
	 void allSoundsOff() override
	 {
		 for (int n=0; n < Amt; ++n)
			 synth.allNotesOff(-1, false);
	 }
	 //Getters & Setters
	 int paramAmt() override { jassertfalse;return 0;}
	 void set(int n, float set) override { jassertfalse;}
	 float get(int n) override { jassertfalse;return 0;}
	 
	 Instr& getInstr(int n) { return *instr[n]; }
	 
	 Instr& operator[](int n) { return *instr[n]; }
	 
	 SynthType& getSynth() { return synth; }
	 
	 virtual ~InstrumentArray()
	 {
		 for (int n=0; n < Amt; ++n)
		 {
			 synth[n].clearVoices();
			 synth[n].clearSounds();
 //			delete instr[n];
		 }
	 }
 protected:
	 SynthType synth[Amt];
	 Instr* instr[Amt];
	 //Allows tailing
 //	bool isTailing[Amt];

 private:
	 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentArray);

 };

 */
