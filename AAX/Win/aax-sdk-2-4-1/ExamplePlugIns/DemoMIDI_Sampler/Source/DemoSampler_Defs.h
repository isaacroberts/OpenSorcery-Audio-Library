/*================================================================================================*/
/*
 *	Copyright 2012-2015 by Avid Technology, Inc.
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
 *	\file   DemoSampler_Defs.h
 *
 *	\brief  Definitions shared by the DemoMIDI_Sampler classes in one easy-to-maintain file.
 *
 *  \author Andriy Goshko
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOSAMPLER_DEFS_H
#define DEMOSAMPLER_DEFS_H

#include "AAX.h"			// for AAX_CTypeID

// Type, product, and relation IDs
const AAX_CTypeID cDemoSampler_ManufactureID =			'AVID';
const AAX_CTypeID cDemoSampler_ProductID =				'DmSM';
const AAX_CTypeID cDemoSampler_PlugInID_Native =		'DmSR';
const AAX_CTypeID cDemoSampler_PlugInID_AudioSuite =	'DmSA';

#define kSupportedSR									44100

#endif // DEMOGAIN_DEFS_H
