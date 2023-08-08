//
//  RTPan.cpp
//  
//
//  Created by Isaac Roberts on 7/11/20.
//

// Created by Isaac Roberts


#include "RTPan.h"


RTPan::RTPan()
: FX(),
panner(),
pan(0)
{
}
RTPan::RTPan(float p)
: FX(),
panner(),
pan(p)
{
}

void RTPan::set(int n, float set) {
	pan=set;
	panner.setPan(set);
}
float RTPan::get(int n) {
	//pan variable exists solely for this purpose
	#ifdef DEBUG
	if (n>=1)
		return extraGet();
	#endif
	return pan;
}
void RTPan::prepare2(Spec& spec)
{
	panner.setPan(pan);
	panner.prepare(spec);
}

void RTPan::process(ContextR& p)
{
	fx_proc_start(p);
	panner.process(p.getContext());
}
void RTPan::process(ContextNR& p)
{
	fx_proc_start(p);
	panner.process(p.getContext());
}
void RTPan::reset2()
{
	panner.reset();
}


RTPan::~RTPan()
{
	
}

