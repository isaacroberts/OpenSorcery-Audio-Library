//
//  DummyFX.h
//  
//
//  Created by Isaac Roberts on 2/1/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "FX.h"

using namespace juce;

class DummyNoFX
{
	/**
		This dummy class can be used to fill space on a preset chain without taking up any variable space.
		Unlike the DummyFX class, it does not derive from FX so it won't waste any time with virtual functions or FX member variables
	 */
public:
	DummyNoFX() : hasPrepared(false) {}
	
	void attachMixer() {}
	
	//Abstract Gettors & Setters
	void set(int n, float set) {}
	float get(int n) {return 0;}
	
	int paramAmt() {return 0;}
	
	//Getters & Setters
	void setBypass(bool set) {}
	
	void setMix(float set) {}
	
	void bypassProcess(ContextR& c, bypassParams) {}
	void bypassProcess(ContextNR& c, bypassParams) {}
	
	void sidechain(InputBlock& levels, ContextR& c) {}
	void sidechain(InputBlock& levels, ContextNR& c) {c.bypassProcess(); }
	
	void prepare(Spec& spec) { hasPrepared=true;}
	void process(ContextR& ) {}
	void process(ContextNR& );
	void process(InputBlock& from, OutputBlock& to, BlockStorage& b);
	void process(OutputBlock&, BlockStorage& b) {}
	void reset() {}
	
	void setInstrData(InstrData* instrData) {}
	
	bool hasBeenPrepared() {return hasPrepared;}
	
	virtual ~DummyNoFX() {}
	
	void setParameters() {}
private:
	//It occured to me that the hasBeenPrepared function could cause problems if not giving accurate results
	//So, this accurately stores whether the Dummy has been prepared
	bool hasPrepared=false;
	
	
	DummyNoFX (const DummyNoFX&) = delete;
	DummyNoFX& operator= (const DummyNoFX&) = delete;
	
	
	//For replacing specific objects
public:
	//In case of replacing filters
	void setParameters(float f, float o, float a, float bw) {}
	void setParameters(float f, float o, float a) {}
	void setParameters(float f, float o) {}
	inline void setFreq(float f) {}
	inline void setOrder(int o) {}
	inline void setAmt(float value) {}
	void setAmountOnly(float value) {}
	
	void setBW(float value) {}
	float getBW() { return 1;}
	
	int getMaxOrder() {return 1;}
};


typedef DummyNoFX Dummy;
