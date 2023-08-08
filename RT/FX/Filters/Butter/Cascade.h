//
//  Cascade.h
//  MagicVocals
//
//  Created by Isaac Roberts on 3/21/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

// Created by Isaac Roberts 

#ifndef Cascade_h
#define Cascade_h

#include <stdio.h>

#include "State.h"
#include "Biquad.h"
#include "Params.h"
#include "LayoutBase.h"
#include "DirectFormI.h"
#include "MathSupplement.h"

#include "Transforms.h"

#include "Types.h"

#include <JuceHeader.h>

namespace Filter {
namespace Butter {
	
class Cascade 
{
	friend class State;
	
public:
	Cascade (int maxOrder_, Filter::Type type_)
		:
		type(type_),
		maxOrder(maxOrder_),
//		maxStages((maxOrder+1)/2),
		m_numStages (0),
		digitalPoles(maxOrder * (Filter::isBand(type) ? 2 : 1 )),
		digitalStages((digitalPoles+1)/2),
		m_stages (new Biquad[digitalStages]),
		m_params()
	{
	}
		
	
public:
	// Calculate filter response at the given normalized frequency.
	complex_t response (double normalizedFrequency) const;
	
	// Process a block of samples in the given form
	void process (int numSamples, float* dest, State& state)
	{
		while (--numSamples >= 0)
		{
			*dest = state.process (*dest, *this);
			dest++;
		}
		state.boundsCheck();
	}
	void process (int numSamples, double* dest, State& state)
	{
		while (--numSamples >= 0)
		{
			*dest = state.process (*dest, *this);
			dest++;
		}
		state.boundsCheck();
	}
	void processInterleaved(int numSamples, int numChannels, float* const* data, State* states)
	{
		for (int n=0; n < numSamples; ++n)
		{
			for (int c=0; c < numChannels; ++c)
			{
				data[c][n] = states[c].process(data[c][n], *this);
			}
		}
		for (int c=0; c < numChannels; ++c)
		{
			states[c].boundsCheck();
		}
	}
	void process(const juce::dsp::AudioBlock<const float>& in, juce::dsp::AudioBlock<float>& out, State* states)
	{
		size_t numChannels = in.getNumChannels();
		size_t numSamples = in.getNumSamples();
		
		//faster due to cache hits on channel arrays
		for (int c=0; c < numChannels; ++c)
		{
			auto* from = in.getChannelPointer(c);
			auto* dest = out.getChannelPointer(c);
			for (int n=0; n < numSamples; ++n)
			{
				dest[n] = states[c].process(from[n], *this);
			}
			states[c].boundsCheck();
		}
	}
	
	void setUsesSmoothing() {}
	
protected:
	Filter::Type type;
	int maxOrder;
//	int maxStages;
	int m_numStages;
	int digitalPoles;
	int digitalStages;
	
	Biquad* m_stages;
	
	//Params = order, rate, freq, bandwidthHz, gainDb;
	Params m_params;
public:
	
	enum Butter_Param_ID {
		Rate_, Order_, Freq_, Bandwidth_, Gain_
	};
	
	//Type-specific
	void setParams (const Params& params)
	{
		m_params = params;
		updateParams();
	}
	void setParams(float rate, int order, float freq, float bw, float gain)
	{
		m_params[Rate_] = rate;
		m_params[Order_] = order;
		m_params[Freq_] = freq;
		m_params[Bandwidth_] = bw;
		m_params[Gain_] = gain;
		updateParams();
	}
	void setParams(float rate, int order, float freq)
	{
		m_params[Rate_] = rate;
		m_params[Order_] = order;
		m_params[Freq_] = freq;
		updateParams();
	}
	void setFreq(float freq)
	{
		m_params[Freq_] = freq;
		updateParams();
	}
	void setRate(float rate) {
		m_params[Rate_] = rate;
	}
	void setOrder(int order) {
		m_params[Order_] = order;
		updateParams();
	}
	void setBandwidth(float bw) {
		m_params[Bandwidth_] = bw;
		updateParams();
	}
	void setGain(float g) {
		m_params[Gain_] = g;
		updateParams();
	}
	float getRate() { return m_params[Rate_]; }
	float getFreq() { return m_params[Freq_]; }
	int   getOrder() { return m_params[Order_]; }
	float getBandwidth() { return m_params[Bandwidth_]; }
	float getGain() { return m_params[Gain_]; }
	
	
	int numParams() const
	{
		return Filter::Butter::numParams(type);
	}
	
	int getDigitalStages() const {
		return digitalStages;
	}
	int getNumStages () const
	{
		return m_numStages;
	}
	int getMaxOrder() const {
		return maxOrder;
	}
	
	void snapToTargets() {}
	
private:
	void updateParams()
	{
		if (m_params[Rate_] <= 0)
			return;
		
		jassert (m_params[Order_] <= maxOrder);
		if (m_params[Order_] > maxOrder)
		{
			m_params[Order_] = maxOrder;
		}
		
		calc_stages(int(m_params[Order_]), m_params[Rate_], m_params[Freq_],
					m_params[Bandwidth_] * m_params[Freq_], m_params[Gain_]);
	}
	inline void incrementParams() {}
	inline void incrementParams(int n) {}


protected:
	void setCascadeStorage (const Storage& storage);
	
	void applyScale (double scale);
	void setLayout (const LayoutBase& proto);
	
	void calc_stages (int order,
						 double sampleRate,
						 double cutoffFrequency,
						 double widthFrequency,
						 double gainDb);
	
	const Biquad& operator[] (int index) {
		assert (index >= 0 && index <= m_numStages);
		return m_stages[index];
	}
};

}}

#endif /* Cascade_h */
