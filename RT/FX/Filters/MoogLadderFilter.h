//
//  MoogLadderFilter.h
//  
//
//  Created by Isaac Roberts on 7/17/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "../FX.h"
#include "Moog/LadderFilterBase.h"

using namespace juce;
/*
 
 This class only produces low-passes
 
 
 */
class MoogLadderFilter : public FX
{
public:
	MoogLadderFilter();
	
	
	//Override
//	//FX Stuff
	void prepare2(Spec& spec) override;
	void process (ContextR&) ;
	void process(ContextNR&) ;
	void reset2() override;
	
	template <class Context>
	void bypassProcess(Context& c, bypassParams) {
//		reset2();
	}
	
	enum MoogParams {
		Freq, Resonance
	};
	
//	//Getters & Setters
	int paramAmt() override { return 2; }
	void set(int n, float set) override {
		if (n==Freq)
			setFreq(set);
		else if (n==Resonance)
			setResonance(set);
	}
	float get(int n) override {
		if (n==Freq)
			return getFreq();
		else if (n==Resonance)
			return getResonance();
		else return extraGet();
	}
	
	void setFreq(float set);
	void setResonance(float set);
	
	float getFreq();
	float getResonance();
	
	virtual ~MoogLadderFilter();
	
protected:
	LadderFilterBase** filter;
	int C;
	float freq, resonance;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MoogLadderFilter);

};
