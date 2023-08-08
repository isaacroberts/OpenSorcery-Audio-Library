//
//  RTDeEsser.h
//  
//
//  Created by Isaac Roberts on 9/2/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;



#if 0

#include "FX.h"
#include "Filters/RTButterworth.h"
#include "RTCompressor.h"

/*
	Behavior changed when filter class did

	Abandoned - can be fixed quite easily

 */



class RTDeEsser : public FX
{
public:
	RTDeEsser();
	
	void prepare2(Spec& spec) override;
	void process(ContextR&) ;
	void process(ContextNR& context) ;
	void reset2() override;
	
	//Getters & Setters
	int paramAmt() override { return 8; }
//	inline int bandCt() {return bandCt;}
	
	enum Params {
		Freq, Order, Threshold, Ratio, Gain, Attack, Release, KneeWidth
	};

	void setParameters(float freq, float q, float t, float r, float gain, float att, float rel, float knee) override;
	
	void set(int n, float set) override;
	float get(int n) override;

//	void setBandAmt(int set);
//	void addBand(float peak, float q, )
	
	virtual ~RTDeEsser();
	
	
protected:
	//Replace with LinkwitzRiley
	RTButterHighPass filter;
	RTButterHighShelf duck;
	
	//Replace with RTSideCompressor with sidechain function of normal compressor
	//Fill cut-block with 1's to get level
	RTSideCompressor compressor;
		
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTDeEsser);

};

#endif
