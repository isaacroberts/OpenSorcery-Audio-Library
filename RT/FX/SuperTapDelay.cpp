//
//  SuperTapDelay.cpp
//  MagicVocals - Shared Code
//
//  Created by Isaac Roberts on 7/9/20.
//  Copyright © 2020 Sweetwater. All rights reserved.
//

// Created by Isaac Roberts


#include "SuperTapDelay.h"

SuperTapDelay::SuperTapDelay()
: FX()
{
	delays = NULL;
	hasCleared=true;
}
void SuperTapDelay::prepare2(Spec& spec)
{
	//Assume all presets are loaded by this point
	for (int n=0; n<delayAmt; ++n)
	{
		#if DEBUG
		if (delays[n] == NULL)
		{
			std::cerr<<"Delay "<<n<<" is null"<<std::endl;
			throw std::logic_error("Delay Amt higher than added delays");
		}
		#endif
		delays[n]->prepare(spec);
	}
	hasCleared=true;
	
		
//	if (heap1==NULL || heap2 == NULL)
//	{
//		orig = AudioBlock<float>(heap1, spec.numChannels, spec.maximumBlockSize);
//		output = AudioBlock<float>(heap2, spec.numChannels, spec.maximumBlockSize);
//	}
}
void SuperTapDelay::process(ContextR& p)
{
	//TODO: Check
	BlockPtr orig(p);
	BlockPtr output(p);
	
	auto& block = p.getOutput();
	
	orig->copyFrom(block);
	
	output->clear();
	
	float total_gain = 0;
	
	for (int n=0;n < delayAmt; ++n)
	{
		if (n!=0)
		{
			//Also copies into input
			block.copyFrom(*orig);
		}
		delays[n]->process(p);
		output->add(block);
		total_gain += delays[n]->getGain();
	}
	
	block.copyFrom(*output);
	
	if (total_gain > 0) {
		block *= 1.f / total_gain;
	}
	hasCleared=false;
}
void SingleTapDelay::process(ContextR& p)
{
	filter.process(p);

	if (allocate_shift)
	{
		if (shifter.hasBeenPrepared())
		{
			shifter.process(p);
		}
		else
		{
			jassert (shifter.hasBeenPrepared());
		}
	}
	
	if (gain != 1)
		p.getOutputBlock() *= gain;
	
	if (delay > 0)
		delayLine.process(p.getContext());
	pan.process(p);
}

void SingleTapDelay::clear()
{
	delayLine.reset();
	if (allocate_shift)
	{
		shifter.reset();
	}
}
void SuperTapDelay::clear()
{
	for (int d=0; d < delayAmt; ++d)
	{
		delays[d]->clear();
	}
	hasCleared=true;
}
void SuperTapDelay::bypassProcess(ContextR& c, bypassParams)
{
	if (!hasCleared)
	{
		clear();
	}
}
void SuperTapDelay::bypassProcess(ContextNR& c, bypassParams)
{
	if (!hasCleared)
	{
		clear();
	}
}


void SuperTapDelay::reset2()
{
	for (int n=0; n < delayAmt; ++n)
	{
		delays[n]->reset();
	}
}




SingleTapDelay::SingleTapDelay(float d, float s, float g, float f, float p)
: FX(),
savedSpec(),
delay(d),
gain(g)
{
	bpmFactor = 1.f;
	
	set(Shift, s);
	allocate_shift = (s != 0);
	set(Freq, f);
	set(Pan, p);
}
SingleTapDelay::SingleTapDelay()
: FX(),//bypass option
savedSpec(),
delay(0),
gain(1)
{
	bpmFactor = 1.f;
	set(Freq, -1);
	set(Shift, 0);
	allocate_shift = false;
//	filter.setOrder(6);
	set(Pan, 0);
}
void SuperTapDelay::setDelayAmt(int n)
{
	delayAmt = n;
	delays = new SingleTapDelay*[delayAmt];
	for (int n=0; n< delayAmt; ++n)
	{
		delays[n]=NULL;
	}
}
void SuperTapDelay::addDelay(SingleTapDelay* add)
{
	//just search it so we don't have to store any values
	for (int n=0; n< delayAmt; ++n)
	{
		if (delays[n]==NULL)
		{
			delays[n] = add;
			return;
		}
	}
	std::cerr<<"Delays OOB at "<<delayAmt<<" delays"<<std::endl;
	throw std::logic_error("Not enough delays for SuperTapDelay");
}

void SingleTapDelay::prepare2(Spec& spec)
{
	savedSpec.sampleRate = spec.sampleRate;
	savedSpec.maximumBlockSize = spec.maximumBlockSize;
	savedSpec.numChannels = spec.numChannels;
	//TODO: May need to set values after / right before preparation on pan and filter
	if (allocate_shift)
		shifter.prepare(spec);
	filter.prepare(spec);
	pan.prepare(spec);
	setDelay(delay);
	delayLine.prepare(spec);
}

void SuperTapDelay::set(int n, float set)
{
	int d = n/4;
	int p = n%4;

	delays[d]->set(p, set);
}
float SuperTapDelay::get(int n)
{
	int d = n/4;
	int p = n%4;

	return delays[d]->get(p);
}
void SuperTapDelay::setNoBPM() {
	for (int d=0; d< delayAmt; ++d)
		delays[d]->setNoBPM();
}
void SuperTapDelay::bpmChanged(double bpm, int sigNum, int sigDen) {
//	Debug::print("bpm Set", bpm);
//	std::cout<<" bpm = "<<bpm<<std::endl;
	for (int d=0; d< delayAmt; ++d)
		delays[d]->setBPM(bpm);
}

void SingleTapDelay::setNoBPM() {
	bpmFactor = 1.f;
}
void SingleTapDelay::setBPM(double bpm)
{
	bpmFactor =  4 * 60 / (bpm) ;
	Debug::print("     bpm =",bpm,"; bpm factor =",bpmFactor, "; original delay",delay,"adjusted to",delay*bpmFactor,"s");
	setDelay(delay);
}
void SingleTapDelay::setDelay(float value)
{
	delay = value;
	
	if (rate > 0)
	{
		int frameDelay = delay * bpmFactor * rate;
		
		
		if (frameDelay > 0)
		{
			auto newline = DspDelayLine(frameDelay);
			newline.setDelay(frameDelay);
			newline.prepare(savedSpec);
			delayLine = newline;
		}
	}
}
void SingleTapDelay::set(int n, float value)
{
	if (n==Delay)
	{
		setDelay(value);
	}
	else if (n==Shift)
	{
		shifter.setShift(value);
	}
	else if (n==Gain)
	{
		gain = value;
	}
	else if (n==Freq)
	{
		if (value <= 0)
		{
			filter.setBypass(true);
		}
		else {
			filter.setBypass(false);
			filter.setFreq(value);
		}
	}
	else if (n==Pan)
	{
		pan.set(0, value);
	}
}
float SingleTapDelay::get(int n)
{
	if (n==Delay)
	{
		return delay;
	}
	else if (n==Shift)
	{
		return shifter.get(MinorPitchShifter::Shift);
	}
	else if (n==Gain)
	{
		return gain;
	}
	else if (n==Freq)
	{
		return filter.get(Filter::Butter::Frequency);
	}
	else if (n==Pan)
	{
		return pan.get(0);
	}
	else
		return extraGet();
}
void SingleTapDelay::reset2()
{
	filter.reset();
	pan.reset();
	delayLine.reset();
}



SuperTapDelay::~SuperTapDelay()
{
	//AAX Will create & delete plugin before preparing
	if (delays != NULL)
	{
		for (int n=0; n<delayAmt; ++n)
		{
			delete delays[n];
		}
		delete[] delays;
	}
}
SingleTapDelay::~SingleTapDelay()
{
	
}
