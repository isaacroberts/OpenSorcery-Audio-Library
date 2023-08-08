//
//  LookaheadCompressor.h
//  
//
//  Created by Isaac Roberts on 9/14/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "../FX.h"
#include "CompressorCommon.h"

class LookaheadCompressor : public FX, public Compressors::Common
{
public:
	LookaheadCompressor();
	LookaheadCompressor(float threshold, float ratio, float attack, float release, float kneewidth=.2, float pregain=0, float postgain=0);
	
	void setMono(bool set);
	void setBufferSize(int set);
	
	void prepare2(Spec& spec) override;
	
	void process(ContextR& c);
	void process(ContextNR& c);
	
	void sidechain(InputBlock& l, ContextR& c);
	void sidechain(InputBlock& l, ContextNR& c);
	
	void bypassProcess(ContextR& c, bypassParams);
	void bypassProcess(ContextNR& c, bypassParams);
	
	virtual ~LookaheadCompressor();
	
	void setType(Compressors::ReadType set) {
		type = set;
	}
	
	inline float getCompressLevelFast() {
		return -dbRing.atRead()+pregain;
	}
	float getCompressLevelCertain() {
		return -dbRing.max() + pregain;
	}

protected:
	void commonProcess(InputBlock& l, InputBlock &i, OutputBlock& b, bool isRep);
	void _doBypass(InputBlock& input, OutputBlock& output);
	
	void processBand(InputBlock& level, OutputBlock& delayed,  BlockStorage& s);
	void skipBand(int n, ContextR& c);
	
	void advance(int n);
	void advance();
	
protected:
	RingBuffer audioRing;
	MonoRingBuffer dbRing;
	
	Compressors::ReadType type;
	bool mono;
	
	//Putting the derived class's functionality here to avoid duplicated code
	//because it's my last day 
	float gauge;

	bool useRMS() { return type == Compressors::RMS; }
	
	
	
public:
	int paramAmt() override {return 7;}
	
	//Getters & Setters
	void set(int n, float set) override
	{
		setCompressorVar(n, set);
	}
	float get(int n) override {
		return getCompressorVar(n);
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LookaheadCompressor);

};
