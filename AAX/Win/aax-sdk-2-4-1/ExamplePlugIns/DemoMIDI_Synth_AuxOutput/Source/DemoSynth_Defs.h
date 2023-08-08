/*================================================================================================*/
/*
 *	Copyright 2012-2015, 2018 by Avid Technology, Inc.
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
 *	\file   DemoSynth_Defs.h
 *
 *	\brief  Definitions shared by the DemoMIDI_Synth_AuxOutput classes in one easy-to-maintain file.
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOSYNTH_DEFS_H
#define DEMOSYNTH_DEFS_H

#include "AAX.h"			// for AAX_CTypeID

// Type, product, and relation IDs
static const AAX_CTypeID cDemoSynth_ManufactureID =				'AVID';
static const AAX_CTypeID cDemoSynth_ProductID =					'DmSA';
static const AAX_CTypeID cDemoSynth_PlugInID_Native_Mono =		'mono';
static const AAX_CTypeID cDemoSynth_PlugInID_Native_Stereo =	'ster';
static const AAX_CTypeID cDemoSynth_PlugInID_AudioSuite =		'offl';

// Additional DemoMIDI_Synth defines
static const int32_t kNumInputMIDINodes = 4;
static const int32_t kNumAuxOutputStems = 8;

static const char* kAuxOutputStemNames[kNumAuxOutputStems] = {
	"DemoMIDI Synth #1",
	"DemoMIDI Synth #2",
	"DemoMIDI Synth #3",
	"DemoMIDI Synth #4",
	"DemoMIDI Synth #5",
	"DemoMIDI Synth #6",
	"DemoMIDI Synth #7",
	"DemoMIDI Synth #8"
};

#endif // DEMOGAIN_DEFS_H
