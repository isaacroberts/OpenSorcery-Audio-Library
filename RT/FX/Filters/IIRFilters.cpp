//
//  IIRNotch.cpp
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/6/20.
//

// Created by Isaac Roberts


#include "IIRFilters.h"

using namespace dsp;

void IIRNotch::notifyAmt(float prev)
{
	if ((prev > 0) != (amt > 0))
	{
		_lockAndMakeFilter();
	}
}
DSPCoeff IIRNotch::makeFilter()
{
	if (amt < 0)
	{
		return IIRCoefficients::makeNotchFilter(rate, frequency, q);
	}
	else
	{
		return IIRCoefficients::makeBandPass(rate, frequency, q);
	}
}
void IIRNotch::handleAmt(InputBlock& input, OutputBlock& output)
{
	if (amt < 0)
	{
		output *= -amt;
		if (amt > -1)
			output.addProductOf(input, 1+amt);
	}
	else
	{
		output *= amt;
		output.add(input);
	}
}
DSPCoeff IIRBandPass::makeFilter()
{
	// TODO: Test how long it takes to do this vs using makePeakFilter and regenning' the filter after each Amt update
	if (amt < 0)
	{
//		return IIRCoefficients::makeNotch(rate, frequency, q);
		return IIRCoefficients::makeNotchFilter(rate, frequency, q);
	}
	else
	{
		return IIRCoefficients::makeBandPass(rate, frequency, q);
	}
}
void IIRBandPass::handleAmt(InputBlock& input, OutputBlock& output)
{
	if (amt < 1)
	{
		output *= amt;
		output.addProductOf(input, 1-amt);
	}
}
DSPCoeff IIRLowPass::makeFilter()
{
	if (frequency <= 0)
	{
		//IIRCoeff = Silence
		return IIRCoefficients();
	}
	else if (frequency > rate * .5)
	{
		return IIRCoefficients::makeBypass();
	}
	else {
		return IIRCoefficients::makeLowPass(rate, frequency, q);
	}
}
void IIRLowPass::handleAmt(InputBlock &input, OutputBlock &output)
{
	if (amt < 1)
	{
		output *= amt;
		output.addProductOf(input, 1-amt);
	}
}

DSPCoeff IIRHighPass::makeFilter()
{
	if (frequency <= 0)
	{
		return IIRCoefficients::makeBypass();
	}
	else if (frequency > rate * .5)
	{
		//IIRCoeff = Silence
		return IIRCoefficients();
	}
	else
		return IIRCoefficients::makeHighPass(rate, frequency, q);
}
void IIRHighPass::handleAmt(InputBlock &input, OutputBlock &output)
{
	if (amt < 1)
	{
		output *= amt;
		output.addProductOf(input, 1-amt);
	}
}

void IIRLowShelf::notifyAmt(float prev)
{
	_lockAndMakeFilter();
}
DSPCoeff IIRLowShelf::makeFilter()
{
	if (frequency <= 0)
	{
		return IIRCoefficients::makeBypass();
	}
	else if (frequency > rate * .5)
	{
		//IIRCoeff = Silence
		return IIRCoefficients();
	}
	else {
		return IIRCoefficients::makeLowShelf(rate, frequency, q, amt);
	}
}
void IIRHighShelf::notifyAmt(float prev)
{
	_lockAndMakeFilter();
}
DSPCoeff IIRHighShelf::makeFilter()
{
	if (frequency <= 0)
	{
		//IIRCoeff = Silence
		return IIRCoefficients();
	}
	else if (frequency > rate * .5)
	{
		return IIRCoefficients::makeBypass();
	}
	else {
		
		return IIRCoefficients::makeHighShelf(rate, frequency, q, amt);
	}
}
DSPCoeff IIRAllPass::makeFilter()
{
	return IIRCoefficients::makeAllPass(rate, frequency, q);
}
void IIRAllPass::handleAmt(InputBlock &input, OutputBlock &output)
{
	if (amt < 1)
	{
		output *= amt;
		output.addProductOf(input, 1-amt);
	}
}
