//
//  MinorPitchShifter.h
//  
//
//  Created by Isaac Roberts on 7/12/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "FX.h"
//#include "Filters/RTFilters.h"

class MinorPitchShifter : public FX
{
public:
	MinorPitchShifter();
	MinorPitchShifter(float shift);
	
	enum MinorPitchParams {
		Shift=0
	};
	int paramAmt() override {return 1;}
	void set(int n, float set) override {
		if (n==0)
			setShift(set);
//		else
//			assert(false);
	}
	float get(int n) override {
		if (n==0)
			return shiftSemi;
		else
			return extraGet();
	}
	
	void prepare2(Spec& spec) override;
	void commonProcess(InputBlock& i, OutputBlock& o, BlockStorage& b);
	void updatePtrs();
	
	void process(ContextR& );
	void process(ContextNR&);
	void reset2() override;

	
	template <class Context>
	void bypassProcess(Context& c, bypassParams) {
		//Actually clear buffer
		jassertfalse;
	}
	
	virtual ~MinorPitchShifter();
	
	void setShift(float set) {
		shiftSemi = set;
		updateShiftFactor();
	}
	float getShift() {return shiftSemi;}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MinorPitchShifter);

//	RTHighPass<> lowcut;
	
	void updateShiftFactor();
	
	inline float blockAccess(int c, int n) {
		n = n % blockSize;
		return buffer.getSample(c, n);
	}
	
	//parameters
	float shiftSemi;
//	float lowCutFreq;
	
	//resultant parameters
	float shiftFactor;
	int overlap;//determines how soon thing starts crossfading
	
	AudioBuffer<float> buffer;
	int blockSize;
	int writePtr;
	float readPtrA, readPtrB;
	enum Mode : char {
		FullA, FallingA, FullB, FallingB,
		Unloaded
	};
	Mode mode;
	float crossFade;
	
};
