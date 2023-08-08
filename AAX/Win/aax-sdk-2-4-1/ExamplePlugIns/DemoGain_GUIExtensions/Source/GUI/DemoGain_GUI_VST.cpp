/*================================================================================================*/
/*
 *	Copyright 2013-2015, 2018 by Avid Technology, Inc.
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
#include "DemoGain_GUI_VST.h"
#include "DemoGain_SliderVST.h"
#include "DemoGain_Defs.h"

#include "AAX_CEffectParameters.h"
#include "AAX_Assert.h"
#include "AAX_UtilsNative.h"

#if WINDOWS
#include "DemoGainResource.h"
#endif

#if MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

// constants
const int kSlideX = 125;
const int kSlideY = 50;

#if MAC
#define IDB_BACKGROUND                  "NoUI_background.png"
#define IDB_SLIDER_HANDLE               "NoUI_handle.png"
#define IDB_SLDR_HNDL_BLUE              "NoUI_handle_blue.png"
#define IDB_SLDR_HNDL_GRN               "NoUI_handle_green.png"
#define IDB_SLDR_HNDL_RED               "NoUI_handle_red.png"
#define IDB_SLDR_HNDL_YEL               "NoUI_handle_yellow.png"
#define IDB_SLIDER_BKGR                 "NoUI_slider.png"

namespace VSTGUI
{
    CFBundleRef gBundleRef = 0;
}


#endif



std::auto_ptr<VSTGUI::CBitmap>	DemoGain_View::m_hPIBackground(NULL);
std::auto_ptr<VSTGUI::CBitmap>	DemoGain_View::m_hSliderBack(NULL);
std::auto_ptr<VSTGUI::CBitmap>	DemoGain_View::m_hSliderHandle(NULL);
std::auto_ptr<VSTGUI::CBitmap>	DemoGain_View::m_hSliderHandleRed(NULL);
std::auto_ptr<VSTGUI::CBitmap>	DemoGain_View::m_hSliderHandleBlue(NULL);
std::auto_ptr<VSTGUI::CBitmap>	DemoGain_View::m_hSliderHandleGreen(NULL);
std::auto_ptr<VSTGUI::CBitmap>	DemoGain_View::m_hSliderHandleYellow(NULL);

// *******************************************************************************
// ROUTINE:	DemoGain_View constructor
// *******************************************************************************
DemoGain_View::DemoGain_View(void *pSystemWindow, AAX_IEffectParameters* pParams, VSTGUI::VSTGUIEditorInterface *pEditor, AAX_IViewContainer* inViewContainer)
	:VSTGUI_ContentView (pSystemWindow, pParams, pEditor),
	 mViewContainer (inViewContainer)
{
	// init Background
	VSTGUI::CRect backSize(0, 0, m_hPIBackground->getWidth(), m_hPIBackground->getHeight());
	this->setSize(m_hPIBackground->getWidth(), m_hPIBackground->getHeight());
	this->setBackground(m_hPIBackground.get());
	
	// init Vertical slider
	int minPos = kSlideY;
	int maxPos = kSlideY + static_cast<int>(m_hSliderBack->getHeight() - m_hSliderHandle->getHeight());
	VSTGUI::CPoint point(0, 0);
	VSTGUI::CPoint offset(1, 0);
	VSTGUI::CRect size(kSlideX, kSlideY,
					   kSlideX + m_hSliderBack->getWidth (), kSlideY + m_hSliderBack->getHeight ());
	
	m_hSliderCtrl = new DemoGain_SliderVST (size, this, eControl_GainSlider, minPos, maxPos,
												 m_hSliderHandle.get(), m_hSliderBack.get(), point, kBottom);
	m_hSliderCtrl->SetViewContainer (mViewContainer);
	m_hSliderCtrl->SetParamID (DemoGain_GainID);
	m_hSliderCtrl->setOffsetHandle (offset);
	m_hSliderCtrl->setMin(0.0);
	m_hSliderCtrl->setMax(1.0);
	
	double defaultValue = 0;
	AAX_IEffectParameters * effectParameters = this->getParameters();
	if (effectParameters)
	{
		effectParameters->GetParameterDefaultNormalizedValue( DemoGain_GainID, &defaultValue );	// VSTGUI: necessary for Alt+Click behavior
		m_hSliderCtrl->setDefaultValue(static_cast<float>(defaultValue));
	}
	this->addView(m_hSliderCtrl);
	
	VSTGUI::CPoint textPos(kSlideX + m_hSliderBack->getWidth () + 10, kSlideY + m_hSliderBack->getHeight ()/2 - 10);
	VSTGUI::CRect gainTextSize(textPos, VSTGUI::CPoint(45, 26));
	m_hGainText = new VSTGUI::CTextEdit(gainTextSize, this, eControl_GainText, 0, 0, 0);
	m_hGainText->setBackColor(VSTGUI::kWhiteCColor);
	m_hGainText->setFontColor(VSTGUI::kBlackCColor);
	m_hGainText->setFont(VSTGUI::kNormalFontSmall);
	
	this->addView(m_hGainText);
}

// *******************************************************************************
// ROUTINE:	valueChanged destructor
// *******************************************************************************
DemoGain_View::~DemoGain_View()
{
}

// *******************************************************************************
// ROUTINE:	LoadViewResources
// *******************************************************************************
void DemoGain_View::LoadViewResources()
{
	if (NULL == m_hPIBackground.get())
		m_hPIBackground.reset(new VSTGUI::CBitmap(IDB_BACKGROUND));
	
	if (NULL == m_hSliderBack.get())
		m_hSliderBack.reset(new VSTGUI::CBitmap(IDB_SLIDER_BKGR));
	
	if (NULL == m_hSliderHandle.get())
		m_hSliderHandle.reset(new VSTGUI::CBitmap(IDB_SLIDER_HANDLE));
	
	if (NULL == m_hSliderHandleRed.get())
		m_hSliderHandleRed.reset(new VSTGUI::CBitmap(IDB_SLDR_HNDL_RED));
	
	if (NULL == m_hSliderHandleBlue.get())
		m_hSliderHandleBlue.reset(new VSTGUI::CBitmap(IDB_SLDR_HNDL_BLUE));
	
	if (NULL == m_hSliderHandleGreen.get())
		m_hSliderHandleGreen.reset(new VSTGUI::CBitmap(IDB_SLDR_HNDL_GRN));
	
	if (NULL == m_hSliderHandleYellow.get())
		m_hSliderHandleYellow.reset(new VSTGUI::CBitmap(IDB_SLDR_HNDL_YEL));	
}

// *******************************************************************************
// ROUTINE:	valueChanged
// *******************************************************************************
void DemoGain_View::valueChanged (VSTGUI::CControl* pControl)
{
	AAX_IEffectParameters * effectParameters = this->getParameters();
	if ( effectParameters )
	{
		long tag = pControl->getTag();
		switch(tag)
		{
			case eControl_GainSlider:
			{
				double normalizedValue = pControl->getValue();
				effectParameters->SetParameterNormalizedValue( DemoGain_GainID, normalizedValue );
			} break;
				
			case (eControl_GainText):
			{
				if (m_hGainText)
				{
					VSTGUI::UTF8StringPtr text = m_hGainText->getText();
					double dValue = ::atof(text);
					effectParameters->SetParameterNormalizedValue( DemoGain_GainID, dValue );
				}
			} break;
			
			default:
			{
				AAX_ASSERT (0); // Need to add a handler for this control
			} break;
		}
	}
}

// *******************************************************************************
// ROUTINE:	controlBeginEdit
// *******************************************************************************
void DemoGain_View::controlBeginEdit (VSTGUI::CControl* pControl)
{
	AAX_IEffectParameters * effectParameters = this->getParameters();
	if ( effectParameters )
	{
		long tag = pControl->getTag();
		switch(tag)
		{
			case eControl_GainSlider:
			{
				effectParameters->TouchParameter( DemoGain_GainID );
			} break;
			default:
			{
				// No action required for other controls
			} break;
		}
	}
}

// *******************************************************************************
// ROUTINE:	controlEndEdit
// *******************************************************************************
void DemoGain_View::controlEndEdit (VSTGUI::CControl* pControl)
{
	AAX_IEffectParameters * effectParameters = this->getParameters();
	if ( effectParameters )
	{
		long tag = pControl->getTag();
		switch(tag)
		{
			case eControl_GainSlider:
			{
				effectParameters->ReleaseParameter( DemoGain_GainID );
			} break;
			default:
			{
				// No action required for other controls
			} break;
		}
	}	
}

// *******************************************************************************
// ROUTINE:	updateGainParameter
// *******************************************************************************
void DemoGain_View::updateGain()
{
	AAX_IEffectParameters * effectParameters = this->getParameters();
	if (effectParameters)
	{
		AAX_Result err = AAX_ERROR_INVALID_PARAMETER_ID;
		if ( m_hSliderCtrl )
		{
			double	normalizedValue = 0;
			err = effectParameters->GetParameterNormalizedValue( DemoGain_GainID, &normalizedValue );
			if (err == AAX_SUCCESS)
			{
				m_hSliderCtrl->setValue(static_cast<float>(normalizedValue));
				m_hSliderCtrl->invalid();
			}
		}
		
		if ( m_hGainText )
		{
			AAX_CString valueString;
			err = effectParameters->GetParameterValueString( DemoGain_GainID, &valueString, 29 );
			if ( err == AAX_SUCCESS )
			{
				m_hGainText->setText(valueString.CString());
				m_hGainText->invalid();
			}
		}
	}
}

// *******************************************************************************
// ROUTINE:	highliteGain
// *******************************************************************************
void DemoGain_View::highliteGain(AAX_CBoolean iIsHighlighted, AAX_EHighlightColor iColor)
{
	if (m_hSliderCtrl)
	{
		if (iIsHighlighted)
		{		
			switch( iColor )
			{
				case AAX_eHighlightColor_Red:
					m_hSliderCtrl->setHandle(m_hSliderHandleRed.get());
					break;
				case AAX_eHighlightColor_Blue:
					m_hSliderCtrl->setHandle(m_hSliderHandleBlue.get());
					break;
				case AAX_eHighlightColor_Green:
					m_hSliderCtrl->setHandle(m_hSliderHandleGreen.get());
					break;
				case AAX_eHighlightColor_Yellow:
					m_hSliderCtrl->setHandle(m_hSliderHandleYellow.get());
					break;
				default: // Unsupported highlight
					break;
			}
		}
		else
		{
			m_hSliderCtrl->setHandle(m_hSliderHandle.get());
		}
		
		m_hSliderCtrl->invalid();
	}	
}

#pragma mark -
// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_IEffectGUI * AAX_CALLBACK DemoGain_GUI::Create()
{
	return new DemoGain_GUI;
}

// *******************************************************************************
// ROUTINE:	DemoGain_GUI constructor
// *******************************************************************************
DemoGain_GUI::DemoGain_GUI()	
	: m_bSliderIsHighlighted(false), m_SliderColor(AAX_eHighlightColor_Red)
{
}

// *******************************************************************************
// ROUTINE:	CreateViewContents
// *******************************************************************************
void DemoGain_GUI::CreateViewContents ()
{
#if MAC
	gBundleRef = ::CFBundleGetBundleWithIdentifier(CFSTR("com.avid.aax-sdk-example.DemoGain-VST"));
	AAX_ASSERT (gBundleRef);
#endif
	DemoGain_View::LoadViewResources();
}

// *******************************************************************************
// METHOD:	CreateVSTFrame
// *******************************************************************************
void DemoGain_GUI::CreateEffectView (void *inSystemWindow)
{
	mViewComponent = new DemoGain_View(inSystemWindow, this->GetEffectParameters(), this, this->GetViewContainer());
}

// *******************************************************************************
// METHOD:	ParameterUpdated
// *******************************************************************************
AAX_Result DemoGain_GUI::ParameterUpdated ( const char * iParameterID )
{
	DemoGain_View* view = dynamic_cast<DemoGain_View *>(mViewComponent);
	if (view && ! strcmp( iParameterID, DemoGain_GainID ) )
	{
		view->updateGain();
	}
	return AAX_SUCCESS;
}	

// *******************************************************************************
//	METHOD: SetControlHighlightInfo
//
//	Called by plug-in process to notify UI that a control's highlight has been changed
// *******************************************************************************
AAX_Result DemoGain_GUI::SetControlHighlightInfo (AAX_CParamID iParameterID, AAX_CBoolean iIsHighlighted, AAX_EHighlightColor iColor)
{
	m_bSliderIsHighlighted = iIsHighlighted;
	m_SliderColor = iColor;
	
	DemoGain_View* view = dynamic_cast<DemoGain_View *>(mViewComponent);
	if (view && AAX::IsParameterIDEqual(iParameterID, DemoGain_GainID))
	{
		view->highliteGain(m_bSliderIsHighlighted, m_SliderColor);
	}
	
	return AAX_SUCCESS;
}
