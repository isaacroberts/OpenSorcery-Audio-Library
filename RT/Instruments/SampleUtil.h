//
//  SampleUtil.h
//  
//
//  Created by Isaac Roberts on 2/16/22.
//

// Created by Isaac Roberts 

#ifndef SampleUtil_h
#define SampleUtil_h


#include <JuceHeader.h>

using namespace juce;

class SampleUtil {
public:
	
	//If you're looking for gainPan it's been moved to Util::getPan
//	static void gainPan(float& lgain, float& rgain, float pan, float amp=1.f);
	
	//TODO: Make this point to FileLoad
	static AudioBuffer<float> readSample(AudioFormatReader& source, bool useStereo, float maxLengthSeconds=5, int maxChannels=2);
	
	
	static void sanCheck(float x)
	{
		jassert(std::isfinite(x));
		jassert( x < 1000);
		jassert (x > -1000);
	}
	
	//Returns shouldStop
	inline static bool playStereoNoteWithADSR (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   ADSR& adsr,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f) {
		return playStereoNoteWithADSR(input, output.getOutput(), position, startSample, numSamples, adsr, pitchRatio, lgain, rgain);
	}
	inline static bool playStereoToMonoNoteWithADSR (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   ADSR& adsr,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f) {
		return playStereoToMonoNoteWithADSR(input, output.getOutput(), position, startSample, numSamples, adsr, pitchRatio, lgain, rgain);
	}
	inline static bool playMonoNoteWithADSR (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   ADSR& adsr,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f) {
		return playMonoNoteWithADSR(input, output.getOutput(), position, startSample, numSamples, adsr, pitchRatio, lgain, rgain);
	}
	
	
	template <class Env>
	inline static bool playStereoNoteWithEnv (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   Env& env,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f) {
		return playStereoNoteWithEnv(input, output.getOutput(), position, startSample, numSamples, env, pitchRatio, lgain, rgain);
	}
	template <class Env>
	inline static bool playStereoToMonoNoteWithEnv (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   Env& env,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f) {
		return playStereoToMonoNoteWithEnv(input, output.getOutput(), position, startSample, numSamples, env, pitchRatio, lgain, rgain);
	}
	template <class Env>
	inline static bool playMonoNoteWithEnv (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   Env& env,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f) {
		return playMonoNoteWithEnv(input, output.getOutput(), position, startSample, numSamples, env, pitchRatio, lgain, rgain);
	}
	
	
	inline static bool playStereoNote (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f) {
		return playStereoNote(input, output.getOutput(), position, startSample, numSamples, pitchRatio, lgain, rgain);
	}
	inline static bool playStereoToMonoNote (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   double pitchRatio,
			 	   float lgain=1.0f, float rgain=1.0f) {
		return playStereoToMonoNote(input, output.getOutput(), position, startSample, numSamples, pitchRatio, lgain, rgain);
	}
	inline static bool playMonoNote (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f) {
		return playMonoNote(input, output.getOutput(), position, startSample, numSamples, pitchRatio, lgain, rgain);
	}
	inline static bool playNoteWithADSR (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   ADSR& adsr,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f) {
		return playNoteWithADSR(input, output.getOutput(), position, startSample, numSamples, adsr, pitchRatio, lgain, rgain);
	}
	inline static bool playNote (const AudioBuffer<float>& input,
				   ContextR& output,
				   double& position,
				   int startSample, int numSamples,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f) {
		return playNote(input, output.getOutput(), position, startSample, numSamples, pitchRatio, lgain, rgain);
	}
	
	static void loopStereoNote (const AudioBuffer<float>& input,
				   OutputBlock& output,
				   double& position,
				   int startSample, int numSamples,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f);
	static void loopStereoToMonoNote (const AudioBuffer<float>& input,
				   OutputBlock& output,
				   double& position,
				   int startSample, int numSamples,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f);
	static void loopMonoNote (const AudioBuffer<float>& input,
				   OutputBlock& output,
				   double& position,
				   int startSample, int numSamples,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f);

	//Returns shouldStop
	static bool playStereoNoteWithADSR (const AudioBuffer<float>& input,
				   OutputBlock& output,
				   double& position,
				   int startSample, int numSamples,
				   ADSR& adsr,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f);
	static bool playStereoToMonoNoteWithADSR (const AudioBuffer<float>& input,
				   OutputBlock& output,
				   double& position,
				   int startSample, int numSamples,
				   ADSR& adsr,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f);
	static bool playMonoNoteWithADSR (const AudioBuffer<float>& input,
				   OutputBlock& output,
				   double& position,
				   int startSample, int numSamples,
				   ADSR& adsr,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f);
//	static bool playStereoNote (const AudioBuffer<float>& input,
//				   OutputBlock& output,
//				   double& position,
//				   int startSample, int numSamples,
//				   double pitchRatio,
//				   float lgain=1.0f, float rgain=1.0f);
//	static bool playStereoToMonoNote (const AudioBuffer<float>& input,
//				   OutputBlock& output,
//				   double& position,
//				   int startSample, int numSamples,
//				   double pitchRatio,
//				   float lgain=1.0f, float rgain=1.0f);
//	static bool playMonoNote (const AudioBuffer<float>& input,
//				   OutputBlock& output,
//				   double& position,
//				   int startSample, int numSamples,
//				   double pitchRatio,
//				   float lgain=1.0f, float rgain=1.0f);

	static bool playNoteWithADSR (const AudioBuffer<float>& input,
				   OutputBlock& output,
				   double& position,
				   int startSample, int numSamples,
				   ADSR& adsr,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f);
	static bool playNote (const AudioBuffer<float>& input,
				   OutputBlock& output,
				   double& position,
				   int startSample, int numSamples,
				   double pitchRatio,
				   float lgain=1.0f, float rgain=1.0f);
	
	
	
	static bool advanceSamplePosition(const AudioBuffer<float>& input, double& position, int numSamples, double pitchRatio);
	static bool advanceSamplePosition(int inputSize, double& position, int numSamples, double pitchRatio);
	static void advanceSamplePosition(double& sourceSamplePosition, int numSamples, double pitchRatio);
	
	template <class Env>
	static bool advanceSamplePosition(int inputSize, double& position, int numSamples, Env& env, double pitchRatio) {
		position += pitchRatio * numSamples;
		env.advanceSamples(numSamples);
		return (int(position) >= inputSize) || (!env.isActive());
	}
	
	template <class Env>
	static bool advanceSamplePosition(const AudioBuffer<float>& input, double& position, int numSamples, Env& env, double pitchRatio) {
		return advanceSamplePosition(input.getNumSamples(), position, numSamples, env, pitchRatio);
	}
	
	template <class Env>
	static bool playStereoNoteWithEnv (const AudioBuffer<float>& input,
			 OutputBlock& outputBuffer,
			 double& sourceSamplePosition,
			 int startSample, int numSamples,
			 Env& adsr,
			 double pitchRatio,
			 float lgain=1.0f, float rgain=1.0f)
	{
		if (input.getNumChannels()==0) {
			jassertfalse;
			return true;
		}
		
		jassert(std::isfinite(pitchRatio) && pitchRatio > 0);
		const float* const inL = input.getReadPointer (0);
		const float* const inR = input.getNumChannels() > 1 ? input.getReadPointer (1) : nullptr;

		int N = input.getNumSamples()-1;
		
		float* outL = outputBuffer.getChannelPointer (0) + startSample;
		float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getChannelPointer (1) + startSample : nullptr;

		while (--numSamples >= 0)
		{
			auto pos = (int) sourceSamplePosition;
			
			if (pos >= input.getNumSamples())
			{
				return true;
			}
			
			auto alpha = (float) (sourceSamplePosition - pos);
			auto invAlpha = 1.0f - alpha;

			// just using a very simple linear interpolation here..
			float l, r;
			if (pos < N)
			{
				l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
				r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha) : l;
			}
			else
			{
				l = inL[pos];
				r = (inR != nullptr) ? (inR[pos]) : l;
			}
			
			auto envelopeValue = adsr.getNextSample();
			jassert(envelopeValue>=-.01 && envelopeValue <= 1.01);
			
			l *= lgain * envelopeValue;
			r *= rgain * envelopeValue;

			if (outR != nullptr)
			{
				*outL++ += l;
				*outR++ += r;
			}
			else
			{
				*outL++ += (l + r) * 0.5f;
			}
			sourceSamplePosition += pitchRatio;
			
			if (!adsr.isActive())
				return true;
		}
		return false;
	}

	template <class Env>
	static bool playStereoToMonoNoteWithEnv
				(const AudioBuffer<float>& input, OutputBlock& outputBuffer,
				 double& sourceSamplePosition,
				 int startSample, int numSamples,
				 Env& adsr,
				 double pitchRatio,
				 float lgain=1.0f, float rgain=1.0f)
	{
		if (input.getNumChannels()==0) {
			jassertfalse;
			return true;
		}
		const float* const inL = input.getReadPointer (0);
		const float* const inR = input.getNumChannels() > 1 ? input.getReadPointer (1) : nullptr;

		float* outL = outputBuffer.getChannelPointer (0) + startSample;
		float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getChannelPointer (1) + startSample : nullptr;

		int N = input.getNumSamples()-1;

		while (--numSamples >= 0)
		{
			auto pos = (int) sourceSamplePosition;
			
			if (pos >= N)
			{
				return true;
			}
			
			auto alpha = (float) (sourceSamplePosition - pos);
			auto invAlpha = 1.0f - alpha;

			float in1 = (inR != nullptr) ? (inL[pos]+inR[pos])/2.f : inL[pos];
			float l;
			if (pos < N)
			{
				float in2 = (inR != nullptr) ? (inL[pos+1]+inR[pos+1])/2.f : inL[pos+1];
				// just using a very simple linear interpolation here..
				l = (in1 * invAlpha) + in2 * alpha;
			}
			else
				l = in1;
			
			auto envelopeValue = adsr.getNextSample();
			jassert(envelopeValue>=-.01 && envelopeValue <= 1.01);

			l *= envelopeValue;
			if (outR != nullptr)
			{
				*outL++ += l * lgain;
				*outR++ += l * rgain;
			}
			else
			{
				*outL++ += l;
			}

			sourceSamplePosition += pitchRatio;
			
			if (!adsr.isActive())
				return true;
		}
		return false;
	}

	
	template <class Env>
	static bool playMonoNoteWithEnv
			(const AudioBuffer<float>& input, OutputBlock& outputBuffer,
			 double& sourceSamplePosition,
			 int startSample, int numSamples,
			 Env& adsr,
			 double pitchRatio,
			 float lgain=1.0f, float rgain=1.0f)
	{
		if (input.getNumChannels()==0) {
			jassertfalse;
			return true;
		}
		const float* const in = input.getReadPointer (0);
		
		float* outL = outputBuffer.getChannelPointer (0) + startSample;
		float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getChannelPointer (1) + startSample : nullptr;
		float avg = (outputBuffer.getNumChannels()==1) ? avg = (lgain + rgain)/2 : 0;
		
		int N = input.getNumSamples()-1;
		int pos;
		while (--numSamples >= 0 && (pos = (int)sourceSamplePosition) < N-1)
		{
			auto alpha = (float) (sourceSamplePosition - pos);
			auto invAlpha = 1.0f - alpha;
			
			// just using a very simple linear interpolation here..
			float s;
			if (pos < N)
			{
				s = (in[pos] * invAlpha + in[pos + 1] * alpha);
			}
			else
			{
				s = in[pos];
			}
			
			float a = adsr.getNextSample();
			if (a<=.0001)
				return true;
			s *= a;
			
			if (outR != nullptr)
			{
				*outL++ += s * lgain;
				*outR++ += s * rgain;
			}
			else
			{
				*outL++ += s * avg;
			}
			
			sourceSamplePosition += pitchRatio;
			
			if (!adsr.isActive())
				return true;
		}
		return pos >= N-1;
	}

	static bool playStereoNote (const AudioBuffer<float>& input,
					OutputBlock& outputBuffer,
					double& sourceSamplePosition,
					int startSample, int numSamples,
					double pitchRatio,
					float lgain=1.0f, float rgain=1.0f);
	static bool playStereoToMonoNote(const AudioBuffer<float>& input, OutputBlock& outputBuffer,
					 double& sourceSamplePosition,
					 int startSample, int numSamples,
					 double pitchRatio,
					 float lgain=1.0f, float rgain=1.0f);
	static bool playMonoNote(const AudioBuffer<float>& input, OutputBlock& outputBuffer,
					 double& sourceSamplePosition,
					 int startSample, int numSamples,
					 double pitchRatio,
					 float lgain=1.0f, float rgain=1.0f);
	
	
	static bool playStereoNoteWithTaper (const AudioBuffer<float>& input,
					OutputBlock& outputBuffer,
					double& sourceSamplePosition,
					int startSample, int numSamples,
					int taperIx, int taperLength,
					double pitchRatio,
					float lgain=1.0f, float rgain=1.0f);
	static bool playStereoToMonoNoteWithTaper(const AudioBuffer<float>& input, OutputBlock& outputBuffer,
					 double& sourceSamplePosition,
					 int startSample, int numSamples,
					 int taperIx, int taperLength,
					 double pitchRatio,
					 float lgain=1.0f, float rgain=1.0f);
	static bool playMonoNoteWithTaper(const AudioBuffer<float>& input, OutputBlock& outputBuffer,
					 double& sourceSamplePosition,
					 int startSample, int numSamples,
					 int taperIx, int taperLength,
					 double pitchRatio,
					 float lgain=1.0f, float rgain=1.0f);

	
private:
	SampleUtil()=delete;
	SampleUtil(const SampleUtil& other)=delete;
	SampleUtil& operator=(const SampleUtil& other)=delete;
	~SampleUtil() {}
};

#endif /* SampleUtil_h */
