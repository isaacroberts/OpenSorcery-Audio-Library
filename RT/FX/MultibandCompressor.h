//
//  MultibandCompressor.h
//  
//
//  Created by Isaac Roberts on 8/2/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "FX.h"
#include "Filters/LinkwitzRiley4th.h"
#include "Filters/LinkwitzRiley2nd.h"
#include "Compressors/RTCompressor.h"

using namespace juce;

class MultibandCompressor : public FX
{
private:
	enum FilterPosition {
		LowCut_=0, HighCut_=1
	};
public:
	MultibandCompressor();
	
	//Sorry, filter library currently requires me to do this
	void setBandAmt(int amt);
	
	void setFrequency(int n, float set);
	
	//FX Stuff
	void prepare2(Spec& spec) override;
	void process(ContextR&);
	void process(ContextNR& context);
	
	void bypassProcess(ContextNR& c, bypassParams) {
		for (int n=0; n < bandCt; ++n)
		{
			filters[n][0].bypassProcess(c, bypassCall);
			filters[n][1].bypassProcess(c, bypassCall);
			compressor[n]->bypassProcess(c, bypassCall);
		}
	}
	void bypassProcess(ContextR& c, bypassParams) {
		for (int n=0; n < bandCt; ++n)
		{
			filters[n][0].bypassProcess(c, bypassCall);
			filters[n][1].bypassProcess(c, bypassCall);
			compressor[n]->bypassProcess(c, bypassCall);
		}
	}
	//TODO: Implement these 
	void sidechain(InputBlock& levels, ContextR& c) { jassertfalse; process(c); }
	void sidechain(InputBlock& levels, ContextNR& c) { jassertfalse; process(c); }

	
	void reset2() override;

	//Getters & Setters
#define paramPerBand 3
	void setFreq(int n, float set);
//	float getFreq(int n);
	
	
#define broadcast(name)\
	void set ## name ## s(float set) { \
		for (int n=0; n < bandCt; ++n) \
			compressor[n]->set ## name(set); \
	} \
	void set ## name (int compressorNo, float set) { \
		jassert(compressorNo < bandCt); \
		compressor[compressorNo]->set ## name(set); \
	}
	//One parameter
	broadcast(Threshold)
	broadcast(Attack);
	broadcast(Release)
	broadcast(Ratio)
	broadcast(KneeWidth)
	broadcast(Knee)
	broadcast(Pregain)
	broadcast(Postgain)
	broadcast(PreGain)
	broadcast(PostGain)
	
	//Two parameters
#undef broadcast
#define broadcast(name, v1, v2)\
	void set ## name ## s(float v1, float v2) { \
		for (int n=0; n < bandCt; ++n) \
			compressor[n]->set ## name(v1, v2); \
	} \
	void set ## name (int compressorNo, float v1, float v2) { \
		jassert(compressorNo < bandCt); \
		compressor[compressorNo]->set ## name(v1, v2); \
	}
	
	broadcast(AttackAndRelease, attack, release)
	
#undef broadcast
	
//	void setCompressorParam(int compN, int paramN, float value);
	void setCompressorOff(int compN, bool isBypassed);
	
	void setCompressorSettings(int compN, float threshold, float ratio, float attack, float release, float kneewidth, float pregain, float postgain);
	
	virtual ~MultibandCompressor();
	
	float getCompressLevel() {
		float sum = 0;
		for (int n=0; n < bandCt; ++n)
		{
			sum += compressor[n]->getCompressLevel();
		}
		return sum / bandCt;
	}
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultibandCompressor);
	
protected:
	LinkwitzRiley4th** filters;
	RTCompressor** compressor;
	int bandCt;
	float* freqs;
	
};
