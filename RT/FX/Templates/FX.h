//
//  FX.h
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/3/20.
//

// Created by Isaac Roberts 

#pragma once

//#include <JuceHeader.h>

//#include "../Presets/Preset.h"
//#include "../InstrData.h"
//#include "../Mixer.h"
#include "Shared.h"

using namespace juce;
using namespace dsp;

#define DEBUG_MODE 0

class InstrData;

class FX
{
public:
	FX();
	
	/*
Override
	//FX Stuff
	void prepare2(Spec& spec) override;
	void process (ContextR&);
	void process (ContextNR&);
	template <class Context>
	void bypassProcess(Context& c, bypassParams) {
	}
//  void bypassProcess(ContextR& c, bypassParams);
//  void bypassProcess(ContextNR& c, bypassParams);
 	void sidechain(InputBlock& level, ContextR& c) { process(c); }
 	void sidechain(InputBlock& level, ContextNR& c) { process(c); }
	
	void reset2() override;
	
 */
	 
	
protected:
	//Override
	virtual void prepare2(Spec& spec) {}
//	virtual void commonProcess(InputBlock &i, OutputBlock& b) {}
//	virtual void processR(ContextR&)= 0;
//	virtual void processNR(ContextNR&)= 0;
	virtual void reset2() {}
	
//	virtual void sidechainR(InputBlock& levels, ContextR& c) { processR(c); }
//	virtual void sidechainNR(InputBlock& levels, ContextNR& c) { processNR(c); }
	
	
public:
//	virtual void sidechain(InputBlock& levels, ContextR& c) {
//		process(c);
//	}
//	virtual void sidechain(InputBlock& levels, ContextNR& c) {
//		process(c);
//	}
	
//	virtual void bypassProcess(ContextR& c, bypassParams) { }
//	virtual void bypassProcess(ContextNR& c, bypassParams) {}
	
	virtual void setInstrData(InstrData*) {}
	
public:
	//Abstract Gettors & Setters
	//TODO: Remove these fuckers 
	virtual void set(int n, float set) {}
	virtual float get(int n) {return 0;}
	
	virtual int paramAmt() {return 0; }
	
protected:
	inline float extraGet() {
		#ifdef DEBUG
			throw std::logic_error("Too many parameters in FX::get");
		#endif
		return 0;
	}
public:
	
	//Getters & Setters
	inline bool isBypassed() { return bypass; }
	inline bool getBypass() { return bypass; }
	void setBypass(bool set=true) {
		bypass = set;
	}
	//I added these and then realized I don't need them so make this virtual if you ever want to use them
	//Alerts FX when chain sets it to bypassed
//	void startBypass() {}
//	//Alerts FX when chain turns off its bypass
//	void endBypass() {}
	
//	virtual void setUsesFilterSmoothing() {}
	
//	virtual void setMix(float set) {
//		if (mixer)
//			mixer->setMix(set);
//	}
//	void setIntMix(int set) {
//		setMix(set/100.f);
//	}
	
	bool hasBeenPrepared() {return rate > 0;}

	void prepare(Spec& spec);
	void prepare(float rate, int channels, int blockSize);
//	void process(ContextR& );
//	void process(ContextNR& );
//	void process(InputBlock& from, OutputBlock& to, BlockStorage& b);
//	void process(OutputBlock&, BlockStorage& b);
	void reset();
	
//	void save(XmlElement* xml);
//	void load(XmlElement* xml);
//
//	virtual void extraSave(XmlElement* xml) {}
//	virtual void extraLoad(XmlElement* xml) {}
	
	virtual ~FX();
	
//	template <typename... Ts>
//	friend class Chain;
	
protected:
	
	bool isBypassed(ContextR& context);
	bool isBypassed(ContextNR&);
//	bool isBypassed(ContextR& context, bool condition);
//	bool isBypassed(ContextNR&, bool condition);

	
	bool bypass;
	
	float rate;
	
//	Mixer* mixer;

//	friend class Chain;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FX);
	
	//Long as list of setParameters(...)
public:
	
	
};
