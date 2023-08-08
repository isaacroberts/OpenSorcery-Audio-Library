/*================================================================================================*/
/*
 *	Copyright 2011-2015, 2018 by Avid Technology, Inc.
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
 *	\file   DemoGain_SliderVST.h
 *
 *	\author Rob Majors
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOGAIN_SLIDERVST_H
#define DEMOGAIN_SLIDERVST_H

#include "AAX.h"

#include "lib/vstguibase.h"

#include "lib/controls/cslider.h"
#include "lib/controls/ccontrol.h"

class AAX_IViewContainer;

class DemoGain_SliderVST : public VSTGUI::CVerticalSlider
{
public:
	DemoGain_SliderVST (const VSTGUI::CRect& size, VSTGUI::IControlListener* listener, long tag, long iMinPos, long iMaxPos, VSTGUI::CBitmap* handle, VSTGUI::CBitmap* background, const VSTGUI::CPoint& offset = VSTGUI::CPoint (0, 0), const long style = VSTGUI::kBottom);
	DemoGain_SliderVST (const VSTGUI::CRect& rect, VSTGUI::IControlListener* listener, long tag, const VSTGUI::CPoint& offsetHandle, long rangeHandle, VSTGUI::CBitmap* handle, VSTGUI::CBitmap* background, const VSTGUI::CPoint& offset = VSTGUI::CPoint(0, 0), const long style = VSTGUI::kBottom);
	DemoGain_SliderVST (const VSTGUI::CVerticalSlider& slider);	
	
public: // VSTGUI::CVerticalSlider
	virtual VSTGUI::CMouseEventResult onMouseDown (VSTGUI::CPoint& where, const VSTGUI::CButtonState& buttons);
	virtual VSTGUI::CMouseEventResult onMouseUp (VSTGUI::CPoint& where, const VSTGUI::CButtonState& buttons);
	virtual VSTGUI::CMouseEventResult onMouseMoved (VSTGUI::CPoint& where, const VSTGUI::CButtonState& buttons);
	virtual bool checkDefaultValue (VSTGUI::CButtonState button);
	
public:
	void SetViewContainer (AAX_IViewContainer* viewContainer);
	void SetParamID (AAX_CParamID paramID);
	
private:
	AAX_IViewContainer* mViewContainer;
	AAX_CParamID mParamID;
};

#endif // DEMOGAIN_SLIDERVST_H
