//
//  SpectrumMeterGauge.cpp
//  
//
//  Created by Isaac Roberts on 9/17/22.
//

// Created by Isaac Roberts

#include "SpectrumMeterGauge.h"



SpectrumMeterGauge::SpectrumMeterGauge()
: FX(),
fftOrder(11),
fftSize (1<<fftOrder),
forwardFFT (fftOrder),
window (fftSize, juce::dsp::WindowingFunction<float>::hann),
fifo (new float [fftSize]),
fftData (new float [2 * fftSize])
{
}
void SpectrumMeterGauge::setOrder(int o)
{
	if (fftOrder == o) return;
	
	fftOrder = o;
	fftSize = 1<<o;
	forwardFFT = dsp::FFT(fftOrder);
	
	//TODO: Haven't tested this bad boy 
	(&window)->dsp::WindowingFunction<float>::~WindowingFunction<float>();
	new (&window) dsp::WindowingFunction<float>(fftSize, juce::dsp::WindowingFunction<float>::hann);
	
	if (fifo) delete[] fifo;
	if (fftData) delete[] fftData;
	
	fifo = new float [fftSize];
	fftData = new float [2 * fftSize];
}
//void SpectrumMeterGauge::setScopeSize(int n)
//{
//	if (scopeSize==n) return;
//
//	scopeSize = n;
//
//	if (scopeData) delete[] scopeData;
//
//	scopeData = new float[scopeSize];
//}

void SpectrumMeterGauge::prepare2(Spec& spec)
{
	
}

int SpectrumMeterGauge::getNumFreqs()
{
	return fftSize/2+1;
}
float SpectrumMeterGauge::getValue(int n)
{
	if (!std::isfinite(fftData[n]))
		return 1;
	return fftData[n];
}
float SpectrumMeterGauge::getValueDb(int n, float min, float max)
{
	if (!std::isfinite(fftData[n]))
		return max;
	if (fftData[n] <= 0)
		return min;
	float db = log10(fftData[n]) * 20;
	if (db < min) return min;
	if (db > max) return max;
	return db;
}
float SpectrumMeterGauge::getScaledDb(int n, float min, float max)
{
	if (min>=max) return 0;
	
	if (!std::isfinite(fftData[n]))
		return 1;
	
	if (fftData[n] <= 0)
		return 0;
	float db = log10(fftData[n]) * 20;
	if (db < min) return 0;
	if (db > max) return 1;
	return (db-min)/(max-min);
}

float SpectrumMeterGauge::getFrequency(int n)
{
	return  float(n)/ fftSize * rate;
}
float SpectrumMeterGauge::getL2Freq(int n)
{
	if (n==0)
		return 0;
	else return log2( getFrequency(n));
}
float SpectrumMeterGauge::getScaledL2Freq(int n, float minf)
{
	if (n==0)
		return 0;
	float maxf = rate/2;
	float f = getFrequency(n);
	if (f < minf)
		return 0;
	if (f > maxf)
		return 1;
	
	float l2f = log2(f);
	float l2min = log2(minf);
	float l2max = log2(maxf);
	
	return (l2f-l2min)/(l2max-l2min);
}

bool SpectrumMeterGauge::isDrawReady()
{
	return nextFFTBlockReady;
}

void SpectrumMeterGauge::markDrawCompleted()
{
	nextFFTBlockReady = false;
}


void SpectrumMeterGauge::pushNextSampleIntoFifo (float sample) noexcept
{
	// if the fifo contains enough data, set a flag to say
	// that the next frame should now be rendered..
	if (fifoIndex == fftSize)               // [11]
	{
		if (! nextFFTBlockReady)            // [12]
		{
			juce::zeromem (fftData+fftSize, sizeof (float)*fftSize);
			::memcpy (fftData, fifo, sizeof (float)*fftSize);
			nextFFTBlockReady = true;
		}

		fifoIndex = 0;
	}

	fifo[fifoIndex++] = sample;             // [12]
}
void SpectrumMeterGauge::startDraw()
{
	// first apply a windowing function to our data
	window.multiplyWithWindowingTable (fftData, fftSize);       // [1]

	// then render our FFT data..
	forwardFFT.performFrequencyOnlyForwardTransform (fftData);  // [2]

}

void SpectrumMeterGauge::commonProcess(InputBlock& block)
{
//	if (needsUpdate)
	{
		auto* channelData = block[0];

		for (auto i = 0; i < block.getNumSamples(); ++i)
			pushNextSampleIntoFifo (channelData[i]);
	}
}
void SpectrumMeterGauge::process (ContextR& c)
{
	commonProcess(c.getOutput());
}
void SpectrumMeterGauge::process(ContextNR& c)
{
	c.bypassProcess();
	commonProcess(c.getOutput());
}
void SpectrumMeterGauge::reset2()
{
	fifoIndex=0;
}


//Destructor
SpectrumMeterGauge::~SpectrumMeterGauge()
{
	if (fifo) delete[] fifo;
	if (fftData) delete[] fftData;
//	if (scopeData) delete[] scopeData;

}

