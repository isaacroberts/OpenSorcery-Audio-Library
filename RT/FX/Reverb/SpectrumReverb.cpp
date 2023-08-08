//
//  SpectrumReverb.cpp
//  
//
//  Created by Isaac Roberts on 8/13/22.
//

// Created by Isaac Roberts

#include "SpectrumReverb.h"


/*
 dsp::Convolution convolver;
 AudioBuffer<float> buffer;
	 
 
 bool hasCleared;
 bool needsUpdate=false;
 int blockSize;
 std::mutex lock;
protected:
 float* impulse;
 
 float lenSec;
 size_t fft_len;
 
 
 float* re;
 float* im;

 */



SpectrumReverb::SpectrumReverb(float lenSec_)
: FX(), Thread("ImpulseCreationThread"),
	queue(10),
convolver(Convolution::Latency{512}, queue),
	buffer(),
	hasCleared(true),
	needsUpdate(false),
	blockSize(-1),
	lock(),
	impulse(NULL),
	lenSec(lenSec_),
	impulse_len(0),
	fft_len(0),
	fft(1),
	re(NULL),
	im(NULL)
{
}

void SpectrumReverb::prepare2(const ProcessSpec& spec)
{
//	channels = spec.numChannels;
	blockSize = spec.maximumBlockSize;
	
	if (impulse_len==0) {
		setLen(256);
	}

	convolver.prepare(spec);

//	if (!isThreadRunning())
//	{
//		startThread(7);
//	}
}
void SpectrumReverb::setLen(size_t il)
{
	int order = (int)(std::ceil(log2(il)));
	impulse_len = pow(2, order);
	fft_len = impulse_len/2;
	
	lock.lock();

	impulse = new float[impulse_len*2];
	
	fft = FFT(order);
	
	re = new float[fft_len];
	im = new float[fft_len];
	
	init(impulse_len, fft_len);
	lock.unlock();
}


void SpectrumReverb::process(ContextR& cnt)
{
	fx_proc_start(cnt);
	convolver.process(cnt.getContext());
//	ReverbCommon::processSingleChannel(cnt, convolver);
	hasCleared=false;
}
void SpectrumReverb::process(ContextNR& cnt)
{
	fx_proc_start(cnt);
	convolver.process(cnt.getContext());
//	ReverbCommon::processSingleChannel(cnt, convolver);
	hasCleared=false;
}
void SpectrumReverb::clear()
{
	convolver.reset();
	hasCleared=true;
}



void SpectrumReverb::normalize()
{
	float total=0;
	for (int n=0; n < impulse_len; ++n)
	{
		total += pow(impulse[n], 2);
	}
	total = sqrt(total);
	if (total < 1e-20)
	{
		return;
	}
	else
	{
		for (int n=0; n < impulse_len; ++n)
		{
			impulse[n] /= total;
		}
	}
}

void SpectrumReverb::runUpdate()
{
	if (re)
	{
		if (lock.try_lock())
		{
			needsUpdate=false;

			createSpectra(re, im);
			
			::memcpy(impulse, re, fft_len);
			fft.performRealOnlyInverseTransform(impulse);
			
//			fft.ifft(impulse, re, im);
			addEnv(impulse);
			normalize();
			
			lock.unlock();
			
			convolver.loadImpulseResponse(AudioBuffer<float>(&impulse, 1, (int)impulse_len), rate, Convolution::Stereo::no, Convolution::Trim::no, Convolution::Normalise::no);
		}
	}
}
void SpectrumReverb::run()
{
	while (!threadShouldExit())
	{
		if (needsUpdate)
		{
			runUpdate();
		}
		wait(-1);
	}
}
void SpectrumReverb::triggerUpdate()
{
	if (!needsUpdate) {
		needsUpdate=true;
		notify();
	}
}
SpectrumReverb::~SpectrumReverb()
{
	stopThread(10);
	delete[] impulse;
	delete[] re;
	delete[] im;
}


