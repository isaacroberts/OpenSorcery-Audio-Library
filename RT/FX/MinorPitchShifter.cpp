//
//  MinorPitchShifter.cpp
//  
//
//  Created by Isaac Roberts on 7/12/20.
//

// Created by Isaac Roberts


#include "MinorPitchShifter.h"

MinorPitchShifter::MinorPitchShifter()
: FX(),
shiftSemi(0)
{
	//parameters
	updateShiftFactor();

	overlap = 500;//TODO: Tie crossfaderange to either shiftFactor or buffer size
}
MinorPitchShifter::MinorPitchShifter(float s)
: FX(),
shiftSemi(s)
{
	updateShiftFactor();
}
void MinorPitchShifter::prepare2(Spec& spec)
{
	float lowCutFreq = 10;//hz,
//	//Covers down to 10 Hz
	//TODO: Consider making this a binary number
	blockSize = (int)( rate / lowCutFreq);

	buffer = AudioBuffer<float>(spec.numChannels, blockSize);
	buffer.clear();
	
	writePtr=0;
	readPtrA=0.0;
	readPtrB = blockSize/2;
	mode = FullB;
	crossFade = 0;
	
}
void MinorPitchShifter::reset2() {
//	lowcut.reset();
	buffer.clear();
	//TODO: when shift > 1, new samples need to be initialized with incoming sound
}
void MinorPitchShifter::updateShiftFactor() {
	shiftFactor = pow(2.f, shiftSemi/12.f);

	assert (shiftFactor < 100);
	assert (shiftFactor > .01);
}
void MinorPitchShifter::commonProcess(InputBlock& i, OutputBlock& o, BlockStorage& bs)
{
	if (shiftSemi == 0)
		return;
		
	int channels = buffer.getNumChannels();
	
	float a, b;
	
	for (int n=0; n < o.getNumSamples(); ++n)
	{
		
		//Set write pointer
		for (int c=0; c < channels; ++c )
		{
			jassert (writePtr >= 0);
			jassert (writePtr < buffer.getNumSamples());
			buffer.setSample(c, writePtr, i.getSample(c, n));
		}
		
		//Read read pointer
		for (int c=0; c < channels; ++c )
		{
			if (crossFade <= 0.01) {
				o.setSample(c, n, buffer.getSample(c, (int)readPtrA));
			}
			else if (crossFade >= .99) {
				o.setSample(c, n, buffer.getSample(c, (int)readPtrB));
			}
			else {
				a = buffer.getSample(c, (int)readPtrA);
				b = buffer.getSample(c, (int)readPtrB);
//				o.setSample( a * (1-crossFade) + (-b) * (crossFade));
				// a * (1-c) - b*c
				// a - ac - bc
				// a - c(a+b)
				o.setSample(c, n,  a - crossFade * (a+b));
			}
		}
		updatePtrs();
	}
}
void MinorPitchShifter::updatePtrs()
{
	writePtr++;
	if (writePtr >= blockSize)
		writePtr -= blockSize;
	
	readPtrA += shiftFactor;
	readPtrB += shiftFactor;
	
	//Ranges before overlap
	int diffA = writePtr - readPtrA;
	int diffB = writePtr - readPtrB;
	
	bool writeIsFaster = shiftFactor < 1;
	
	if (writeIsFaster)
	{//write pointer overtaking read ptr
		//'frames left before overtake' is negative since direction is negative
		diffA = -diffA;
		diffB = -diffB;
	}
	if (diffA < 0) diffA += blockSize;
	if (diffB < 0) diffB += blockSize;
	
	
	if (mode==FullA) {
		if (diffA < overlap) {
			mode = FallingA;
		}
	}
	else if (mode == FullB) {
		if (diffB < overlap) {
			mode = FallingB;
		}
	}
	
	if (mode == FallingA) {
		//crossFade = 0 @ blockSize-readPtrB == overlap; 1 @ readPtrB==blockSize
		crossFade = (diffA) / overlap;
		
		if ((writeIsFaster && readPtrA <  writePtr)
		 || (!writeIsFaster && readPtrA >= writePtr))
		{
			mode = FullB;
			crossFade = 1;
		}
	}
	else if (mode == FallingB) {
		//crossFade = 1 @ blockSize-readPtrB == overlap; 0 @ readPtrB==blockSize
		crossFade = 1 - (diffB) / overlap;
		//same as above but in one expression
		if ((writeIsFaster) == (readPtrB <  writePtr))
		{
			mode = FullA;
			crossFade = 0;
		}
	}
	if (readPtrA >= blockSize)
		readPtrA -= blockSize;
	if (readPtrB >= blockSize)
		readPtrB -= blockSize;
}

void MinorPitchShifter::process (ContextR& r)
{
	fx_proc_start(r);
//	lowcut.process(r);
	if (shiftSemi == 0)
		return;
	commonProcess(r.getOutput(), r.getOutput(), r.getStorage());
}
void MinorPitchShifter::process(ContextNR& s)
{
	fx_proc_start(s);
	//Lowcut moves the audio to output block
//	lowcut.process(s);
	if (shiftSemi==0)
	{
		s.bypassProcess();
		return;
//		bypassProcess(s, false, false);
	}
	else
		//commonProcess works in-place on the output block
		commonProcess(s.getOutput(), s.getOutput(), s.getStorage());
}


MinorPitchShifter::~MinorPitchShifter()
{
	
}

