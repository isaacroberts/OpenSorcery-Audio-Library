/*================================================================================================*/
/*
 *	Copyright 2016-2017 by Avid Technology, Inc.
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
 *	\brief  Definitions shared by the DemoGain classes in one easy-to-maintain file.
 */ 
/*================================================================================================*/

#pragma once
#ifndef DEMOGAIN_DEFS_H
#define DEMOGAIN_DEFS_H

#include "AAX.h"			// for AAX_CTypeID


// Type, product, and relation IDs
const AAX_CTypeID cDemoGain_ManufactureID							= 'AVID';
const AAX_CTypeID cDemoGain_UpMixer_ProductID						= 'DgUP';

#define MAX_STEM_FORMAT AAX_eStemFormat_Ambi_3_ACN
const uint16_t cMaxNumChannels = AAX_STEM_FORMAT_CHANNEL_COUNT(MAX_STEM_FORMAT);
const uint16_t cMaxNumMeters   = cMaxNumChannels * 2;

static const AAX_CTypeID cDemoGain_InputMeterID[cMaxNumChannels] = {
	 'InM1'
	,'InM2'
	,'InM3'
	,'InM4'
	,'InM5'
	,'InM6'
	,'InM7'
	,'InM8'
	,'InM9'
	,'IM10'
	,'IM11'
	,'IM12'
	,'IM13'
	,'IM14'
	,'IM15'
	,'IM16'
};

static const AAX_CTypeID cDemoGain_OutputMeterID[cMaxNumChannels] = {
	 'OuM1'
	,'OuM2'
	,'OuM3'
	,'OuM4'
	,'OuM5'
	,'OuM6'
	,'OuM7'
	,'OuM8'
	,'OuM9'
	,'OM10'
	,'OM11'
	,'OM12'
	,'OM13'
	,'OM14'
	,'OM15'
	,'OM16'
};

static const char * cDemoGain_GainIDs [cMaxNumChannels] = {
	 "Gain1"
	,"Gain2"
	,"Gain3"
	,"Gain4"
	,"Gain5"
	,"Gain6"
	,"Gain7"
	,"Gain8"
	,"Gain9"
	,"Gain10"
	,"Gain11"
	,"Gain12"
	,"Gain13"
	,"Gain14"
	,"Gain15"
	,"Gain16"
};

static const int32_t cDemoGain_TI_Counts[cMaxNumChannels][2] =
{
	// InstanceCycleCount, SharedCycleCount
	{ 141, 88 }, // 1 channel
	{ 248, 87 }, // 2 channels
	{ 347, 90 }, // 3 channels
	{ 452, 93 }, // 4 channels
	{ 557, 95 }, // 5 channels
	{ 662, 96 }, // 6 channels
	{ 767, 99 }, // 7 channels
	{ 872, 100 }, // 8 channels
	{ 978, 95 }, // 9 channels
	{ 1083, 96 }, // 10 channels
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 1715, 98 } // 16 channels
};

static const char * cDemoGain_RoutingIDs [cMaxNumChannels] = {
    "Route1"
    ,"Route2"
    ,"Route3"
    ,"Route4"
    ,"Route5"
    ,"Route6"
    ,"Route7"
    ,"Route8"
    ,"Route9"
    ,"Route10"
    ,"Route11"
    ,"Route12"
    ,"Route13"
    ,"Route14"
    ,"Route15"
    ,"Route16"
};

#endif // DEMOGAIN_DEFS_H
