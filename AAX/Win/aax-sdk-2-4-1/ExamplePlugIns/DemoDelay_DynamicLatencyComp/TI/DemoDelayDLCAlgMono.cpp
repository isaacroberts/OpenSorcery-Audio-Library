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
#include "DemoDelay_Alg.h"
#include "../TI/CCSv5/elf_linkage_aax_ccsv5.h"
#include "AAX.h"
#include "AAX_Enums.h"

extern "C"
TI_EXPORT
void AlgEntry( SDemoDelay_AlgContext * const	inInstancesBegin [],
			   const void *						inInstancesEnd)
{
      DemoDelay_AlgorithmProcessFunction(
            inInstancesBegin,
            inInstancesEnd);
}
