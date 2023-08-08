/*================================================================================================*/
/*
 *	Copyright 2009-2015 by Avid Technology, Inc.
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
 *	\file   DemoGain_GUI_Win32.h
 *
 *	\brief	DemoGain_GUI class definition.
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOGAIN_GUI_WIN32_H
#define DEMOGAIN_GUI_WIN32_H

#include "AAX_CEffectGUI_Win32.h"

enum EEditAction
{
	eEditAction_EndEdit = 0,
	eEditAction_BeginOrContinueEdit
};

enum EMouseEventType
{
	eMouseEventType_MouseDown = 0,
	eMouseEventType_MouseDrag,
	eMouseEventType_MouseUp
};


//==============================================================================
class DemoGain_GUI : public AAX_CEffectGUI_Win32
{
public:
	static AAX_IEffectGUI *		AAX_CALLBACK Create ();

	DemoGain_GUI();
	virtual ~DemoGain_GUI();

public: ////////// AAX_CEffectGUI
	virtual AAX_Result			ParameterUpdated ( AAX_CParamID iParameterID );
	virtual	AAX_Result			TimerWakeup();

public: ////////// AAX_CEffectGUI_Win32
	virtual WORD				GetPlugInHWNDResourceID () const;
	virtual	void				InitPlugInHWNDContents ();
	
	virtual AAX_EEventResult	HandleKeyDown( HWND hwnd, WPARAM wParam );
	virtual AAX_EEventResult	DoMouseWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool				HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void				ForceParameterUpdateForHWND( HWND hwnd );
	
private: ////////// DemoGain_GUI
	void						UpdateParameterEditEvent (AAX_CParamID iParamID, EEditAction iAction);
	bool						HandleMouseEvent (EMouseEventType iEventType, const AAX_CParamID & iParamID, const WPARAM & iEventMods);
	uint32_t					GetAAXModifiersFromWin32MouseEvent (const WPARAM & wParam);

private: ////////// DemoGain_GUI
	HWND						mGainText;
	HWND						mGainSlider;
	HWND						mInputBar;
	HWND						mOutputBar;

	bool						mGainSliderIsEditing;
};

#endif
