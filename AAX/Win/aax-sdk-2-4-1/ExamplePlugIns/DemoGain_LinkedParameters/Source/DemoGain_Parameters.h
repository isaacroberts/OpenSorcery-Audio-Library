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

#include "DemoGain_Alg.h"
#include "AAX_CEffectParameters.h"
#include <map>
#include <string>


class DemoGain_Parameters : public AAX_CEffectParameters
{
public:
	DemoGain_Parameters (void);
	AAX_DEFAULT_DTOR_OVERRIDE (DemoGain_Parameters);
	
	// Create callback
	static AAX_CEffectParameters *AAX_CALLBACK Create();

public:
	//Overrides from AAX_CEffectParameters
	AAX_Result EffectInit() AAX_OVERRIDE;
	AAX_Result SetChunk (AAX_CTypeID chunkID, const AAX_SPlugInChunk * iChunk ) AAX_OVERRIDE;
	AAX_Result UpdateParameterNormalizedValue ( AAX_CParamID inParameterID, double inValue, AAX_EUpdateSource inSource ) AAX_OVERRIDE;
	AAX_Result UpdateParameterTouch ( AAX_CParamID inParameterID, AAX_CBoolean inTouchState ) AAX_OVERRIDE;
	
private:
	AAX_Result UpdatePacket_Gains(AAX_CPacket& ioPacket);
	AAX_CParamID GetLinkedControl ( AAX_CParamID inParameterID );
	void SetGains ( SDemoGain_CoefsGain * ioGains, bool inType, double inValue, DemoGain_ESide inSide );

private:
	std::multimap<std::string,std::string>	mLinkTouchMap;
};


#endif
