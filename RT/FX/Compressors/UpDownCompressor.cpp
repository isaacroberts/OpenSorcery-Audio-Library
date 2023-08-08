//
//  UpDownCompressor.cpp
//  
//
//  Created by Isaac Roberts on 2/17/21.
//

// Created by Isaac Roberts


#include "UpDownCompressor.h"

//#include "../BufferManager.h"

#include <math.h>
#include <algorithm>

UpDownCompressor::UpDownCompressor(float ut, float urat, float uknee, float dt, float dr, float dknee, float da, float drel, float pg, float g)
:
FX(),
upThresh(ut),
upRatio(urat),
upKnee(uknee),
dnThresh(dt),
dnRatio(dr),
dnKnee(dknee),
dnAttack(da),
dnRelease(drel),
pregain(pg),
postgain(g),
timeFactor(1),
blockType(Compressors::RMS),
y_t(0)
{
}

void UpDownCompressor::prepare2(Spec& spec)
{
	y_t = 0 ;
}

UpDownCompressor::~UpDownCompressor()
{
}

//#define alphaScale .21f

#define alphaScale 1.0f

float UpDownCompressor::alphaAtt(float input)
{
	if (input > 0)
		return exp( -1.0f / (alphaScale * input * timeFactor * rate));
	else
		return 0;
}
float UpDownCompressor::alphaRel(float input)
{
	if (input > 0)
		return exp( -1.0f / (alphaScale * input * timeFactor * rate));
	else
		return 0;
}
float UpDownCompressor::doShoot(float shoot, float knee, float slope)
{//TODO: Nervous that this is different for up and down
	const float kneeHalf = knee/2;
	if (shoot <= -kneeHalf)
		return 0;
	else if (shoot >= kneeHalf)
		return slope * shoot;
	else
		return slope * pow(shoot + kneeHalf, 2) / (2*knee);
}
float UpDownCompressor::timeStep(float y_t, float x_t, float alpha)
{
	if (alpha <= 0)
		return x_t;
	else
		return alpha * y_t + (1- alpha) * x_t;
}
float UpDownCompressor::switchStep(float y_t, float x_t, float alphaAtt, float alphaRel)
{
	if (x_t > y_t)
		return timeStep(y_t, x_t, alphaAtt);
	else
		return timeStep(y_t, x_t, alphaRel);
}
//const int upAlpha = exp( -1.0f / (.1 * rate));
//#define upAtk exp( -1.0f / (.1 * rate))
//#define upAtk alphaRel
float UpDownCompressor::ballistics(float level, float& alphaAtt, float& alphaRel)
{
//	float overshoot = level - dnThresh;
//	float undershoot = upThresh - level;
//	//X_T
//	float x_T, xT2;
//	x_T = doShoot(overshoot, dnKnee, 1-dnRatio);
//	xT2 = doShoot(undershoot, upKnee, 1-upRatio);
//
//	xT2 = std::min(xT2, 36.1f);
//
//	y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);
//	y_t2 = switchStep(y_t2, xT2, alphaRel, alphaAtt);
//
////	return y_t-y_t2;
//	float yy = y_t - y_t2;
// 	yy = std::max(-36.1f, yy);
// 	return yy;
	
	float undershoot = upThresh - level;
	float xT2 = doShoot(undershoot, upKnee, 1-upRatio);
	xT2 = std::min(xT2, 36.1f);

	y_t2 = switchStep(y_t2, xT2, alphaRel, alphaAtt);

	float overshoot = (level + y_t2) - dnThresh;
	float x_T = doShoot(overshoot, dnKnee, 1-dnRatio);
	y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);

	float yy = y_t - y_t2;
	yy = std::max(-36.1f, yy);
	return yy;
}
float UpDownCompressor::passBallistics(float& alphaAtt, float& alphaRel)
{
	y_t = switchStep(y_t, 0, alphaAtt, alphaRel);
	y_t2 = switchStep(y_t2, 0, alphaRel, alphaAtt);
	
	float yy = y_t - y_t2;
	yy = std::max(-36.1f, yy);
	return yy;
}
float UpDownCompressor::passBallistics()
{
	float aa = alphaAtt(dnAttack);
	float ar = alphaRel(dnRelease);
	return passBallistics(aa, ar);
}

void UpDownCompressor::_skipProcess(int numSamples)
{
	if (y_t != 0)
		y_t *= pow(alphaRel(dnRelease), numSamples);
	y_t2 = 36.1;
}
void UpDownCompressor::_doBypass(InputBlock& input, OutputBlock& output)
{
	if (input != output)
	{
		output.copyFrom(input);
	}
	_skipProcess((int)input.getNumSamples());
	
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
void UpDownCompressor::commonProcess(InputBlock& level, InputBlock &i, OutputBlock& b)
{
	blockProcess(level, i, b, blockType==Compressors::RMS);
}

void UpDownCompressor::blockProcess(InputBlock& levelBlock, InputBlock& inData, OutputBlock& output, bool useRMS)
{
	if (upRatio >=1 && dnRatio >= 1)
	{
		_doBypass(inData, output);
		return;
	}
	float level = Compressors::getLevel(levelBlock, blockType);
	level += pregain;
	
	if (-400 > level)//passes nan checks
	{
		_doBypass(inData, output);
	}
	else
		blockProcess(level, inData, output);
}
void UpDownCompressor::getGainReductionDb(InputBlock& levelBlock, OutputBlock& dbBlock)
{
	//No pregain requested by github library
	assert(false);
//	ffDb(levelBlock, dbBlock);
	Compressors::sanitize(y_t);
}
void UpDownCompressor::applyGainReduction(InputBlock& db, InputBlock& input, OutputBlock& output)
{
	Compressors::applyGainReductionDb(db, input, output, pregain + postgain);
}
void UpDownCompressor::applyGainReduction(InputBlock& db, OutputBlock& output)
{
	Compressors::applyGainReductionDb(db, output, pregain + postgain);
}

void UpDownCompressor::ffProcess(InputBlock& levelBlock, InputBlock& input, OutputBlock& output)
{
	if (upRatio >=1 && dnRatio >= 1)
	{
		_doBypass(input, output);
		return;
	}
	
	int N = (int)input.getNumSamples();
	int C = (int)input.getNumChannels();
	
	float alphaAttDn = alphaAtt(dnAttack);
	float alphaRelDn = alphaRel(dnRelease);
			
	float level;
	float y;
	
	for (int n=0; n < N; ++n)
	{
		level = Compressors::getSampleLevel(levelBlock, n, blockType) + pregain;
		
		if (level < -200)
		{
			y = passBallistics(alphaAttDn, alphaRelDn);

			const float gain= y + pregain + postgain;//hardmax
			const float amp = pow(10.f, (gain/20));
			
			for (int c=0;c< C; ++c)
			{
				output.setSample(c, n, amp * input.getSample(c, n));
			}
		}
		else
		{
			y = ballistics(level, alphaAttDn, alphaRelDn);
			
			float control_db = pregain + postgain - y;
			float gain = pow(10, control_db / 20.0f);
			
			for (int c=0;c< C; ++c)
			{
				output.setSample(c, n, gain * input.getSample(c, n));
			}
		}
	}
	Compressors::sanitize(y_t);
}
void UpDownCompressor::ffFromDb(InputBlock& dbLevel, InputBlock& input, OutputBlock& output)
{
	if (upRatio >=1 && dnRatio >= 1)
	{
		_doBypass(input, output);
		return;
	}
	
	int N = (int)input.getNumSamples();
	int C = (int)input.getNumChannels();
	
	float alphaAttDn = alphaAtt(dnAttack);
	float alphaRelDn = alphaRel(dnRelease);
					
	const float* db = dbLevel[0];
	float y;
	
	for (int n=0; n < N; ++n)
	{
		float level = db[n] + pregain;
		
		if (level < -200)
		{
			y = passBallistics(alphaAttDn, alphaRelDn);
			
			const float gain=y + pregain + postgain;//hardmax
			const float amp = pow(10.f, (gain/20));
			
			for (int c=0;c< C; ++c)
			{
				output.setSample(c, n, amp * input.getSample(c, n));
			}
		}
		else
		{
			y = ballistics(level, alphaAttDn, alphaRelDn);
			
			float control_db = pregain + postgain - y;
			float gain = pow(10, control_db / 20.0f);
			
			for (int c=0;c< C; ++c)
			{
				output.setSample(c, n, gain * input.getSample(c, n));
			}
		}
	}
	Compressors::sanitize(y_t);
}
void UpDownCompressor::bypassFromDb(InputBlock& dbLevel)
{
	if (upRatio >=1 && dnRatio >= 1)
	{
		_skipProcess((int)dbLevel.getNumSamples());
		return;
	}
	
	int N = (int)dbLevel.getNumSamples();
	
	float alphaAttDn = alphaAtt(dnAttack);
	float alphaRelDn = alphaRel(dnRelease);
					
	const float* db = dbLevel[0];
	
	for (int n=0; n < N; ++n)
	{
		float level = db[n] + pregain;
		
		if (level < -200)
		{
			passBallistics(alphaAttDn, alphaRelDn);
		}
		else
		{
			ballistics(level, alphaAttDn, alphaRelDn);
		}
	}
	Compressors::sanitize(y_t);
}
void UpDownCompressor::computeCompression(InputBlock& input, OutputBlock& db, bool addPreAndPost)
{
	if (upRatio >=1 && dnRatio >= 1)
	{
		_skipProcess((int)input.getNumSamples());
		
		if (addPreAndPost)
			db.fill(pregain + postgain);
		else
			db.fill(0);
		return;
	}
	
	int N = (int)input.getNumSamples();
	int C = (int)input.getNumChannels();

	float alphaAttDn = alphaAtt(dnAttack);
	float alphaRelDn = alphaRel(dnRelease);
			
	float level;
		
//	const float* db = dbLevel[0];
	float y;
	
	for (int n=0; n < N; ++n)
	{
		level = Compressors::getSampleLevel(input, n, blockType);
		
		if (addPreAndPost)
			level += pregain;
		
		if (level < -200)
		{
			y = passBallistics(alphaAttDn, alphaRelDn);

			const float gain=y + pregain + postgain;//max up
			for (int c=0;c< C; ++c)
			{
				db.setSample(c, n, gain);
			}
		}
		else
		{
			y = ballistics(level, alphaAttDn, alphaRelDn);
			
			float control_db = pregain + postgain - y;
			
			for (int c=0;c< C; ++c)
				db.setSample(c, n, control_db);
		}
	}
	Compressors::sanitize(y_t);
}



void UpDownCompressor::blockProcess(float level, InputBlock& input, OutputBlock& output)
{
//	int N = (int)input.getNumSamples();
//	int C = (int)input.getNumChannels();
//
//	float upSlope = 1-upRatio;
//	float dnSlope = 1-dnRatio;
//
//	float alphaAttDn = alphaAtt(dnAttack);
//	float alphaRelDn = alphaRel(dnRelease);
//
//	float overshoot = level - dnThresh;
//	float undershoot = upThresh - level;
//
//	//X_T
//	float x_T, xT2;
//	if (overshoot > 0)
//	{
//		x_T = doShoot(overshoot, dnKnee, dnSlope);
//		xT2 = 0;
//	}
//	else if (undershoot > 0)
//	{
//		xT2 = doShoot(undershoot, upKnee, upSlope);
//		x_T = 0;
//	}
//	else
//	{
//		x_T = doShoot(overshoot, dnKnee, dnSlope);
//		xT2 = doShoot(undershoot, upKnee, upSlope);
//	}
//
//	float control_db, gain, inX;
//	for (int n = 0; n < N; ++n)
//	{
//		//Y_T
//		y_t = switchStep(y_t, x_T+xT2, alphaAttDn, alphaRelDn);
//
//		control_db = pregain + postgain - y_t;
//		gain = pow(10, control_db / 20.0f);
//		assert (std::isfinite(gain));
//		for (int c=0;c< C; ++c)
//		{
//			inX = input.getSample(c, n);
//			Compressors::sanitize(inX);
//			output.setSample(c, n, gain * inX);
//		}
//	}
}
