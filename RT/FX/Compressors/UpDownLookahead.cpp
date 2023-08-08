//
//  UpDownLookahead.cpp
//  
//
//  Created by Isaac Roberts on 4/1/21.
//

// Created by Isaac Roberts


#include "UpDownLookahead.h"

#include <math.h>
#include <algorithm>

UpDownLookahead::UpDownLookahead(float ut, float urat, float uknee, float dt, float dr, float dknee, float ua, float ur, float da, float drel, float pg, float g)
:
FX(),
upThresh(ut),
upRatio(urat),
upKnee(uknee),
dnThresh(dt),
dnRatio(dr),
dnKnee(dknee),
upAttack(ua),
upRelease(ur),
dnAttack(da),
dnRelease(drel),
pregain(pg),
postgain(g),
timeFactor(5),
usePeak(false),
buffer(),
writePos(0),
bufferLength(128),
y_t(0),
ytUp(0)
{
}

void UpDownLookahead::prepare2(Spec& spec)
{
	y_t = 0 ;
	ytUp = 0;
	
	buffer.setSize(spec.numChannels, bufferLength, false, true, false);
	buffer.clear();
	writePos = 0;
}
UpDownLookahead::~UpDownLookahead()
{
}

float UpDownLookahead::alpha(float input)
{
	if (input > 0)
		return exp( -1.0f / (upAttack * timeFactor * rate));
	else
		return 0;
}
float UpDownLookahead::doShoot(float shoot, float knee, float slope)
{
	if (shoot <= -knee/2)
		return 0;
	else if (shoot >= knee/2)
		return slope * shoot;
	else
		return slope * pow(shoot + knee/2, 2) / (2*knee);
}
float UpDownLookahead::timeStep(float y_t, float x_t, float alpha)
{
	if (alpha <= 0)
		return x_t;
	else
		return alpha * y_t + (1- alpha) * x_t;
}
float UpDownLookahead::switchStep(float y_t, float x_t, float alphaAtt, float alphaRel)
{
	if (x_t > y_t)
		return timeStep(y_t, x_t, alphaAtt);
	else
		return timeStep(y_t, x_t, alphaRel);
}
void UpDownLookahead::doStateVariableBypass(int numSamples)
{
	if (y_t != 0)
		y_t *= pow(alpha(dnRelease), numSamples);
	if (ytUp != 0)
	ytUp*= pow(alpha(upRelease), numSamples);
}
void UpDownLookahead::copyToBuffer(InputBlock& inData)
{
	jassert(inData.getNumChannels() == buffer.getNumChannels());

	//Copy data
	if (inData.getNumSamples() > buffer.getNumSamples())
	{
		 //Orig
		int remaining = buffer.getNumSamples() - writePos;
		
		int inDataStart = (int)(inData.getNumSamples() - buffer.getNumSamples());
		jassert(remaining > 0);
		inData.copyTo(buffer, inDataStart, writePos, remaining);
		//writePos = buffer.numSamples() - remaining
		
		if (writePos > 0)
			inData.copyTo(buffer, inDataStart+remaining, 0, writePos);
		 
	}
	else if (writePos + inData.getNumSamples() > buffer.getNumSamples())
	{
		int remaining = buffer.getNumSamples() - writePos;
		
		inData.copyTo(buffer, 0, writePos, remaining);
		if (inData.getNumSamples() > remaining)
		{
			inData.copyTo(buffer, remaining, 0, inData.getNumSamples() - remaining);
		}
	}
	else
	{
		inData.copyTo(buffer, 0, writePos, inData.getNumSamples());
	}
}
void UpDownLookahead::bufferToOutput(InputBlock& input, OutputBlock& block)
{
	//If block longer than buffer
	if (block.getNumSamples() > buffer.getNumSamples())
	{
		int framesNeeded = (int)(block.getNumSamples() - buffer.getNumSamples());
		Util::copyBlock(input, block, 0, buffer.getNumSamples(), framesNeeded);
		
		int remaining = buffer.getNumSamples() - writePos;
		block.copyFrom(buffer, writePos, 0, remaining);
		if (writePos > 0)
		{
			block.copyFrom(buffer, 0, remaining, writePos);
		}
		
	}
	else if (writePos + block.getNumSamples() > buffer.getNumSamples())
	{
		int remaining = buffer.getNumSamples() - writePos;
		block.copyFrom(buffer, writePos, 0, remaining);
		if (block.getNumSamples() > remaining)
		{
			block.copyFrom(buffer, 0, remaining, block.getNumSamples() - remaining);
		}
	}
	else
	{
		block.copyFrom(buffer, writePos, 0, block.getNumSamples());
	}
}
void UpDownLookahead::bypassProcess(ContextNR& context, bypassParams)
{
	int numSamples = (int)context.getInput().getNumSamples();
	if (allowTail)
		bufferToOutput(context.getInput(), context.getOutput());

	if (!inputIsSilent)
	{
		copyToBuffer(context.getInput());
	}
	advanceWritePtr(numSamples);


	doStateVariableBypass(numSamples);
}
void UpDownLookahead::bypassProcess(ContextR& context, bypassParams)
{
	if (!inputIsSilent)
	{
		BlockPtr input(context);
		input->copyFrom(context.getInput());
		//	context.getOutput().clear();
		if (allowTail)
			bufferToOutput(*input, context.getOutput());
		
		copyToBuffer(*input);
		
	}
	else if (allowTail)
		bufferToOutput(context.getInput(), context.getOutput());

	int numSamples = (int)context.getInput().getNumSamples();
	advanceWritePtr(numSamples);
	doStateVariableBypass(numSamples);
}

void UpDownLookahead::_doBypass(InputBlock& input, OutputBlock& output)
{
	doStateVariableBypass((int)output.getNumSamples());
	bufferToOutput(input, output);
	
	float gain = 1;
	
	if (pregain != 0)
	{
		gain *= pow(10, pregain/20);
	}
	if (postgain!=0)
	{
		gain *= pow(10, postgain/20);
	}
	if (gain != 1)
		output *= gain;
}
void UpDownLookahead::commonProcess(InputBlock &i, OutputBlock& b, bool isRep)
{
	blockProcess(i, b, usePeak);
}

void UpDownLookahead::blockProcess(InputBlock& inData, OutputBlock& output, bool useRMS)
{
	if (upRatio >=1 && dnRatio >= 1)
	{
		_doBypass(inData, output);
	}
	else
	{
		float level;
		if (useRMS)
			level = rmsLevel(buffer);
		else
			level = peakLevel(buffer);
		
		level += pregain;
		
		if (-400 > level)//passes nan checks
		{
			_doBypass(inData, output);
		}
		else
			processAsBlock(level, inData, output);
	}
	copyToBuffer(inData);
	advanceWritePtr((int)inData.getNumSamples());
}

float UPL_sanitize(float x)
{
	if (std::isfinite(x))
		return x;
	else if (std::isinf(x))
	{
		if (x > 0)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}

void UpDownLookahead::processAsBlock(float level, InputBlock& input, OutputBlock& output)
{
	int N = (int)output.getNumSamples();
	int C = (int)output.getNumChannels();
	
	int BN = (int)buffer.getNumSamples();
		
	float upSlope = 1-upRatio;
	float dnSlope = 1-dnRatio;
	
	float alphaAttUp = alpha(upAttack);
	float alphaRelUp = alpha(upRelease);
	float alphaAttDn = alpha(dnAttack);
	float alphaRelDn = alpha(dnRelease);

	float overshoot = level - dnThresh;
	float undershoot = upThresh - level;
	
	//X_T
	float x_T, xT2;
	if (overshoot > 0)
	{
		x_T = doShoot(overshoot, dnKnee, dnSlope);
		xT2 = 0;
	}
	else if (undershoot > 0)
	{
		xT2 = doShoot(undershoot, upKnee, upSlope);
		x_T = 0;
	}
	else
	{
		x_T = doShoot(overshoot, dnKnee, dnSlope);
		xT2 = doShoot(undershoot, upKnee, upSlope);
	}
   
	float control_db, gain, inX;
	int inI, bi;
	for (int n = 0; n < N; ++n)
	{
		//Y_T
		y_t = switchStep(y_t, x_T, alphaAttDn, alphaRelDn);
		ytUp = switchStep(ytUp, xT2, alphaAttUp, alphaRelUp);
		
		control_db = pregain + postgain - y_t + ytUp;
		gain = pow(10, control_db / 20.0f);
		gain = 1;
		assert (std::isfinite(gain));
				
		inI = n+BN;
		bi = (n+writePos) % BN;
		
		for (int c=0;c< C; ++c)
		{
			if (inI < N)
			{
				inX = input.getSample(c, inI);
			}
			else
			{
				inX = buffer.getSample(c, bi);
			}
//			inX = UPL_sanitize(inX);
			output.setSample(c, n, gain * inX);
		}
	}
}

float UpDownLookahead::peakLevel(AudioBuffer<float>& input)
{
	float amp = 0;
	
	for (int c=0;c< input.getNumChannels(); ++c)
	{
		auto* data = input.getReadPointer(c);
		for (int n=0; n < input.getNumSamples(); ++n)
		{
			if (std::isfinite(data[n]))
			{
				amp = std::max(fabs(data[n]), amp);
			}
		}
	}
									//epsilon
	float level = 20 * log10(fabs(amp) + 1.e-44f);
	return level;
}
float UpDownLookahead::rmsLevel(AudioBuffer<float>& input)
{
	float amp = 0;
	for (int c=0;c< input.getNumChannels(); ++c)
	{
		auto* data = input.getReadPointer(c);
		for (int n=0; n < input.getNumSamples(); ++n)
		{
			if (std::isfinite(data[n]))
			{
				amp += pow(data[n], 2);
			}
		}
	}
	amp /= input.getNumChannels() * input.getNumSamples();
	amp = sqrt(amp);
									//epsilon
	float level = 20 * log10(fabs(amp) + 1.e-44f);
	return level;
}

float UpDownLookahead::peakLevel(InputBlock& input)
{
	float amp = 0;
	
	for (int c=0;c< input.getNumChannels(); ++c)
	{
		auto* data = input.getChannelPointer(c);
		for (int n=0; n < input.getNumSamples(); ++n)
		{
			if (std::isfinite(data[n]))
			{
				amp = std::max(fabs(data[n]), amp);
			}
		}
	}
									//epsilon
	float level = 20 * log10(fabs(amp) + 1.e-44f);
	return level;
}
float UpDownLookahead::rmsLevel(InputBlock& input)
{
	float amp = 0;
	for (int c=0;c< input.getNumChannels(); ++c)
	{
		auto* data = input.getChannelPointer(c);
		for (int n=0; n < input.getNumSamples(); ++n)
		{
			if (std::isfinite(data[n]))
			{
				amp += pow(data[n], 2);
			}
		}
	}
	amp /= input.getNumChannels() * input.getNumSamples();
	amp = sqrt(amp);
									//epsilon
	float level = 20 * log10(fabs(amp) + 1.e-44f);
	return level;
}

//Original version for side-chain
/*
void UpDownLookahead::sideChainBlockProcess(InputBlock& levelBlock, InputBlock& inData, OutputBlock& output, bool useRMS, bool isRep)
{
	jassert(levelBlock.getNumChannels() == numChannels);
	for (int c=0; c < numChannels; )
	
	if (upRatio >=1 && dnRatio >= 1)
	{
		_doBypass(inData, output, isRep);
		return;
	}
	float level;
	if (useRMS)
		level = rmsLevel(levelBlock);
	else
		level = peakLevel(levelBlock);
	level += pregain;
	
	if (-400 > level)//passes nan checks
	{
		_doBypass(inData, output, isRep);
	}
	else
		blockProcess(level, inData, output, isRep);
}*/
/*
 
 //Currently unused
 void UpDownLookahead::ffProcess(InputBlock& levelBlock, InputBlock& inData, OutputBlock& output, bool isRep)
 {
 //	auto& input = c.getInput();
 //	auto& output = c.getOutput();
	 
	 if (upRatio >=1 && dnRatio >= 1)
	 {
		 _doBypass(inData, output, isRep);
		 return;
	 }
	 
	 int N = (int)levelBlock.getNumSamples();
	 int C = (int)levelBlock.getNumChannels();
	 
		 
	 float upSlope = 1-upRatio;
	 float dnSlope = 1-dnRatio;
	 
	 float alphaAttUp = alpha(upAttack);
	 float alphaRelUp = alpha(upRelease);
	 float alphaAttDn = alpha(dnAttack);
	 float alphaRelDn = alpha(dnRelease);
			 
	 float level;
	 
	 float x_T, xT2;
	 
	 for (int n=0; n < N; ++n)
	 {
		 float x=0;
		 for (int c=0;c< C; ++c)
		 {
			 x += fabs(levelBlock.getSample(c, n));
		 }
		 x /= levelBlock.getNumChannels();
		 
		 const float eps = 1.e-40f;
		 
								 //epsilon
		 level = 20 * log10(x + eps);
		 level += pregain;
		 
		 if (level < -400)
		 {
			 y_t = alphaRelDn * y_t;
			 ytUp = alphaRelUp * ytUp;
		 }
		 else
		 {
			 
			 float overshoot = level - dnThresh;
			 float undershoot = upThresh - level;
		 
			 
			 //X_T
			 if (overshoot > 0)
			 {
				 x_T = doShoot(overshoot, dnKnee, dnSlope);
				 xT2 = 0;
			 }
			 else if (undershoot > 0)
			 {
				 xT2 = doShoot(undershoot, upKnee, upSlope);
				 x_T = 0;
			 }
			 else
			 {
				 x_T = doShoot(overshoot, dnKnee, dnSlope);
				 xT2 = doShoot(undershoot, upKnee, upSlope);
			 }
			 
			 y_t = switchStep(y_t, x_T, alphaAttDn, alphaRelDn);
			 ytUp = switchStep(ytUp, xT2, alphaAttUp, alphaRelUp);

		 }
		 float control_db = pregain + postgain + ytUp; // - y_t;
		 
		 float gain = pow(10, control_db / 20.0f);
				 
		 for (int c=0;c< C; ++c)
		 {
			 
			 output.setSample(c, n, gain * inData.getSample(c, n));
		 }
	 }
 }
 */
