/*================================================================================================*/
/*
 *	Copyright 2014-2015 by Avid Technology, Inc.
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
 *	\file   DemoDelay_Defs.h
 *
 *	\brief  Definitions shared by the DemoDelay_DynamicLatencyComp classes in one easy-to-maintain file.
 *
 *  \author Dave Tremblay
 */ 
/*================================================================================================*/

#pragma once
#ifndef DemoDelay_DEFS_H
#define DemoDelay_DEFS_H

#include "AAX.h"			// for AAX_CTypeID


// Type, product, and relation IDs
const AAX_CTypeID cDemoDelay_ManufactureID =		'AVID';
const AAX_CTypeID cDemoDelay_ProductID =			'DmLC';
const AAX_CTypeID cDemoDelay_PlugInID_AudioSuite =	'LCas';
const AAX_CTypeID cDemoDelay_PlugInID_Native =		'LCna';
const AAX_CTypeID cDemoDelay_PlugInID_TI =			'LCti';

const AAX_CTypeID cDemoDelay_MeterID [2] =			{'mtrI','mtrO'};

#endif // DemoDelay_DEFS_H
