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

// Standard Library Includes
#include <utility>

class DemoDelay_Parameters : public AAX_CEffectParameters
{
public:
	//Construction and destruction
	static AAX_CEffectParameters *AAX_CALLBACK	Create();
	/* default constructor */					DemoDelay_Parameters (void);
	virtual /* destructor */					~DemoDelay_Parameters();

	//Overrides from AAX_CEffectParameters
	virtual AAX_Result  EffectInit();
	virtual	AAX_Result  NotificationReceived( /* AAX_ENotificationEvent */ AAX_CTypeID iNotificationType, const void * iNotificationData, uint32_t	iNotificationDataSize);
	virtual AAX_Result	UpdateParameterTouch ( AAX_CParamID iParameterID, AAX_CBoolean iTouchState );
	virtual AAX_Result	UpdateParameterNormalizedValue (AAX_CParamID iParameterID, double iValue, AAX_EUpdateSource iSource );
	virtual AAX_Result	UpdateParameterNormalizedRelative (AAX_CParamID iParameterID, double iValue );
	virtual AAX_Result	ResetFieldData (AAX_CFieldIndex iFieldIndex, void * oData, uint32_t iDataSize) const;
	
private:
	enum EConversionDirection
	{
		eConversionDirection_MSToSamples,
		eConversionDirection_SamplesToMS
	};
	
	// Parameter update helper methods
	//
	// Returns true and updates ioValue if the value for the given parameter ID should be overridden
	bool		OverrideParameterChange(AAX_CParamID iParameterID, bool iRelative, double& ioValue);
	// Performs actions associated with a parameter update (e.g. signal latency change, setting linked controls)
	void		HandleParameterUpdate(AAX_CParamID iParameterID, AAX_EUpdateSource iSource);
	
	// Data model helper methods
	//
	// Retrieves the number of samples of delay implied by the current data model state
	AAX_Result	GetRequestedDelaySamples(int32_t& oSamples) const;
	// Converts between ms and samples
	AAX_Result	ConvertBetweenMSAndSamples(EConversionDirection iConversionDirection, float& ioMS, int32_t& ioSamples) const;
	// Returns true if the plug-in is currently bypassed
	bool		BypassEnabled() const;
	// Returns true if delay compensation is currently enabled
	bool		DelayCompEnabled() const;
	
	// Packet generation methods
	AAX_Result  GenerateCoefficientPacket(AAX_CPacket& ioPacket);
	AAX_Result  GenerateDelaySamplesPacket(AAX_CPacket& ioPacket);
	
	// DEMONSTRATION ONLY
	// This plug-in uses an auto-generated GUI. In order to graphically demonstrate how the plug-in
	// is working, some internal data are registered as parameters for display in the auto-generated
	// GUI. In a "real" plug-in, these data would simply be added as member variables. The following
	// methods handle the behavior of these display-only data parameters so that they behave correctly
	// in the auto-generated GUI (e.g. they cannot be edited directly from the GUI.)
	void		Demo_RegisterInternalDataParameters();
	void		Demo_UpdateParameterTouch(AAX_CParamID iParameterID, AAX_CBoolean iTouchState);
	bool		Demo_BlockParameterChange(AAX_CParamID iParameterID) const;
	AAX_Result	Demo_UpdateAlgDelay();
	void		Demo_SetAlgDelay(AAX_EUpdateSource iSource);
	AAX_Result	Demo_GetAlgDelaySamples(int32_t& oSamples) const;
	
private:
	float	mMinDelayMS;
	float	mMaxDelayMS;
	float	mMixCache;
};

#endif
