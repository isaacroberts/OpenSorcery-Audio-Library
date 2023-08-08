/*================================================================================================*/
/*
 *	Copyright 2013-2015 by Avid Technology, Inc.
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
 *	\file   HostProcessor_Comp_Describe.h
 *
 *	\brief  Declaration of GetEffectDescriptions function.
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/
#ifndef __DemoDelay_HostProcessor__DelayComp_Describe__
#define __DemoDelay_HostProcessor__DelayComp_Describe__

#include "AAX.h"

class AAX_ICollection;

AAX_Result HostProcessor_Comp_GetEffectDescriptions( AAX_ICollection * outCollection );

#endif /* defined(__DemoDelay_HostProcessor__DelayComp_Describe__) */
