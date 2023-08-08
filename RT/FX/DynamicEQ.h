//
//  DynamicEQ.h
//  
//
//  Created by Isaac Roberts on 8/27/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;


#if 0

#include "FX.h"
//#include "Filters/RTFilters.h"
#include "RTCompressor.h"

/*
	Behavior changed when filter class did

	Abandoned - can be fixed quite easily

 */

class DynamicEQ : public FX
{
public:
	DynamicEQ();
	
	void prepare2(Spec& spec) override;
	void process (ContextR&) ;
	void process(ContextNR&) ;
	void reset2() override;
	
	//Getters & Setters
	int paramAmt() override;
	inline int paramPerBand() { return 9;}
//	inline int bandCt() {return bandCt;}
	
	enum ParamPerBand {
		Freq, Q, Threshold, Ratio, Attack, Release, KneeWidth, PreGain, PostGain
	};
	
	void setBand(int b, float f, float q, float thresh, float ratio, float attack, float release, float knee, float pregain, float postgain);
	void setBand(int b, float thresh, float ratio, float attack, float release, float knee, float pregain, float postgain);
	
	void set(int b, int n, float set);
	float get(int b, int n);
	
	void set(int n, float set) override;
	float get(int n) override;

	void setBands(int amt, float* freqs, float* qs);
	
//	void setBandAmt(int set);
//	void addBand(float peak, float q, )
	
	virtual ~DynamicEQ();
	
	
protected:
	struct Band {
		Band()
		: filter(), duck(), compressor()
		{}
		RTPeak filter;
		RTNotch duck;
		//Replace with RTSideCompressor with sidechain function of normal compressor
		//Fill cut-block with 1's to get level
		RTSideCompressor compressor;
	};
	
	Band* band;
	
	int bandCt;
//	float* freqs;
	
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DynamicEQ);

};

#endif
