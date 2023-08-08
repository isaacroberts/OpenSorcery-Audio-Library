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
const AAX_CTypeID cDemoGain_ManufactureID               = 'AVID';
const AAX_CTypeID cDemoGain_ProductID                   = 'DmgM';

//
const AAX_CTypeID cDemoGain_PlugInID_Native             = 'DGMR';
const AAX_CTypeID cDemoGain_PlugInID_Native_Stereo      = 'DGST';
const AAX_CTypeID cDemoGain_PlugInID_Native_LCR         = 'DGcR';
const AAX_CTypeID cDemoGain_PlugInID_Native_QUAD        = 'DGQa';
const AAX_CTypeID cDemoGain_PlugInID_Native_LCRS        = 'DGRs';
const AAX_CTypeID cDemoGain_PlugInID_Native_5_0         = 'DG50';
const AAX_CTypeID cDemoGain_PlugInID_Native_5_1         = 'DG51';
const AAX_CTypeID cDemoGain_PlugInID_Native_6_0         = 'DG60';
const AAX_CTypeID cDemoGain_PlugInID_Native_6_1         = 'DG61';
const AAX_CTypeID cDemoGain_PlugInID_Native_7_0_SDDS    = 'G70R';
const AAX_CTypeID cDemoGain_PlugInID_Native_7_1_SDDS    = 'G71R';
const AAX_CTypeID cDemoGain_PlugInID_Native_7_0_DTS     = 'G70r';
const AAX_CTypeID cDemoGain_PlugInID_Native_7_1_DTS     = 'G71r';
const AAX_CTypeID cDemoGain_PlugInID_Native_7_0_2       = 'G702';
const AAX_CTypeID cDemoGain_PlugInID_Native_7_1_2       = 'G712';
const AAX_CTypeID cDemoGain_PlugInID_Native_1_ACN       = 'G1AN';
const AAX_CTypeID cDemoGain_PlugInID_Native_2_ACN       = 'G2AN';
const AAX_CTypeID cDemoGain_PlugInID_Native_3_ACN       = 'G3AN';

//
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite             = 'DGAS';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_Stereo      = 'DGSA';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_LCR         = 'DcRA';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_QUAD        = 'DQAS';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_LCRS        = 'DLAS';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_5_0         = 'G50A';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_5_1         = 'G51A';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_6_0         = 'G60A';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_6_1         = 'G61A';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_7_0_SDDS    = 'G70A';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_7_1_SDDS    = 'G71A';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_7_0_DTS     = 'G7as';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_7_1_DTS     = 'G71a';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_7_0_2       = 'G70a';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_7_1_2       = 'G72a';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_1_ACN       = 'G1AA';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_2_ACN       = 'G2AA';
const AAX_CTypeID cDemoGain_PlugInID_AudioSuite_3_ACN       = 'G3AA';

//
const AAX_CTypeID cDemoGain_PlugInID_TI             = 'DGMi';
const AAX_CTypeID cDemoGain_PlugInID_TI_Stereo      = 'DGSi';
const AAX_CTypeID cDemoGain_PlugInID_TI_LCR         = 'DcRi';
const AAX_CTypeID cDemoGain_PlugInID_TI_QUAD        = 'DGQi';
const AAX_CTypeID cDemoGain_PlugInID_TI_LCRS        = 'DGRi';
const AAX_CTypeID cDemoGain_PlugInID_TI_5_0         = 'G50i';
const AAX_CTypeID cDemoGain_PlugInID_TI_5_1         = 'G51i';
const AAX_CTypeID cDemoGain_PlugInID_TI_6_0         = 'G60i';
const AAX_CTypeID cDemoGain_PlugInID_TI_6_1         = 'G61i';
const AAX_CTypeID cDemoGain_PlugInID_TI_7_0_SDDS    = 'G7TR';
const AAX_CTypeID cDemoGain_PlugInID_TI_7_1_SDDS    = 'G7iR';
const AAX_CTypeID cDemoGain_PlugInID_TI_7_0_DTS     = 'G7Tr';
const AAX_CTypeID cDemoGain_PlugInID_TI_7_1_DTS     = 'G7ir';
const AAX_CTypeID cDemoGain_PlugInID_TI_7_0_2       = 'G7i0';
const AAX_CTypeID cDemoGain_PlugInID_TI_7_1_2       = 'G7i2';
const AAX_CTypeID cDemoGain_PlugInID_TI_1_ACN       = 'G1AT';
const AAX_CTypeID cDemoGain_PlugInID_TI_2_ACN       = 'G2AT';
const AAX_CTypeID cDemoGain_PlugInID_TI_3_ACN       = 'G3AT';

#define MAX_STEM_FORMAT AAX_eStemFormat_Ambi_3_ACN
const uint16_t cMaxNumChannels = 20; // for large channel count test case, otherwise use AAX_STEM_FORMAT_CHANNEL_COUNT(MAX_STEM_FORMAT);
const uint16_t cMaxNumMeters   = cMaxNumChannels * 2 + 2;

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

static const AAX_CTypeID cDemoGain_CmpLim_GRMeterID =  'GRCL';
static const AAX_CTypeID cDemoGain_ExpGate_GRMeterID = 'GREG';

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
	{ 207, 120 }, // 1 channel
	{ 338, 120 }, // 2 channels
	{ 492, 120 }, // 3 channels
	{ 649, 120 }, // 4 channels
	{ 802, 120 }, // 5 channels
	{ 956, 120 }, // 6 channels
	{ 1111, 120 }, // 7 channels
	{ 1266, 120 }, // 8 channels
	{ 1423, 120 }, // 9 channels
	{ 1575, 120 }, // 10 channels
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 2508, 121 } // 16 channels
};

#endif // DEMOGAIN_DEFS_H
