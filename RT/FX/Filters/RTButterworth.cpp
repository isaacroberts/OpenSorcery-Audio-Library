//
//  RTButterworth.cpp
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/23/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

// Created by Isaac Roberts


#include "RTButterworth.h"

template <bool CanSmooth>
RTButterworth<CanSmooth>::RTButterworth(Filter::Type type_, float freq_, int order_)
: 	FX(),
	filter(nullptr),
	type(type_),
	channels(-1),
	order(order_),
	freq(freq_),
	bandwidth(1),
	gain(0),
	hasReset(false),
	mode(Filter::Unprepared)
{
}
template <bool CanSmooth>
void RTButterworth<CanSmooth>::prepare2(Spec& s)
{
	channels = s.numChannels;
//	freqSmoother.hardSet(freq);
	setMode();
	makeFilter();
	filter->snapToTargets();
	filter->setChannelAmt(channels);
}

template <bool CanSmooth>
float RTButterworth<CanSmooth>::processSingle(float x)
{
	jassert(mode != Filter::Unprepared);
	if (mode==Filter::Bypass || mode==Filter::Unprepared || filter==nullptr)
		return x;
	if (mode==Filter::Silence)
		return 0;

	hasReset=false;
	return filter->process(x);
}
template <bool CanSmooth>
void RTButterworth<CanSmooth>::process(ContextR& p)
{
	jassert(mode != Filter::Unprepared);
	
	fx_proc_start(p);
	if (mode==Filter::Bypass || mode==Filter::Unprepared)
		return;
	
	if (mode==Filter::Silence)
	{
		p.getOutputBlock().clear();
		return;
	}

	if (filter == nullptr)
		return;

	auto& output = p.getOutput();
	
//	auto f = filter;
//	f->process(output, output);
	
	filter->process(output, output);
	
	hasReset=false;
}
template <bool CanSmooth>
void RTButterworth<CanSmooth>::process(ContextNR& p)
{
	jassert(mode != Filter::Unprepared);
	fx_proc_start(p);
	if (mode==Filter::Bypass || mode==Filter::Unprepared || filter==nullptr)
	{
		p.bypassProcess();
		return;
	}

	if (mode==Filter::Silence)
	{
		p.getOutputBlock().clear();
		return;
	}
	
	auto& output = p.getOutput();
	auto& input = p.getInput();
	
	output.copyFrom(input);
	
	auto f = filter;
	f->process(input, output);
	
	hasReset=false;
}

template <bool CanSmooth>
void RTButterworth<CanSmooth>::reset2() {
	auto f = filter;
	f->reset();
}
template <bool CanSmooth>
void RTButterworth<CanSmooth>::_skipProcess(int numsamples)
{
   if (!hasReset)
   {
	   //TODO: combine with method that drives state variables towards zero
//	   reset2();
	   //TODO: Set state variables to 0 
	   hasReset=true;
   }
}
template <bool CanSmooth>
RTButterworth<CanSmooth>::~RTButterworth()
{
}



template class RTButterworth<true>;
template class RTButterworth<false>;
