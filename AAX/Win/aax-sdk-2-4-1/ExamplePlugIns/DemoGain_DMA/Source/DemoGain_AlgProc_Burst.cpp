/*================================================================================================*/
/*
 *	Copyright 2008-2015 by Avid Technology, Inc.
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


// Component includes
#include "DemoGain_AlgProc_Burst.h"
#include "DemoGain_Defs.h"

// AAX includes
#include "AAX.h"

// Standard includes
#include <math.h>		// for fabsf()
#include <algorithm>	// for max()



//==============================================================================
// Processing function definition
//==============================================================================

void
AAX_CALLBACK
DemoGain_DMABurst_AlgorithmProcessFunction (
	SDemoGain_DMABurst_Alg_Context * const	inInstancesBegin [],
	const void *							inInstancesEnd)
{
	// Get a pointer to the beginning of the memory block table
	SDemoGain_DMABurst_Alg_Context* AAX_RESTRICT instance = inInstancesBegin [0];


	//--------- Iterate over plug-in instances ---------//
	//
	for (SDemoGain_DMABurst_Alg_Context * const * walk = inInstancesBegin; walk < inInstancesEnd; ++walk)
	{
		instance = *walk;

		//--------- Retrieve instance-specific information ---------//
		//
		// Coefs
		const SDemoGain_DMA_CoefsGain*	const AAX_RESTRICT	coefsGainP			= instance->mCoefsGainP;	// Input (const)		
		const float											gain				= coefsGainP->mGain;
		// Bypass
		const int32_t										bypass				= *instance->mCtrlBypassP; // 'int' not 'bool' for optimization
		// Buffer length
		const int32_t										bufferlength		= *instance->mBufferLength;
		// DMA facilities
		      AAX_IDma*					const AAX_RESTRICT	dmaBurstP			= instance->ext.mDmaBurstP;
		// Meter taps
		      float*					const AAX_RESTRICT	meterTaps			= *instance->mMetersPP;
		// Private data - state
		      SDemoGain_Burst_DMALocalData*	const AAX_RESTRICT	localDMAData	= instance->ext.mDMALocalDataP;
		const uint32_t										externalWritePos	= localDMAData->curExternalWritePosition;
		      float*					const				localAudioBuffer	= localDMAData->mSample;
		// Private data - external buffer
		      float*					const				dmaAudioBuffer		= instance->ext.mDMAAudioBufferP->mSample;
		// Audio channels
		const float*					const AAX_RESTRICT	pdI					= instance->mInputPP [0]; // First channel (mono)
		      float*					const AAX_RESTRICT	pdO					= instance->mOutputPP [0]; // First channel (mono)
		
		// Local data
		const uint32_t						externalReadPos = (externalWritePos - cDemoGain_DMABurst_ReadOffset) & cDemoGain_DMABurst_ExternalBufferLengthMask;
		      float*	const				dmaAudioBuffer_Write = &(dmaAudioBuffer[externalWritePos]);
		const float*	const				dmaAudioBuffer_Read = &(dmaAudioBuffer[externalReadPos]);
		const uint32_t						localWritePos = localDMAData->curLocalWritePosition;
		const uint32_t						localReadPos = (localWritePos - cDemoGain_DMABurst_ReadOffset) & cDemoGain_DMABurst_LocalReadBufferLengthMask;
		      float*	const AAX_RESTRICT	localAudioBuffer_Write = &(localAudioBuffer[localWritePos]);
		const float*	const AAX_RESTRICT	localAudioBuffer_Read = &(localAudioBuffer[localReadPos]);
		const uint32_t						curBufferSize = bufferlength * sizeof(float);
		
		
		//--------- Post DMA requests ---------//
		//
		{
			// Copy data from external memory
			dmaBurstP->SetSrc ((int8_t *)dmaAudioBuffer_Read);
			dmaBurstP->SetDst ((int8_t *)localAudioBuffer_Write);
			dmaBurstP->SetBurstLength (cDemoGain_DMABurst_BurstLength);
			dmaBurstP->SetNumBursts (cDemoGain_DMABurst_NumBursts);
			dmaBurstP->SetTransferSize (curBufferSize);
			dmaBurstP->PostRequest();	
			
			// Copy data to external memory
			dmaBurstP->SetSrc ((int8_t *)pdI);
			dmaBurstP->SetDst ((int8_t *)dmaAudioBuffer_Write);
			dmaBurstP->SetBurstLength (cDemoGain_DMABurst_BurstLength);
			dmaBurstP->SetNumBursts (cDemoGain_DMABurst_NumBursts);
			dmaBurstP->SetTransferSize (curBufferSize);
			dmaBurstP->PostRequest();
		}
		
		
		//--------- Process samples from last DMA transfer ---------//
		//
		{
			for (int t = 0; t < bufferlength; t++)
			{
				// Process audio samples
				//
				pdO [t] = gain * localAudioBuffer_Read [t];
				if (bypass) { pdO [t] = localAudioBuffer_Read [t]; }
			} // Go to the next sample
		}

		
		//--------- Metering ---------//
		//
		// Accumulate the max value for metering. This will get cleared for us by the shell
		// when it sends the accumulated value up to the host.
		for (int t = 0; t < bufferlength; t++)
		{
			meterTaps[eMeterTap_PreGain] = std::max(fabsf(localAudioBuffer_Read [t]), meterTaps[eMeterTap_PreGain]);
			meterTaps[eMeterTap_PostGain] = std::max(fabsf(pdO [t]), meterTaps[eMeterTap_PostGain]);
		} // Go to the next sample
		
		
		//--------- Store instance-specific information ---------//
		//
		{
			// Audo buffer write head positions
			localDMAData->curExternalWritePosition = (externalWritePos + bufferlength) & cDemoGain_DMABurst_ExternalBufferLengthMask;
			localDMAData->curLocalWritePosition = (localWritePos + bufferlength) & cDemoGain_DMABurst_LocalReadBufferLengthMask;
		}
	} // End instance-iteration loop
}
