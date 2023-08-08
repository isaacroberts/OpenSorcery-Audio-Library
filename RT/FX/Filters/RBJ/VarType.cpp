//
//  RTRBJ.cpp
//  
//
//  Created by Isaac Roberts on 2/23/21.
//

// Created by Isaac Roberts


#include "VarType.h"

namespace RBJ
{
VarType::VarType()
: FX(),
coeff(),
state(),
type(None_T),
status(Silence),
freq(440),
qbws(0),
gain(0)
{
	coeff.bypass();
}

void VarType::prepare2(Spec& spec)
{
	jassert (type != None_T);
	
	assert (spec.numChannels != 0);
	
	state.setNumChannels(spec.numChannels);
	
	if (checkStatus())
	{
		getCoeff(coeff);
	}
	else
	{
		if (status==Bypass)
			coeff.bypass();
		else// if (status==Silence)
			coeff.silence();
	}
	
}
bool VarType::checkStatus()
{
	if (qbws<=0)
	{
		status= Silence;
		return false;
	}
	
	if (freq > rate/2)
	{
		if (type == LowPass_T || type==LowShelf_T)
			status = Bypass;
		else if (type == HighPass_T || type==HighShelf_T)
			status = Silence;
		else if (type == PeakingEQ_T)
			status = Bypass;
		else {
			jassert(false);
			status = Bypass;
		}
		return false;
	}
	else if (freq <= 0)
	{
		if (type == LowPass_T || type == LowShelf_T)
			status = Silence;
		else if (type == HighPass_T || type == HighShelf_T)
			status = Bypass;
		else if (type == PeakingEQ_T)
			status = Bypass;
		else {
			jassert(false);
			status = Bypass;
		}
		return false;
	}
	else {
		status = Active;
		return true;
	}
}
void VarType::remakeFilter()
{
	if (hasBeenPrepared())
	{
		if (checkStatus())
		{
			Coeff newCoeff;
			getCoeff(newCoeff);
			//This will cause tearing in state variables
			coeff = newCoeff;
		}
		else
		{
			if (status==Bypass)
				coeff.bypass();
			else
				coeff.silence();
		}
	}
}
	

void VarType::setType(Type set) {
	type = set;
	remakeFilter();
}
void VarType::setParameters(float f, float q)
{
	freq = f;
	qbws = q;
	remakeFilter();
}
void VarType::setParameters(float f, float q, float g)
{
	freq = f;
	qbws = q;
//	gain = pow(10, g/20);
	gain = g;
	remakeFilter();
}
void VarType::setFreq(float set)
{
	freq = set;
	remakeFilter();
}
void VarType::setQ(float set)
{
	qbws = set;
	remakeFilter();
}
void VarType::setGain(float set)
{
	gain = set;
//	gain = pow(10, set/20);
	remakeFilter();
}


VarType::~VarType()
{
}

}
