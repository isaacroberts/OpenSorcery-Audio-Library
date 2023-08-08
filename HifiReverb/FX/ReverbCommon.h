//
//  ReverbCommon.h
//  
//
//  Created by Isaac Roberts on 8/13/22.
//

// Created by Isaac Roberts 

#ifndef ReverbCommon_h
#define ReverbCommon_h

#include "../HifiReverb_Common.h"

namespace ReverbCommon {
	/*
	void processSingleChannel(Context&, Convolver&);
	void processMultiChannel (Context&, Convolver[channels]);
	*/
	
	// Utility
	//Normalizes to total RMS
	void normalize(float* impulse, size_t impulse_len);
	
	
	// Channel Handling
	enum class ImpulseChannelLayerMode {
		Repeat, RepeatLast
	};
	
	inline int getImpulseChannel(int c, int impulse_channels, ImpulseChannelLayerMode layerMode) {
		if (layerMode == ImpulseChannelLayerMode::Repeat)
			return c % impulse_channels;
		else if (layerMode == ImpulseChannelLayerMode::RepeatLast)
		{
			return std::min(c, impulse_channels-1);
		}
		else  {
			jassertfalse;
			return 0;
		}
	}

	
	// Process functions
	template <class Convolver>
	void processSingleChannel(ContextR& cnt, Convolver& convolver)
	{
		auto& block = cnt.getOutput();
		int channels = cnt.getNumChannels();
		
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
			convolver.process(block[0], block[0], block.getNumSamples());
			
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
			convolver.process(block[0], block[0], block.getNumSamples());
		}
	}
	
	template <class Convolver>
	void processSingleChannel(ContextNR& cnt, Convolver& convolver)
	{
		auto& block = cnt.getInput();
		auto& output = cnt.getOutput();
		int channels = cnt.getNumChannels();

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
			convolver.process(output[0], output[0], block.getNumSamples());
			
			//Copy first output to all output channels
			for (int c=1; c<channels; ++c)
			{
				for (int n=0; n < output.getNumSamples(); ++n)
					output[c][n] = output[0][n];
			}
		}
		else
		{
			convolver.process(block[0], output[0], block.getNumSamples());
		}
	}
	
	template <class Convolver>
	void processMultiChannel(ContextR& cnt, Convolver* convolvers)
	{
		auto& block = cnt.getOutput();
		int channels = cnt.getNumChannels();
		for (int c=0; c < channels; ++c)
		{
			convolvers[c].process(block[c], block[c], block.getNumSamples());
		}
	}
	template<class Convolver>
	void processMultiChannel(ContextNR& cnt , Convolver* convolvers)
	{
		auto& block = cnt.getInput();
		auto& output = cnt.getOutput();
		int channels = cnt.getNumChannels();
		for (int c=0; c < channels; ++c)
		{
			convolvers[c].process(block[c], output[c], block.getNumSamples());
		}
	}

	
};

#endif /* ReverbCommon_h */
