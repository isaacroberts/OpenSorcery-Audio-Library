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
#include "DemoGain_AlgProc_Burst.cpp"
#include "DemoGain_AlgProc_ScatterGather.cpp"

#include "../TI/CCSv5/elf_linkage_aax_ccsv5.h"


// The main algorithm entry point - burst
extern "C"
TI_EXPORT
void AlgEntry_Burst(
	SDemoGain_DMABurst_Alg_Context * const	inInstancesBegin [],
	const void *							inInstancesEnd)
{
	DemoGain_DMABurst_AlgorithmProcessFunction(
		inInstancesBegin,
		inInstancesEnd);
}

// The main algorithm entry point - scatter/gather
extern "C"
TI_EXPORT
void AlgEntry_ScatterGather(
	SDemoGain_DMASG_Alg_Context * const		inInstancesBegin [],
	const void *							inInstancesEnd)
{
	DemoGain_DMASG_AlgorithmProcessFunction(
		inInstancesBegin,
		inInstancesEnd);
}
