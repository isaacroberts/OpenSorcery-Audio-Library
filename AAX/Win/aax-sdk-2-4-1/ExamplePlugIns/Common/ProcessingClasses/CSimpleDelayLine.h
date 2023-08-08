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
 *	\file   CSimpleDelayLine.h
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/

#ifndef __DemoDelay__CSimpleDelayLine__
#define __DemoDelay__CSimpleDelayLine__

#include <stdint.h>


class CSimpleDelayLine
{
public:
	CSimpleDelayLine();
	~CSimpleDelayLine();
	
	static int32_t GetMaxDelay() { return sDelayLineLength; }
	
	void ProcessSamples(int32_t inNumSamples, uint32_t inDelaySamples, const float* inAudioInput, float* outAudioOutput);
	void Clear();
	
private:
	// 500 ms @ 192k = 57600. (fits in 2^16 samples)
	static const uint32_t sDelayLineLength = 2 << 15;
	static const uint32_t sDelayLineMask = sDelayLineLength - 1;
	
	int32_t mPos;
	float mDelayLine[sDelayLineLength];
};

#endif /* defined(__DemoDelay__CSimpleDelayLine__) */
