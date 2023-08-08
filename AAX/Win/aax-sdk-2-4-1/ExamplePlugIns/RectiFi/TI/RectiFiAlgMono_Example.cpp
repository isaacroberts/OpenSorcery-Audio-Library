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
/*================================================================================================*/

// Host Alg wrapper shim headers
#include "RectiFi_Alg.h"
#include "RectiFi_AlgProc.cpp"		// Needed for templates
#include "AAX.h"
#include "../TI/CCSv5/elf_linkage_aax_ccsv5.h"

const AAX_EStemFormat kStemFormat = AAX_eStemFormat_Mono;
const int kNumChannels = AAX_STEM_FORMAT_CHANNEL_COUNT(kStemFormat);

// The main algorithm entry point
extern "C"
TI_EXPORT
void AlgEntry(	SRectiFi_Algorithm_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	RectiFi_Algorithm_ProcessFunction<kNumChannels, kNumChannels, 1 << AAX_eAudioBufferLengthDSP_Default>(
			inInstancesBegin,
			inInstancesEnd);
}
