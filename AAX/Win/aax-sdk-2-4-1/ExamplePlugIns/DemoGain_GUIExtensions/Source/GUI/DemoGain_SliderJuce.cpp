/*================================================================================================*/
/*
 *	Copyright 2011-2015 by Avid Technology, Inc.
 *	All rights reserved.
 *
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 */
/*================================================================================================*/

#include "DemoGain_SliderJuce.h"



DemoGain_SliderJuce::DemoGain_SliderJuce (const juce::String& componentName)
  : juce::Slider (componentName),
    mViewContainer (NULL),
    mParamID (0),
	mDefaultValue (0.0)
{
//	mDefaultValue.addListener (this);
}

DemoGain_SliderJuce::~DemoGain_SliderJuce (void)
{
//	mDefaultValue.removeListener (this);
}

void DemoGain_SliderJuce::mouseDown (const juce::MouseEvent& e)
{
	AAX_Result err = AAX_ERROR_UNIMPLEMENTED;
	
	uint32_t mods = GetAAXModifiersFromJuceMouseEvent (e);
	if (mViewContainer)
	{
		err = mViewContainer->HandleParameterMouseDown ( mParamID, mods );
	}
	
	if (err != AAX_SUCCESS)
	{
		juce::Slider::mouseDown (e);
	}
}

void DemoGain_SliderJuce::mouseUp (const juce::MouseEvent& e)
{
	AAX_Result err = AAX_ERROR_UNIMPLEMENTED;
	
	uint32_t mods = GetAAXModifiersFromJuceMouseEvent (e);
	if (mViewContainer)
	{
		err = mViewContainer->HandleParameterMouseUp ( mParamID, mods );
	}
	
	if (err != AAX_SUCCESS)
	{
		juce::Slider::mouseUp (e);
	}
}

void DemoGain_SliderJuce::mouseDrag (const juce::MouseEvent& e)
{
	AAX_Result err = AAX_ERROR_UNIMPLEMENTED;
	
	uint32_t mods = GetAAXModifiersFromJuceMouseEvent (e);
	if (mViewContainer)
	{
		err = mViewContainer->HandleParameterMouseDrag ( mParamID, mods );
	}
	
	if (err != AAX_SUCCESS)
	{
		if (mods == AAX_eModifiers_Option)
		{
			juce::Slider::setValue (mDefaultValue.getValue () );
		}
		else
		{
			juce::Slider::mouseDrag (e);
		}
	}
}

void DemoGain_SliderJuce::SetViewContainer (AAX_IViewContainer* viewContainer)
{
	mViewContainer = viewContainer;
}

void DemoGain_SliderJuce::SetParamID (AAX_CParamID paramID)
{
	mParamID = paramID;
}

void DemoGain_SliderJuce::SetDefaultValue (double value)
{
	mDefaultValue = value;
}

uint32_t DemoGain_SliderJuce::GetAAXModifiersFromJuceMouseEvent (const juce::MouseEvent& e)
{
	uint32_t aax_mods = 0;
	
	const juce::ModifierKeys m = e.mods;
	
	if (m.isAltDown())
		aax_mods |= AAX_eModifiers_Option;
	if (m.isCommandDown())
		aax_mods |= AAX_eModifiers_Command;
	if (m.isCtrlDown())
		aax_mods |= AAX_eModifiers_Control;
	
	if (m.isShiftDown())
		aax_mods |= AAX_eModifiers_Shift;
	if (m.isRightButtonDown())
		aax_mods |= AAX_eModifiers_SecondaryButton;
	
	// It is best practice to always query the host as
	// well, since the host's key handler may have
	// prevented some modifier key states from reaching
	// the plug-in.
	if (mViewContainer)
	{
		uint32_t aaxViewMods = 0;
		mViewContainer->GetModifiers (&aaxViewMods);
		aax_mods |= aaxViewMods;
	}
	
	return aax_mods;
}
