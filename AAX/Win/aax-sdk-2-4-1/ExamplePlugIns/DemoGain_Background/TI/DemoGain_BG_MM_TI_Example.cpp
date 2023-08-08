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
#include "DemoGain_AlgProc.cpp"
#include "../TI/CCSv5/elf_linkage_aax_ccsv5.h"


// The main algorithm entry point
extern "C"
TI_EXPORT
void AlgEntry(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction(
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
int32_t AlgInit(SDemoGain_Alg_Context const *    inInstance,
				AAX_EComponentInstanceInitAction inAction)
{
	return DemoGain_AlgorithmInitFunction(inInstance, inAction);
}

extern "C"
TI_EXPORT
int32_t BGEntry()
{
	return DemoGain_AlgorithmBackgroundFunction();
}

