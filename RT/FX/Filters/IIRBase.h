//
//  RTFilter.h
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/4/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "../FX.h"

//#include "VFFilters/VF.h"

#define defaultQ  .707107


//typedef dsp::IIR::Filter<float>::CoefficientsPtr DSPCoeff;
typedef IIRCoefficients DSPCoeff;

/*
 JUCE UPDATE:
 
 Goto: juce_audio_basics > utilities > juce_IIRFilter.h > line 58
 
 Add:
 
 static IIRCoefficients makeBypass() noexcept
{
	return IIRCoefficients(1, 0, 0, 1, 0, 0);
}

 */


class RT_IIR : public FX
{
public:
	RT_IIR();
	RT_IIR(float freq);
	RT_IIR(float freq, float q, float amt);
	
	void prepare2(Spec&)override;
	void reset2()override;
	void process(ContextR&) ;
	void process(ContextNR&) ;
	
	
	template <class Context>
	void bypassProcess(Context& c, bypassParams) {
		reset2();
	}
	
	enum FilterType {
		LowPass, HighPass, Peak, Notch, LowShelf, HighShelf, AllPass
	};
	
	virtual ~RT_IIR();
	
//	enum NotchParams : unsigned int;
	
	virtual int paramAmt() override {
		return 3;
	}
	
	enum FilterParam : int{
		Frequency=0, Q, Amt
	};
	
	float get(int n) override {
		if (n==Frequency)
			return frequency;
		if (n==Q)
			return q;
		if (n==Amt)
			return amt;
		else return extraGet();
	}
	
	virtual void notifyAmt(float prev) {}
	
	void setFreq(float value) {
		if (frequency != value)
		{
			frequency = value;
			if (frequency <= 0 )
				setBypass(true);
			else {
				_lockAndMakeFilter();
			}
		}
	}
	inline void setQ(float value) {
		q = value;
		_lockAndMakeFilter();
	}
	inline void setAmt(float value)
	{
		float prev = amt;
		amt = value;
		notifyAmt(prev);
	}
	virtual void setParameters(float f, float q) {
		setParameters(f, q, amt);
	}
	virtual void setParameters(float f, float q_, float a)
	{
		frequency =  f;
		q = q_;
		float prev = amt;
		amt = a;
		if (frequency<=0)
			setBypass(true);
		else
		{
			_lockAndMakeFilter();
			notifyAmt(prev);
		}
	}
	virtual void set(int n, float value) override
	{
		if (n==Frequency) {
			setFreq(value);
		}
		else if (n==Q) {
			if (q != value) {
				setQ(value);
			}
		}
		else if (n==Amt)
		{
			setAmt(value);
		}
	}
	
	

protected:
	
//	IIRCoefficients coefficients;
	IIRFilter* filter;

//	dsp::IIR::Filter<float>* filter;
//	IIR::Filter<float>::CoefficientsPtr coeff;

	
//	std::mutex filterLock;
	virtual bool needsOrigBlock() { return false; }
	virtual void handleAmt(InputBlock&, OutputBlock&) {} 
	
	virtual DSPCoeff makeFilter() = 0;
//	virtual IIRCoefficients makeIIRFilter() = 0;
	void _lockAndMakeFilter();
	
	float frequency, q, amt;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RT_IIR);
	
	int channels;
};
