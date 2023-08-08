//
//  RTBendCompressor.cpp
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/6/20.
//

// Created by Isaac Roberts


#include "RTLimiter.h"


RTLimiter::RTLimiter()
: FX(),
gain(1),
thresh(1),
ratio(.2)
{
//	preset = new Preset("RTLimiter",
//		new Parameter*[]{
//			new DbToAmpParameter("Gain", -10, 10, 0, false),
//			new AmpParameter("Threshold", 0, 10, 1),
//			new FloatParameter("Ratio", 0, 1, .2)
//		},3);
}

RTLimiter::RTLimiter(float g, float t, float r)
: FX(),
gain(g),
thresh(t),
ratio(r)
{
}

void RTLimiter::process(ContextR& p)
{
	fx_proc_start(p);
	auto& output = p.getOutput();
	
	output *= gain;
	for (int c=0;c<output.getNumChannels(); ++c)
	{
		float *x = output.getChannelPointer(c);
		if (ratio == 0 )
		{
			for (int n=0;n<output.getNumSamples();++n)
			{
				if (x[n] > thresh)
				{
					x[n] = thresh;
				}
				else if (x[n] < -thresh)
				{
					x[n] = -thresh;
				}
			}
		}
		else
		{
			for (int n=0;n<output.getNumSamples();++n)
			{
				if (x[n] > thresh)
				{
					x[n] = thresh + (x[n]-thresh) * ratio;
				}
				else if (x[n] < -thresh)
				{
					x[n] = -thresh - (-x[n]-thresh) * ratio;
				}
			}
		}
	}
}

RTLimiter::~RTLimiter()
{
	
}

