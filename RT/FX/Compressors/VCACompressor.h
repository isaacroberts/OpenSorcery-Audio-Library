//
//  VCACompressor.h
//  
//
//  Created by Isaac Roberts on 11/21/22.
//

// Created by Isaac Roberts 

#ifndef VCACompressor_h
#define VCACompressor_h

#include <JuceHeader.h>

/*
	VCA Compressor - I BELIEVE!
		Uses a ballistics filter and a funky gain funciton
 
		Gain func = (env / threshold) ** (1/ratio - 1)
	
 
	This is just copied from JUCE and adapted to my code

	This might be either unfinished or untested. If you delete this file you don't have to do the update below.
*/
class VCACompressor
{
public:
	VCACompressor();
			
	//Getters & Setters
	void setBypass(bool set) {bypass = set;}
	bool isBypassed() {return bypass;}

	void prepare(Spec& spec);
 
//	void process(ContextR& c);
//	void process(ContextNR& c);
	
	template <typename ProcessContext>
	void process (ProcessContext& context) noexcept
	{
		fx_proc_start(context);
		
		const auto& inputBlock = context.getInputBlock();
		auto& outputBlock      = context.getOutputBlock();
		const auto numChannels = outputBlock.getNumChannels();
		const auto numSamples  = outputBlock.getNumSamples();

		jassert (inputBlock.getNumChannels() == numChannels);
		jassert (inputBlock.getNumSamples()  == numSamples);

		for (size_t channel = 0; channel < numChannels; ++channel)
		{
			auto* inputSamples  = inputBlock .getChannelPointer (channel);
			auto* outputSamples = outputBlock.getChannelPointer (channel);

			for (size_t i = 0; i < numSamples; ++i)
				outputSamples[i] = processSample ((int) channel, inputSamples[i]);
		}
	}

	/** Performs the processing operation on a single sample at a time. */
	float processSample (int channel, float inputValue);


	bool hasBeenPrepared() {return sampleRate > 0;}

	void bypassProcess(ContextR& c, bool allowTail, bool isInputSilent);
	void bypassProcess(ContextNR& c, bool allowTail, bool isInputSilent);

	void sidechain(InputBlock& levels, ContextR& c) {process(c); }
	void sidechain(InputBlock& levels, ContextNR& c) {process(c); }

	void reset();
	void setInstrData(InstrData* instrData) {}
		
	float getCompressLevel();
	
	~VCACompressor();
	
public:
	/** Sets the threshold in dB of the compressor.*/
	void setThreshold (float newThreshold);

	/** Sets the ratio of the compressor (must be higher or equal to 1).*/
	void setRatio (float newRatio);

	/** Sets the attack time in milliseconds of the compressor.*/
	void setAttack (float newAttack);

	/** Sets the release time in milliseconds of the compressor.*/
	void setRelease (float newRelease);

	void setAttackAndRelease(float atk, float rel);
	inline void setTimes(float atk, float rel) { setAttackAndRelease(atk, rel); }
	
	void setPregain(float set);
	void setPostgain(float set);
	
	void setParameters(float thresh, float ratio, float attack, float release, float pre=0, float post=0);
	
protected:
	void update();
	
protected:
	float threshold, thresholdInverse, ratioFactor;
	BallisticsFilter<float> envelopeFilter;

	double sampleRate = 44100.0;
	float thresholddB = 0.0, ratio = 1.0, attackTime = 1.0, releaseTime = 100.0;

	float pregain=1;
	float postgain=1;
	
	bool bypass=false;
	
	float last_gain=0;
	
private:
	VCACompressor (const VCACompressor&) = delete;
	VCACompressor& operator= (const VCACompressor&) = delete;
};


#endif /* VCACompressor_h */


/*
JUCE UPDATE :

Add to juce_dsp > processors > juce_BallisticsFilter

	//My library uses bypassProcess and silenceProcess to signify to processors that they don't need to do full processing
	//For example, Filters can use this to assume input=0 and simplify calculations 
	//These functions are added piecemeal where needed
	void bypassProcess(const AudioBlock<const float>& inputBlock) noexcept
	{
		const auto numChannels = inputBlock.getNumChannels();
		const auto numSamples = inputBlock.getNumSamples();

		jassert(inputBlock.getNumChannels() <= yold.size());

		for (size_t channel = 0; channel < numChannels; ++channel)
		{
			auto* inputSamples = inputBlock.getChannelPointer(channel);

			for (size_t i = 0; i < numSamples; ++i)
				skipSample((int)channel, inputSamples[i]);
		}

	#if JUCE_DSP_ENABLE_SNAP_TO_ZERO
		snapToZero();
	#endif
	}
	void silenceProcess(int numSamples) noexcept
	{
		for (size_t channel = 0; channel < yold.size(); ++channel)
		{
			if (yold[channel] < 0)
				yold[channel] *= pow(cteAT, numSamples);
			else
				yold[channel] *= pow(cteRL, numSamples);

		}

	#if JUCE_DSP_ENABLE_SNAP_TO_ZERO
		snapToZero();
	#endif
	}
	void skipSample(int channel, SampleType input);
	void skipSample(int channel);


goto juce_BallisticsFilter.cppand add ~line 112

	template <typename SampleType>
	void BallisticsFilter<SampleType>::skipSample(int channel, SampleType inputValue)
	{
		jassert(isPositiveAndBelow(channel, yold.size()));

		if (levelType == LevelCalculationType::RMS)
			inputValue *= inputValue;
		else
			inputValue = std::abs(inputValue);

		SampleType cte = (inputValue > yold[(size_t)channel] ? cteAT : cteRL);

		SampleType result = inputValue + cte * (yold[(size_t)channel] - inputValue);
		yold[(size_t)channel] = result;
	}

	template <typename SampleType>
	void BallisticsFilter<SampleType>::skipSample(int channel)
	{
		jassert(isPositiveAndBelow(channel, yold.size()));

		if (yold[(size_t)channel] < 0)
			yold[(size_t)channel] *= cteAT;
		else
			yold[(size_t)channel] *= cteRL;
	}



*/
