/*================================================================================================*/
/*
 *	Copyright 2009-2015, 2018 by Avid Technology, Inc.
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
 *	\file   Rectifi_Describe.h
 *
 *	\brief  Declaration of GetEffectDescriptions function.
 *
 *  \author Rus Maxham
 */ 
/*================================================================================================*/

#include "AAX.h"

class AAX_ICollection;
class AAX_IEffectDescriptor;

AAX_Result GetEffectDescriptions ( AAX_ICollection * outDescriptions );
AAX_Result Rectifi_GetPlugInDescription ( AAX_IEffectDescriptor & outDescriptor );
	
