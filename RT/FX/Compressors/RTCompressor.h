//
//  RTCompressor.h
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/3/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "../FX.h"

#include "CompressorCommon.h"

using namespace juce;


/*
		1176 Compressor
			FET (Field Effect Transistor)
 
			Best for guitar and drums 
 
 */

class RTCompressor : public Compressors::Common {
public:
	RTCompressor()
	: Compressors::Common(0, .2, .003, .1, .2, 0, 0),
	type(Compressors::Feedforward)
	{
	}
	RTCompressor(float threshold, float ratio, float attack, float release, float kneewidth=.2, float pregain=0, float postgain=0)
	: Compressors::Common(threshold, ratio, attack, release, kneewidth, pregain, postgain),
	type(Compressors::Feedforward)
	{
	}

	void prepare(Spec& spec)  {
		rate = spec.sampleRate;
	}
	
	void commonProcess(InputBlock& l, InputBlock &i, OutputBlock& b) {
		compressorProcess(l, i, b, rate, type);
	}
	
	void process(ContextR& c)  {
		fx_proc_start(c);
		commonProcess(c.getInput(), c.getInput(), c.getOutput());
	}
	void process(ContextNR& c)  {
		fx_proc_start(c);
		commonProcess(c.getInput(), c.getInput(), c.getOutput());
	}
//	void process(InputBlock& from, OutputBlock& to, BlockStorage& b) {
//	}
//	void process(OutputBlock& b, BlockStorage& s) {
//	}

	
	template <class ContextType>
	void bypassProcess(ContextType& c, bypassParams) {
		if (inputIsSilent)
			releaseNFrames(c.getNumSamples(), rate);
		else
			compressorBypass(c.getInput(), rate, type);
	}
	
	void setBypass(bool set) {bypass = set;}
	bool isBypassed() {return bypass;}
	
	void sidechain(InputBlock& l, ContextR& c)  {
		commonProcess(l,  c.getInput(), c.getOutput());
	}
	void sidechain(InputBlock& l, ContextNR& c)  {
		commonProcess(l,  c.getInput(), c.getOutput());
	}
	
	virtual ~RTCompressor() {
		
	}
	
	void setType(Compressors::ReadType settype) {
		type = settype;
	}
	
	void setInstrData(InstrData* instrData) {}

	void reset()
	{
		resetState();
	}
	
protected:
	Compressors::ReadType type;
	float rate=69;
	bool bypass=false;
private:
	JUCE_DECLARE_NON_COPYABLE (RTCompressor);
};

