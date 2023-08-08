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
 *	\file   DemoDelay_HostProcessor_Comp.h
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/

#ifndef __DemoDelay_HostProcessor__DemoDelay_HostProcessor_Comp__
#define __DemoDelay_HostProcessor__DemoDelay_HostProcessor_Comp__

#include "AAX_CHostProcessor.h"
#include <stdint.h>

class CSimpleDelayLine;

class DemoDelay_HostProcessor_Comp : public AAX_CHostProcessor
{
public:
	//Construction and destruction
	static AAX_CHostProcessor *AAX_CALLBACK	Create();
	/* default constructor */				DemoDelay_HostProcessor_Comp (void);
	virtual /* destructor */				~DemoDelay_HostProcessor_Comp();
	
	//Overrides from AAX_CHostProcessor
	virtual AAX_Result	RenderAudio ( const float * const inAudioIns [], int32_t inAudioInCount, float * const inAudioOuts [], int32_t inAudioOutCount, int32_t * ioWindowSize );
	virtual AAX_Result	PreRender ( int32_t iAudioInCount, int32_t iAudioOutCount, int32_t iWindowSize );
	virtual AAX_Result	PostRender ();
	
private:
	void CreateDelayLines(int32_t inNumDelayLines);
	void DeleteDelayLines();
	
	bool mIsFirstPass;
	CSimpleDelayLine* mDelayLines;
	int32_t mDelaySamples;
};

#endif /* defined(__DemoDelay_HostProcessor__DemoDelay_HostProcessor_Comp__) */
