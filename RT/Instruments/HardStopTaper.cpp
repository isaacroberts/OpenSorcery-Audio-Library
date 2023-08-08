//
//  HardStopTaper.cpp
//  
//
//  Created by Isaac Roberts on 11/14/22.
//

// Created by Isaac Roberts

#include "HardStopTaper.h"

#include "RTSynth.h"

HardStopTaper::HardStopTaper(int bufferSize)
: ringBufferSize(bufferSize),
ring(ringBufferSize)
{
	samplesSinceLastWrite=ringBufferSize;
}
void HardStopTaper::prepare(Spec& spec)
{
	ring.setNumChannels(spec.numChannels);
	ring.clear();
	samplesSinceLastWrite=ringBufferSize;
}
void HardStopTaper::gatherVoiceTaper(RTSynthVoice* voice, int alreadyCaptured)
{
	int len = ringBufferSize - alreadyCaptured;
	if (len<=0) return;
	
	ring.setWriteToRead();
	
	AudioBlock<float> block = ring.getWriteBlock1(alreadyCaptured, len);
	//Fuck
	BlockStorage b;
	if (block.getNumSamples() > 0)
	{
		b.prepare(ring.numC(), (int)block.getNumSamples());
		ContextR c(b, block);
		voice->renderTaper (c, 0, (int)block.getNumSamples(), alreadyCaptured, ringBufferSize);
	}
	
	alreadyCaptured += block.getNumSamples();
	len = ringBufferSize - alreadyCaptured;
	if (len > 0)
	{
		block = ring.getWriteBlock2(alreadyCaptured, len);
		
		if (block.getNumSamples() > 0)
		{
			b.prepare(ring.numC(), (int)block.getNumSamples());
			ContextR c(b, block);
			voice->renderTaper (c, 0, (int)block.getNumSamples(), alreadyCaptured, ringBufferSize);
		}
	}
	samplesSinceLastWrite=0;
}
void HardStopTaper::renderTaper(OutputBlock &b, int startSample, int numSamples)
{
	if (samplesSinceLastWrite < ringBufferSize)
	{
		ring.readAddAndClear(b, startSample, numSamples);
		samplesSinceLastWrite += numSamples;
	}
	else
		ring.advanceReadPtrs(numSamples);
}

void HardStopTaper::advance(int n)
{
	ring.advanceWritePtrs(n);
}

//Destructor
HardStopTaper::~HardStopTaper()
{
	
}

