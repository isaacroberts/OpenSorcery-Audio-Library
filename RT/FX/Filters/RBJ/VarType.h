//
//  VarType.h
//  
//
//  Created by Isaac Roberts on 2/23/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include <math.h>

#include "../../FX.h"

#include "Coeff.h"
#include "State.h"
#include "Design.h"

/*
 Q/BW/S parameter:
	 LowPass, HighPass, AllPass:
		 Q is the classical EE definition
	 PeakingEQ:
		A*Q is the classical EE definition of Q where A is square root of amplitude {pow(10, gain/40)}
	 BandPass1, BandPass2, Notch, PeakingEQ:
		 Bandwidth is in Octaves
	 LowShelf, HighShelf:
		 S is (0 and 1], where 0 is silence, .1 is gradual, and  1 is the maximum possible steepness.
 
 BandPass1 is constant skirt gain, peak gain = Q
 BandPass2 is constant db peak gain
 */

namespace RBJ {

class VarType : public FX
{
public:
	
	VarType();

//	//FX Stuff
	void prepare2(Spec& spec) override;
	void process (ContextR& c) 	{
		state.process(c.getOutputBlock(), c.getOutputBlock(), coeff);
	}
	void process(ContextNR& c) {
		state.process(c.getInputBlock(), c.getOutputBlock(), coeff);
	}
	float processSingle(float x, int c=0) {
		return state.processSingle(x, c, coeff);
	}
	void reset2() override {
		state.reset();
	}
	
//	//Getters & Setters
	int paramAmt() override { return 3; }
	void set(int n, float set) override {}
	float get(int n) override {return 1;}
	
public:
	void setType(Type set);
	
	void setParameters(float f, float q) ;
	void setParameters(float f, float q, float g) ;
	
	void setFreq(float set);
	
	void setQ(float set);
	void setBW(float set) { setQ(set);}
	void setBandwidth(float set) { setQ(set);}
	void setSlope(float set) {setQ(set);}
	
	void setGain(float set);
	
	virtual ~VarType();
	
protected:
	double omega() {
		return 2 * M_PI * freq / rate;
	}
	double alpha(double omega);
	double alpha() {
		return alpha(omega());
	}
	
	
	
protected:
	Coeff coeff;
	State<> state;
	
	Type type;
	Status status;

	float freq;
	float qbws;//Stands for Q, BW, or S
	float gain;

	float eps = 1e-8;

protected:
	virtual void remakeFilter();
	bool checkStatus();
	void getCoeff(Coeff&);
	
public:
	void sanCheck() {
		state.sanCheck();
	}
	
protected:
	void rbjLowpassCoeff(Coeff&);
	void rbjHighpassCoeff(Coeff&);
	void rbjBp1Coeff(Coeff&);
	void rbjBp2Coeff(Coeff&);
	void rbjNotchCoeff(Coeff&);
	void rbjApCoeff(Coeff&);
	void rbjPeakCoeff(Coeff&);
	void rbjLowShelfCoeff(Coeff&);
	void rbjHighShelfCoeff(Coeff&);
	
	inline bool usesBW ()
		{ return type == BandPass1_T || type == BandPass2_T || type == Notch_T || type == PeakingEQ_T; }
	inline bool usesS ()
		{ return type == LowShelf_T || type == HighShelf_T; }
	inline bool usesQ ()
		{ return type == LowPass_T || type == HighPass_T || type == AllPass_T; }
	
	
	
private:
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VarType);

};

}
