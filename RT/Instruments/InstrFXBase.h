//
//  InstrFXBase.h
//  
//
//  Created by Isaac Roberts on 3/30/22.
//

#ifndef InstrFXBase_h
#define InstrFXBase_h

#include <JuceHeader.h>

//#include "../FX/FX.h"
#include "RTSynth.h"
#include "InstrData.h"

#include "../FX/Templates/Shared.h"

class InstrFXBase
{
	/*	 
	 Derivative:
	 
	 
	 
	 void process(ContextNR& c)
	 {
		 c.bypassProcess();
		 ContextR c2(c.getStorage(), c.getOutput());
		 process(c2);
	 }
	 void bypassProcess(ContextNR& c, bypassParams)
	 {
		 c.bypassProcess();
		 ContextR c2(c.getStorage(), c.getOutput());
		 bypassProcess(c2, bypassCall);
	 }


	 void sidechain(InputBlock& levels, ContextR& c) {process(c); }
	 void sidechain(InputBlock& levels, ContextNR& c) {process(c); }
 */
	 
public:
	InstrFXBase() : instrData(NULL), bypass(false) {}
	virtual ~InstrFXBase() {}
	void setInstrData(InstrData* i) {
		if (i)
		{
//			bufferPtr = &i->bufferPtr;
			instrData= i ;
		}
		else jassertfalse;
	}
	MidiBuffer& getMidiBuffer() {
		/*
		 If you're using FXProcessor, make sure HasInstr = true
		 */
		if (!instrData || !(instrData->bufferPtr))
		{//Safety, because for inscrutiable reasons opening logic pro made this fail
			//Happened twice
			//Giving up, removing the jasserts
//			jassertfalse;
			dummyBuffer.clear();
			return dummyBuffer;
		}
		return *instrData->bufferPtr;
	}
	bool isBypassed() { return bypass; }
	void setBypass(bool set, bool allowTail) {
		if (bypass != set)
		{
			bypass=set;
			if (bypass)
			{
//				if (allowTail)
					allNotesOff();
//				else
//					allSoundsOff();
			}
		}
	}
	
	void prepare(Spec& spec) {
		rate = spec.sampleRate;
		prepare2(spec);
	}

	bool hasBeenPrepared() {return rate > 0;}

	virtual void prepare2(Spec& spec)=0;
	
	virtual void allNotesOff() = 0;
	virtual void allSoundsOff()= 0;
	
	void reset() {
		allSoundsOff();
	}

protected:
	static MidiBuffer dummyBuffer;
private:
//	MidiBuffer** bufferPtr;
	InstrData* instrData;
protected:
	bool bypass;
	float rate;

	
};




#endif /* InstrFXBase_h */
