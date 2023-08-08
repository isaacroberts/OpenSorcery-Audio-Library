//
//  CompressorCommon.cpp
//  
//
//  Created by Isaac Roberts on 4/5/21.
//

#include "CompressorCommon.h"

	

void Compressors::dbLevel(InputBlock& input, OutputBlock db)
{
	for (int c=0;c< input.getNumChannels(); ++c)
	{
		auto* data = input.getChannelPointer(c);
		auto* out = db[c];
		for (int n=0; n < input.getNumSamples(); ++n)
		{
			out[n] = log10(fabs(data[n])+1.e-44) * 20;
		}
	}
}

float Compressors::getSampleLevel(InputBlock& levelBlock, int n, ReadType blockType)
{
	const int C = (int)levelBlock.getNumChannels();
	float x=0;
	if (blockType == Compressors::ReadType::RMS)
	{
		for (int c=0;c< C; ++c)
		{
			x += pow(levelBlock.getSample(c, n), 2);
		}
		x /= C;
		x = sqrt(x);
	}
	else if (blockType==Compressors::ReadType::Peak)
	{
		for (int c=0;c< C; ++c)
		{
			x = std::max(fabs(levelBlock[c][n]), x);
		}
	}
	else if (blockType==Compressors::ReadType::Mean)
	{
		for (int c=0;c< C; ++c)
		{
			x += fabs(levelBlock[c][n]);
		}
		x /= C;
	}
	else jassertfalse;
	//Compute db from level
	float level;
	const float eps = 1.e-30f;//epsilon
	if (x < eps)
		level = -600;
	else
		level = 20 * log10(x);
	return level;
}
float Compressors::getSampleLevel(AudioBuffer<float>& levelBlock, int n, ReadType blockType)
{
	const int C = (int)levelBlock.getNumChannels();
	float x=0;
	if (blockType == Compressors::ReadType::RMS)
	{
		for (int c=0;c< C; ++c)
		{
			x += pow(levelBlock.getSample(c, n), 2);
		}
		x /= levelBlock.getNumChannels();
		x = sqrt(x);
	}
	else if (blockType == Compressors::ReadType::Mean)
	{
		for (int c=0;c< C; ++c)
		{
			x += levelBlock.getSample(c, n);
		}
		x /= levelBlock.getNumChannels();
	}
	else // Peak
	{
		for (int c=0;c< C; ++c)
		{
			x = std::max(fabs(levelBlock.getSample(c, n)), x);
		}
	}
	//Compute db from level
	float level;
	const float eps = 1.e-30f;//epsilon
	if (x < eps)
		level = -600;
	else
		level = 20 * log10(x);
	return level;
}

float Compressors::peakSampleLevel(InputBlock& levelBlock, int n)
{
	const int C = (int)levelBlock.getNumChannels();
	float x=0;
	for (int c=0;c< C; ++c)
	{
		x = std::max(fabs(levelBlock[c][n]), x);
	}
	//Compute db from level
	float level;
	const float eps = 1.e-30f;//epsilon
	if (x < eps)
		level = -600;
	else
		level = 20 * log10(x);
	return level;
}
float Compressors::peakSampleLevel(AudioBuffer<float>& levelBlock, int n)
{
	const int C = (int)levelBlock.getNumChannels();
	float x=0;
	for (int c=0;c< C; ++c)
	{
		x = std::max(fabs(levelBlock.getSample(c, n)), x);
	}
	//Compute db from level
	float level;
	const float eps = 1.e-30f;//epsilon
	if (x < eps)
		level = -600;
	else
		level = 20 * log10(x);
	return level;
}

float Compressors::getSampleLevel(float x)
{
	//Compute db from level
	float level;
	const float eps = 1.e-30f;//epsilon
	if (x < eps)
		level = -600;
	else
		level = 20 * log10(x);
	return level;
}

float Compressors::peakLevel(InputBlock& input)
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
float Compressors::meanLevel(InputBlock& input)
{
	float amp = 0;
	
	for (int c=0;c< input.getNumChannels(); ++c)
	{
		auto* data = input.getChannelPointer(c);
		for (int n=0; n < input.getNumSamples(); ++n)
		{
			if (std::isfinite(data[n]))
			{
				amp += fabs(data[n]);
			}
		}
	}
	amp /= input.getNumChannels() * input.getNumSamples();
	//epsilon
	float level = 20 * log10(fabs(amp) + 1.e-44f);
	return level;
}
float Compressors::rmsLevel(InputBlock& buffer)
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
//	return totalRMS;
	return 20 * log10(fabs(totalRMS) + 1.e-44f);

}
float Compressors::getLevel(InputBlock& i, ReadType type)
{
	if (type == Feedforward)
	{
		jassertfalse;
		return 0;
	}
	if (type == RMS)
		return rmsLevel(i);
	if (type == Peak)
		return peakLevel(i);
	if (type == Mean)
		return meanLevel(i);
	
	jassertfalse;
	return 0;
}

void Compressors::applyGainReductionDb(InputBlock& db, InputBlock& input, OutputBlock& output, float gain)
{
	jassert(db.getNumSamples() == input.getNumSamples());
	for (int n=0; n < input.getNumSamples(); ++n)
	{
		float amp = pow(10., (db[0][n] + gain) / 20.f );
		for (int c=0; c < input.getNumChannels(); ++c)
		{
			output.setSample(c, n, input[c][n] * amp);
		}
	}
}
void Compressors::applyGainReductionDb(InputBlock& levelDb, OutputBlock& output, float gain)
{
	jassert(levelDb.getNumSamples() == output.getNumSamples());
	
	auto *db = levelDb.getChannelPointer(0);
	for (int c=0; c < output.getNumChannels(); ++c)
	{
		auto* x = output.getChannelPointer(c);
		for (int n=0; n < output.getNumSamples(); ++n)
		{
//			float sds = db[n];
			float amp = pow(10., (db[n] + gain) / 20.f );
//			if (sds > 0)
//			{
//				sds++;
//			}
			x[n] *= amp;
		}
	}
}


float Compressors::processSingleNoBallistics(float x, float threshold, float ratio, float knee, float pregain, float postgain)
{
	float level = getSampleLevel(x) + pregain;
	float overshoot = level - threshold;
	float xT = doShoot(overshoot, knee, 1-ratio);
	
	float control_db = pregain + postgain - xT;
	float gain = pow(10, control_db / 20.0f);
	return x * gain;
}



float Compressors::alpha(float input, float rate)
{
	if (input > 0)
		return exp( -1.0f / (input * rate));
	else
		return 0;
}
float Compressors::timeStep(float y_t, float x_t, float alpha)
{
	if (alpha <= 0)
		return x_t;
	else
		return alpha * y_t + (1- alpha) * x_t;
}
float Compressors::switchStep(float y_t, float x_t, float alphaAtt, float alphaRel)
{
	if (!std::isfinite(x_t))
		return y_t;
	if (x_t > y_t)
		return timeStep(y_t, x_t, alphaAtt);
	else
		return timeStep(y_t, x_t, alphaRel);
}
float Compressors::doShoot(float shoot, float knee, float slope)
{
	if (shoot <= -knee/2)
		return 0;
	else if (shoot >= knee/2)
		return slope * shoot;
	else
		return slope * pow(shoot + knee/2, 2) / (2*knee);
}

namespace Compressors
{
	
	void Common::setCompressorVar(int n, float set)
	{
		if (n==Threshold)
			threshold = set;
		else if (n==Ratio)
			ratio = set;
		else if (n==Attack)
			attack = set;
		else if (n==Release)
			release = set;
		else if (n==KneeWidth)
			kneewidth = set;
		else if (n==PreGain)
			pregain = set;
		else if (n==PostGain)
			postgain = set;
		else jassertfalse;
	}
	float Common::getCompressorVar(int n) {
		if (n==Threshold)
			return threshold;
		else if (n==Ratio)
			return ratio;
		else if (n==Attack)
			return attack;
		else if (n==Release)
			return release;
		else if (n==KneeWidth)
			return kneewidth;
		else if (n==PreGain)
			return pregain;
		else if (n==PostGain)
			return postgain;
		else {
			jassertfalse;
			return 0;
		}
	}
	
	void Common::compressorProcess(InputBlock& level, InputBlock& input, OutputBlock& output, float rate, ReadType type)
	{
		
		if (type == RMS)
			rmsProcess(level, input, output, rate);
		else if (type == Peak)
			peakProcess(level, input, output, rate);
		else if (type == Mean)
			meanProcess(level, input, output, rate);
		else if (type == Feedforward)
			ffProcess(level, input, output, rate);
			
	}
	void Common::rmsProcess(InputBlock& levelBlock, InputBlock& inData, OutputBlock& output, float rate)
	{
		float level = rmsLevel(levelBlock);
		level += pregain;
		
		if (-400 > level)//passes nan checks
		{
			doBypass(inData, output, rate);
		}
		else
			blockProcess(level, inData, output, rate);
	}
	void Common::meanProcess(InputBlock& levelBlock, InputBlock& inData, OutputBlock& output, float rate)
	{
		float level = meanLevel(levelBlock);
		level += pregain;
		
		if (-400 > level)//passes nan checks
		{
			doBypass(inData, output, rate);
		}
		else
			blockProcess(level, inData, output, rate);
	}
	void Common::peakProcess(InputBlock& levelBlock, InputBlock& inData, OutputBlock& output, float rate)
	{
		float level = peakLevel(levelBlock);
		level += pregain;
		
		if (-400 > level)//passes nan checks
		{
			doBypass(inData, output, rate);
		}
		else
			blockProcess(level, inData, output, rate);
	}

	void Common::blockProcess(float level, InputBlock& input, OutputBlock& output, float rate)
	{
		int N = (int)input.getNumSamples();
		int C = (int)input.getNumChannels();

		float alphaAtt = alpha(attack, rate);
		float alphaRel = alpha(release, rate);

		float overshoot = level - threshold;

		//X_T
		float x_T = doShoot(overshoot, kneewidth, 1-ratio);

		float control_db, gain;
		//TODO: Stop looping and apply gain to array after alpha stabilizes
		//Or, only recalculate gain if y_t has changed
		for (int n = 0; n < N; ++n)
		{
			y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);

			control_db = pregain + postgain - y_t;
			gain = pow(10.f, control_db / 20.0f);
//			jassert (std::isfinite(gain));
			for (int c=0;c< C; ++c)
			{
				output.setSample(c, n, gain * input.getSample(c, n));
			}
		}
	}
	
//	void Common::blockProcess(float level, InputBlock& input, OutputBlock& output, float rate)
//	{
//		int N = (int)input.getNumSamples();
//		int C = (int)input.getNumChannels();
//
//		float alphaAtt = alpha(attack/N, rate);
//		float alphaRel = alpha(release/N, rate);
//
//		float overshoot = level - threshold;
//
//		//X_T
//		float x_T = doShoot(overshoot, kneewidth, 1-ratio);
//
//		float control_db, gain;
//		//TODO: Stop looping and apply gain to array after alpha stabilizes
//		//Or, only recalculate gain if y_t has changed
//		y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);
//		control_db = pregain + postgain - y_t;
//		gain = pow(10.f, control_db / 20.0f);
//		for (int c=0;c< C; ++c)
//		{
//			for (int n = 0; n < N; ++n)
//			{
//				output.setSample(c, n, gain * input.getSample(c, n));
//			}
//		}
//	}

	void Common::ffProcess(InputBlock& levelBlock, InputBlock& input, OutputBlock& output, float rate)
	{
		//	auto& input = c.getInput();
		//	auto& output = c.getOutput();
		
		if (ratio >=1)
		{
			if (output != input)
			{
				output.copyFrom(input);
			}
			float g = postgain + pregain;
			if (fabs(g)>.1)
			{
				float boost = pow(10, g/20);
				output *= boost;
			}
			return;
		}
		
		//	std::cout<<"common Proc"<<std::endl;
		int N = (int)input.getNumSamples();
		int C = (int)input.getNumChannels();
		
		
		jassert (C==output.getNumChannels());
		
		float slope = 1-ratio;
		
		float alphaAtt = alpha(attack, rate);
		float alphaRel = alpha(release, rate);
			
		float x_T;
		
		for (int n=0; n < N; ++n)
		{
			float level = getSampleLevel(levelBlock, n, Mean);
			level += pregain;
			
			float overshoot = level - threshold;
			
			x_T = doShoot(overshoot, kneewidth, slope);
			y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);

			float control_db = pregain + postgain - y_t;
						
			float gain = pow(10, control_db / 20.0f);
			
			for (int c=0;c< C; ++c)
			{
				output.setSample(c, n, gain * input.getSample(c, n));
			}
		}
	}
	float Common::dbReductionSample(float level, float rate)
	{
		//TODO: Streamline these, maybe put them on object
		float slope = 1-ratio;
		
		float alphaAtt = alpha(attack, rate);
		float alphaRel = alpha(release, rate);
		
		level += pregain;
		
		float overshoot = level - threshold;
		
		float x_T = doShoot(overshoot, kneewidth, slope);
		y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);

		float control_db = pregain + postgain - y_t;

		return control_db;
	}
	float Common::dbReductionSampleWithoutPostgain(float level, float rate)
	{
		//TODO: Streamline these, maybe put them on object
		float slope = 1-ratio;
		
		float alphaAtt = alpha(attack, rate);
		float alphaRel = alpha(release, rate);
			
		level += pregain;
		
		float overshoot = level - threshold;
		
		float x_T = doShoot(overshoot, kneewidth, slope);
		y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);

		float control_db = pregain - y_t;

		return control_db;
	}

	
// =============================================================================================
// =============================== Bypasses ====================================================
// =============================================================================================
	
	void Common::compressorBypass(InputBlock& level, float rate, ReadType type)
	{
		
		if (type == RMS)
			rmsBypass(level, rate);
		else if (type == Peak)
			peakBypass(level, rate);
		else if (type == Mean)
			meanBypass(level, rate);
		else if (type == Feedforward)
			ffBypass(level, rate);
			
	}
	void Common::rmsBypass(InputBlock& levelBlock, float rate)
	{
		float level = rmsLevel(levelBlock);
		level += pregain;
		
		if (-400 > level)//passes nan checks
		{
			releaseNFrames((int)levelBlock.getNumSamples(), rate);
		}
		else
			blockBypass(level, (int)levelBlock.getNumSamples(), rate);
	}
	void Common::meanBypass(InputBlock& levelBlock, float rate)
	{
		float level = meanLevel(levelBlock);
		level += pregain;
		
		if (-400 > level)//passes nan checks
		{
			releaseNFrames((int)levelBlock.getNumSamples(), rate);
		}
		else
			blockBypass(level, (int)levelBlock.getNumSamples(), rate);
	}
	void Common::peakBypass(InputBlock& levelBlock, float rate)
	{
		float level = peakLevel(levelBlock);
		level += pregain;
		
		if (-400 > level)//passes nan checks
		{
			releaseNFrames((int)levelBlock.getNumSamples(), rate);
		}
		else
			blockBypass(level, (int)levelBlock.getNumSamples(), rate);
	}
	
	void Common::blockBypass(float level, int N, float rate)
	{
		float overshoot = level - threshold;

		//X_T
		float x_T = doShoot(overshoot, kneewidth, 1-ratio);
		
		float al;
		if (x_T > y_t)
			al = alpha(attack/N, rate);
		else
			al = alpha(release/N, rate);
		
		//I think this is right
		y_t = al * y_t + (1-al) * x_T;
		return;
		
//		for (int n = 0; n < N; ++n)
//		{
//			// alpha = e**( -1.0f / (input * rate))
//
//
//			//	return alpha * y_t + (1- alpha) * x_t;
//
//			//a = alpha; b= 1-alpha
//			//y = y * a^N + (y*a^(N-1)+bx) + (y*a^(N-2)+b^2x) + ... (y*a^1+b^(N-1)x) + (y+b^(N)x)
//			y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);
//		}
	}

	void Common::ffBypass(InputBlock& levelBlock, float rate)
	{
		//	auto& input = c.getInput();
		//	auto& output = c.getOutput();
		
		if (ratio >=1)
		{
			return;
		}
		
		//	std::cout<<"common Proc"<<std::endl;
		int N = (int)levelBlock.getNumSamples();
		
		float slope = 1-ratio;
		
		float alphaAtt = alpha(attack, rate);
		float alphaRel = alpha(release, rate);
		
		float x_T;
		
		for (int n=0; n < N; ++n)
		{
			float level = getSampleLevel(levelBlock, n, Mean);
			level += pregain;
			
			float overshoot = level - threshold;
			
			x_T = doShoot(overshoot, kneewidth, slope);
			y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);
		}
	}
	
	


}
