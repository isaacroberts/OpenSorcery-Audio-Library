//
//  Sanitizer.h
//  
//
//  Created by Isaac Roberts on 4/6/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include <math.h>

using namespace juce;

#ifndef SANITIZE_ON_BYPASS
#define SANITIZE_ON_BYPASS 1
#endif

#ifndef DEBUG_SAN
#define DEBUG_SAN 0
#endif 


#if DEBUG_SAN
#define SANTHROW jassertfalse;
#else
#define SANTHROW 
#endif


class SanitizerNoFX
{
	/**
		This dummy class can be used to fill space on a preset chain without taking up any variable space.
		Unlike the Sanitizer class, it does not derive from FX so it won't waste any time with virtual functions or FX member variables
	 */
	inline void _sanitize(OutputBlock& b) {
		for (int c=0; c < b.getNumChannels(); ++c)
		{
			auto* f = b.getChannelPointer(c);
			for (int n=0; n < b.getNumSamples(); ++n)
			{
				if (isnan(f[n])) 	  {SANTHROW; f[n] = 0;}
				else if (f[n] > maxLevel)  {SANTHROW; f[n] = maxLevel;}
				else if (f[n] < -maxLevel) {SANTHROW; f[n] = -maxLevel;}
			}
		}
	}
	inline void _copysan(InputBlock& input, OutputBlock& b) {
		for (int c=0; c < b.getNumChannels(); ++c)
		{
			auto* i = input.getChannelPointer(c);
			auto* f = b.getChannelPointer(c);
			for (int n=0; n < b.getNumSamples(); ++n)
			{
				if (isnan(i[n])) 		{SANTHROW; f[n] = 0;}
				else if (i[n] > maxLevel) 	{SANTHROW; f[n] = maxLevel;}
				else if (i[n] < -maxLevel) 	{SANTHROW; f[n] = -maxLevel;}
				else 					f[n] = i[n];
			}
		}
	}
	inline void _sanitizeBP(OutputBlock& b)
	{
#if SANITIZE_ON_BYPASS
		_sanitize(b);
#endif
	}
	inline void _sanitizeBP(InputBlock& i, OutputBlock& b)
	{
#if SANITIZE_ON_BYPASS
		_copysan(i, b);
#else
//		b.copyFrom(i);
#endif
	}
	inline void condSanitize(OutputBlock& b)
	{
		#if !SANITIZE_ON_BYPASS
		if (!bypass)
		#endif
			_sanitize(b);
	}
	inline void condSanitize(InputBlock& i, OutputBlock& b)
	{
		#if !SANITIZE_ON_BYPASS
		if (!bypass)
			_copysan(i, b);
		else
			b.copyFrom(i);
		#else
			_copysan(i, b);
		#endif
	}
public:
	
	SanitizerNoFX() : hasPrepared(false) {}
	
	
	float maxLevel = 10;

	void setMaxLevel(float amp) {
		maxLevel = amp;
	}
	void setMaxDb(float db) {
		maxLevel = pow(10., db/20.);
	}
	
	
	void attachMixer() {}
	
	//Abstract Gettors & Setters
	void set(int n, float set) {}
	float get(int n) {return 0;}
		
	//Getters & Setters
	void setBypass(bool set) {
#if !SANITIZE_ON_BYPASS
		bypass = set;
#endif
	}
	void setInstrData(InstrData* i) {}
	void setMix(float set) {}
	
	void sanitize(OutputBlock& b) { condSanitize(b); }
	void sanitize(ContextR& c) { condSanitize(c.getInput(), c.getOutput()); }
	void sanitize(ContextNR& c) { condSanitize(c.getInput(), c.getOutput()); }
	
	void startBypass() {}
	void endBypass() {}
	
	void bypassProcess(ContextR& c, bypassParams) { if (!inputIsSilent)  _sanitizeBP(c.getOutput()); }
	void bypassProcess(ContextNR& c, bypassParams) { if (!inputIsSilent) _sanitizeBP(c.getInput(), c.getOutput()); }
	
	void sidechain(InputBlock& l, ContextR& c) { condSanitize(c.getOutput()); }
	void sidechain(InputBlock& l, ContextNR& c) { condSanitize(c.getInput(), c.getOutput()); }
	
	void prepare(Spec& spec) { hasPrepared=true;}
	
	void process(ContextR& c) { condSanitize(c.getOutput()); }
	void process(ContextNR& c){ condSanitize(c.getInput(), c.getOutput()); }
	void process(InputBlock& from, OutputBlock& to, BlockStorage& b) { condSanitize(from, to); }
	void process(OutputBlock& o, BlockStorage& b) { condSanitize(o); }
	
	void reset() {}
	
	bool hasBeenPrepared() {return hasPrepared;}
	
	virtual ~SanitizerNoFX() {}
	
	void setParameters() {}
private:
	//It occured to me that the hasBeenPrepared function could cause problems if not giving accurate results
	//So, this accurately stores whether the Dummy has been prepared
	bool hasPrepared=false;
#if !SANITIZE_ON_BYPASS
	bool bypass=false;
#endif
	
	SanitizerNoFX (const SanitizerNoFX&) = delete;
	SanitizerNoFX& operator= (const SanitizerNoFX&) = delete;
};

typedef SanitizerNoFX Sanitizer;



class AssertQuiet
{
	/**
		This dummy class can be used to fill space on a preset chain without taking up any variable space.
		Unlike the Sanitizer class, it does not derive from FX so it won't waste any time with virtual functions or FX member variables
	 */
	void crash() {
		
	}
	
	inline void _sanitize(OutputBlock& b) {
		for (int c=0; c < b.getNumChannels(); ++c)
		{
			auto* f = b.getChannelPointer(c);
			for (int n=0; n < b.getNumSamples(); ++n)
			{
				if (isnan(f[n])) 	  {crash();
					
				}
				else if (f[n] > maxLevel)  {crash();}
				else if (f[n] < -maxLevel) {crash();}
			}
		}
	}
	inline void _copysan(InputBlock& input, OutputBlock& b) {
		for (int c=0; c < b.getNumChannels(); ++c)
		{
			auto* i = input.getChannelPointer(c);
			auto* f = b.getChannelPointer(c);
			for (int n=0; n < b.getNumSamples(); ++n)
			{
				if (isnan(i[n])) 		{crash(); }
				else if (i[n] > maxLevel) 	{crash(); }
				else if (i[n] < -maxLevel) 	{crash(); }
				f[n] = i[n];				
			}
		}
	}
	inline void _sanitizeBP(OutputBlock& b)
	{
#if SANITIZE_ON_BYPASS
		_sanitize(b);
#endif
	}
	inline void _sanitizeBP(InputBlock& i, OutputBlock& b)
	{
#if SANITIZE_ON_BYPASS
		_copysan(i, b);
#else
		b.copyFrom(i);
#endif
	}
	inline void condSanitize(OutputBlock& b)
	{
		#if !SANITIZE_ON_BYPASS
		if (!bypass)
		#endif
			_sanitize(b);
	}
	inline void condSanitize(InputBlock& i, OutputBlock& b)
	{
		#if !SANITIZE_ON_BYPASS
		if (!bypass)
			_copysan(i, b);
		else
			b.copyFrom(i);
		#else
			_copysan(i, b);
		#endif
	}
public:
	
	AssertQuiet() : hasPrepared(false) {}
	
	float maxLevel = 10;

	void setMaxLevel(float amp) {
		maxLevel = amp;
	}
	void setMaxDb(float db) {
		maxLevel = pow(10., db/20.);
	}
	
	void attachMixer() {}
	
	//Abstract Gettors & Setters
	void set(int n, float set) {}
	float get(int n) {return 0;}
		
	//Getters & Setters
	void setBypass(bool set) {
#if !SANITIZE_ON_BYPASS
		bypass = set;
#endif
	}
	void setInstrData(InstrData* i) {}
	void setMix(float set) {}
	
	void sanitize(OutputBlock& b) { condSanitize(b); }
	void sanitize(ContextR& c) { condSanitize(c.getInput(), c.getOutput()); }
	void sanitize(ContextNR& c) { condSanitize(c.getInput(), c.getOutput()); }
	
	void startBypass() {}
	void endBypass() {}
	
	void bypassProcess(ContextR& c, bypassParams) { _sanitizeBP(c.getOutput()); }
	void bypassProcess(ContextNR& c, bypassParams) {_sanitizeBP(c.getInput(), c.getOutput()); }
	
	void sidechain(InputBlock& l, ContextR& c) { condSanitize(c.getOutput()); }
	void sidechain(InputBlock& l, ContextNR& c) { condSanitize(c.getInput(), c.getOutput()); }
	
	void prepare(Spec& spec) { hasPrepared=true;}
	
	void process(ContextR& c) { condSanitize(c.getOutput()); }
	void process(ContextNR& c){ condSanitize(c.getInput(), c.getOutput()); }
	void process(InputBlock& from, OutputBlock& to, BlockStorage& b) { condSanitize(from, to); }
	void process(OutputBlock& o, BlockStorage& b) { condSanitize(o); }
	
	void reset() {}
	
	bool hasBeenPrepared() {return hasPrepared;}
	
	~AssertQuiet() {}
	
	void setParameters() {}
private:
	//It occured to me that the hasBeenPrepared function could cause problems if not giving accurate results
	//So, this accurately stores whether the Dummy has been prepared
	bool hasPrepared=false;
#if !SANITIZE_ON_BYPASS
	bool bypass=false;
#endif
	
	AssertQuiet (const AssertQuiet&) = delete;
	AssertQuiet& operator= (const AssertQuiet&) = delete;
};
