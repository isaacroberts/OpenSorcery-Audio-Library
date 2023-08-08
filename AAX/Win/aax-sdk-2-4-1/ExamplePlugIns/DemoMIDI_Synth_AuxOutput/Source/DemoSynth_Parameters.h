/*================================================================================================*/
/*
 *	Copyright 2012-2015, 2019 by Avid Technology, Inc.
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
 *	\file   DemoSynth_Parameters.h
 *
 *	\brief  DemoSynth_Parameters class declaration.
 *
 *  \author Andriy Goshko
 */ 
/*================================================================================================*/ 
#pragma once
#ifndef DEMOSYNTH_PARAMETERS_H
#define DEMOSYNTH_PARAMETERS_H

// DemoSynth Includes
#include "DemoSynth_Defs.h"

// AAX Includes
#include "AAX_CMonolithicParameters.h"
#include "AAX_CMutex.h"

// Standard Includes
#include <map>
#include <set>
#include <vector>
#include <list>
#include <utility>

class AAX_IParameter;
class CSimpleSynth;

//==============================================================================
class DemoSynth_Parameters : public AAX_CMonolithicParameters
{
public:
	DemoSynth_Parameters (void);
	~DemoSynth_Parameters (void) AAX_OVERRIDE;
	
	static AAX_CEffectParameters *AAX_CALLBACK	Create();
	
public:
	//Overrides from AAX_CMonolithicParameters
	AAX_Result	EffectInit() AAX_OVERRIDE;
	AAX_Result	UpdateParameterNormalizedValue(AAX_CParamID iParamID, double aValue, AAX_EUpdateSource inSource) AAX_OVERRIDE;
	void		RenderAudio(AAX_SInstrumentRenderInfo* ioRenderInfo, const TParamValPair* inSynchronizedParamValues[], int32_t inNumSynchronizedParamValues) AAX_OVERRIDE;
	
private:
	void		InitializeSynthesizerState(CSimpleSynth& ioSynth, int32_t inIndex);
	void		UpdateCoefficients(const TParamValPair* inSynchronizedParamValues[], int32_t inNumSynchronizedParamValues);
	void		ProcessInputMIDI(CSimpleSynth& ioSynth, uint32_t inSample, AAX_CMidiPacket*& ioPacket, uint32_t& ioPacketsRemaining);
	
	static const std::vector<AAX_IString*>& GetNodeAssignmentStateStrings();
	
private:
	enum ENodeAssignment
	{
		eNodeAssignment_None = 0 // Node is muted
		,eNodeAssignment_Direct = 1 // Assign to only the plug-in's immediate audio output
		,eNodeAssignment_All = 2 // Assign to the plug-in's immediate audio output and to all AOS stems
		,eNodeAssignment_SingleStemBegin = 3 // First index for single-AOS assignment selectors
		,eNodeAssignment_SingleStemEnd = eNodeAssignment_SingleStemBegin + kNumAuxOutputStems // One-past-the-last index for single-AOS assignment selectors
		
		,eNodeAssignment_NumStates = eNodeAssignment_SingleStemEnd
		,eNodeAssignment_Default = eNodeAssignment_Direct
	};
	
private:
	ENodeAssignment		mNodeAssignments[kNumAuxOutputStems];
	std::vector<CSimpleSynth> mSynthesizers;
};

#endif
