//
//  BufferManager.h
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/3/20.
//

// Created by Isaac Roberts 


namespace Utility {
	
	inline void copyBlock(InputBlock& from, OutputBlock& to, int srcPos, int dstPos, int samples)
	{
		for (int c=0; c < from.getNumChannels(); ++c)
		{
			auto* src = from.getChannelPointer(c);
			auto* dst = to.getChannelPointer(c);
			
			for (int n=0; n < samples; ++n)
			{
				dst[dstPos + n] = src[srcPos + n];
			}
		}
	}
	
	
	
	static inline float rmsLevel(const juce::dsp::AudioBlock<const float>& buffer)
	{
		float totalRMS = 0;
		for (int c=0; c < buffer.getNumChannels(); ++c)
		{
			float rms = 0;
			auto* data = buffer.getChannelPointer(c);
			for (int n=0; n < buffer.getNumSamples(); ++n)
			{
				if (std::isfinite(data[n]))
				{
					rms += pow(data[n], 2);
				}
			}
			rms /= buffer.getNumSamples();
			rms = sqrt(rms);
			totalRMS += rms;
		}
		totalRMS /= buffer.getNumChannels();
		return totalRMS;
	}
	static inline float rmsLevelBuffer(const juce::AudioBuffer<const float>& buffer)
	{
		float totalRMS = 0;
		for (int c=0; c < buffer.getNumChannels(); ++c)
		{
			float rms = buffer.getRMSLevel(c, 0, buffer.getNumSamples());
			totalRMS += rms;
		}
		totalRMS /= buffer.getNumChannels();
		return totalRMS;
	}
	static inline float rmsLevelBuffer(const juce::AudioBuffer<float>& buffer)
	{
		float totalRMS = 0;
		for (int c=0; c < buffer.getNumChannels(); ++c)
		{
			float rms = buffer.getRMSLevel(c, 0, buffer.getNumSamples());
			totalRMS += rms;
		}
		totalRMS /= buffer.getNumChannels();
		return totalRMS;
	}
	
	extern void copy(juce::dsp::AudioBlock<float>& to, InputBlock &from);
	extern void copy(juce::AudioBuffer<float>& to, const juce::AudioBuffer<float>& from);
//	void copy(AudioBlock<float>& to, const AudioBuffer<const float>& from);
	extern void add(ABlock &a, const ABlock &b);
	extern void add(ABlock &a, const ABlock &b, float mixA, float mixB);
	inline void add(ABlock &a, const ABlock &b, float mixA) {
		add(a, b, mixA, 1-mixA);
	}
	extern void addProduct(ABlock& a, const ABlock& b, float product);
	
	extern float getBlockMax(InputBlock& a);
	
	inline void clearChannel(OutputBlock& b, int c, int start, int end)
	{
		juce::FloatVectorOperations::clear(b[c]+start, end);
	}

	void sanitize(AudioBuffer<float>& buffer);
	void normalizeRMS(AudioBuffer<float>& buffer, float targetLevel=1);
	void normalizePeak(AudioBuffer<float>& buffer, float targetLevel=1);
	
}

