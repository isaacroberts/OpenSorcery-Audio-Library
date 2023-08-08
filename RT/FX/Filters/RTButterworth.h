//
//  RTButterworth.h
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/23/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "RT/FX/FX.h"
#include "FilterDefinitions.h"

#include "Butter/Types.h"

#include "Butter/Butter_Filter.h"

using namespace juce;

namespace Filter {
	namespace Butter {
		enum FilterParams {
			Frequency=0, Order, Bandwidth, Gain
		};
	}
}

template <bool CanSmooth=true>
class RTButterworth : public FX
{
public:
	RTButterworth(Filter::Type type) : RTButterworth(type, 440, 6) {}
	RTButterworth(Filter::Type type, float freq) : RTButterworth(type, freq, 6) {}
	RTButterworth(Filter::Type type, float freq, int order);
	
	void prepare2(Spec&)override;
	void reset2()override;
	float processSingle(float x);
	void process(ContextR& context);
	void process(ContextNR& context);
	
	void _skipProcess(int numSamples);
	
	void bypassProcess(ContextR& c, bypassParams) {
		_skipProcess(c.getNumSamples());
	}
	void bypassProcess(ContextNR& c, bypassParams) {
		_skipProcess(c.getNumSamples());
	}
		
	virtual ~RTButterworth();
	
	int paramAmt() override {
		if (filter)
			return filter->getNumParams()-1;
		
		else return 0;
	}
//	void setUsesFilterSmoothing() override {
//		getFilter()->setUsesSmoothing();
//	}

	float get(int n) override {
		if (n==Filter::Butter::Frequency)
			return freq;
		if (n==Filter::Butter::Order)
			return order;
		if (n==2)
		{
			if (Filter::Butter::thirdParamGain(type))
			{
				return gain;
			}
			else return bandwidth;
		}
		if (n==Filter::Butter::Gain)
			return gain;
		else return extraGet();
	}
	virtual void set(int n, float value) override
	{
		if (n==Filter::Butter::Frequency)
			setFreq(value);
		else if (n==Filter::Butter::Order)
			setOrder(value);
		else if (n==2)
		{
			if (Filter::Butter::thirdParamGain(type))
			{
				jassertfalse;
				setGain(value);
			}
			setBandwidth(value);
		}
		else if (n==Filter::Butter::Gain)
			setGain(value);
	}
	
	void snap() {
		if (filter) {
			auto f = filter;
			f->snapToTargets();
		}
	}
	void snapToTargets() { snap(); }
	
	void setParameters(float f) {
		setFreq(f);
	}
	void setParameters(float f, float o) {
		freq = f;
		order = o;
		setP();
	}
	
	void setParameters(float f, float o, float bwOrG) {
		freq = f;
		order = o;
		if (Filter::Butter::thirdParamGain(type))
		{
			gain = bwOrG;
		}
		else bandwidth = bwOrG;
		setP();
	}
	
	void setParameters(float f, float o, float bw, float g) {
		freq = f;
		bandwidth = bw;
		gain = g;
		order = o;
		setP();
	}
	
	inline void setFreq(float f) {
		if (freq != f)
		{
			freq = f;
			if (goodToSet())
				getFilter()->setFreq(freq);
		}
	}
	inline void setOrder(int o) {
		order = o;
		setP();
	}
	
	inline void setBandwidth(float bw) {
		bandwidth = bw;
		if (goodToSet())
			getFilter()->setBandwidth(bandwidth);
	}
	inline void setGain(float g) {
		gain = g;
		if (goodToSet())
			getFilter()->setGain(gain);
	}
	
	
protected:
	void setMode()
	{
		if (rate <= 0)
		{
			mode = Filter::Unprepared;
		}
		else
		{
			if (freq >= rate/2)
			{
				mode = Filter::modeAboveNyquist(type);
			}
			else if (freq <= 0)
			{
				mode = Filter::modeBelowZero(type);
			}
			else
			{
				mode = Filter::Active;
			}
		}
	}
	
	virtual void runChecks() {}
	
	bool goodToSet() {
		runChecks();
		setMode();
		return (filter && rate > 0 && mode == Filter::Active);
	}
	bool oobOrder() {
		//Lower-bounds checking order lets us keep ridiculous amounts of stages from taking up memory
		//But doing half makes sure we don't have to remake the filter every time
		return ((order > filter->getMaxOrder()) || (order < filter->getMaxOrder() / 2));
	}
	void makeFilter() {
		if (rate > 0)
		{
			runChecks();
			setMode();
			
			auto f = std::make_shared< Filter::Butter::Butter_Filter<CanSmooth>>
						(order, getType());
			
			if (mode == Filter::Active)
			{
				f->setParams(rate, order, freq, bandwidth, gain);
				f->snapToTargets();
			}
			if (channels > 0)
			{
				f->setChannelAmt(channels);
			}
			filter = f;
		}
	}
	
	void setP()
	{
		if (filter)
		{
			if (rate > 0)
			{
				runChecks();
				setMode();
				
				if (mode == Filter::Active)
				{
					if (oobOrder())
					{
						makeFilter();
					}
					else
					{
						getFilter()->setParams(rate, order, freq, bandwidth, gain);
					}
				}
			}
		}
	}
	
	//Using this function is more thread safe
	std::shared_ptr<Filter::Butter::Butter_Filter<CanSmooth>> getFilter() { return filter; }
	
	Filter::Type getType() { return type; }
	
protected:
	std::shared_ptr<Filter::Butter::Butter_Filter<CanSmooth>> filter;
	Filter::Type type;
	int channels;
	int order;
	float freq;
	float bandwidth;
	float gain;
	
	bool hasReset=false;
		
	Filter::Mode mode;

	std::mutex lock;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTButterworth);
	
};

// LowPass, HighPass, BandPass, BandStop, LowShelf, HighShelf, BandShelf


template <bool CanSmooth=true>
class RTButterLowPass : public RTButterworth<CanSmooth> {
public:
	RTButterLowPass() : RTButterworth<CanSmooth>(Filter::LowPass) {}
	RTButterLowPass(float f) : RTButterworth<CanSmooth>(Filter::LowPass, f) {}
	RTButterLowPass(float f, int order) : RTButterworth<CanSmooth>(Filter::LowPass, f, order) {}
};

template <bool CanSmooth=true>
class RTButterHighPass : public RTButterworth<CanSmooth> {
public:
	RTButterHighPass() : RTButterworth<CanSmooth>(Filter::HighPass) {}
	RTButterHighPass(float f) : RTButterworth<CanSmooth>(Filter::HighPass, f) {}
	RTButterHighPass(float f, int order) : RTButterworth<CanSmooth>(Filter::HighPass, f, order) {}
};

template <bool CanSmooth=true>
class RTButterBandPass : public RTButterworth<CanSmooth> {
public:
	RTButterBandPass() : RTButterworth<CanSmooth>(Filter::BandPass) {}
	RTButterBandPass(float f) : RTButterworth<CanSmooth>(Filter::BandPass, f) {}
	RTButterBandPass(float f, int order) : RTButterworth<CanSmooth>(Filter::BandPass, f, order) {}
};

template <bool CanSmooth=true>
class RTButterBandStop : public RTButterworth<CanSmooth> {
public:
	RTButterBandStop() : RTButterworth<CanSmooth>(Filter::BandStop) {}
	RTButterBandStop(float f) : RTButterworth<CanSmooth>(Filter::BandStop, f) {}
	RTButterBandStop(float f, int order) : RTButterworth<CanSmooth>(Filter::BandStop, f, order) {}
};

template <bool CanSmooth=true>
class RTButterLowShelf : public RTButterworth<CanSmooth> {
public:
	RTButterLowShelf() : RTButterworth<CanSmooth>(Filter::LowShelf) {}
	RTButterLowShelf(float f) : RTButterworth<CanSmooth>(Filter::LowShelf, f) {}
	RTButterLowShelf(float f, int order) : RTButterworth<CanSmooth>(Filter::LowShelf, f, order) {}
};

template <bool CanSmooth=true>
class RTButterHighShelf : public RTButterworth<CanSmooth> {
public:
	RTButterHighShelf() : RTButterworth<CanSmooth>(Filter::HighShelf) {}
	RTButterHighShelf(float f) : RTButterworth<CanSmooth>(Filter::HighShelf, f) {}
	RTButterHighShelf(float f, int order) : RTButterworth<CanSmooth>(Filter::HighShelf, f, order) {}
};

template <bool CanSmooth=true>
class RTButterBandShelf : public RTButterworth<CanSmooth> {
public:
	RTButterBandShelf() : RTButterworth<CanSmooth>(Filter::BandShelf) {}
	RTButterBandShelf(float f) : RTButterworth<CanSmooth>(Filter::BandShelf, f) {}
	RTButterBandShelf(float f, int order) : RTButterworth<CanSmooth>(Filter::BandShelf, f, order) {}
};

//typedef RTButterworth<true> RTButterworth;
typedef RTButterworth<false> RTButterworthNoSmooth;

//typedef RTButterLowPass<true> RTButterLowPass;
//typedef RTButterHighPass<true> RTButterHighPass;
//typedef RTButterBandPass<true> RTButterBandPass;
//typedef RTButterBandStop<true> RTButterBandStop;
//typedef RTButterLowShelf<true> RTButterLowShelf;
//typedef RTButterHighShelf<true> RTButterHighShelf;
//typedef RTButterBandShelf<true> RTButterBandShelf;

typedef RTButterLowPass<false> RTButterLowPassNoSmooth;
typedef RTButterHighPass<false> RTButterHighPassNoSmooth;
typedef RTButterBandPass<false> RTButterBandPassNoSmooth;
typedef RTButterBandStop<false> RTButterBandStopNoSmooth;
typedef RTButterLowShelf<false> RTButterLowShelfNoSmooth;
typedef RTButterHighShelf<false> RTButterHighShelfNoSmooth;
typedef RTButterBandShelf<false> RTButterBandShelfNoSmooth;

template <bool CanSmooth> using RTLowPass  	= RTButterLowPass	<CanSmooth>;
template <bool CanSmooth> using RTHighPass 	= RTButterHighPass	<CanSmooth>;
template <bool CanSmooth> using RTBandPass 	= RTButterBandPass	<CanSmooth>;
template <bool CanSmooth> using RTBandStop	= RTButterBandStop	<CanSmooth>;
template <bool CanSmooth> using RTHighShelf = RTButterHighShelf	<CanSmooth>;
template <bool CanSmooth> using RTBandShelf = RTButterBandShelf	<CanSmooth>;
