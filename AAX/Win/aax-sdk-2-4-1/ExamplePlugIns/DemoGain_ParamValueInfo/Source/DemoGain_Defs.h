/*================================================================================================*/
/*
 *	Copyright 2012-2016, 2018 by Avid Technology, Inc.
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
 *	\brief  Definitions shared by the DemoDist_ParamValueInfo classes in one easy-to-maintain file.
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOGAIN_DEFS_H
#define DEMOGAIN_DEFS_H

#include "AAX.h"			// for AAX_CTypeID

// Effect IDs and names
const AAX_CEffectID kEffectID_DemoGain	= "com.avid.aax.sdk.demogain.paramvalueinfo";
const AAX_CEffectID kEffectName_DemoGain[] = { "Demo Gain Param Value Info", "DmGnPVI", "DGvi", "Di", 0 };

// Type, product, and relation IDs
const AAX_CTypeID cDemoGain_ManufactureID			= 'AVID';
const AAX_CTypeID cDemoGain_ProductID				= 'DmGn';
const AAX_CTypeID cDemoGain_PlugInID_Native			= 'DGVI';

const AAX_CTypeID cDemoGain_MeterID [2]				= {'mtrI','mtrO'};

const char* const DemoGain_GainID					= "Gain";
const char* const DemoGain_FilterTypeID				= "EQ Type";
const char* const DemoGain_Q						= "Q";
const char* const DemoGain_Slope					= "Slope";

const char* const DemoGain_HighPassStatus			= "High Pass Status";
const char* const DemoGain_LowPassStatus			= "Low Pass Status";
const char* const DemoGain_HighShelfStatus			= "High Shelf Status";
const char* const DemoGain_LowShelfStatus			= "Low Shelf Status";

#endif // DEMOGAIN_DEFS_H
