
#pragma once

#include "Shared.h"

//#include "StdFX.h"
#include "OnelinerFX.h"
#include "FX.h"

/*
class Template
{
 protected:
	 float rate=0;
	 bool bypass=false;
public:
	Template() {}
			
	//Getters & Setters
	void setBypass(bool set) {bypass = set;}
	bool isBypassed() {return bypass;}

	void prepare(Spec& spec) {
		rate = spec.sampleRate;
	}
 
	void process(ContextR& c);
	void process(ContextNR& c);

	//Not necessary but convenient
	bool hasBeenPrepared() {return rate > 0;}

	void bypassProcess(ContextR& c, bool allowTail, bool isInputSilent) {}
	void bypassProcess(ContextNR& c, bool allowTail, bool isInputSilent) {}

	void sidechain(InputBlock& levels, ContextR& c) {process(c); }
	void sidechain(InputBlock& levels, ContextNR& c) {process(c); }

	void reset() {}
	void setInstrData(InstrData* instrData) {}
		
	~Template() {}
private:
	Template (const Template&) = delete;
	Template& operator= (const Template&) = delete;
};
*/
