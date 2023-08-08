/*================================================================================================*/
/*
 *	Copyright 2009-2015, 2019 by Avid Technology, Inc.
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
 *	\file   DemoGain_Parameters.h
 *
 *	\brief  DemoGain_Parameters class declaration.
 */ 
/*================================================================================================*/ 
#pragma once
#ifndef DEMOGAIN_PARAMETERS_H
#define DEMOGAIN_PARAMETERS_H

#include "AAX_CEffectParameters.h"


class DemoGain_Parameters : public AAX_CEffectParameters
{
public:
	DemoGain_Parameters (void);
	AAX_DEFAULT_DTOR_OVERRIDE(DemoGain_Parameters);
	
	// Create callback
	static AAX_CEffectParameters *AAX_CALLBACK Create();
	
public:
	//Overrides from AAX_CEffectParameters
	AAX_Result EffectInit() AAX_OVERRIDE;
	AAX_Result GetParameterValueInfo ( AAX_CParamID iParameterID, int32_t iSelector, int32_t* oValue) const AAX_OVERRIDE;
	
private:
	AAX_Result UpdatePacket_Gain(AAX_CPacket& ioPacket);

private:
	enum EFilterType
	{
		eFilterType_HighPass = 0,
		eFilterType_LowShelf,
		eFilterType_Bell,
		eFilterType_HighShelf,
		eFilterType_LowPass,
		eFilterType_Count
	};

	enum EEQStatus
	{
		eEQStatus_Disabled = 0,
		eEQStatus_Bypassed,
		eEQStatus_Enabled,
		eEQStatus_Count
	};

	enum EFilterSlope
	{
		eFilterSlope_6dBperOct = 0,
		eFilterSlope_12dBperOct, 
		eFilterSlope_18dBperOct,
		eFilterSlope_24dBperOct,
		eFilterSlope_Count
	};
};


#endif
