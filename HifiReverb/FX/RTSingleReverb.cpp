//
//  RTSingleReverb.cpp
//  
//
//  Created by Isaac Roberts on 3/19/21.
//

// Created by Isaac Roberts

#include "RTSingleReverb.h"



template<>
void RTSingleReverb<true>::loadImpulse()
{
	for (int c=0; c < channels; ++c)
	{
		int impulse_c = 0;
		if (layerMode == Repeat)
			impulse_c = c % impulse_channels;
		else if (layerMode == RepeatLast)
		{
			impulse_c = c;
			if (impulse_c >= impulse_channels)
				impulse_c = impulse_channels - 1;
		}
		
		bool sxs = convolver[c].init(blockSize, impulse[impulse_c], impulse_len);
		jassert(sxs);
		if (!sxs)
			setBypass(true);
	}
	hasCleared=true;
}
template<>
void RTSingleReverb<false>::loadImpulse()
{
	bool sxs = convolver->init(blockSize, *impulse, impulse_len);
	jassert(sxs);
	if (!sxs)
		setBypass(true);
}

template<>
void RTSingleReverb<true>::prepare2(const ProcessSpec& spec)
{
	channels = spec.numChannels;
	blockSize = spec.maximumBlockSize;
			
	convolver = new fftconvolver::FFTConvolver[channels];
	
	if (impulse != NULL)
	{
		loadImpulse();
	}
}

template<>
void RTSingleReverb<false>::prepare2(const ProcessSpec& spec)
{
	channels = spec.numChannels;
	blockSize = spec.maximumBlockSize;
	
	convolver = new fftconvolver::FFTConvolver();
		
	if (impulse != NULL)
		loadImpulse();
}

template<>
void RTSingleReverb<true>::setImpulseFromBinary(const char* data, size_t dataSize)
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
void RTSingleReverb<false>::setImpulseFromBinary(const char* data, size_t dataSize)
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
void RTSingleReverb<false>::setImpulse(const float* data, size_t len)
{
	impulse = &data;
	impulse_len = len;
	impulse_channels = 0;
	if (hasBeenPrepared())
		loadImpulse();
}
template<>
void RTSingleReverb<true>::setImpulse(const float** data, size_t len, int numChannels)
{
	impulse_channels = numChannels;
	impulse_len = len;
	impulse = data;
}
template<>
void RTSingleReverb<true>::setImpulseChannel(const float* data, size_t len, int c, int totalC)
{
	impulse_channels = totalC;
	impulse_len = std::max(impulse_len, len);

	if (impulse == NULL)
		impulse = new const float*[impulse_channels];
	impulse[c] = data;
}

template<>
void RTSingleReverb<false>::setImpulseChannel(const float* data, size_t len, int c, int totalC)
{
	setImpulse(data, len);
}
template<>
void RTSingleReverb<true>::setImpulse(const float* data, size_t len)
{
	setImpulse(&data, len, 1);
}

template<>
void RTSingleReverb<false>::setImpulse(const float** data, size_t len, int numChannels)
{
	assert(false);
}


template<>
void RTSingleReverb<false>::process(ContextR& cnt)
{
	fx_proc_start(cnt);
	auto& block = cnt.getOutput();
	
	if (channels > 1)
	{
		//Combine all channels into first channel
		for (int c=1; c<channels; ++c)
		{
			for (int n=0; n < block.getNumSamples(); ++n)
			block[0][n] += block[c][n];
		}
		//Divide by number of channels
		for (int n=0; n < block.getNumSamples(); ++n)
			block[0][n] /= block.getNumChannels();
		
		//Process
		convolver->process(block[0], block[0], block.getNumSamples());
		
		//Copy first channel to all channels
		for (int c=1; c<channels; ++c)
		{
			for (int n=0; n < block.getNumSamples(); ++n)
				block[c][n] = block[0][n];
		}
	}
	else
	{
		//Else process
		convolver->process(block[0], block[0], block.getNumSamples());
	}
	hasCleared=false;
}

template<>
void RTSingleReverb<false>::process(ContextNR& cnt)
{
	fx_proc_start(cnt);
	auto& block = cnt.getInput();
	auto& output = cnt.getOutput();
	
	//TODO: double check me
	
	if (channels > 1)
	{
		//Clear first output channel
		for (int n=0; n < output.getNumSamples(); ++n)
			output[0][n] = 0;

		//Copy input channels to first output channel
		//Since input channels are read-only
		for (int c=0; c<channels; ++c)
		{
			for (int n=0; n < block.getNumSamples(); ++n)
				output[0][n] += block[c][n];
		}
		//Divide by total number
		for (int n=0; n < block.getNumSamples(); ++n)
			output[0][n] /= block.getNumChannels();
		
		//Process output -> output
		convolver->process(output[0], output[0], block.getNumSamples());
		
		//Copy first output to all output channels
		for (int c=1; c<channels; ++c)
		{
			for (int n=0; n < output.getNumSamples(); ++n)
				output[c][n] = output[0][n];
		}
	}
	else
	{
		convolver->process(block[0], output[0], block.getNumSamples());
	}
	hasCleared=false;
}
template<>
void RTSingleReverb<true>::process(ContextR& cnt)
{
	fx_proc_start(cnt);
	auto& block = cnt.getOutput();
	for (int c=0; c < channels; ++c)
	{
//		int c=1;
		convolver[c].process(block[c], block[c], block.getNumSamples());
	}
	hasCleared=false;
}

template<>
void RTSingleReverb<true>::process(ContextNR& cnt)
{
	fx_proc_start(cnt);
	auto& block = cnt.getInput();
	auto& output = cnt.getOutput();
	for (int c=0; c < channels; ++c)
	{
		convolver[c].process(block[c], output[c], block.getNumSamples());
	}
	hasCleared=false;
}


template<>
void RTSingleReverb<true>::clear()
{
	for (int c=0; c < channels; ++c)
	{
		convolver[c].clearBuffer();
	}
	hasCleared=true;
}
template<>
void RTSingleReverb<false>::clear()
{
	convolver->clearBuffer();
	hasCleared=true;
}





template <>
RTSingleReverb<true>::~RTSingleReverb()
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
}
template <>
RTSingleReverb<false>::~RTSingleReverb()
{
	delete convolver;
}




