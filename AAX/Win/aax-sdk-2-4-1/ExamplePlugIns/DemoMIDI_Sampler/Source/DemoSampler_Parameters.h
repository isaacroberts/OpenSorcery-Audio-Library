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
 *	\file   DemoSampler_Parameters.h
 *
 *	\brief  DemoSampler_Parameters class declaration.
 *
 *  \author Andriy Goshko
 */ 
/*================================================================================================*/ 
#pragma once
#ifndef DEMOSAMPLER_PARAMETERS_H
#define DEMOSAMPLER_PARAMETERS_H

// AAX Includes
#include "AAX_CMonolithicParameters.h"
#include "AAX_CMutex.h"

// Standard Includes
#include <map>
#include <set>
#include <list>
#include <utility>


enum EDrumType
{
	eBass = 0,
	eSnare,
	eTom,
	eDrum_Count
};

enum EBeatType
{
	eNone = 0,
	eAccented,
	eUnaccented
};

//==============================================================================
class CPattern
{
public:
	CPattern (void) {};
	virtual ~CPattern (void) {};
	virtual void AddDrum (EDrumType drum, int32_t beatPosition, int32_t beatDelay);
	virtual bool isDrumTime(int32_t inDrum, int32_t beat, int32_t tick) const;

private:
	struct SSoundPosition
	{
		int32_t		mBeatNumber;
		int32_t		mBeatOffset;
		EDrumType	mDrum;
	};

	std::vector<SSoundPosition>	mSounds;
};

//==============================================================================
class DemoSampler_Parameters : public AAX_CMonolithicParameters
{
public:
	DemoSampler_Parameters (void);
	~DemoSampler_Parameters (void) AAX_OVERRIDE;
	
	static AAX_CEffectParameters *AAX_CALLBACK	Create();
	
public:
	//Overrides from AAX_CMonolithicParameters
	AAX_Result			EffectInit() AAX_OVERRIDE;
	void				RenderAudio(AAX_SInstrumentRenderInfo* ioRenderInfo, const TParamValPair* inSynchronizedParamValues[], int32_t inNumSynchronizedParamValues) AAX_OVERRIDE;

private:
	void				ResetSampler();
	void				UpdateCoefficients(const TParamValPair* inSynchronizedParamValues[], int32_t inNumSynchronizedParamValues);
	void				ProcessInputMIDI(const AAX_CMidiPacket*& ioPacketPtr, uint32_t& ioPacketsRemaining, uint32_t inSampleTime);
	void				ProcessGlobalMIDI(const AAX_CMidiPacket*& ioPacketPtr, uint32_t& ioPacketsRemaining, uint32_t inSampleTime);
	void				ProcessPattern();
	int32_t				GetSamplesPerTick();
	
private:
	static const int	cNumPatterns = 3;
	
	bool				mAlgBypass;
	const CPattern*		mCurrentPattern;
	
	float*				mSampleBanks[eDrum_Count];
	int32_t				mBanksLength[eDrum_Count];
	int32_t				mPendingDrums[eDrum_Count];
	CPattern*			mPatterns[cNumPatterns];
	
	EBeatType			mPendingBeat;				// the new incoming beat-start event.
	uint32_t			mTicksPerBeat;
	double				mTempo;
	
	int32_t				mBeatNumber;				// during playback: current beat number
	int32_t				mTickNumber;				// during playback: current tick number inside beat
	int32_t				mSampleNumber;				// during playback: number of current sample inside tick
};

#endif
