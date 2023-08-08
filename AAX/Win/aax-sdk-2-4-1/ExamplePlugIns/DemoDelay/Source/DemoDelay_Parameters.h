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

/**  
 *	\file   DemoDelay_Parameters.h
 *
 *	\brief  DemoDelay_Parameters class declaration.
 *
 *  \author Dave Tremblay
 */ 
/*================================================================================================*/ 
#pragma once
#ifndef DEMODELAY_PARAMETERS_H
#define DEMODELAY_PARAMETERS_H

#include "AAX_CEffectParameters.h"


class DemoDelay_Parameters : public AAX_CEffectParameters
{
public:
	//Construction and destruction
	static AAX_CEffectParameters *AAX_CALLBACK	Create();
	/* default constructor */					DemoDelay_Parameters (void);
	virtual /* destructor */					~DemoDelay_Parameters();

	//Overrides from AAX_CEffectParameters
	virtual AAX_Result  EffectInit();
	virtual AAX_Result	ResetFieldData (AAX_CFieldIndex iFieldIndex, void * oData, uint32_t iDataSize) const;
	
private:
	enum EConversionDirection
	{
		eConversionDirection_MSToSamples,
		eConversionDirection_SamplesToMS
	};
	
	// Data model helper methods
	//
	// Retrieves the number of samples of delay implied by the current data model state
	AAX_Result	GetDelaySamples(int32_t& oSamples) const;
	// Converts between ms and samples
	AAX_Result	ConvertBetweenMSAndSamples(EConversionDirection iConversionDirection, float& ioMS, int32_t& ioSamples) const;
	// Returns true if the plug-in is currently bypassed
	bool		BypassEnabled() const;
	
	// Packet generation methods
	AAX_Result  GenerateCoefficientPacket(AAX_CPacket& ioPacket);
	AAX_Result  GenerateDelaySamplesPacket(AAX_CPacket& ioPacket);
	
private:
	float	mMinDelayMS;
	float	mMaxDelayMS;
};

#endif
