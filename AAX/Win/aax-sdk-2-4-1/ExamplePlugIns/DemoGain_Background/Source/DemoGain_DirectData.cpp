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


// Note: The way that this plug-in adjusts its algorithmic latency based on
// buffer size is fundamentally broken given the requirement for AAX Native
// plug-ins to support variable buffer sizes at run-time.  However, we have
// left DemoGain_DirectData.cpp and .h in place as an example of how to use
// the Direct Data interface until a better example is available.
// -rmajors 3/12


#include "DemoGain_DirectData.h"

#include "DemoGain_Alg.h"

#include "AAX_IPrivateDataAccess.h"
#include "AAX_IController.h"


// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_CEffectDirectData *AAX_CALLBACK DemoGain_DirectData::Create()
{
	return new DemoGain_DirectData();
}

// *******************************************************************************
// METHOD:	Constructor
// *******************************************************************************
DemoGain_DirectData::DemoGain_DirectData () :
	AAX_CEffectDirectData(),
	mCachedDelay(-1)
{
}

// *******************************************************************************
// METHOD:	TimerWakeup_PrivateDataAccess
// *******************************************************************************
AAX_Result DemoGain_DirectData::TimerWakeup_PrivateDataAccess(AAX_IPrivateDataAccess* iPrivateDataAccess)
{
	AAX_Result result = AAX_SUCCESS;
	
	AAX_IController* controller = Controller();
	DelaySamp curdelay = 0;
	result = controller->GetSignalLatency(&curdelay);
	if (result != AAX_SUCCESS) return result;
	if (curdelay != mCachedDelay)
	{
		result = iPrivateDataAccess->WritePortDirect (eAlgPortID_DelaySamples,
													  offsetof (SDemoGain_Delay, mCurDelay),
													  sizeof (DelaySamp),
													  &curdelay);
		if (result != AAX_SUCCESS) return result;
		mCachedDelay = curdelay;
	}
	
	int32_t reqdelay;
	result = iPrivateDataAccess->ReadPortDirect (eAlgPortID_DelaySamples,
												 offsetof (SDemoGain_Delay, mReqDelay),
												 sizeof (DelaySamp),
												 &reqdelay);
	if (result != AAX_SUCCESS) return result;
	
	result = controller->SetSignalLatency (reqdelay);
	if (result != AAX_SUCCESS) return result;
	
	return result;
}
