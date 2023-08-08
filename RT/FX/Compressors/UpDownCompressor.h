//
//  UpDownCompressor.h
//  
//
//  Created by Isaac Roberts on 2/17/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "../FX.h"

#include "CompressorCommon.h"

using namespace juce;


class UpDownCompressor : public FX {
public:
	enum Params : unsigned int  {
		UpThresh, UpRatio, UpKnee,
		DnThresh, DnRatio, DnKnee,
		DnAttack, DnRelease,
		PreGain, PostGain,
		TimeFactor
		//TODO: Mix
	};
	
	
	UpDownCompressor(float upThresh, float upRatio, float upKnee, float dnThresh, float dnRatio, float dnKnee, float downAttack, float downRelease, float pregain=0, float postgain=0);
	UpDownCompressor(): UpDownCompressor(-40, .7, 1, 1, .2, .2, .004, .2, 0, 0) {}

	
	void prepare2(Spec& spec) override;
	
	void commonProcess(InputBlock &level, InputBlock& inData, OutputBlock& b);
	void blockProcess(InputBlock& level, InputBlock& inData, OutputBlock& b, bool useRMS);
	
//	void ffDb(InputBlock& level, OutputBlock& gainTarget);
	void ffFromDb(InputBlock& dbLevel, InputBlock& input, OutputBlock& output);
	void ffProcess(InputBlock &level, InputBlock& inData, OutputBlock& b);

	void bypassFromDb(InputBlock& dbLevel);
	
	void computeCompression(InputBlock& level, OutputBlock& db, bool addPreAndPost);
//	void peakProcess(InputBlock &i, OutputBlock& b);
//	void rmsProcess(InputBlock& i, OutputBlock& b);
	
	void getGainReductionDb(InputBlock& level, OutputBlock& dbBlock);
	void applyGainReduction(InputBlock& db, InputBlock& input, OutputBlock& output);
	void applyGainReduction(InputBlock& db, OutputBlock& output);
	
//	void replaceBlockWithDb()
	void blockProcess(float level, InputBlock& i, OutputBlock& b);
	
	void process(ContextR& c)  {
		fx_proc_start(c);
		commonProcess(c.getInput(), c.getInput(), c.getOutput());
	}
	void process(ContextNR& c)  {
		fx_proc_start(c);
		commonProcess(c.getInput(), c.getInput(), c.getOutput());
	}
	void sidechain(InputBlock& levels, ContextR& c)  {
		fx_proc_start(c);
		commonProcess(levels, c.getInput(), c.getOutput());
	}
	void sidechain(InputBlock& levels, ContextNR& c)  {
		fx_proc_start(c);
		commonProcess(levels, c.getInput(), c.getOutput());
	}
	
	void _skipProcess(int numSamples);
	
	
	void bypassProcess(ContextR& c, bypassParams)  {
		_skipProcess(c.getNumSamples());
	}
	void bypassProcess(ContextNR& c, bypassParams)  {
		_skipProcess(c.getNumSamples());
	}
	
	
private:
	void _doBypass(InputBlock&i, OutputBlock& b);
public:
	
	void reset2() override { y_t = 0;}
	
	virtual ~UpDownCompressor();
	

	int paramAmt() override {return 13;}
	
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
	
	float getPreGain() { return pregain; }
	float getPostGain() { return postgain; }
	float getTotalGain() { return pregain + postgain; }
	
	void setUpSlope(float set) {upRatio = 1-set;}
	void setDnSlope(float set) {dnRatio = 1-set;}
	
	void setUsePeak(bool set) { set ? blockType = Compressors::Peak : Compressors::RMS; }
	void setType(Compressors::ReadType type) { blockType = type; }
	void setKnees(float set) { upKnee = dnKnee = set; }
	
	Compressors::ReadType getType() { return blockType; }
	
	float getTotalAmp() {
		return pow(10.f, (pregain+postgain)/20.f);
	}
	
protected:
	float alphaAtt(float inputVar);
	float alphaRel(float inputVar);
	float timeStep(float y_t, float x_t, float alpha);
	float switchStep(float y_t, float x_t, float alphaAtt, float alphaRel);
	float doShoot(float shoot, float knee, float slope);
	
	float ballistics(float level, float& alphaAtt, float& alphaRel);
	float passBallistics(float& alphaAtt, float& alphaRel);
	float passBallistics();

	
	float upThresh, upRatio, upKnee;
	float dnThresh, dnRatio, dnKnee;
	float dnAttack, dnRelease;
	float pregain, postgain;
	float timeFactor;

	Compressors::ReadType blockType;
	
	float y_t, y_t2;
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpDownCompressor);

};
