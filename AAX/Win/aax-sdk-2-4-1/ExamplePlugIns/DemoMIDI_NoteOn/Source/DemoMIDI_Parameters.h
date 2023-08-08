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
 *	\file   DemoMIDI_Parameters.h
 *
 *	\brief  DemoMIDI_Parameters class declaration.
 */ 
/*================================================================================================*/ 
#pragma once
#ifndef DEMOMIDI_PARAMETERS_H
#define DEMOMIDI_PARAMETERS_H

#include "AAX_CEffectParameters.h"

//==============================================================================
class DemoMIDI_Parameters : public AAX_CEffectParameters
{
public:
	
	//Constructor
	DemoMIDI_Parameters (void);
	static AAX_CEffectParameters *AAX_CALLBACK	Create();
	
	//Overrides from AAX_CEffectParameters
	virtual AAX_Result							EffectInit();
	
	// Override this method to receive MIDI 
	// packets for the described MIDI nodes
	virtual AAX_Result							UpdateMIDINodes ( AAX_CFieldIndex inFieldIndex,	AAX_CMidiPacket& inPacket );
};

#endif
