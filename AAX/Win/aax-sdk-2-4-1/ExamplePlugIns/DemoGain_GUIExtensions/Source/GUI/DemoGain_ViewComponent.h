/*================================================================================================*/
/*
 *	Copyright 2010-2015, 2019 by Avid Technology, Inc.
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
 *	\file   DemoGain_ViewComponent.h
 *
 *	\brief  DemoGain_ViewComponent class definition.
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOGAIN_VIEWCOMPONENT_H
#define DEMOGAIN_VIEWCOMPONENT_H

#include "AAX_CEffectGUI_Juce.h"

class DemoGain_SliderJuce;

//==============================================================================
class DemoGain_ViewComponent	: public AAX_JuceContentView
								, public juce::SliderListener
								, public juce::LabelListener
								, public juce::ButtonListener
{
public:	
	DemoGain_ViewComponent ( AAX_CEffectGUI * inEffectGUI );
	~DemoGain_ViewComponent() override;
	
public: ///////////////////////////////////////////// DemoGain_ViewComponent
	virtual void UpdateGainParameter();
	virtual void UpdateGainDefaultValue();

public: ///////////////////////////////////////////// AAX_JuceContentView
	AAX_Result	setControlHighlightInfo (AAX_CParamID iParameterID, AAX_CBoolean iIsHighlighted, AAX_EHighlightColor iColor) AAX_OVERRIDE;
	AAX_Result	TimerWakeup() AAX_OVERRIDE;
	
	void SetViewContainer ( AAX_IViewContainer * inViewContainer ) AAX_OVERRIDE; //TODO: notification hook would be better
	
protected:
	void sliderDragStarted (juce::Slider* inSlider) override;
	void sliderValueChanged (juce::Slider * inSlider) override;
	void sliderDragEnded (juce::Slider* inSlider) override;
	void labelTextChanged (juce::Label * labelThatHasChanged) override;
	void buttonClicked (juce::Button* button) override;
	
private:
	DemoGain_SliderJuce		* mGainSlider;
	juce::Label		* mGainText;
	juce::Label		* mMonoText;
	juce::TextButton	* mResizeButton;
	juce::Label		* mInputMeterText;
	juce::Label		* mOutputMeterText;
	juce::Slider		* mInputMeter;
	juce::Slider 	* mOutputMeter;
};	

#endif
