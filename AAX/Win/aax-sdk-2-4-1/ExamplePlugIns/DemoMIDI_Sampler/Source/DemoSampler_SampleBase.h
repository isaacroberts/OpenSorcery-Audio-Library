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
 *	\file   DemoSampler_SampleBase.h
 *
 *	\brief  Definitions shared by the DemoMIDI_Sampler classes in one easy-to-maintain file.
 */ 
/*================================================================================================*/
#pragma once
#ifndef _DEMOSAMPLERSAMPLEBASE
#define _DEMOSAMPLERSAMPLEBASE

#include "AAX.h"

const int32_t bass_size = 44096;
extern float bass_bank[bass_size];

const int32_t snare_size = 14992;
extern float snare_bank[snare_size];

const int32_t tom_size = 10192;
extern float tom_bank[tom_size];

#endif	//	_DEMOSAMPLERSAMPLEBASE
