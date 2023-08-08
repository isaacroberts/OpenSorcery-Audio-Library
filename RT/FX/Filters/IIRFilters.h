//
//  IIRNotch.h
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/6/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include "IIRBase.h"


class IIRNotch : public RT_IIR
{
public:
	IIRNotch() : RT_IIR() {}
	IIRNotch(float f) : RT_IIR(f) {}
	IIRNotch(float f, float q, float a) : RT_IIR(f,q,a) {}
	
	virtual ~IIRNotch() {}
	
protected:
	void notifyAmt(float prev)override;
	DSPCoeff makeFilter()override;
	void handleAmt(InputBlock&, OutputBlock&)override;
	bool needsOrigBlock() override
	{
		return amt > -.999;
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IIRNotch);
};
typedef IIRNotch IIRPeak;

class IIRBandPass : public RT_IIR
{
	/*
	 In this class, Peak is exclusive and cuts frequencies - amount is mix
	 */
public:
	IIRBandPass() : RT_IIR() {}
	IIRBandPass(float f) : RT_IIR(f) {}
	IIRBandPass(float f, float q, float a) : RT_IIR(f,q,a) {}
	
	virtual ~IIRBandPass() {}
	
protected:
	void notifyAmt(float prev)override;
	DSPCoeff makeFilter()override;
	void handleAmt(InputBlock&, OutputBlock&)override;
	bool needsOrigBlock() override
	{
		return amt < 1;
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IIRBandPass);
};

class IIRLowPass : public RT_IIR
{
public:
	IIRLowPass() : RT_IIR() {}
	IIRLowPass(float f) : RT_IIR(f) {}
	IIRLowPass(float f, float q, float a) : RT_IIR(f,q,a) {}
	virtual ~IIRLowPass() {}
	
protected:
	DSPCoeff makeFilter()override;
	void handleAmt(InputBlock&, OutputBlock&)override;
	bool needsOrigBlock() override
	{
		return amt < .999;
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IIRLowPass);
};

class IIRHighPass : public RT_IIR
{
public:
	IIRHighPass() : RT_IIR() {}
	IIRHighPass(float f) : RT_IIR(f) {}
	IIRHighPass(float f, float q, float a) : RT_IIR(f,q,a) {}
	virtual ~IIRHighPass() {}
	
protected:
	DSPCoeff makeFilter()override;
	void handleAmt(InputBlock&, OutputBlock&)override;
	bool needsOrigBlock() override
	{
		return amt < .999;
	}
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IIRHighPass);
};

class IIRLowShelf : public RT_IIR
{
public:
	IIRLowShelf() : RT_IIR() {}
	IIRLowShelf(float f) : RT_IIR(f) {}
	IIRLowShelf(float f, float q, float a) : RT_IIR(f,q,a) {}
	virtual ~IIRLowShelf() {}
	
protected:
	void notifyAmt(float prev)override;
	DSPCoeff makeFilter()override;
	void handleAmt(InputBlock&, OutputBlock&)override {}
	bool needsOrigBlock() override { return false; }
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IIRLowShelf);
};

class IIRHighShelf : public RT_IIR
{
public:
	IIRHighShelf() : RT_IIR() {}
	IIRHighShelf(float f) : RT_IIR(f) {}
	IIRHighShelf(float f, float q, float a) : RT_IIR(f,q,a) {}
	virtual ~IIRHighShelf() {}
	
protected:
	void notifyAmt(float prev)override;
	DSPCoeff makeFilter()override;
	void handleAmt(InputBlock&, OutputBlock&)override {}
	bool needsOrigBlock() override { return false; }
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IIRHighShelf);
};

class IIRAllPass : public RT_IIR
{
public:
	IIRAllPass() : RT_IIR() {}
	IIRAllPass(float f) : RT_IIR(f) {}
	IIRAllPass(float f, float q, float a) : RT_IIR(f,q,a) {}
	virtual ~IIRAllPass() {}
	
protected:
	DSPCoeff makeFilter()override;
	void handleAmt(InputBlock&, OutputBlock&)override;
	bool needsOrigBlock() override
	{
		return amt < .999;
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IIRAllPass);
};
