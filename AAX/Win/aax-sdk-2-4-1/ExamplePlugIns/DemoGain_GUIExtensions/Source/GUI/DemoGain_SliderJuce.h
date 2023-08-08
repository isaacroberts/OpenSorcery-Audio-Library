/*================================================================================================*/
/*
 *	Copyright 2011-2015, 2019 by Avid Technology, Inc.
 *	All rights reserved.
 *
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 */

/**  
 *	\file   DemoGain_SliderJuce.h
 *
 *	\author Rob Majors
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOGAIN_SLIDERJUCE_H
#define DEMOGAIN_SLIDERJUCE_H

#include "DemoGain_Defs.h"
#include "AAX_IViewContainer.h"
#include "juce.h"

class DemoGain_SliderJuce : public juce::Slider
{
public: ///////////////////////////////////////// constructor/destructor
	explicit DemoGain_SliderJuce (const juce::String& componentName = juce::String::empty);
	~DemoGain_SliderJuce() override;
	
protected: ////////////////////////////////////// juce::Slider
	void mouseDown (const juce::MouseEvent& e) override; // HACK: Non-virtual in juce::Slider
    void mouseUp (const juce::MouseEvent& e) override;   // HACK: Non-virtual in juce::Slider
    void mouseDrag (const juce::MouseEvent& e) override; // HACK: Non-virtual in juce::Slider

public: ///////////////////////////////////////// DemoGain_SliderJuce
	void SetViewContainer (AAX_IViewContainer* viewContainer);
	void SetParamID (AAX_CParamID paramID);
	void SetDefaultValue (double value);
	
private: //////////////////////////////////////// DemoGain_SliderJuce
	uint32_t GetAAXModifiersFromJuceMouseEvent (const juce::MouseEvent& e);
	
private: //////////////////////////////////////// DemoGain_SliderJuce
	AAX_IViewContainer* mViewContainer;
	AAX_CParamID mParamID;
	juce::Value mDefaultValue;
};

#endif // DEMOGAIN_SLIDERJUCE_H
