/*================================================================================================*/
/*
 *	Copyright 2010-2015 by Avid Technology, Inc.
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
#include "DemoGain_ViewComponent.h"
#include "DemoGain_Defs.h"
#include "DemoGain_Alg.h" // For meter tap IDs
#include "DemoGain_SliderJuce.h"

#include "AAX_CEffectParameters.h"
#include "AAX_IViewContainer.h"
#include "AAX_UtilsNative.h"

#define INITIAL_WIDTH	328
#define INITIAL_HEIGHT	209
#define MAX_WIDTH		500
#define MAX_HEIGHT		300

// *******************************************************************************
// ROUTINE:	DemoGain_ViewComponent
// *******************************************************************************
DemoGain_ViewComponent::DemoGain_ViewComponent( AAX_CEffectGUI * inEffectGUI )
	: AAX_JuceContentView ( juce::Colours::lightgrey, inEffectGUI )
	, mGainSlider(NULL), mGainText(NULL), mMonoText(NULL), mResizeButton(NULL), mInputMeter(0), mOutputMeter(0)
{
	using namespace juce;
	
	// Gain
	this->addAndMakeVisible (mGainSlider = new DemoGain_SliderJuce (String::empty));
	mGainSlider->SetParamID(DemoGain_GainID);
	mGainSlider->setSliderStyle (Slider::LinearVertical);
	mGainSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20 );
	mGainSlider->setBounds (115, 17, 21, 140);
	mGainSlider->setRange (0.0, 1.0, 0.01);
	if (inEffectGUI)
	{
		AAX_IEffectParameters* effectParameters = inEffectGUI->GetEffectParameters();
		if (effectParameters)
		{
			double defaultGainValue = 0.0;
			AAX_Result err = effectParameters->GetParameterDefaultNormalizedValue(DemoGain_GainID, &defaultGainValue);
			if (err == AAX_SUCCESS)
			{
				mGainSlider->SetDefaultValue (defaultGainValue);
			}
		}
	}
	mGainSlider->addListener(this);
	
	// Gain Text
	this->addAndMakeVisible (mGainText = new Label (String::empty, String::empty));
	mGainText->setEditable (true);
	mGainText->setBounds (154, 85, 48, 16);
	mGainText->addListener(this);
	
	Value& sliderValue = mGainSlider->getValueObject();
	mGainText->getTextValue().referTo(sliderValue);
	
	// Input/Output Meters
	this->addAndMakeVisible(mInputMeterText = new Label (String::empty, "Input"));
	mInputMeterText->setBounds (10, 170, 40, 15);
	this->addAndMakeVisible(mInputMeter =  new Slider(String::empty));
	mInputMeter->setSliderStyle(Slider::LinearBar);
	mInputMeter->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	mInputMeter->setBounds(58,170,260,15);
	mInputMeter->setRange(0,1);
	
	this->addAndMakeVisible(mOutputMeterText = new Label (String::empty, "Output"));
	mOutputMeterText->setBounds (10, 190, 40, 15);
	this->addAndMakeVisible(mOutputMeter =  new Slider(String::empty));
	mOutputMeter->setSliderStyle(Slider::LinearBar);
	mOutputMeter->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	mOutputMeter->setBounds(58,190,260,15);
	mOutputMeter->setRange(0,1);
	
	// Mono Text
	this->addAndMakeVisible (mMonoText = new Label (String::empty, "Mono"));
	mMonoText->setBounds (160, 104, 42, 19);
	
	// Mono Text
	this->addAndMakeVisible (mResizeButton = new TextButton (String::empty, "Resize"));
	mResizeButton->setButtonText ( "Resize" );
	mResizeButton->setBounds (20, 20, 50, 19);
	mResizeButton->addListener ( this );
	
	this->setSize (INITIAL_WIDTH, INITIAL_HEIGHT);
	this->setVisible(true);
	this->setOpaque (true);
}
	
// *******************************************************************************
// ROUTINE:	~DemoGain_ViewComponent
// *******************************************************************************
DemoGain_ViewComponent::~DemoGain_ViewComponent()
{
	if (mGainSlider)
		deleteAndZero (mGainSlider);
	if (mGainText)
		deleteAndZero (mGainText);
	if (mMonoText)
		deleteAndZero (mMonoText);
	if (mResizeButton)
		deleteAndZero (mResizeButton);
	if (mInputMeter)
		deleteAndZero (mInputMeter);
	if (mOutputMeter)
		deleteAndZero (mOutputMeter);
	if (mInputMeterText)
		deleteAndZero(mInputMeterText);
	if (mOutputMeterText)
		deleteAndZero(mOutputMeterText);
}
	
// *******************************************************************************
// ROUTINE:	UpdateGainParameter
// *******************************************************************************
void DemoGain_ViewComponent::UpdateGainParameter()
{
	AAX_IEffectParameters * effectParameters = this->GetParameters();
	if (effectParameters)
	{
		if ( mGainSlider )
		{
			double	normalizedValue = 0;
			if ( effectParameters->GetParameterNormalizedValue( DemoGain_GainID, &normalizedValue ) == AAX_SUCCESS )
			{
				mGainSlider->setValue(normalizedValue);
			}
		}
		
		if ( mGainText )
		{
			AAX_CString valueString;
			if ( effectParameters->GetParameterValueString( DemoGain_GainID, &valueString, 29 ) == AAX_SUCCESS )
			{
				juce::String textValue(valueString.CString());
				mGainText->setText(textValue, false);
			}
		}
	}
}

// *******************************************************************************
// ROUTINE:	UpdateGainDefaultValue
// *******************************************************************************
void DemoGain_ViewComponent::UpdateGainDefaultValue()
{
	AAX_IEffectParameters * effectParameters = this->GetParameters();
	if (effectParameters)
	{
		if ( mGainSlider )
		{
			double	normalizedValue = 0;
			if ( effectParameters->GetParameterDefaultNormalizedValue( DemoGain_GainID, &normalizedValue ) == AAX_SUCCESS )
			{
				mGainSlider->SetDefaultValue(normalizedValue);
			}
		}
	}
}

// *******************************************************************************
// ROUTINE:	sliderDragStarted
// *******************************************************************************
void DemoGain_ViewComponent::sliderDragStarted (juce::Slider* inSlider)
{
	AAX_IEffectParameters * effectParameters = this->GetParameters();
	if ( effectParameters )
	{
		effectParameters->TouchParameter( DemoGain_GainID );
	}
}

// *******************************************************************************
// ROUTINE:	sliderValueChanged
// *******************************************************************************
void DemoGain_ViewComponent::sliderValueChanged (juce::Slider * inSlider)
{
	AAX_IEffectParameters * effectParameters = this->GetParameters();
	if ( effectParameters )
	{
		double normalizedValue = inSlider->getValue();
		effectParameters->SetParameterNormalizedValue( DemoGain_GainID, normalizedValue );
	}	
}

// *******************************************************************************
// ROUTINE:	sliderDragEnded
// *******************************************************************************
void DemoGain_ViewComponent::sliderDragEnded (juce::Slider* inSlider)
{
	AAX_IEffectParameters * effectParameters = this->GetParameters();
	if ( effectParameters )
	{
		effectParameters->ReleaseParameter( DemoGain_GainID );
	}
}

// *******************************************************************************
// ROUTINE:	labelTextChanged
// *******************************************************************************
void DemoGain_ViewComponent::labelTextChanged (juce::Label* labelThatHasChanged)
{
	AAX_IEffectParameters * effectParameters = this->GetParameters();
	if ( effectParameters && mGainSlider)
	{
		this->sliderValueChanged(mGainSlider);
	}
}

// *******************************************************************************
// ROUTINE:	buttonClicked
// *******************************************************************************
void DemoGain_ViewComponent::buttonClicked ( juce::Button * inButton )
{
	if ( inButton && mResizeButton && this->GetViewContainer () )
	{
		AAX_Point newSize ( INITIAL_HEIGHT, INITIAL_WIDTH );
		if ( this->getHeight () == INITIAL_HEIGHT )
		{
			newSize.horz = INITIAL_WIDTH;
			newSize.vert = MAX_HEIGHT;
		}
		
		this->setSize ( static_cast<int>(newSize.horz), static_cast<int>(newSize.vert) );

		if ( this->GetViewContainer()->SetViewSize ( newSize ) != AAX_SUCCESS )
			this->setSize ( INITIAL_WIDTH, INITIAL_HEIGHT );
		
		this->repaint ();
	}
}


// *******************************************************************************
// METHOD:	setControlHighlightInfo
// *******************************************************************************
AAX_Result DemoGain_ViewComponent::setControlHighlightInfo (AAX_CParamID iParameterID, AAX_CBoolean iIsHighlighted, AAX_EHighlightColor iColor)
{
	if ( AAX::IsParameterIDEqual( iParameterID, DemoGain_GainID ) )
	{
		if ( mGainSlider )
		{
			if ( iIsHighlighted )
			{
				switch ( iColor )
				{
					case AAX_eHighlightColor_Red:		mGainSlider->setColour( juce::Slider::thumbColourId, juce::Colours::red ); break;
					case AAX_eHighlightColor_Blue:		mGainSlider->setColour( juce::Slider::thumbColourId, juce::Colours::blue ); break;
					case AAX_eHighlightColor_Green:		mGainSlider->setColour( juce::Slider::thumbColourId, juce::Colours::green ); break;
					case AAX_eHighlightColor_Yellow:	mGainSlider->setColour( juce::Slider::thumbColourId, juce::Colours::yellow ); break;
					default:							mGainSlider->removeColour( juce::Slider::thumbColourId ); break;
				}
			}
			else mGainSlider->removeColour( juce::Slider::thumbColourId );
			
			mGainSlider->repaint ();
		}
	}
	
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	TimerWakeup
// *******************************************************************************
AAX_Result	DemoGain_ViewComponent::TimerWakeup()
{
	//grab the current meter values from the system. 
	float inputMeterValue, outputMeterValue;
	GetController()->GetCurrentMeterValue(cDemoGain_MeterID[eMeterTap_PreGain], &inputMeterValue);
	GetController()->GetCurrentMeterValue(cDemoGain_MeterID[eMeterTap_PostGain], &outputMeterValue);
	
	//Set the values on the sliders.
	mInputMeter->setValue(inputMeterValue);
	mOutputMeter->setValue(outputMeterValue);
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	SetViewContainer
// *******************************************************************************
void DemoGain_ViewComponent::SetViewContainer ( AAX_IViewContainer * inViewContainer )
{
	mGainSlider->SetViewContainer (inViewContainer);
	AAX_JuceContentView::SetViewContainer (inViewContainer);
}
