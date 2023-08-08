/*================================================================================================*/
/*
 *	Copyright 2008-2016, 2018 by Avid Technology, Inc.
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
 *	\file   DemoGain_Defs.h
 *
 *	\brief  Definitions shared by the DemoGain_GUIExtensions classes in one easy-to-maintain file.
 */ 
/*================================================================================================*/

#pragma once
#ifndef DEMOGAIN_DEFS_H
#define DEMOGAIN_DEFS_H

#include "AAX.h"			// for AAX_CTypeID


// Type, product, and relation IDs
const AAX_CTypeID cDemoGain_ManufactureID =			'AVID';

#if PLUGGUI == 1 // Native (Win32 or Cocoa)
const AAX_CTypeID cDemoGain_ProductID =				'DmGN';
#elif PLUGGUI == 2 // Juce
const AAX_CTypeID cDemoGain_ProductID =				'DmGJ';
#elif PLUGGUI == 3 // VSTGUI
const AAX_CTypeID cDemoGain_ProductID =				'DmGV';
#elif PLUGGUI == 4 // Cocoa GUI in separate bundle
const AAX_CTypeID cDemoGain_ProductID =				'DGsb';
#else
const AAX_CTypeID cDemoGain_ProductID =				'DmGn';
#endif // PLUGGUI

const AAX_CTypeID cDemoGain_PlugInID_Native =		'DGDR';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite =	'DGDA';
const AAX_CTypeID cDemoGain_PlugInID_TI =			'DGDT';

const AAX_CTypeID cDemoGain_MeterID [2] =			{'mtrI','mtrO'};

const AAX_CParamID DemoGain_GainID =				"Gain";

#endif // DEMOGAIN_DEFS_H
