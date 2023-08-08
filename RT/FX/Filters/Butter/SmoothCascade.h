//
//  SmoothCascade.h
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/24/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

// Created by Isaac Roberts 

#ifndef SmoothCascade_h
#define SmoothCascade_h

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

class SmoothCascade
{
	friend class State;
public:
	SmoothCascade (int maxOrder_, Type type_)
		:
		type(type_),
		maxOrder(maxOrder_),
//		maxStages((maxOrder+1)/2),
		m_numStages (0),
		digitalPoles(maxOrder * (Filter::isBand(type) ? 2 : 1 )),
		digitalStages((digitalPoles+1)/2),
		m_stages (new Biquad[digitalStages]),
		m_params(),
		useSmoothing(true),
		isSmoothing(false),
		target_params(),
		m_digitalProto(maxOrder),
		m_analogProto(type, maxOrder)
	
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
			incrementParams(1);
			*dest = state.process (*dest, *this);
			dest++;
		}
		state.boundsCheck();
	}
	void process (int numSamples, double* dest, State& state)
	{
		while (--numSamples >= 0)
		{
			incrementParams(1);
			*dest = state.process (*dest, *this);
			dest++;
		}
		state.boundsCheck();
	}
	void processInterleaved(int numSamples, int numChannels, float* const* data, State* states)
	{
		for (int n=0; n < numSamples; ++n)
		{
			incrementParams(1);
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
	
	static const int updateRate = 8;
	
	void process(const juce::dsp::AudioBlock<const float>& in, juce::dsp::AudioBlock<float>& out, State* states)
	{
		size_t numChannels = in.getNumChannels();
		size_t numSamples = in.getNumSamples();
		if (isSmoothing)
		{
			//allows smoothing across channels
			for (int n=0; n < numSamples; ++n)
			{
				if (n % updateRate == 0)
				{
					//TODO: This will increment slightly faster if the block size is not divisible by 8,
					// 		or much significantly if the block size is less than 8
					//		However, the smoothing speed is arbitrary so this may not be a problem
					incrementParams(updateRate);
				}
				for (int c=0; c < numChannels; ++c)
				{
					out[c][n] = states[c].process(in[c][n], *this);
				}
			}
			for (int c=0; c < numChannels; ++c)
			{
				states[c].boundsCheck();
			}
		}
		else
		{
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
	}
	
protected:
	Type type;
	int maxOrder;
//	int maxStages;
	int m_numStages;
	int digitalPoles;
	int digitalStages;
	
	Biquad* m_stages;
	
	//Params = order, rate, freq, bandwidthHz, gainDb;
	
	Params m_params;
	bool useSmoothing=true;
	bool isSmoothing=false;
	
	const double smoothSpeed[5] {0, 0, 10, 100, 100}; //per Second
	
	Params target_params;
	
	//This one stores these two because stack allocations each audio frame is slowing things down
	LayoutBase m_digitalProto;
	AnalogPrototype m_analogProto;
	
public:
	
	enum Butter_Param_ID {
		Rate_, Order_, Freq_, Bandwidth_, Gain_
	};
	
	//Type-specific
	void setParams (const Params& params)
	{
		target_params = params;
		updateTargets();
	}
	void setParams(float rate, int order, float freq, float bw, float gain)
	{
		target_params[Rate_] = rate;
		target_params[Order_] = order;
		target_params[Freq_] = freq;
		target_params[Bandwidth_] = bw;
		target_params[Gain_] = gain;
		updateTargets();
	}
	void setParams(float rate, int order, float freq)
	{
		target_params[Rate_] = rate;
		target_params[Order_] = order;
		target_params[Freq_] = freq;
		updateTargets();
	}
	void setFreq(float freq)
	{
		target_params[Freq_] = freq;
		updateTargets();
	}
	void setRate(float rate) {
		target_params[Rate_] = rate;
	}
	void setOrder(int order) {
		target_params[Order_] = order;
		updateTargets();
	}
	void setBandwidth(float bw) {
		target_params[Bandwidth_] = bw;
		updateTargets();
	}
	void setGain(float g) {
		target_params[Gain_] = g;
		updateTargets();
	}
	float getRate() { return target_params[Rate_]; }
	float getFreq() { return target_params[Freq_]; }
	int getOrder() { return target_params[Order_]; }
	float getBandwidth() { return target_params[Bandwidth_]; }
	float getGain() { return target_params[Gain_]; }
	
	int numParams() const {
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
	
	void setUsesSmoothing() {
		//True by default
		useSmoothing = true;
	}
	void snapToTargets()
	{
		bool anyDiff = false;
		for (int n=0; n < Params::maxParameters; ++n)
		{
			if (m_params[n] != target_params[n] && usesParam(n))
			{
				anyDiff=true;
				break;
			}
		}
		if (anyDiff) {
			_snapToTargets();
		}
		isSmoothing=false;
	}
private:
	void updateTargets()
	{
		if (m_params[Rate_] <= 0 || !useSmoothing)
			_snapToTargets();
		else
			isSmoothing=true;
	}
	void _snapToTargets()
	{
		m_params = target_params;
		updateParams();
		
		isSmoothing=false;
	}
	
	void incrementFreq(int frames, bool& anyStillSmoothing);
	void incrementParams(int frames);
	
	bool usesParam(int n)
	{
		if (n<=2) return true;
		if (type==LowPass || type == HighPass) return false;
		if (type==BandShelf) return true;
		return ((n==Gain_) == (type == LowShelf || type==HighShelf));
	}
	
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
	
	void calc_stages (int order,
						 double sampleRate,
						 double cutoffFrequency,
						 double widthFrequency,
						 double gainDb);
	
	void setCascadeStorage (const Storage& storage);
	
	void applyScale (double scale);
	void setLayout (const LayoutBase& proto);
	
public:
	const Biquad& operator[] (int index)
	{
		assert (index >= 0 && index <= m_numStages);
		return m_stages[index];
	}
};

}}
	
#endif /* SmoothCascade_h */
