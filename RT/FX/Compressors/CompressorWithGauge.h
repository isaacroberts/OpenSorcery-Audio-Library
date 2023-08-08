//
//  CompressorWithGauge.h
//  
//
//  Created by Isaac Roberts on 10/6/22.
//

// Created by Isaac Roberts 

#ifndef CompressorWithGauge_h
#define CompressorWithGauge_h

#include "CompressorCommon.h"
#include "../RTMeterGauge.h"

class CompressorWithGauge : public Compressors::Common, public MeterGaugeBase {
public:
	CompressorWithGauge()
	: Compressors::Common(0, .2, .003, .1, .2, 0, 0),
	MeterGaugeBase(-40, 0, 60.f, 0.f, MeterGaugeBase::MeterTrackMode::NoSmooth),
	type(Compressors::RMS)
	{
	}
//	CompressorWithGauge(float threshold, float ratio, float attack, float release, float kneewidth=.2, float pregain=0, float postgain=0)
//	: Compressors::Common(threshold, ratio, attack, release, kneewidth, pregain, postgain),
//	MeterGaugeBase(-40, 0, 60.f, .0005f, TrackMode::NoSmooth),
//	type(Compressors::RMS)
//	{
//	}

	void prepare(Spec& spec)  {
		rate = spec.sampleRate;
		prepare_meter(rate);
	}
	
protected:
	void commonProcess(InputBlock& l, InputBlock &i, OutputBlock& b) {
		compressorProcess(l, i, b, rate, type);
//		if (shouldTakeReading(l.getNumSamples()))
		{
			updateDb(getCompressLevel());
		}
	}
	float getBypassLevel()
	{
		return 0;
	}
	
public:
	void process(ContextR& c)  {
		commonProcess(c.getInput(), c.getInput(), c.getOutput());
	}
	void process(ContextNR& c)  {
		commonProcess(c.getInput(), c.getInput(), c.getOutput());
	}
//	void process(InputBlock& from, OutputBlock& to, BlockStorage& b) {
//	}
//	void process(OutputBlock& b, BlockStorage& s) {
//	}

	
	template <class ContextType>
	void bypassProcess(ContextType& c, bypassParams) {
		if (inputIsSilent)
			releaseNFrames(c.getNumSamples(), rate);
		else
			compressorBypass(c.getInput(), rate, type);
		
//		if (shouldTakeReading(c.getNumSamples()))
		{
			updateDb(getBypassLevel());
		}
	}
	
	void sidechain(InputBlock& l, ContextR& c)  {
		commonProcess(l,  c.getInput(), c.getOutput());
	}
	void sidechain(InputBlock& l, ContextNR& c)  {
		commonProcess(l,  c.getInput(), c.getOutput());
	}
	
	virtual ~CompressorWithGauge() {
		
	}
	
	void setType(Compressors::ReadType settype) {
		type = settype;
	}
	
	void setInstrData(InstrData* instrData) {}

	void reset()
	{
		resetState();
	}
	
	void setParameters(float threshold, float ratio, float attack, float release, float kneewidth, float pregain, float postgain)
	{
		this->threshold = threshold;
		this->ratio = ratio;
		this->attack=attack;
		this->release = release;
		this->kneewidth = kneewidth;
		this->pregain = pregain;
		this->postgain = postgain;
	}
	void setParameters(float threshold, float ratio, float attack, float release, float kneewidth, float pregain)
	{
		this->threshold = threshold;
		this->ratio = ratio;
		this->attack=attack;
		this->release = release;
		this->kneewidth = kneewidth;
		this->pregain = pregain;
	}
	void setParameters(float threshold, float ratio, float attack, float release, float kneewidth)
	{
		this->threshold = threshold;
		this->ratio = ratio;
		this->attack=attack;
		this->release = release;
		this->kneewidth = kneewidth;
	}
	void setParameters(float threshold, float ratio, float attack, float release)
	{
		this->threshold = threshold;
		this->ratio = ratio;
		this->attack=attack;
		this->release = release;
	}
	
protected:
	Compressors::ReadType type;
	float rate=69;
private:
	JUCE_DECLARE_NON_COPYABLE (CompressorWithGauge);
};




#endif /* CompressorWithGauge_h */
