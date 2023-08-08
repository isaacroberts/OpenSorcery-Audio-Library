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
 *	\file   DemoGain_GUI.h
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOGAIN_GUI_H
#define DEMOGAIN_GUI_H

#if WINDOWS_VERSION
#include "DemoGain_GUI_Win32.h"
#else
#include "DemoGain_GUI_Cocoa.h"
#endif

#endif
