//
//  RTMeterGauge.h
//  
//
//  Created by Isaac Roberts on 2/17/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "FX.h"

using namespace juce;


class MeterGaugeBase
{
public:
	
//	//Getters & Setters
	enum MeterTrackMode {
		Smoothed, FallingMax, NoSmooth
	};
//	enum Params : unsigned int {
//		TypeParam, MinLevel, MaxLevel, RefreshHz, Smoothing
//	};
	
	
	MeterGaugeBase(float min=-60, float max=0, float refreshHz=26.f, float smoothTime=.0005, MeterTrackMode mode=FallingMax);

	
	void prepare_meter(float rate);
protected:
	void meter_reset();
	
	bool shouldTakeReading(int numSamples);
	inline bool shouldTakeReading(size_t numSamples) {
		return shouldTakeReading((int)(numSamples));
	}

	void updateAmp(float amp);
	void updateDb(float db);
	void updateScaledDb(float db);

	
public:
	
	void setRefreshHz(float hz) {
		refreshHz = hz;
		if (meter_rate > 0)
			refreshSamples = std::ceil(meter_rate/refreshHz);
		
		curSample=0;
	}
	void setConstantRefresh(int blockSize) {
		//Knowing block size is important for smoothFunc
		
		//Not implemented yet 
		jassertfalse;
		refreshSamples = 1;
		refreshHz = 1000;
		curSample=0;
	}
	void setSmoothTime(float seconds) {
		smoothTime = seconds;
	}
	
	virtual ~MeterGaugeBase() {}
	
	
	float getDb();
	void setMin(float set) {min=set;}
	void setMax(float set) {max=set;}
	
	void setRange(float min_, float max_) {min=min_; max=max_;}
	
	std::function<float()> getDbFunc() { return [&]() {return getDb();};}
	
	void setZeroOnBypass(bool set) { zeroOnBypass = set; }
	
	void setTrackMode(MeterTrackMode set) { trackMode = set; }

	float getMin() { return min; }
	float getMax() { return max; }
	
	
	
protected:
	float meter_rate=0;
	
	float min, max;
	float refreshHz;
	float smoothTime;
	
	MeterTrackMode trackMode;
	
	float amp;

	float smoothDb;
	
	int refreshSamples;
	int curSample;
	
	bool zeroOnBypass=false;
private:
//	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeterGaugeBase);

};

class RTMeterGauge : public MeterGaugeBase, public FX
{
public:
	
	enum Type {
		RMS, Mean, Max
	};
	
protected:
	enum Type type;

	
public:
	RTMeterGauge()
	: MeterGaugeBase(), FX(),
	type(RTMeterGauge::RMS)
	{
		
	}
	
	
	void setType(Type set) { type = set; }

	
protected:
	
	void readLevel(InputBlock& i, float extraBoost=0) {
		commonProcess(i, extraBoost);
	}
	void commonProcess(InputBlock& i, float extraDb=0);

	
public:
	
	void prepare2(Spec& spec) override {
		prepare_meter(spec.sampleRate);
	}
	void process (ContextR& c) {
		fx_proc_start(c);
		commonProcess(c.getInputBlock());
	}
	void process(ContextNR& c) {
		fx_proc_start(c);
		c.bypassProcess();
		commonProcess(c.getInputBlock());
	}
	
	void bypassProcess(ContextR& c, bypassParams)
	{
		if (inputIsSilent || zeroOnBypass)
			amp = 0;
		else
			commonProcess(c.getInputBlock());
	}
	void bypassProcess(ContextNR& c, bypassParams)
	{
		if (inputIsSilent || zeroOnBypass)
		{
			c.bypassProcess();
			amp = 0;
		}
		else
			commonProcess(c.getInputBlock());
	}
	
	
	void sidechain(InputBlock& levels, ContextR& c)  {
		commonProcess(levels);
	}
	void sidechain(InputBlock& levels, ContextNR& c){
		commonProcess(levels);
		c.bypassProcess();
	}
	
	
//	int paramAmt() override { return 4; }
//	void set(int n, float set) override {
//		if (n==TypeParam)
//			n = (int)(set);
//		else if (n==MinLevel)
//			min = set;
//		else if (n==MaxLevel)
//			max = set;
//		else if (n==RefreshHz)
//			setRefreshHz(set);
//		else if (n==Smoothing)
//			setSmoothTime(set);
//	}
//	float get(int n) override {
//		if (n==TypeParam)
//			return type;
//		else if (n==MinLevel)
//			return min;
//		else if (n==MaxLevel)
//			return max;
//		else if (n==RefreshHz)
//			return refreshHz;
//		else if (n==Smoothing)
//			return smoothTime;
//		else return extraGet();
//	}
	

};
