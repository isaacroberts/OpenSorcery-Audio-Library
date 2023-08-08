//
//  AirWindowsDeEsser.h
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/6/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "FX.h"

using namespace juce;

class AirWindowsDeEsser : public FX
{
public:
	AirWindowsDeEsser();
	
	void prepare2(Spec& spec) override;
	
	void commonProc(InputBlock& i, OutputBlock& b, BlockStorage& bs);

	void process (ContextR& p)  {
		fx_proc_start(p);
		commonProc(p.getOutput(), p.getOutput(), p.getStorage());
	}
	void process(ContextNR& p)  {
		fx_proc_start(p);
		commonProc(p.getInput(), p.getOutput(), p.getStorage());
	}
	
	template <class Context>
	void bypassProcess(Context& c, bypassParams) {
		jassertfalse;
	}
	
	enum AirWindowsParam : int {
		Intensity=0, MaxDess, Frequency
	};
	
	int paramAmt() override { return 3; }
	
	float get(int n) override {
		if (n==Intensity)
			return input_intensity;
		if (n==MaxDess)
			return input_maxdess;
		if (n==Frequency)
			return frequency;
		else return extraGet();
	}
	void set(int n, float value) override {
		if (n==Intensity)
			input_intensity = value;
		else if (n==MaxDess)
			input_maxdess = value;
		else if (n==Frequency)
			frequency = value;
	}
	
	virtual ~AirWindowsDeEsser();
	
	
protected:
	int channels;
	double ** sa=NULL;
	const int s_ct = 3;
	double ** ma=NULL;
	const int m_ct = 2;
	
	double *ratio=NULL;
	double *iirSample=NULL;
	
	float input_intensity, input_maxdess, frequency;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AirWindowsDeEsser);

};
