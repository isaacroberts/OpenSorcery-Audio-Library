//
//  RTMeterGauge.cpp
//  
//
//  Created by Isaac Roberts on 2/17/21.
//

// Created by Isaac Roberts


#include "RTMeterGauge.h"


MeterGaugeBase::MeterGaugeBase(float mn, float mx, float refreshHz, float smoothTime, MeterTrackMode mode)
: min(mn), max(mx),
refreshHz(refreshHz),
smoothTime(smoothTime),
trackMode(mode),
amp(0),
refreshSamples(-1),
curSample(0)
{
	
}


float MeterGaugeBase::getDb()
{
	if (smoothDb <= 0)
		return min;
	else
	{
		return std::min(smoothDb + min, max);
	}
}
void MeterGaugeBase::updateAmp(float amp)
{
	float db;
	
	if (amp<=0)
		db = 0;
	else
		db = 20 * log10(amp) - min;
	
	updateScaledDb(db);
}
void MeterGaugeBase::updateDb(float db)
{
	updateScaledDb(db-min);
}

void MeterGaugeBase::updateScaledDb(float db)
{
	if (smoothDb <= 0 && db <= 0 )
	{
		smoothDb = 0;
		return;
	}
	
	if (trackMode == Smoothed)
	{
		float alphaSmooth = exp( -1.f / (smoothTime*refreshHz));
		if (db <= 0)
		{
			smoothDb *= alphaSmooth;
			if (smoothDb < .000001)
				smoothDb = 0;
		}
		else
		{
			smoothDb = alphaSmooth * smoothDb + (1- alphaSmooth) * db;
		}
		
	}
	else if (trackMode == FallingMax)
	{
		if (db >= smoothDb)
			smoothDb = db;
		else if (smoothDb > 0)
		{
			float diff = smoothDb - db;
			
			//sec/set
			//Ref/sec

			//sec/set*R/sec = Num Refreshes/set
			
			float fall = (max-min)/(smoothTime * refreshHz);
			smoothDb -= std::min(diff, fall);
			if (smoothDb < 0) smoothDb = 0;
		}
	
	}
	else if (trackMode == NoSmooth)
	{
		smoothDb = db;
	}
	
}

void MeterGaugeBase::prepare_meter(float r)
{
	meter_rate = r;
	amp = 0;
	smoothDb = 0;
	
	//(smp/sec) / (R/sec) = smp/R
	refreshSamples = std::ceil(meter_rate/refreshHz);
}
bool MeterGaugeBase::shouldTakeReading(int numSamples)
{
	
	if (refreshSamples == -1)
	{
		jassertfalse;
//		refreshCycles = 1/refreshHz * meter_rate / input.getNumSamples();
	}
	//Only check every N cycles
	curSample += numSamples;
	
	if (curSample < refreshSamples)
	{
		return false;
	}
	else
	{
		curSample -= refreshSamples;
		return true;
	}
}

void RTMeterGauge::commonProcess(InputBlock& input, float boostDb)
{
	if (shouldTakeReading((int)input.getNumSamples()))
	{
		float newAmp = -1;
		
		if (type == RMS)
		{
			float rms = 0;
			for (int c=0; c < input.getNumChannels(); ++c)
			{
				for (int n=0; n < input.getNumSamples(); ++n)
				{
					rms += pow(input[c][n], 2);
				}
			}
			rms /= (input.getNumChannels() * input.getNumSamples());
			newAmp = sqrt(rms);
		}
		else if (type==Mean)
		{
			float mean = 0;
			for (int c=0; c < input.getNumChannels(); ++c)
			{
				for (int n=0; n < input.getNumSamples(); ++n)
				{
					mean += fabs(input[c][n]);
				}
			}
			mean /= (input.getNumChannels() * input.getNumSamples());
			newAmp = mean;
		}
		else if (type == Max)
		{
			float m = 0;
			for (int c=0; c < input.getNumChannels(); ++c)
			{
				for (int n=0; n < input.getNumSamples(); ++n)
				{
					m = std::max(m, fabs(input[c][n]));
				}
			}
			newAmp = m;
		}
		
		if (boostDb!=0)
			newAmp *= pow(10, boostDb/20);
		if (!std::isfinite(newAmp))
		{
			amp=1;
		}
		else
			amp = newAmp;
		
		updateAmp(amp);
	}
}

void MeterGaugeBase::meter_reset()
{
	amp = 0;
	curSample=0;
}



//RTMeter
//Needs to be here to keep includes sane
void RTMeter::connectGauge(MeterGaugeBase& g)
{
	min = g.getMin();
	max = g.getMax();
	callback = g.getDbFunc();
}
