//
//  InstrSwitch.h
//  
//
//  Created by Isaac Roberts on 5/24/22.
//

#ifndef InstrSwitch_h
#define InstrSwitch_h

#include "InstrFXBase.h"

using namespace juce;

template <class Instr, int Amt>
class InstrumentSwitch : public InstrFXBase
{
public:
	InstrumentSwitch()
	: InstrFXBase(), instr()
	{
		for (int n=0; n < Amt; ++n)
		{
			instr[n] = NULL;
			isTailing[n]=false;
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
	void createInstr(int n, Args... args)
	{
		jassert (!instr[n]);
		instr[n] = new Instr(args...);
		synth[n].addSound(instr[n]);
		for (int v=0; v < instr[n]->polyphony(); ++v)
		{
			synth[n].addVoice(instr[n]->getAssociatedVoice());
		}
	}
	
	template<class... Args>
	void createInstrWithPolyphony(int polyphony, int n, Args... args)
	{
		jassert (!instr[n]);
		instr[n] = new Instr(args...);
		synth[n].addSound(instr[n]);
		for (int v=0; v < polyphony; ++v)
		{
			synth[n].addVoice(instr[n]->getAssociatedVoice());
		}
	}
	void setInstr(int n)
	{
		if (curSynth != n)
		{
			synth[curSynth].allNotesOff(0, true);
			isTailing[curSynth]=true;
			curSynth = n;
		}
	}
	//FX Stuff
	void prepare2(Spec& spec) override
	{
		for (int n=0; n < Amt; ++n)
			synth[n].prepare(spec);
	}
	template <class Context>
	void process (Context& c)
	{
		fx_proc_start(c);

		synth[curSynth].process(c, getMidiBuffer());
		for (int n=0; n < Amt; ++n)
		{
			if (isTailing[n] && n!=curSynth)
			{
				isTailing[n] = synth[n].processTail(c, getMidiBuffer());
			}
		}
	}
	template <class Context>
	void bypassProcess(Context& c, bypassParams)
	{
		if (allowTail)
		{
			for (int n=0; n < Amt; ++n)
			{
				if (isTailing[n])
				{
					isTailing[n] = synth[n].processTail(c, getMidiBuffer());
				}
				//TODO: Consider
//				else
//					synth[n].processSilence(c, getMidiBuffer());
			}
		}
		else
		{
			for (int n=0; n < Amt; ++n)
			{
				synth[n].processSilence(c, getMidiBuffer());
			}

		}
	}
	
	void sidechain(InputBlock& levels, ContextR& c) {process(c); }
	void sidechain(InputBlock& levels, ContextNR& c) {process(c); }
	
	void allNotesOff() override
	{
		synth[curSynth].allNotesOff(-1, true);
		isTailing[curSynth]=true;
	}
	void allSoundsOff() override
	{
		synth[curSynth].allNotesOff(-1, false);
		isTailing[curSynth]=false;
		for (int n=0; n < Amt; ++n)
		{
			if (isTailing[n])
			{
				synth[n].allNotesOff(-1, false);
				isTailing[n]=false;
			}
		}
	}
	
	Instr& getInstr(int n) { return *instr[n]; }
	
	Instr& operator[](int n) { return *instr[n]; }
	
	virtual ~InstrumentSwitch()
	{
		for (int n=0; n < Amt; ++n)
		{
			synth[n].clearVoices();
			synth[n].clearSounds();
//			delete instr[n];
		}
	}
protected:
	RTSynth synth[Amt];
	Instr* instr[Amt];
	//Allows tailing
	bool isTailing[Amt];
	int curSynth = 0;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentSwitch);

};

#endif /* InstrSwitch_h */
