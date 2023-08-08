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

/**  
 *	\file   DemoGain_DirectData.h
 */ 
/*================================================================================================*/
#pragma once
#ifndef _DEMOGAIN_DIRECTDATA_H_
#define _DEMOGAIN_DIRECTDATA_H_


#include "AAX_CEffectDirectData.h"

#include "DemoGain_Alg.h"


class AAX_IPrivateDataAccess;


class DemoGain_DirectData : public AAX_CEffectDirectData
{
public:
	//Constructor
	DemoGain_DirectData (void);
	
	//Overrides from AAX_CEffectDirectData
	AAX_Result			TimerWakeup_PrivateDataAccess(AAX_IPrivateDataAccess* iPrivateDataAccess);
	
	static AAX_CEffectDirectData *AAX_CALLBACK Create();
	
private:
	DelaySamp mCachedDelay;
};

#endif // _DEMOGAIN_DIRECTDATA_H_
