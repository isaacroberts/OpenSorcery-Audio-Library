

#pragma once

#include <JuceHeader.h>

#include <math.h>

#include "Coeff.h"
#include "Design.h"
#include "State.h"

#include "../../FX.h"

namespace RBJ
{
	/*
			Continuously exponentially approaches the target coefficients
	 
			Lo/Hi/All-Pass do not work great with this - they blow up at low frequency
	 */
	template <class Design, class Form=TransposedDirectForm2>
	class Semi : public FX
	{
	public:
		Semi()
		: FX(),
		coeff(),
		state(),
		fadeTime(.1)
		{
			forward.bypass();
			coeff.bypass();
			slope.silence();
		}
		void setFadeTime(double set)
		{
			fadeTime = set;
		}
		void setResolution(int framesBetweenAdds) {
			resolution = framesBetweenAdds;
		}
		void snap()
		{
			coeff = forward;
			samplesRemaining=0;
		}
		
	//	//FX Stuff
		void prepare2(Spec& spec) override {
			state.setNumChannels(spec.numChannels);
			
			design.setRate(spec.sampleRate);
			design.make(forward);
			
			snap();
		}
		void approach(int numSamples)
		{
			if (samplesRemaining<=0)
				return;
			if (numSamples==0)
				return;
			if (numSamples >= samplesRemaining)
			{
				coeff = forward;
				samplesRemaining=0;
			}
			else
			{
				coeff.add(slope, numSamples);
				samplesRemaining-=numSamples;
			}
		}
		void process (ContextR& c)  {
			fx_proc_start(c);
			
			auto block = c.getOutput();
			int N = (int)block.getNumSamples();
			int lastApp=0;
			
			for (int n=0; n < N; ++n)
			{
				if (n%resolution==0)
				{
					approach(n-lastApp);
					lastApp=n;
				}
				state.processAcrossChannels(block, block, n, coeff);
			}
			approach(N-lastApp);
			state.sanCheck();
		}
		void process(ContextNR& c)  {
			fx_proc_start(c);
			auto inputBlock  = c.getInputBlock();
			auto outputBlock = c.getOutputBlock();
			
			int N = (int)outputBlock.getNumSamples();
			int lastApp=0;
			for (int n=0; n < N; ++n)
			{
				if (n%resolution==0)
				{
					approach(n-lastApp);
					lastApp=n;
				}
				state.processAcrossChannels(inputBlock, outputBlock, n, coeff);
			}
			approach(N-lastApp);
			state.sanCheck();
		}
		
		template <class Context>
		void bypassProcess(Context& c, bypassParams) {
			approach(c.getNumSamples());
			state.reset();
		}
		void sidechain(InputBlock& level, ContextR& c) { process(c); }
		void sidechain(InputBlock& level, ContextNR& c) { process(c); }
		
		
		float processSingle(float x, int c=0) {
			approach(1);
			return state.processSingle(x, c, coeff);
		}
		void reset2() override {
			snap();
			state.reset();
		}
		
	//	//Getters & Setters
		int paramAmt() override { return 3; }
		void set(int n, float set) override {}
		float get(int n) override {return 1;}
		
	public:
		void setFreq(double set) {
			design.setFreq(set);
			makeFilter();
		}
		void setQ(double set) {
			design.setQBWS(set);
			makeFilter();
		}
		void setGain(double set) {
			design.setGain(set);
			makeFilter();
		}
		
		void setBW(float set) { setQ(set);}
		void setBandwidth(float set) { setQ(set);}
		void setSlope(float set) {setQ(set);}
		
		void setResonance(double set) {
			design.setResonance(set);
			makeFilter();
		}
		
		void setParameters(float f, float q)  {
			design.setFreq(f);
			design.setQBWS(q);
			makeFilter();
		}
		void setParameters(float f, float q, float g)  {
			design.setFreq(f);
			design.setQBWS(q);
			design.setGain(g);
			makeFilter();
		}
		
		~Semi() {}
		
	public:
		Type type() { return design.getType(); }
		Status status() { return design.getStatus(); }
		
	protected:
		Coeff coeff;
		Coeff forward;
		Coeff slope;
		int samplesRemaining=0;
		int resolution=16;
		
		Design design;
		State<Form> state;
		double fadeTime=.1;

		void makeFilter() {
			design.make(forward);
			samplesRemaining = fadeTime * rate;
			slope.setAsSlope(coeff, forward, samplesRemaining);
		}
		
	public:
		void sanCheck() {
			state.sanCheck();
		}
		
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Semi);
	};

	
	
	
	
	
	
	/*
			Continuously exponentially approaches the target coefficients
	 
			Lo/Hi/All-Pass do not work great with this - they get bloops when quickly ascending in frequency
	 */
	template <class Design, class Form=TransposedDirectForm2>
	class SemiExp : public FX
	{
		
		
	public:
		SemiExp()
		: FX(),
		coeff(),
		state(),
		fadeTime(.1)
		{
			forward.bypass();
			coeff.bypass();
			updateAlpha();
		}
		void setFadeTime(double set)
		{
			fadeTime = set;
			updateAlpha();
		}
		void snap()
		{
			coeff = forward;
		}
		
	//	//FX Stuff
		void prepare2(Spec& spec) override {
			state.setNumChannels(spec.numChannels);
			
//			forward.setRate(spec.sampleRate);
			design.setRate(spec.sampleRate);
			design.make(forward);
			
			updateAlpha();
			snap();
		}
		void approach()
		{
//			design.approach(alpha);
			coeff.approach(forward, alpha);
		}
		void approach(int N)
		{
			coeff.approach(forward, pow(alpha, N));
		}
		void process (ContextR& c) {
			fx_proc_start(c);
			auto block = c.getOutput();
			int N = (int)block.getNumSamples();
			for (int n=0; n < N; ++n)
			{
				approach();
				state.processAcrossChannels(block, block, n, coeff);
			}
			state.sanCheck();
		}
		void process(ContextNR& c) {
			fx_proc_start(c);
			auto inputBlock  = c.getInputBlock();
			auto outputBlock = c.getOutputBlock();
			
			int N = (int)outputBlock.getNumSamples();

			for (int n=0; n < N; ++n)
			{
				approach();
				state.processAcrossChannels(inputBlock, outputBlock, n, coeff);
			}
			state.sanCheck();
		}
		
		template <class Context>
		void bypassProcess(Context& c, bypassParams) {
			approach(c.getNumSamples());
			c.reset();
		}
		void sidechain(InputBlock& level, ContextR& c) { process(c); }
		void sidechain(InputBlock& level, ContextNR& c) { process(c); }
		
		
		float processSingle(float x, int c=0) {
			approach();
			return state.processSingle(x, c, coeff);
		}
		void reset2() override {
			coeff = forward;
			state.reset();
		}
		
	//	//Getters & Setters
		int paramAmt() override { return 3; }
		void set(int n, float set) override {}
		float get(int n) override {return 1;}
		
	public:
		void updateAlpha() {
			alpha = exp( -1.0f / (fadeTime * rate));
		}
		void setFreq(double set) {
			design.setFreq(set);
			makeFilter();
		}
		void setQ(double set) {
			design.setQBWS(set);
			makeFilter();
		}
		void setGain(double set) {
			design.setGain(set);
			makeFilter();
		}
		
		void setBW(float set) { setQ(set);}
		void setBandwidth(float set) { setQ(set);}
		void setSlope(float set) {setQ(set);}
		
		void setResonance(double set) {
			design.setResonance(set);
			makeFilter();
		}

		
		void setParameters(float f, float q)  {
			design.setFreq(f);
			design.setQBWS(q);
			makeFilter();
		}
		void setParameters(float f, float q, float g)  {
			design.setFreq(f);
			design.setQBWS(q);
			design.setGain(g);
			makeFilter();
		}
		
		~SemiExp() {}
		
	public:
		Type type() { return design.getType(); }
		Status status() { return design.getStatus(); }
		
	protected:
		Coeff coeff;
		Coeff forward;
		int smpRemaining;
		Design design;
		State<Form> state;
		double fadeTime=.1;
		double alpha;

		void makeFilter() {
			design.make(forward);
		}
		
	public:
		void sanCheck() {
			state.sanCheck();
		}
		
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SemiExp);
	};

}
