/*================================================================================================*/
/*
 *	Copyright 2009-2017, 2021 by Avid Technology, Inc.
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

#include "DemoGain_GUI.h"

#include "DemoGain_Alg.h"
#include "DemoGain_Defs.h"
#include "DemoGainResource.h"

#include "AAX_IEffectParameters.h"
#include "AAX_IViewContainer.h"
#include "AAX_IController.h"
#include "AAX_CString.h"
#include "AAX_UtilsNative.h"

#include <commctrl.h>
#include <winuser.h>
#include <windowsx.h>
#include <map>
#include <set>


// GUI widget constants
static const int cMaxSliderPos = 4086;
static const int cMaxMeterBarPos = 256;

// GUI view constants
#define DEMOGAIN_ORIG_WINDOW_PROC_PROP	"DemoGain_OrigWinProc_Prop"
#define DEMOGAIN_PARAM_ID_PROP "DemoGain_Param_ID_Prop"


// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_IEffectGUI * AAX_CALLBACK DemoGain_GUI::Create ()
{
	AAX_IEffectGUI * effectGUI = new DemoGain_GUI;
	return effectGUI;
}

// *******************************************************************************
// METHOD:	DemoGain_GUI
// *******************************************************************************
DemoGain_GUI::DemoGain_GUI()
: mGainText (0),
  mGainSlider (0),
  mInputBar (0),
  mOutputBar (0),
  mGainSliderIsEditing (false)
{
}

// *******************************************************************************
// METHOD:	~DemoGain_GUI
// *******************************************************************************
DemoGain_GUI::~DemoGain_GUI()
{
}

// *******************************************************************************
// METHOD:	ParameterUpdated
// *******************************************************************************
AAX_Result DemoGain_GUI::ParameterUpdated ( AAX_CParamID iParameterID )
{
	AAX_Result result = AAX_ERROR_INVALID_PARAMETER_ID;
	if ( AAX::IsParameterIDEqual (iParameterID, DemoGain_GainID) )
	{
		AAX_IEffectParameters* effectParameters = this->GetEffectParameters ();
		if (effectParameters != NULL)
		{
			if ( mGainText && mGainText != GetFocus() )
			{
				AAX_CString	valueString;
				if ( effectParameters->GetParameterValueString( DemoGain_GainID, &valueString, 29 ) == AAX_SUCCESS )
				{
					SendMessage( mGainText, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) valueString.Get());
				}
			}

			if ( mGainSlider )
			{
				double	normalizedValue = 0;
				if ( effectParameters->GetParameterNormalizedValue( DemoGain_GainID, &normalizedValue ) == AAX_SUCCESS )
				{
					int	sendValue = (int)(normalizedValue * cMaxSliderPos); // Normalized double [0:1] to slider range
					SendMessage( mGainSlider, TBM_SETPOS, true, (LPARAM) sendValue );
				}
			}

			result = AAX_SUCCESS;
		}
	}

	return result;
}

// *******************************************************************************
// METHOD:	TimerWakeup
// *******************************************************************************
AAX_Result	DemoGain_GUI::TimerWakeup()
{
	if (mInputBar && mOutputBar)
	{
		//grab the current meter values from the system
		float inputMeterValue, outputMeterValue;
		GetController()->GetCurrentMeterValue(cDemoGain_MeterID[eMeterTap_PreGain], &inputMeterValue);
		GetController()->GetCurrentMeterValue(cDemoGain_MeterID[eMeterTap_PostGain], &outputMeterValue);

		//Normalize the values to progress bar ranges
		int inputMeterPos = (int)(inputMeterValue * cMaxMeterBarPos);
		int outputMeterPos = (int)(outputMeterValue * cMaxMeterBarPos);

		//Set the values on the progress bar controls
		::SendMessage (mInputBar, PBM_SETPOS, inputMeterPos, 0);
		::SendMessage (mOutputBar, PBM_SETPOS, outputMeterPos, 0);
	}

	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	GetPlugInHWNDResourceID
// *******************************************************************************
WORD DemoGain_GUI::GetPlugInHWNDResourceID () const
{
	return IDD_GAINVIEW;
}

// *******************************************************************************
// METHOD:	InitPlugInHWNDContents
// *******************************************************************************
void DemoGain_GUI::InitPlugInHWNDContents ()
{
	const HWND	plugInHWND = GetPlugInHWND();
	if ( plugInHWND )
	{
		AAX_Point viewSize;
		GetViewSize (&viewSize);

		// Text box
		mGainText = ::GetDlgItem ( plugInHWND, IDC_GAINTEXT );
		if ( mGainText )
		{
			this->SetupForKeyboardInput(mGainText);
		}

		// Gain slider control
		mGainSlider = ::GetDlgItem ( plugInHWND, IDC_GAINSLIDER );
		if ( mGainSlider )
		{
			::SendMessage( mGainSlider, TBM_SETRANGEMIN, 0, 0 );
			::SendMessage( mGainSlider, TBM_SETRANGEMAX, 0, cMaxSliderPos );

			this->SetupForMouseInput (mGainSlider);
		}

		// Input meter
		mInputBar = ::GetDlgItem ( plugInHWND, IDC_PROGRESS1 );
		if ( mInputBar )
		{
			::SendMessage( mInputBar, PBM_SETRANGE, 0, MAKELPARAM(0, cMaxMeterBarPos) );
		}

		// Output meter
		mOutputBar = ::GetDlgItem ( plugInHWND, IDC_PROGRESS2 );
		if ( mOutputBar )
		{
			::SendMessage( mOutputBar, PBM_SETRANGE, 0, MAKELPARAM(0, cMaxMeterBarPos) );
		}
	}
}

// *******************************************************************************
// METHOD:	HandleKeyDown
// *******************************************************************************
AAX_EEventResult DemoGain_GUI::HandleKeyDown( HWND hwnd, WPARAM wParam )
{
	switch ( wParam )
	{
	case VK_RETURN:	return AAX_eEventResult_ClearFocus;
	case VK_ESCAPE:	return AAX_eEventResult_CancelFocus;
	case VK_SPACE:	return AAX_eEventResult_PassEventToClient;
	}

	return AAX_eEventResult_PassEventToSystem;
}

// *******************************************************************************
// METHOD:	MouseInputWindowProc
// *******************************************************************************
AAX_EEventResult DemoGain_GUI::DoMouseWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	AAX_EEventResult eventRoute = AAX_eEventResult_PassEventToSystem;

	AAX_CParamID paramID = NULL;
	if (hwnd == mGainSlider)
	{
		paramID = DemoGain_GainID;
	}

	if (paramID)
	{
		switch (uMsg)
		{
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONDOWN:
#if (_WIN32_WINNT >= 0x0500)
		case WM_XBUTTONDOWN:
#endif
			{
				if (HandleMouseEvent (eMouseEventType_MouseDown, paramID, wParam))
				{
					eventRoute = AAX_eEventResult_DoNotPassEvent;
				}
				break;
			}

		case WM_MOUSEMOVE:
			{
				if (wParam & MK_LBUTTON || wParam & MK_RBUTTON)
				{
					if (HandleMouseEvent (eMouseEventType_MouseDrag, paramID, wParam))
					{
						eventRoute = AAX_eEventResult_DoNotPassEvent;
					}
				}
				break;
			}

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
#if (_WIN32_WINNT >= 0x0500)
		case WM_XBUTTONUP:
#endif
			{
				if (HandleMouseEvent (eMouseEventType_MouseUp, paramID, wParam))
				{
					eventRoute = AAX_eEventResult_DoNotPassEvent;
				}
				break;
			}
		}
	}

	return eventRoute;
}

// *******************************************************************************
// METHOD:	HandleMessage
// *******************************************************************************
bool DemoGain_GUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	bool result = false; // Did we handle the message?
	AAX_IEffectParameters* effectParameters = this->GetEffectParameters();
	
	switch(uMsg)
	{
		case WM_HSCROLL:
		{
			if ( (HWND) lParam == mGainSlider )
			{
				WORD request = LOWORD( wParam );
				
				// Tell the data model whether we want control of this parameter
				const bool editEnding = request == SB_ENDSCROLL || request == SB_THUMBPOSITION;
				const EEditAction editAction = editEnding ? eEditAction_EndEdit : eEditAction_BeginOrContinueEdit;
				UpdateParameterEditEvent (DemoGain_GainID, editAction);
				
				int32_t	newPos = static_cast<int32_t>(SendMessage( mGainSlider, TBM_GETPOS, 0, 0 ));
				double newValue = (double)newPos / cMaxSliderPos; // Div. by max slider position to convert to gain
				if (effectParameters != NULL)
				{
					effectParameters->SetParameterNormalizedValue( DemoGain_GainID, newValue );
				}
			}
			result = false; // Pass the message on to the dialog manager for further handling
			break;
		}

		case WM_COMMAND:
		{
			WORD	notificationCode = HIWORD( wParam );
			if ( notificationCode != 0 && notificationCode != 1 )
			{
				if ( (HWND) lParam == mGainText )
				{
					if ( notificationCode == EN_KILLFOCUS )
					{
						char	newString[40];
						double	newValue;
						SendMessage( mGainText, WM_GETTEXT, 39, (LPARAM) newString );
						if ( effectParameters != NULL && effectParameters->GetParameterValueFromString( DemoGain_GainID, &newValue, AAX_CString(&(newString[0])) ) == AAX_SUCCESS )
						{						
							effectParameters->SetParameterNormalizedValue( DemoGain_GainID, newValue );
						}
					}
				}
			}
			result = false; // Pass the message on to the dialog manager for further handling
			break;
		}
	}

	return result;
}

// *******************************************************************************
// METHOD:	ForceParameterUpdateForHWND
// *******************************************************************************
void DemoGain_GUI::ForceParameterUpdateForHWND( HWND hwnd )
{
	if ( hwnd == mGainText )
	{
		AAX_IEffectParameters* effectParameters = this->GetEffectParameters();
		if (effectParameters != NULL)
		{
			int32_t		controlIndex = 0;
			if ( effectParameters->GetParameterIndex( DemoGain_GainID, &controlIndex ) == AAX_SUCCESS )
			{
				AAX_CString	valueString;
				if ( effectParameters->GetParameterValueString( DemoGain_GainID, &valueString, 29 ) == AAX_SUCCESS )
				{
					SendMessage( mGainText, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) valueString.Get());
				}
			}
		}
	}
}

// *******************************************************************************
// METHOD:	UpdateParameterEditEvent
// *******************************************************************************
void DemoGain_GUI::UpdateParameterEditEvent (AAX_CParamID iParamID, EEditAction iAction)
{
	AAX_IEffectParameters* effectParameters = this->GetEffectParameters();
	switch (iAction)
	{
		case (eEditAction_BeginOrContinueEdit):
		{
			if (effectParameters != NULL && !mGainSliderIsEditing)
			{
				mGainSliderIsEditing = true;
				effectParameters->TouchParameter (iParamID);
			}
			break;
		}
		case (eEditAction_EndEdit):
		{
			if (effectParameters != NULL && mGainSliderIsEditing)
			{
				effectParameters->ReleaseParameter (iParamID);
				mGainSliderIsEditing = false;
			}
			break;
		}
		default:
		{
			// Unhandled action
			break;
		}
	}
}

// *******************************************************************************
// METHOD:	HandleMouseEvent
// *******************************************************************************
bool DemoGain_GUI::HandleMouseEvent (EMouseEventType iEventType, const AAX_CParamID & iParamID, const WPARAM & iEventMods)
{
	bool result = false;
	AAX_IViewContainer* viewContainer = GetViewContainer();

	if (iParamID != NULL && viewContainer != NULL)
	{
		uint32_t mods = GetAAXModifiersFromWin32MouseEvent (iEventMods);
		switch (iEventType)
		{
		case eMouseEventType_MouseDown:
			{
				AAX_Result res = viewContainer->HandleParameterMouseDown (iParamID, mods);
				if ( (AAX_ERROR_UNIMPLEMENTED == res) && 
					((mods & AAX_eModifiers_Alt) || (mods & AAX_eModifiers_Option)))
				{
					AAX_IEffectParameters* effectParameters = this->GetEffectParameters ();
					if (effectParameters != NULL)
					{
						double defaultValue = 0.0;
						if (AAX_SUCCESS == effectParameters->GetParameterDefaultNormalizedValue (iParamID, &defaultValue)) 
							effectParameters->SetParameterNormalizedValue (iParamID, defaultValue);
					}

				}

				result = (res == AAX_SUCCESS);

				break;
			}
		case eMouseEventType_MouseDrag:
			{
				result = (viewContainer->HandleParameterMouseDrag (iParamID, mods) == AAX_SUCCESS);
				break;
			}
		case eMouseEventType_MouseUp:
			{
				result = (viewContainer->HandleParameterMouseUp (iParamID, mods) == AAX_SUCCESS);
				break;
			}
		}
	}

	return result; // true if event was handled, false otherwise
}

// *******************************************************************************
// METHOD:	GetAAXModifiersFromWin32MouseEvent
// *******************************************************************************
uint32_t DemoGain_GUI::GetAAXModifiersFromWin32MouseEvent (const WPARAM & wParam)
{
	uint32_t aax_mods = 0;
	
	if (wParam & MK_CONTROL)
		aax_mods |= AAX_eModifiers_Cntl;
	if (wParam & MK_SHIFT)
		aax_mods |= AAX_eModifiers_Shift;
	if (wParam & MK_RBUTTON)
		aax_mods |= AAX_eModifiers_SecondaryButton;

	// Since the Win32 mouse event flags do not include
	// Alt and Windows key states, we query the host
	AAX_IViewContainer* const viewContainer = this->GetViewContainer ();
	if (viewContainer)
	{
		uint32_t aaxViewMods = 0;
		viewContainer->GetModifiers (&aaxViewMods);
		aax_mods |= aaxViewMods;
	}

	return aax_mods;
}
