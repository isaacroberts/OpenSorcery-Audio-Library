//
//  SpectrumReverb.cpp
//  
//
//  Created by Isaac Roberts on 8/13/22.
//

// Created by Isaac Roberts

#include "SpectrumReverb.h"


/*
 fftconvolver::MobileConvolver convolver;
 bool hasCleared;
 bool needsUpdate=false;
 int blockSize;
protected:
 float* impulse;
 
 float lenSec;
 size_t impulse_len;
 size_t fft_len;
 
 audiofft::AudioFFT fft;
 
 float* re;
 float* im;
 
 int channels;
 */
SpectrumReverb::SpectrumReverb(float lenSec_)
: FX(), Thread("ImpulseCreationThread"),
	convolver(),
	hasCleared(true),
	needsUpdate(false),
	blockSize(-1),
	impulse(NULL),
	lenSec(lenSec_),
	impulse_len(0),
	fft_len(0),
	fft(),
	re(NULL),
	im(NULL),
	channels(-1)
{
}

void SpectrumReverb::prepare2(const ProcessSpec& spec)
{
	lock.lock();
	
	channels = spec.numChannels;
	blockSize = spec.maximumBlockSize;
	
	impulse_len = size_t(rate * lenSec);
	impulse_len = pow(2, (int)(std::ceil(log2(impulse_len))));
//	impulse_len = size_t(blockSize * std::ceil(float(impulse_len)/blockSize));
//	impulse_len = 24576;
	
	impulse = new float[impulse_len];
	
	fft_len = audiofft::AudioFFT::ComplexSize(impulse_len);
	
	re = new float[fft_len];
	im = new float[fft_len];
	
	init(impulse_len, fft_len);
	fft.init(impulse_len);	
	
	bool sxs = convolver.init(blockSize, impulse_len);

	lock.unlock();
	
	jassert(sxs);
	if (!sxs)
		setBypass(true);
	else
	{
		if (!isThreadRunning())
		{
			startThread(7);
		}
	}
}

void SpectrumReverb::process(ContextR& cnt)
{
	fx_proc_start(cnt);
	ReverbCommon::processSingleChannel(cnt, convolver);
	hasCleared=false;
}
void SpectrumReverb::process(ContextNR& cnt)
{
	fx_proc_start(cnt);
	ReverbCommon::processSingleChannel(cnt, convolver);
	hasCleared=false;
}
void SpectrumReverb::clear()
{
	convolver.clearBuffer();
	hasCleared=true;
}
void SpectrumReverb::runUpdate()
{
	if (re)
	{
		if (lock.try_lock())
		{
			createSpectra(re, im);
			fft.ifft(impulse, re, im);
			addEnv(impulse);
			ReverbCommon::normalize(impulse, impulse_len);
			needsUpdate=false;
			lock.unlock();
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
			convolver.setIR(impulse, impulse_len);
//			std::cout<<"set "<< impulse[20] << std::endl;
		}
		wait(1000);
	}
}
void SpectrumReverb::triggerUpdate()
{
	needsUpdate=true;
	notify();
}
SpectrumReverb::~SpectrumReverb()
{
	stopThread(10);
	delete[] impulse;
	delete[] re;
	delete[] im;
}


