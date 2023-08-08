//
//  RBJ.h
//  
//
//  Created by Isaac Roberts on 2/23/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include <math.h>

#include "Coeff.h"
#include "Design.h"
#include "State.h"

#include "../../FX.h"

//#include "Coeff.h"

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

namespace RBJ
{

	template <class Design, class Form=DirectForm2>
	class Fixed : public FX
	{
	public:
		Fixed()
		: FX(),
		coeff(),
		state()
		{
			coeff.bypass();
		}

	//	//FX Stuff
		void prepare2(Spec& spec) override {
			state.setNumChannels(spec.numChannels);
			design.setRate(spec.sampleRate);
			//Yes this is compensating for lazy programming
			design.setFreq(design.getFreq());
			design.setQBWS(design.getQBWS());
			design.setGain(design.getGain());

			design.make(coeff);
		}
		void process (ContextR& c) {
			fx_proc_start(c);
			if (design.needsUpdate())
				design.make(coeff);
			if (coeff.status==Bypass)
				return;
			if (coeff.status==Silence)
			{
				c.getOutput().clear();
				return;
			}
			state.process(c.getInput(), c.getOutput(), coeff);
		}
		void process(ContextNR& c) {
			fx_proc_start(c);
			if (design.needsUpdate())
				design.make(coeff);
			
			if (coeff.status==Bypass)
			{
				c.bypassProcess();
				return;
			}
			if (coeff.status==Silence)
			{
				c.getOutput().clear();
				return;
			}
			state.process(c.getInput(), c.getOutput(), coeff);
		}
		template <class Context>
		void bypassProcess(Context& c, bypassParams) {
			state.reset();
		}
		void sidechain(InputBlock& levels, ContextR& c) {process(c); }
		void sidechain(InputBlock& levels, ContextNR& c) {process(c); }

		////  void bypassProcess(ContextR& c, bypassParams);
		////  void bypassProcess(ContextNR& c, bypassParams);

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
		void setFreq(double set) {
			design.setFreq(set);
			remakeFilter();
		}
		void setQ(double set) {
			design.setQBWS(set);
			remakeFilter();
		}
		void setGain(double set) {
			design.setGain(set);
			remakeFilter();
		}
		void setResonance(double set) {
			design.setResonance(set);
			remakeFilter();
		}
		
		void setBW(float set) { setQ(set);}
		void setBandwidth(float set) { setQ(set);}
		void setSlope(float set) {setQ(set);}
		
		void setParameters(float f, float q) {
			design.setFreq(f);
			design.setQBWS(q);
			remakeFilter();
		}
		void setParameters(float f, float q, float g) {
			design.setFreq(f);
			design.setQBWS(q);
			design.setGain(g);
			remakeFilter();
		}
		
		~Fixed() {}
		
	protected:
		Type type() { return design.getType(); }
		Status status() { return design.getStatus(); }
		
	protected:
		Coeff coeff;
		Design design;
		State<Form> state;

	protected:
		void remakeFilter() {
//			design.make(coeff);
		}
	public:
		void sanCheck();
		
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Fixed);
	};

}

