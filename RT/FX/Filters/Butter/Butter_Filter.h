//
//  FlatButter.h
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/21/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

// Created by Isaac Roberts 

#ifndef FlatButter_h
#define FlatButter_h

#include <stdio.h>
#include <vector>

#include "Params.h"
#include "DirectFormI.h"
#include "Cascade.h"
#include "SmoothCascade.h"

#include "Biquad.h"

#include "State.h"

#include <JuceHeader.h>

namespace Filter {
namespace Butter {

typedef DirectFormI StateType;

template <bool CanSmooth>
struct GeneralCascade {};

//using namespace Filter;

template<>
struct GeneralCascade<true> : public SmoothCascade
{
	GeneralCascade(int o, Type t) : SmoothCascade(o, t) {}
};

template<>
struct GeneralCascade<false> : public Cascade
{
	GeneralCascade(int o, Type t) : Cascade(o, t) {}
};


template <bool CanSmooth=true>
class Butter_Filter
{
	
public:
	//TODO: Add interface for multiple audio streams to use the same filter by adding extra states
	Butter_Filter(int order, Type type)
	: states(NULL),
	m_design(order, type),
	numChannels(0)
	{
	}
	
	int getNumChannels() {
		return numChannels;
	}
	int getMaxOrder() {
		return m_design.getMaxOrder();
	}
	void setChannelAmt(int set)  {
		if (numChannels != set)
		{
			numChannels = set;
			
			int maxStages = m_design.getDigitalStages();
			
			auto states2 = new State[numChannels];
			for (int n=0; n < numChannels; ++n)
			{
				states2[n].setMaxStages(maxStages);
			}
			//Thread safer
			states = states2;
		}
	}
	
	virtual ~Butter_Filter() {
		delete[] states;
	}
	
	void process (int numSamples, float* const* arrayOfChannels)
	{
		_process (numSamples, arrayOfChannels);
	}
	
	void process (int numSamples, double* const* arrayOfChannels)
	{
		_process (numSamples, arrayOfChannels);
	}
	
	void process (int numSamples, int c, float* array)
	{
		_process (numSamples, c, array);
	}
	float process (float x, int c)
	{
		_process (1, c, &x);
		return x;
	}
	float process (float x)
	{
		jassert(numChannels==1);
		_process (1, 0, &x);
		return x;
	}

	void process(const juce::dsp::AudioBlock<const float>& in, juce::dsp::AudioBlock<float>& out)
	{
		jassert(numChannels == (int)in.getNumChannels() );
		jassert((int)in.getNumChannels() == (int)out.getNumChannels());
		m_design.process(in, out, states);
	}
private:
	//Pulled from ChannelsState
	template <typename Sample>
	void _process (int numSamples,
				   Sample* const* arrayOfChannels)
	{
		for (int i = 0; i < numChannels; ++i)
		{
			m_design.process (numSamples, arrayOfChannels[i], states[i]);
		}
	}
	
	void _process (int numSamples, int c,
				   double* array)
	{
		m_design.process (numSamples, array, states[c]);
	}
	
	void _process (int numSamples, int c,
				   float* array)
	{
		m_design.process (numSamples, array, states[c]);
	}
public:
	void snapToTargets()
	{
		m_design.snapToTargets();
	}
	void setUsesSmoothing() {
		m_design.setUsesSmoothing();
	}
	//Also ChannelState
	void reset ()
	{
		for (int i = 0; i < numChannels; ++i)
		  states[i].reset();
	}
	
	int getNumParams () const
	{
	  return m_design.numParams();
	}

	complex_t response (double normalizedFrequency) const
	{
	  return m_design.response (normalizedFrequency);
	}

public:
	inline float getRate() { return m_design.getRate(); }
	inline float getFreq() { return m_design.getFreq(); }
	inline int getOrder() { return m_design.getOrder(); }
	inline float getBandwidth() { return m_design.getBandwidth(); }
	inline float getGain() { return m_design.getGain(); }
	
	inline void setParams(float rate, int order, float freq, float bw, float gain) {
		m_design.setParams(rate, order, freq, bw, gain);
	}
	inline void setParams(float rate, int order, float freq) {
		m_design.setParams(rate, order, freq);
	}
	inline void setFreq(float freq) {m_design.setFreq(freq); }
	inline void setRate(float rate) {m_design.setRate(rate); }
	inline void setOrder(int order) {m_design.setOrder(order); }
	inline void setBandwidth(float bw) {m_design.setBandwidth(bw); }
	inline void setGain(float g) {m_design.setGain(g); }
	
protected:
	State* states;
	GeneralCascade<CanSmooth> m_design;
	int numChannels;
//	int maxOrder;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Butter_Filter);

};

}}

#endif /* FlatButter_h */
