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

/**  
 *	\file   DemoMIDI_Defs.h
 *
 *	\brief  Definitions shared by the DemoMIDI_NoteOn classes in one easy-to-maintain file.
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOMIDI_DEFS_H
#define DEMOMIDI_DEFS_H

#include "AAX.h"			// for AAX_CTypeID

// Type, product, and relation IDs
const AAX_CTypeID cDemoMIDI_ManufactureID =			'AVID';
const AAX_CTypeID cDemoMIDI_ProductID =				'DmMD';
const AAX_CTypeID cDemoMIDI_PlugInID_Native =		'DmMR';
const AAX_CTypeID cDemoMIDI_PlugInID_AudioSuite =	'DmMA';

const AAX_CTypeID cDemoMIDI_MeterID [2] =			{'mtrI','mtrO'};

#define DemoMIDI_GainID			"Gain"
#define DemoMIDI_HoldID			"hold"

#endif // DEMOGAIN_DEFS_H
