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

#include "RectiFi_Alg.h"

template<int kNumChannelsIn, int kNumChannelsOut>
void RectiFi_Algorithm_ResetFunction(
	SRectiFi_Algorithm_Context * const inInstancesBegin [],
	const void * AAX_RESTRICT					inInstancesEnd)
{
	for (SRectiFi_Algorithm_Context * const * walk = inInstancesBegin; walk != inInstancesEnd; ++walk)
	{
		SRectiFi_Algorithm_Context* contextP = *walk;

		RectiFi_Algorithm_InitStateBlock(eAlgFieldIndex_State, contextP->mStateP, sizeof(SRectiFiStateBlock), 0);
	}
}
