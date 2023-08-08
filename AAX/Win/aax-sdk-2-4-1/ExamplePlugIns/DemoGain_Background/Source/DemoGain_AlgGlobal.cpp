/*================================================================================================*/
/*
 *	Copyright 2011-2015 by Avid Technology, Inc.
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

#include "DemoGain_Alg.h"
#include "AAX.h"


SDemoGain_BGBuffer * gBGBuffer[cMaxNumInstances];
float * gBGGain[cMaxNumInstances];
AAX_CBoolean gInstanceIsActive[cMaxNumInstances];
uint32_t gNumInstances = 0;
