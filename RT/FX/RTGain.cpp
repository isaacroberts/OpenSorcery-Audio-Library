//
//  RTGain.cpp
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/6/20.
//

// Created by Isaac Roberts


#include "RTGain.h"


RTGain::RTGain()
: FX(),
gain(1)
{
}
RTGain::RTGain(float amt)
: FX(),
gain(amt)
{
}


void RTGain::process(ContextR& c)
{
	fx_proc_start(c);
	c.getOutput() *= gain;
}
void RTGain::process(ContextNR& c)
{
	fx_proc_start(c);
	c.getOutput().replaceWithProductOf(c.getInput(), gain);
}

RTGain::~RTGain()
{
	
}

