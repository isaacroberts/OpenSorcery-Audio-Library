//
//  RTMobileReverb.cpp
//  
//
//  Created by Isaac Roberts on 1/16/21.
//

// Created by Isaac Roberts



#include "RTMobileReverb.h"


void RTMobileReverb::loadImpulse()
{
	if (hasBeenPrepared())
	{
		convolver.setIR(impulse, impulse_len);
	}
}

void RTMobileReverb::prepare2(const ProcessSpec& spec)
{
	channels = spec.numChannels;
	blockSize = spec.maximumBlockSize;
	
	impulse_len = size_t(rate * .5f);
	impulse = new float[impulse_len];
	
	bool sxs = convolver.init(blockSize, impulse_len);

	jassert(sxs);
	if (!sxs)
		setBypass(true);
}

void RTMobileReverb::createBlankImpulse(float sec)
{
	jassert(hasBeenPrepared());
	
//	impulse_len = sec * rate;
//	impulse = new float[impulse_len];
	::memset(impulse, 0, impulse_len*sizeof(float));
	impulse[0]=1;
	
	loadImpulse();
}
void RTMobileReverb::createInterestingImpulse()
{
	jassert(hasBeenPrepared());
	
	for (int n=0; n < impulse_len; ++n)
	{
		float t = float(n/impulse_len);
		impulse[n] = pow(1 - t, 2) * (Util::rand1()*2-1) * .25;
	}
	normalize();
	loadImpulse();
}

#include "RTGraphics/FileLoad.h"

void RTMobileReverb::loadImpulseFromFile(File f)
{
	float rate;
	const float maxSeconds = 2.f;
	AudioBuffer<float> sample = FileLoad::loadSample(f, rate,  false, maxSeconds);

	size_t N = std::min((size_t)sample.getNumSamples(), impulse_len);
	
//	impulse = new float[impulse_len];
	::memcpy(impulse, sample.getReadPointer(0), N * sizeof(float));
	if (N < impulse_len)
	{
		::memset(impulse+N, 0, (impulse_len-N)*sizeof(float));
	}
	
	normalize();
	loadImpulse();
}
void RTMobileReverb::setImpulseFromBinary(const char* data, size_t dataSize)
{ 
	MemoryInputStream *inputStream = new MemoryInputStream(data, dataSize, false);
	WavAudioFormat wavFormat;
	AudioFormatReader* reader = wavFormat.createReaderFor(inputStream, false);
	
	size_t N = std::min(impulse_len, (size_t)reader->lengthInSamples);
		
	AudioBuffer<float> sample (1, (int)N);

	reader->read (&sample, 0, (int)N, 0, true, false);
	
	delete reader;
	
//	impulse = new float[impulse_len];
	::memcpy(impulse, sample.getReadPointer(0), N * sizeof(float));
	if (N < impulse_len)
	{
		::memset(impulse+N, 0, (impulse_len-N)*sizeof(float));
	}

	normalize();
	loadImpulse();
}

void RTMobileReverb::setImpulse(float* data, size_t len)
{
	impulse = data;
	impulse_len = len;
	normalize();
	if (hasBeenPrepared())
		loadImpulse();
}

void RTMobileReverb::normalize()
{
	ReverbCommon::normalize(impulse, impulse_len);
}

void RTMobileReverb::process(ContextR& cnt)
{
	fx_proc_start(cnt);
	ReverbCommon::processSingleChannel(cnt, convolver);
	hasCleared=false;
}
void RTMobileReverb::process(ContextNR& cnt)
{
	fx_proc_start(cnt);
	ReverbCommon::processSingleChannel(cnt, convolver);
	hasCleared=false;
}


void RTMobileReverb::clear()
{
//	backgroundThreads.cancelAllJobs();
//	convolver.cancelJobs();
//	convolver.queue_uncancel();
	convolver.clearBuffer();
	hasCleared=true;
}

RTMobileReverb::~RTMobileReverb()
{
	delete[] impulse;
//	delete convolver;
//	if (backgroundThreads != NULL)
//		delete[] backgroundThreads;
}


