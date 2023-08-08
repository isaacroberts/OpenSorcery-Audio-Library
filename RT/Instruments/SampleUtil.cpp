//
//  SampleUtil.cpp
//  
//
//  Created by Isaac Roberts on 2/16/22.
//

// Created by Isaac Roberts

#include "SampleUtil.h"

AudioBuffer<float> SampleUtil::readSample(AudioFormatReader& source, bool useStereo, float maxLengthSeconds, int maxChannels)
{
	
	int length = jmin ((int) source.lengthInSamples,
				   (int) (maxLengthSeconds * source.sampleRate));
	
	jassert(source.numChannels > 0);
	
	int numC;
	if (useStereo)
		numC = jmin(source.numChannels, (unsigned int)(maxChannels));
	else
		numC = 1;
	
	AudioBuffer<float> sample (numC, length);
	source.read (&sample, 0, length, 0, true, useStereo);
	
	for (int c=0; c < numC; ++c)
	{
		for (int n=0; n< length;++n)
		{
			sanCheck(sample.getSample(c,n));
		}
	}

	return sample;
}


bool SampleUtil::playStereoNote (const AudioBuffer<float>& input, OutputBlock& outputBuffer,
							double& sourceSamplePosition,
							int startSample, int numSamples,
							double pitchRatio,
							float lgain, float rgain)
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
		
		l *= lgain;
		r *= rgain;


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
	}
//	jassert(std::isfinite(*outL) && std::isfinite(*outR));

	return false;
}

bool SampleUtil::playStereoToMonoNote (const AudioBuffer<float>& input, OutputBlock& outputBuffer,
							double& sourceSamplePosition,
							int startSample, int numSamples,
							double pitchRatio,
							float lgain, float rgain)
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

		if (outR != nullptr)
		{
			*outL++ += l * lgain;
			*outR++ += l * rgain;
		}
		else
		{
			//TODO: Figure out how this is supposed to handle lgain/rgain
			*outL++ += l * lgain;
		}

		sourceSamplePosition += pitchRatio;
	}
	return false;
}

bool SampleUtil::playMonoNote (const AudioBuffer<float>& input, OutputBlock& outputBuffer,
						   double& sourceSamplePosition,
						   int startSample, int numSamples,
						   double pitchRatio,
						   float lgain, float rgain)
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
				
		if (outR != nullptr)
		{
			*outL++ += s * lgain;
			*outR++ += s * rgain;
		}
		else
			*outL++ += s * avg;
		
		sourceSamplePosition += pitchRatio;
	}
	return pos >= N-1;
}



bool SampleUtil::playStereoNoteWithTaper (const AudioBuffer<float>& input, OutputBlock& outputBuffer,
							double& sourceSamplePosition,
							int startSample, int numSamples,
							int taperIx, int taperLen,
							double pitchRatio,
							float lgain, float rgain)
{
	if (input.getNumChannels()==0) {
		jassertfalse;
		return true;
	}
	if (taperIx >= taperLen) return true;

	jassert(std::isfinite(pitchRatio) && pitchRatio > 0);
	const float* const inL = input.getReadPointer (0);
	const float* const inR = input.getNumChannels() > 1 ? input.getReadPointer (1) : nullptr;

	int N = input.getNumSamples()-1;

	float* outL = outputBuffer.getChannelPointer (0) + startSample;
	float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getChannelPointer (1) + startSample : nullptr;
	
	float taperVal = 1.f-float(taperIx)/taperLen;
	float taperDecay = 1.f/taperLen;
		
	float avg = (outputBuffer.getNumChannels()==1) ? avg = (lgain + rgain)/4 : 0;

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

		if (outR != nullptr)
		{
			*outL++ += l * lgain * taperVal;
			*outR++ += r * rgain * taperVal;
		}
		else
		{
			*outL++ += (l + r) * avg * taperVal;
		}

		sourceSamplePosition += pitchRatio;
		taperVal -= taperDecay;
		
		if (taperVal < 0)
			return true;
	}
//	jassert(std::isfinite(*outL) && std::isfinite(*outR));

	return false;
}

bool SampleUtil::playStereoToMonoNoteWithTaper (const AudioBuffer<float>& input, OutputBlock& outputBuffer,
							double& sourceSamplePosition,
							int startSample, int numSamples,
							int taperIx, int taperLen,
							double pitchRatio,
							float lgain, float rgain)
{
	if (input.getNumChannels()==0) {
		jassertfalse;
		return true;
	}
	if (taperIx >= taperLen) return true;
	
	const float* const inL = input.getReadPointer (0);
	const float* const inR = input.getNumChannels() > 1 ? input.getReadPointer (1) : nullptr;

	float* outL = outputBuffer.getChannelPointer (0) + startSample;
	float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getChannelPointer (1) + startSample : nullptr;

	int N = input.getNumSamples()-1;
	
	float taperVal = 1.f-float(taperIx)/taperLen;
	float taperDecay = 1.f/taperLen;
	
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

		if (outR != nullptr)
		{
			*outL++ += l * lgain * taperVal;
			*outR++ += l * rgain * taperVal;
		}
		else
		{
			//TODO: Figure out how this is supposed to handle lgain/rgain
			jassertfalse;
			*outL++ += l * lgain * taperVal;
		}

		sourceSamplePosition += pitchRatio;
		taperVal -= taperDecay;
		
		if (taperVal < 0)
			return true;

	}
	return false;
}

bool SampleUtil::playMonoNoteWithTaper (const AudioBuffer<float>& input, OutputBlock& outputBuffer,
						   double& sourceSamplePosition,
						   int startSample, int numSamples,
						   int taperIx, int taperLen,
						   double pitchRatio,
						   float lgain, float rgain)
{
	if (input.getNumChannels()==0) {
		jassertfalse;
		return true;
	}
	if (taperIx >= taperLen) return true;

	const float* const in = input.getReadPointer (0);
	
	float* outL = outputBuffer.getChannelPointer (0) + startSample;
	float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getChannelPointer (1) + startSample : nullptr;
	float avg = (outputBuffer.getNumChannels()==1) ? avg = (lgain + rgain)/2 : 0;
	
	int N = input.getNumSamples()-1;
	int pos;
	
	float taperVal = 1.f-float(taperIx)/taperLen;
	float taperDecay = 1.f/taperLen;
	
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
				
		if (outR != nullptr)
		{
			*outL++ += s * lgain * taperVal;
			*outR++ += s * rgain * taperVal;
		}
		else
			*outL++ += s * avg * taperVal;
		
		sourceSamplePosition += pitchRatio;
		taperVal -= taperDecay;
		
		if (taperVal < 0)
			return true;
	}
	return pos >= N-1;
}




void SampleUtil::loopStereoNote (const AudioBuffer<float>& input, OutputBlock& outputBuffer,
							double& sourceSamplePosition,
							int startSample, int numSamples,
							double pitchRatio,
							float lgain, float rgain)
{
	if (input.getNumChannels()==0) {
		jassertfalse;
		return;
	}
	
	jassert(std::isfinite(pitchRatio) && pitchRatio > 0);
	const float* const inL = input.getReadPointer (0);
	const float* const inR = input.getNumChannels() > 1 ? input.getReadPointer (1) : nullptr;

	int N = input.getNumSamples();

	float* outL = outputBuffer.getChannelPointer (0) + startSample;
	float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getChannelPointer (1) + startSample : nullptr;

	while (--numSamples >= 0)
	{
		auto pos = (int) sourceSamplePosition;

		if (pos >= N)
		{
			sourceSamplePosition -= N;
			pos -= N;
		}

		auto alpha = (float) (sourceSamplePosition - pos);
		auto invAlpha = 1.0f - alpha;

		// just using a very simple linear interpolation here..
		int p1 = (pos+1);
		if (p1 >= N)
			p1 = 0;
		float l = (inL[pos] * invAlpha + inL[p1] * alpha);
		float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[p1] * alpha) : l;

		l *= lgain;
		r *= rgain;
		
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
	}
}

void SampleUtil::loopStereoToMonoNote (const AudioBuffer<float>& input, OutputBlock& outputBuffer,
							double& sourceSamplePosition,
							int startSample, int numSamples,
							double pitchRatio,
							float lgain, float rgain)
{
	if (input.getNumChannels()==0) {
		jassertfalse;
		return;
	}
	
	const float* const inL = input.getReadPointer (0);
	const float* const inR = input.getNumChannels() > 1 ? input.getReadPointer (1) : nullptr;

	float* outL = outputBuffer.getChannelPointer (0) + startSample;
	float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getChannelPointer (1) + startSample : nullptr;

	int N = input.getNumSamples();

	while (--numSamples >= 0)
	{
		auto pos = (int) sourceSamplePosition;

		if (pos >= N)
		{
			sourceSamplePosition -= N;
			pos -= N;
		}

		auto alpha = (float) (sourceSamplePosition - pos);
		auto invAlpha = 1.0f - alpha;

		int p1 = (pos+1);
		if (p1 >= N)
			p1 = 0;
		
		float in1 = (inR != nullptr) ? (inL[pos]+inR[pos])/2.f : inL[pos];
		float in2 = (inR != nullptr) ? (inL[p1]+inR[p1])/2.f : inL[p1];
		// just using a very simple linear interpolation here..
		float l = (in1 * invAlpha) + in2 * alpha;

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
	}
}

void SampleUtil::loopMonoNote (const AudioBuffer<float>& input, OutputBlock& outputBuffer,
						   double& sourceSamplePosition,
						   int startSample, int numSamples,
						   double pitchRatio,
						   float lgain, float rgain)
{
	if (input.getNumChannels()==0) {
		jassertfalse;
		return;
	}
	
	const float* const in = input.getReadPointer (0);
	
	float* outL = outputBuffer.getChannelPointer (0) + startSample;
	float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getChannelPointer (1) + startSample : nullptr;
	float avg = (outputBuffer.getNumChannels()==1) ? avg = (lgain + rgain)/2 : 0;
	
	int N = input.getNumSamples();
	while (--numSamples >= 0)
	{
		auto pos = (int) sourceSamplePosition;

		if (pos >= N)
		{
			sourceSamplePosition -= N;
			pos -= N;
		}

		auto alpha = (float) (sourceSamplePosition - pos);
		auto invAlpha = 1.0f - alpha;
		
		// just using a very simple linear interpolation here..
		int p1 = pos+1;
		if (p1>=N)
			p1 = 0;
		float s = (in[pos] * invAlpha + in[p1] * alpha);
		
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
	}
}


bool SampleUtil::advanceSamplePosition(const AudioBuffer<float>& input, double& sourceSamplePosition, int numSamples, double pitchRatio)
{
	sourceSamplePosition += pitchRatio * numSamples;
	return (int(sourceSamplePosition) >= input.getNumSamples());
}
bool SampleUtil::advanceSamplePosition(int inputSize, double& sourceSamplePosition, int numSamples, double pitchRatio)
{
	sourceSamplePosition += pitchRatio * numSamples;
	return (int(sourceSamplePosition) >= inputSize);
}
void SampleUtil::advanceSamplePosition(double& sourceSamplePosition, int numSamples, double pitchRatio)
{
	sourceSamplePosition += pitchRatio * numSamples;
}


bool SampleUtil::playStereoNoteWithADSR (const AudioBuffer<float>& input,
			   OutputBlock& output,
			   double& position,
			   int startSample, int numSamples,
			   ADSR& adsr,
			   double pitchRatio,
			   float lgain, float rgain)
{
	return playStereoNoteWithEnv(input, output, position, startSample, numSamples, adsr, pitchRatio, lgain, rgain);
}
bool SampleUtil::playStereoToMonoNoteWithADSR (const AudioBuffer<float>& input,
			   OutputBlock& output,
			   double& position,
			   int startSample, int numSamples,
			   ADSR& adsr,
			   double pitchRatio,
			   float lgain, float rgain)
{
	return playStereoToMonoNoteWithEnv(input, output, position, startSample, numSamples, adsr, pitchRatio, lgain, rgain);
}
bool SampleUtil::playMonoNoteWithADSR (const AudioBuffer<float>& input,
			   OutputBlock& output,
			   double& position,
			   int startSample, int numSamples,
			   ADSR& adsr,
			   double pitchRatio,
			   float lgain, float rgain)
{
	return playMonoNoteWithEnv(input, output, position, startSample, numSamples, adsr, pitchRatio, lgain, rgain);
}

//bool SampleUtil::playStereoNote (const AudioBuffer<float>& input,
//			   OutputBlock& output,
//			   double& position,
//			   int startSample, int numSamples,
//			   double pitchRatio,
//			   float lgain, float rgain)
//{
//	return playStereoNote_(input, output, position,  startSample, numSamples, pitchRatio, lgain, rgain);
//}
//bool SampleUtil::playStereoToMonoNote (const AudioBuffer<float>& input,
//			   OutputBlock& output,
//			   double& position,
//			   int startSample, int numSamples,
//			   double pitchRatio,
//			   float lgain, float rgain)
//{
//	return playStereoToMonoNote_(input, output, position,  startSample, numSamples, pitchRatio, lgain, rgain);
//}
//bool SampleUtil::playMonoNote (const AudioBuffer<float>& input,
//			   OutputBlock& output,
//			   double& position,
//			   int startSample, int numSamples,
//			   double pitchRatio,
//			   float lgain, float rgain)
//{
//	return playMonoNote_(input, output, position, startSample, numSamples, pitchRatio, lgain, rgain);
//}
bool SampleUtil::playNoteWithADSR (const AudioBuffer<float>& input,
			   OutputBlock& output,
			   double& position,
			   int startSample, int numSamples,
			   ADSR& adsr,
			   double pitchRatio,
			   float lgain, float rgain) {
	if (input.getNumChannels() == 1)
		return playMonoNoteWithADSR(input, output, position, startSample, numSamples, adsr, pitchRatio, lgain, rgain);
	else
		return playStereoNoteWithADSR(input, output, position, startSample, numSamples, adsr, pitchRatio, lgain, rgain);
}
bool SampleUtil::playNote (const AudioBuffer<float>& input,
			   OutputBlock& output,
			   double& position,
			   int startSample, int numSamples,
			   double pitchRatio,
			   float lgain, float rgain) {
	if (input.getNumChannels() == 1)
		return playMonoNote(input, output, position, startSample, numSamples, pitchRatio, lgain, rgain);
	else
		return playStereoNote(input, output, position, startSample, numSamples, pitchRatio, lgain, rgain);
}
