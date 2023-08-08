//
//  RTFastReverb.cpp
//  
//
//  Created by Isaac Roberts on 1/16/21.
//

// Created by Isaac Roberts

#include "RTFastReverb.h"

template<>
void RTFastReverb<true>::loadImpulse()
{
	for (int c=0; c < channels; ++c)
	{
		int impulse_c = ReverbCommon::getImpulseChannel(c, impulse_channels, layerMode);
		bool sxs = convolver[c].init(blockSize, blockSize*2, impulse[impulse_c], impulse_len);
		jassert(sxs);
		if (!sxs)
			setBypass(true);
	}
	hasCleared=true;
}
template<>
void RTFastReverb<false>::loadImpulse()
{
	bool sxs = convolver->init(blockSize, blockSize*2, *impulse, impulse_len);
	jassert(sxs);
	if (!sxs)
		setBypass(true);
	hasCleared=true;
}

template<>
void RTFastReverb<true>::prepare2(const ProcessSpec& spec)
{
	channels = spec.numChannels;
	blockSize = spec.maximumBlockSize;
	
	num_threads = RTJobQueue<ThreadedConvolver>::availableCpus();
	num_threads = std::min(num_threads, channels);
	num_threads = std::max(num_threads, 1);
	
	backgroundThreads = new RTJobQueue<ThreadedConvolver>[num_threads];
	
	convolver = new ThreadedConvolver[channels];
	
	for (int c= 0 ; c < channels; ++c)
	{
		int thread_num = c % num_threads;
		convolver[c].setThread(&backgroundThreads[thread_num]);
		convolver[c].prepare(blockSize, channels, rate);
	}
	
	if (impulse != NULL)
	{
		loadImpulse();
	}
	
	// Priority = 8; One less than the audio thread
	for (int t=0; t < num_threads; ++t)
		backgroundThreads[t].startThread(8);
}

template<>
void RTFastReverb<false>::prepare2(const ProcessSpec& spec)
{
	channels = spec.numChannels;
	blockSize = spec.maximumBlockSize;
	
	num_threads = 1;
	backgroundThreads = new RTJobQueue<ThreadedConvolver>[1];
	
	convolver = new ThreadedConvolver();
	
	convolver->setThread(&backgroundThreads[0]);
	convolver->prepare(blockSize, channels, rate);
	
	if (impulse != NULL)
		loadImpulse();
	
	// Priority = 8; One less than the audio thread
	backgroundThreads[0].startThread(8);
}

template<>
void RTFastReverb<true>::setImpulseFromBinary(const char* data, size_t dataSize)
{
	MemoryInputStream *inputStream = new MemoryInputStream(data, dataSize, false);
	WavAudioFormat wavFormat;
	AudioFormatReader* reader = wavFormat.createReaderFor(inputStream, false);
	
	impulse_len = (int)reader->lengthInSamples;
	
	impulse_channels = reader->numChannels;
	
	AudioBuffer<float> sample (impulse_channels, (int)impulse_len);

	reader->read (&sample, 0, (int)impulse_len, 0, true, true);
	
	delete reader;
	
	impulse = new const float*[impulse_channels];
	
	for (int c=0; c < impulse_channels; ++c)
	{
		impulse[c] = sample.getReadPointer(c);
	}
	
	loadImpulse();
}
template<>
void RTFastReverb<false>::setImpulseFromBinary(const char* data, size_t dataSize)
{
	MemoryInputStream *inputStream = new MemoryInputStream(data, dataSize, false);
	WavAudioFormat wavFormat;
	AudioFormatReader* reader = wavFormat.createReaderFor(inputStream, false);
	
	impulse_len = (int)reader->lengthInSamples;
	
	impulse_channels = 1;
	
	AudioBuffer<float> sample (impulse_channels, (int)impulse_len);

	reader->read (&sample, 0, (int)impulse_len, 0, true, false);
	
	delete reader;
	
	impulse = new const float*;
	*impulse = sample.getReadPointer(0);
	
	loadImpulse();
}


template<>
void RTFastReverb<false>::setImpulse(const float* data, size_t len)
{
	impulse = &data;
	impulse_len = len;
	impulse_channels = 0;
	if (hasBeenPrepared())
		loadImpulse();
}
template<>
void RTFastReverb<true>::setImpulse(const float** data, size_t len, int numChannels)
{
	impulse_channels = numChannels;
	impulse_len = len;
	impulse = data;
}
template<>
void RTFastReverb<true>::setImpulseChannel(const float* data, size_t len, int c, int totalC)
{
	impulse_channels = totalC;
	impulse_len = std::max(impulse_len, len);

	if (impulse == NULL)
		impulse = new const float*[impulse_channels];
	impulse[c] = data;
}

template<>
void RTFastReverb<false>::setImpulseChannel(const float* data, size_t len, int c, int totalC)
{
	setImpulse(data, len);
}
template<>
void RTFastReverb<true>::setImpulse(const float* data, size_t len)
{
	setImpulse(&data, len, 1);
}

template<>
void RTFastReverb<false>::setImpulse(const float** data, size_t len, int numChannels)
{
	assert(false);
}


template<>
void RTFastReverb<false>::process(ContextR& cnt)
{
	fx_proc_start(cnt);
	ReverbCommon::processSingleChannel(cnt, *convolver);
	hasCleared=false;
}

template<>
void RTFastReverb<false>::process(ContextNR& cnt)
{
	fx_proc_start(cnt);
	ReverbCommon::processSingleChannel(cnt, *convolver);
	hasCleared=false;
}
template<>
void RTFastReverb<true>::process(ContextR& cnt)
{
	fx_proc_start(cnt);
	ReverbCommon::processMultiChannel(cnt, convolver);
	hasCleared=false;
}

template<>
void RTFastReverb<true>::process(ContextNR& cnt)
{
	fx_proc_start(cnt);
	ReverbCommon::processMultiChannel(cnt, convolver);
	hasCleared=false;
}


template<>
void RTFastReverb<true>::clear()
{
	for (int t=0; t < num_threads; ++t)
	{
		backgroundThreads[t].cancelAllJobs();
	}
	for (int c=0; c < channels; ++c)
	{
		convolver[c].cancelJobs();
		convolver[c].queue_uncancel();
		convolver[c].clearBuffer();
	}
	hasCleared=true;
}
template<>
void RTFastReverb<false>::clear()
{
	backgroundThreads[0].cancelAllJobs();
	convolver->cancelJobs();
	convolver->queue_uncancel();
	convolver->clearBuffer();
	hasCleared=true;
}





template <>
RTFastReverb<true>::~RTFastReverb()
{
	//assume we do not own impulse samples
	
	if (convolver != NULL)
	{
//		for (int c =0 ; c < channels; ++c)
//		{
//			delete convolver[c];
//		}
		delete[] convolver;
	}
	if (backgroundThreads != NULL)
		delete[] backgroundThreads;
}
template <>
RTFastReverb<false>::~RTFastReverb()
{
	delete convolver;
	if (backgroundThreads != NULL)
		delete[] backgroundThreads;
}


