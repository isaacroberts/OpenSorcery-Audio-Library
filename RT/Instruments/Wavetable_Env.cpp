//
//  Wavetable_Env.cpp
//  Library - Shared Code
//
//  Created by Isaac Roberts on 7/8/20.
//  Copyright © 2020 RobertsAudio. All rights reserved.
//

// Created by Isaac Roberts


#include "Wavetable_Env.h"
#include "SampleUtil.h"

Wavetable_Env::Wavetable_Env()
: Instrument(),
table(),
baseFreq(-1),
sourceRate(44100),

/*
 Table Env:
	 float sourceLength;
	 float sourceFade;
	 
	 float attack, noteHold, noteFade;
 */
sourceLength(4),
sourceFade(.1),
attack(0),
noteHold(1),
noteFade(.1)
{
	table.setSize(1, 0);
}

Wavetable_Env::~Wavetable_Env()
{
	
}

void Wavetable_Env::setParam(int param, float set)
{
	//	SourceLength, SourceFade,
	//	Attack, Hold, Fade
	if (param == SourceLength)
		sourceLength = set;
	else if (param == SourceFade)
		sourceFade = set;
	else if (param == Attack)
		attack = set;
	else if (param == Hold)
		noteHold = set;
	else if (param == Fade)
		noteFade = set;
}
float Wavetable_Env::getParam(int param)
{
	if (param == SourceLength)
		return sourceLength;
	else if (param == SourceFade)
		return sourceFade;
	else if (param == Attack)
		return attack;
	else if (param == Hold)
		return noteHold;
	else if (param == Fade)
		return noteFade;
	else return 0;
}

Note* Wavetable_Env::getAssociatedVoice() {
	return new Voice();
}
bool Wavetable_Env::Voice::canPlaySound (RTSynthSound* s)
{
    if (Wavetable_Env* w = dynamic_cast<Wavetable_Env*>(s)) {
		return true;
    }
	else
		return false;
}
void Wavetable_Env::setBaseNote(int note)
{
	float f = pow(2, (note-69)/12.) * 440.;
	setBaseFreq(f);
}
void Wavetable_Env::setBaseFreq(float freq)
{
	baseFreq = freq;
}
void Wavetable_Env::setTable(float rate, float sampleFreq, AudioBuffer<float> data)
{
	sourceRate = rate;
	baseFreq = sampleFreq;
	table = data;
}
void Wavetable_Env::setTable(float rate, int n, int o, AudioBuffer<float> data)
{
	sourceRate = rate;
	setBaseNote(n, o);
	table = data;
}
void Wavetable_Env::clearTable()
{
	table = AudioBuffer<float>(1, 0);
}
void Wavetable_Env::Voice::onStart()
{
	floatPhase = 0;
	time = 0;
//	playing=true;
//	std::cout<<"onStart phase="<<floatPhase<<std::endl;
}
void Wavetable_Env::Voice::onStop(float velo, bool allowTail)
{
//	playing=false;
}


float Wavetable_Env::Voice::getEnv(Wavetable_Env* instr, int sample, float ratio, float rate)
{
	/*
	 float sourceLength;
	 float sourceFade;
	 float attack, noteHold, noteFade;
	 */
	float sec = (sample) / rate + time;
	if (instr->noteHold > 0 && sec > instr->noteHold)
		return 0;
	float rsec = sec / ratio;
	if (rsec > instr->sourceLength)
		return 0;
	float amp = 1;
	if (instr->attack > 0 && sec < instr->attack) {
		float g = sec/instr->attack;
		amp *= jlimit(0.f, 1.f, g);
	}
	//pct
	auto noteFade = instr->noteHold * instr->noteFade;
	auto sourceFade = instr->sourceLength * instr->sourceFade;
	const float fstart = instr->noteHold - noteFade;
	if (instr->noteHold > 0 && sec > fstart) {
		if (noteFade > 0)
		{
			float g = 1 - (sec-fstart)/noteFade;
			amp *= jlimit(0.f, 1.f, g);
		}
		else
			return 0;
	}
	const float rfstart = instr->sourceLength - sourceFade;
	if (instr->sourceLength > 0 && rsec > rfstart) {
		if (sourceFade > 0)
		{
			float g = 1 - (rsec - rfstart)/sourceFade;
			amp *= jlimit(0.f, 1.f, g);
		}
		else
			return 0;
	}
	return amp;
}
float Wavetable_Env::Voice::copyData(const float* input, float* data, float ratio, int endIx, float tablePhase, int tableSize, float volume, Wavetable_Env* instr)
{//returns phase
//	int intphase;
//	float sample;
	
	const double rate = getSampleRate();
	
	for (int n=0;n < endIx; ++n)
	{
		float env = getEnv(instr, n, ratio, rate);
		
		if (env > 0)
		{
			int intphase = (int)tablePhase;
			if (intphase >= tableSize )
				return tablePhase+1;
		
//			float remainder = tablePhase - intphase;
		
			//linear interpolation
			//TODO: Better interpolation
//			sample = (1-remainder) * input[intphase];
//			if (intphase+1 < endIx)
//			{
//				sample += input[intphase+1] * remainder;
//			}
			float sample = input[intphase];
			data[n] += sample * volume * env;
		}
		else
			return tablePhase+1;
		tablePhase += ratio;
	}
	return tablePhase;
}

void Wavetable_Env::Voice::playNote(ContextR& c, int startIx, int numSamples)
{
	
	auto* instr = dynamic_cast<Wavetable_Env*>(instrument);
	if (!instr)
		return;
	
	if (instr->noteHold > 0 && time > instr->noteHold)
	{
		clearCurrentNote();
		return;
	}
	
	const AudioBuffer<float>& table = instr->getTable();
	
	if (table.getNumChannels()==0)
		return;
	
	float baseFreq = instr->getBaseFreq();
	float ratio =  freq / baseFreq;
	ratio *= instr->getSourceSampleRate() / getSampleRate();
	bool multiC = table.getNumChannels() > 1;
	
//	output.clear();
	
	if (table.getNumSamples() < floatPhase )
	{
		return;
	}
	
	float lastPhase;
	
	const float* input = table.getReadPointer(0);
	auto& output = c.getOutput();
	
	for (int c=0;c<output.getNumChannels(); ++c)
	{
		if (multiC && c!=0)//just some little optimization
			input = table.getReadPointer(c);
		
		float* data = output[c];

		lastPhase = copyData(input, data, ratio, (int)output.getNumSamples(), floatPhase, table.getNumSamples(), velo, instr);
	}
	
	time += output.getNumSamples() / getSampleRate();
	
	floatPhase = lastPhase;
	phase = (int)floatPhase;
	
	if (floatPhase >= table.getNumSamples())
	{
//		std::cout<<"clear "<<noteNum<<std::endl;
		clearCurrentNote();
	}
//	else
//		std::cout<<floatPhase<<std::endl;
}
