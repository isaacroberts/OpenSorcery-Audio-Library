//
//  UpDownLookahead.h
//  
//
//  Created by Isaac Roberts on 4/1/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "../FX.h"

using namespace juce;

class UpDownLookahead : public FX {
public:
	UpDownLookahead(float upThresh, float upRatio, float upKnee, float dnThresh, float dnRatio, float dnKnee, float upAttack, float upRelease, float downAttack, float downRelease, float pregain=0, float postgain=0);
	UpDownLookahead(): UpDownLookahead(-40, .7, 1, 1, .2, .2, .003, .1, .004, .2, 0, 0) {}
	
	void prepare2(Spec& spec) override;
	
	void commonProcess(InputBlock& inData, OutputBlock& b, bool isRep);
	void blockProcess(InputBlock& inData, OutputBlock& b, bool useRMS);
//	void ffProcess(InputBlock &level, InputBlock& inData, OutputBlock& b, bool isRep);
	
	float peakLevel(AudioBuffer<float>& i);
	float rmsLevel(AudioBuffer<float>& i);
	
	float peakLevel(InputBlock& i);
	float rmsLevel(InputBlock& i);
	
	void processAsBlock(float level, InputBlock& i, OutputBlock& b);
	
	void process(ContextR& c)  {
		fx_proc_start(c);
		BlockPtr b(c);
		b->copyFrom(c.getInput());
		//TODO: Investigate whether passing a reference from B to function works
		commonProcess(*b, c.getOutput(), true);
	}
	void process(ContextNR& c)  {
		fx_proc_start(c);
		commonProcess(c.getInput(), c.getOutput(), false);
	}
//	void sideChain(Context& c, InputBlock& level) {
//		commonProcess(c.getInput(), c.getOutput(), true);
//	}
//	void sideChain(ContextNR& c, InputBlock& level) {
//		commonProcess(c.getInput(), c.getOutput(), false);
//	}
	
	void bypassProcess(ContextR& c, bypassParams) ;
	void bypassProcess(ContextNR& c, bypassParams) ;
	
	
private:
	void doStateVariableBypass(int numSamples);
	void copyToBuffer(InputBlock& i);
	void bufferToOutput(InputBlock& i, OutputBlock& b);
	void _doBypass(InputBlock& i, OutputBlock& b);
	inline void advanceWritePtr(int numSamples) {
		writePos += numSamples;
		writePos = writePos % buffer.getNumSamples();
	}
public:
	
	void reset2() override { y_t = 0; ytUp = 0;}
	
	virtual ~UpDownLookahead();
	

	int paramAmt() override {return 13;}
	
	//TODO: Mix
	enum Params : unsigned int  {
		UpThresh, UpRatio, UpKnee,
		DnThresh, DnRatio, DnKnee,
		UpAttack, UpRelease,
		DnAttack, DnRelease,
		PreGain, PostGain,
		TimeFactor
	};
	
	//Getters & Setters
	
	void set(int n, float set) override
	{
//		std::cout<<"param "<<n<<" = "<<set<<std::endl;
		if (n==UpThresh)
			upThresh = set;
		else if (n==UpRatio)
			upRatio = set;
		else if (n==UpKnee)
			upKnee = set;
		else if (n==DnThresh)
			dnThresh = set;
		else if (n==DnRatio)
			dnRatio = set;
		else if (n==DnKnee)
			dnKnee = set;
		else if (n==UpAttack)
			upAttack = set;
		else if (n==UpRelease)
			upRelease = set;
		else if (n==DnAttack)
			dnAttack = set;
		else if (n==DnRelease)
			dnRelease = set;
		else if (n==PreGain)
			pregain = set;
		else if (n==PostGain)
			postgain = set;
		else if (n==TimeFactor)
			timeFactor = set;
	}
	float get(int n) override {
		if (n==UpThresh)
			return upThresh;
		else if (n==UpRatio)
			return upRatio;
		else if (n==UpKnee)
			return upKnee;
		else if (n==DnThresh)
			return dnThresh;
		else if (n==DnRatio)
			return dnRatio;
		else if (n==DnKnee)
			return dnKnee;

		else if (n==UpAttack)
			return upAttack;
		else if (n==UpRelease)
			return upRelease;
		else if (n==DnAttack)
			return dnAttack;
		else if (n==DnRelease)
			return dnRelease;
		else if (n==PreGain)
			return pregain;
		else if (n==PostGain)
			return postgain;
		else if (n==TimeFactor)
			return timeFactor;
		else return extraGet();
	}
	
	void setUpSlope(float set) {upRatio = 1-set;}
	void setDnSlope(float set) {dnRatio = 1-set;}
	
	void setUsePeak(bool set) { usePeak = set; }
	void setKnees(float set) { upKnee = dnKnee = set; }
	
	void setBufferLength(int set) { jassert (rate < 0); bufferLength = set; }
	
protected:
	float alpha(float inputVar);
	float timeStep(float y_t, float x_t, float alpha);
	float switchStep(float y_t, float x_t, float alphaAtt, float alphaRel);
	float doShoot(float shoot, float knee, float slope);
	
	float upThresh, upRatio, upKnee;
	float dnThresh, dnRatio, dnKnee;
	float upAttack, upRelease;
	float dnAttack, dnRelease;
	float pregain, postgain;
	float timeFactor;

	bool usePeak = false;
	
	AudioBuffer<float> buffer;
	int writePos;
	int bufferLength;
	
	float y_t;
	float ytUp;
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpDownLookahead);

};
