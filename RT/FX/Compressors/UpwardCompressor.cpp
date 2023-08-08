//
//  UpwardCompressor.cpp
//  
//
//  Created by Isaac Roberts on 2/1/21.
//

// Created by Isaac Roberts


#include "UpwardCompressor.h"

#include <math.h>
#include <algorithm>

UpwardCompressor::UpwardCompressor()
: RTCompressor(-30, .2, .003, .1, .2, 0, 0)
{
}

UpwardCompressor::UpwardCompressor(float t, float rat, float a, float r, float kw, float pg, float g)
: RTCompressor(t, rat, a, r, kw, pg, g)
{
}

void UpwardCompressor::commonProcess(InputBlock& input, OutputBlock& output, BlockStorage& bs, bool isRep)
{
//	auto& input = c.getInput();
//	auto& output = c.getOutput();
	
	if (ratio >=1)
	{
		if (!isRep)
		{
			output.copyFrom(input);
		}
		if (postgain!=0)
		{
			float boost = pow(10, postgain/20);
			output *= boost;
		}
		return;
	}
	
	
//	std::cout<<"common Proc"<<std::endl;
	int N = (int)input.getNumSamples();
	int C = (int)input.getNumChannels();
	
	
	jassert (C==output.getNumChannels());
	
	float slope = 1-ratio;
	
	float alphaAtt;
	if (attack > 0)
		alphaAtt = exp( -1.0f / (attack * rate));
	else
		alphaAtt = 0;
	float alphaRel = 0;
	if (release > 0)
		alphaRel = exp( -1.0f / (release * rate));
	else
		alphaRel = 0;
		
	float level;
	
	float x_T;
	
	
	for (int n=0; n < N; ++n)
	{
		float x=0;
		for (int c=0;c< C; ++c)
		{
			x += input.getSample(c, n);
		}
		x /= input.getNumChannels();
		
								//epsilon
		level = 20 * log10(fabs(x) + 1.e-44f);
//		level += 40;
		
		level += pregain;
		
		float undershoot =  threshold - level;
		
		if (isinf(kneewidth))
			x_T = 0;
		else if (undershoot <= -kneewidth/2)
			x_T = 0;
		else if (undershoot >= kneewidth/2)
			x_T = slope * undershoot;
		else
			x_T = slope * pow(undershoot + kneewidth/2, 2) / (2*kneewidth);
		
		
		if (x_T > y_t)
		{
			if (alphaAtt <= 0)
				y_t = x_T;
			else
				y_t = alphaAtt * y_t + (1- alphaAtt) * x_T;
		}
		else {
			if (alphaRel <= 0)
				y_t = x_T;
			else
				y_t = alphaRel * y_t + (1- alphaRel) * x_T;
		}
		
		float control_db = pregain + postgain + y_t;
		
		
		float gain = pow(10, control_db / 20.0f);
		
		for (int c=0;c< C; ++c)
		{
			output.setSample(c, n, gain * input.getSample(c, n));
		}
	}
}
UpwardCompressor::~UpwardCompressor()
{
	
}
