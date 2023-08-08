//
//  OscInstrument.cpp
//  
//
//  Created by Isaac Roberts on 7/16/21.
//

// Created by Isaac Roberts


#include "OscInstrument.h"

#include <math.h>

using namespace Osc;

RTSynthVoice* OscInstr::getAssociatedVoice()
{
	return new OscNote();
}
bool OscNote::canPlaySound (RTSynthSound* s)
{
	if (OscInstr* si = dynamic_cast<OscInstr*>(s)) {
		return true;
	}
	else
		return false;
}
OscInstr::OscInstr(Osc::Type t)
: Instrument(), type(t) {
	adsr.attack = .03;
	adsr.decay  = .2f;
	adsr.sustain = .3f;
	adsr.release = .5f;
}


void OscInstr::setNoAdsr() {
	adsr.attack=0;
	adsr.decay = 0.1f;
	adsr.sustain=1.0f;
	adsr.release= 0.0f;
}
void OscInstr::setOnOffTesterAdsr() {
	adsr.attack=.01;
	adsr.decay = .2f;
	adsr.sustain=.5f;
	adsr.release=0.0f;
}

//float OscNote::env(float t)
//{
//	return pow(.1, t);
////	return pow(.1 * velo, t);
////	return 1 - 2.5 * t;
//}
Osc::Type OscNote::getType()
{
	return static_cast<OscInstr*>(instrument)->getType();
}


void OscNote::playNote(ContextR& buffer, int startIx, int numSamples)
{
	const int C = buffer.getNumChannels();
	const float ff = freq / getSampleRate() * 2;// * pi / pi
//	const float tf = 1 / getSampleRate() * 2;// * pi / pi
	auto& out = buffer.getOutput();
	osc_func_ptr func = Osc::getFunc(getType());
	
	int N2 = startIx + numSamples;
	
	if (!adsr.isActive())
		stopNote(0, false);
	for (int n=startIx; n < N2; ++n)
	{
//		int pn = phase+n;
		auto env = adsr.getNextSample();
		//(.66f+ .33f * velo);
		float x = env * func(ff * (phase)) * velo;
		for (int c=0; c < C; ++c)
		{
			out.addSample(c, n, x);
		}
		++phase;
	}
}
