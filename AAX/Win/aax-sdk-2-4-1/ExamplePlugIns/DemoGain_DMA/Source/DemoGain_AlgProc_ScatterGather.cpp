/*================================================================================================*/
/*
 *	Copyright 2012-2015 by Avid Technology, Inc.
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
#include "DemoGain_AlgProc_ScatterGather.h"
#include "DemoGain_Defs.h"

// AAX includes
#include "AAX.h"
#include "AAX_Assert.h"
#include "AAX_MiscUtils.h"	// for AAX::Max()


//==============================================================================
// Processing function definition
//==============================================================================

void
AAX_CALLBACK
DemoGain_DMASG_AlgorithmProcessFunction (
	SDemoGain_DMASG_Alg_Context * const	inInstancesBegin [],
	const void *						inInstancesEnd)
{
	// Get a pointer to the beginning of the memory block table
	SDemoGain_DMASG_Alg_Context* AAX_RESTRICT instance = inInstancesBegin [0];

	
	//--------- Iterate over plug-in instances ---------//
	//
	for (SDemoGain_DMASG_Alg_Context * const * walk = inInstancesBegin; walk < inInstancesEnd; ++walk)
	{
		instance = *walk;
		
		//--------- Retrieve instance-specific information ---------//
		// 
		// Coefs
		const SDemoGain_DMA_CoefsGain*     const AAX_RESTRICT	coefsGainP         = instance->mCoefsGainP;		
		const float                                             gain               = coefsGainP->mGain;
		// Bypass
		const int32_t                                           bypass             = *instance->mCtrlBypassP;
		// Buffer length
		const int32_t                                           curBufferLength    = *instance->mBufferLength;
		// DMA facilities
		      AAX_IDma*                      const AAX_RESTRICT dmaScatterP        = instance->ext.mDmaScatterP;
		      AAX_IDma*                      const AAX_RESTRICT dmaGatherP         = instance->ext.mDmaGatherP;
		// Meter taps
		      float*                         const AAX_RESTRICT	meterTaps          = *instance->mMetersPP;
		// Private data	- State	
		      SDemoGain_DMASG_DMALocalData*  const AAX_RESTRICT	localDataP         = instance->ext.mDMALocalDataP;
		      float*                         const              localReadBuffer_f  = localDataP->mReadBuffer; // Not sure if restrict is appropriate: when buffer size is max repeatedly, this sample will be touched during re-ordering
		      uint8_t*                       const              localWriteBuffer_B = localDataP->mWriteBuffer;
		const uint32_t                                          curLocalReadPos    = localDataP->mCurLocalReadPosition;
		const uint32_t                                          lastBufferLength   = localDataP->mLastBufferLength;
		// Private data - External buffer
		      SDemoGain_DMASG_RWBuffer*      const AAX_RESTRICT	extBufferP         = instance->ext.mDMAAudioBufferP;
		// Private date - Scratch buffer
		      SDemoGain_DMASG_ScratchBuffer* const AAX_RESTRICT	scratchBufferP     = instance->ext.mScratchBufferP;
		      uint8_t*                       const              scratchBuffer_B    = scratchBufferP->mSample;
			  float*                         const              scratchBuffer_f    = reinterpret_cast<float*> (scratchBuffer_B);
		// Audio channels
#if DEMOGAIN_DMA_TEST_MODE
		const float*                         const AAX_RESTRICT pdI                = &localDataP->mTestModeInputSamples[localDataP->mTestModeSampleCount];
		float*                               const AAX_RESTRICT pdO                = &localDataP->mTestModeOutputSamples[localDataP->mTestModeSampleCount];
#else
		const float*                         const AAX_RESTRICT pdI                = instance->mInputPP [0]; // First channel (mono)
		float*                               const AAX_RESTRICT pdO                = instance->mOutputPP [0]; // First channel (mono)
#endif

		// Local data
		const uint32_t curBufferSize = sizeof(float) * curBufferLength;
		const uint32_t lastBufferSize = sizeof(float) * lastBufferLength;
		const uint32_t curBurstSize = Pow2Div(curBufferSize, cDemoGain_DMASG_NumReadWritePoints);
		const uint32_t curLocalGatherPos = (curLocalReadPos + cDemoGain_DMA_MaxBufferLength) & cDemoGain_DMASG_LocalReadBufferLengthMask;
		const uint32_t lastGatherBufferStart = (curLocalGatherPos + cDemoGain_DMASG_LocalReadBufferLength - lastBufferLength) & cDemoGain_DMASG_LocalReadBufferLengthMask;
		

		//--------- Process samples from last Gather ---------//
		//
		{
			// SORT READ BUFFER TO SCRATCH BUFFER
			const int8_t* localReadBufferAtGatherPos_B = reinterpret_cast<int8_t*> (&localReadBuffer_f[lastGatherBufferStart]);
			for (uint32_t i = 0; i < lastBufferSize; i++)
			{	
				scratchBuffer_B[i] = localReadBufferAtGatherPos_B[TransformOffset(i, cDemoGain_DMASG_GatherBurstSize)];
			}
			
			// WRITE SORTED DATA BACK TO READ BUFFER
			for (uint32_t t = 0; t < lastBufferLength; t++)
			{
				uint32_t localReadBufferIndex_f = (lastGatherBufferStart + t) & cDemoGain_DMASG_LocalReadBufferLengthMask;
				localReadBuffer_f[localReadBufferIndex_f] = scratchBuffer_f[t];
			}
			
			// OUTPUT FROM READ BUFFER
			for (int32_t t = 0; t < curBufferLength; t++)
			{
				const uint32_t readPos = (curLocalReadPos + t) & cDemoGain_DMASG_LocalReadBufferLengthMask;
				pdO[t] = bypass ? localReadBuffer_f[readPos] : gain * localReadBuffer_f[readPos] ;
			}
			
			// SORT INPUT TO WRITE BUFFER
			const int8_t* const pdI_B = reinterpret_cast<const int8_t*> (pdI);
			for (uint32_t i = 0; i < curBufferSize; i++)
			{
				localWriteBuffer_B[TransformOffset(i,curBurstSize)] = pdI_B[i];
			}
		}

		
		//--------- Metering ---------//
		//
		// Accumulate the max value for metering. This will get cleared for us by the shell
		// when it sends the accumulated value up to the host.
		for (int tf = 0; tf < curBufferLength; tf++)
		{
			const float curReadSamp = localReadBuffer_f[(curLocalReadPos + tf) & cDemoGain_DMASG_LocalReadBufferLengthMask];
			meterTaps[eMeterTap_PreGain] = AAX::Max(AAX::fabsf(curReadSamp), meterTaps[eMeterTap_PreGain]);
			meterTaps[eMeterTap_PostGain] = AAX::Max(AAX::fabsf(pdO[tf]), meterTaps[eMeterTap_PostGain]);
		} // Go to the next sample	
		
		
		//--------- Post DMA requests ---------//
		//
		{			
			// Since we may have variable buffer sizes on Native systems
			// and we cannot incorporate a local buffer wraparound
			// directly within the Gather request, we pad the local
			// read buffer by the maximum buffer length and use a "soft
			// wraparound"
			//
			// Gather: read the maximum possible number of samples for a
			// single render callback on this platform from the external
			// buffer into local memory
			dmaGatherP->SetFifoBuffer(reinterpret_cast<int8_t*> (extBufferP->mSample));						// Setup a pointer to the src buffer
			dmaGatherP->SetFifoSize(cDemoGain_DMASG_ExternalBufferSize);									// Report the size of src buffer
			dmaGatherP->SetLinearBuffer(reinterpret_cast<int8_t*> (&localReadBuffer_f[curLocalGatherPos]));	// Setup a pointer to the dst buffer
			dmaGatherP->SetOffsetTable(localDataP->mExtBufferOffsetTable);									// Setup a pointer to the table of offsets
			dmaGatherP->SetBaseOffset(localDataP->mCurExternalReadOffset);									// Report the base offset in bytes
			dmaGatherP->SetNumOffsets(cDemoGain_DMASG_NumReadWritePoints);									// Report the number of offsets
			dmaGatherP->SetBurstLength(cDemoGain_DMASG_GatherBurstSize);									// Report the burst size in bytes
			dmaGatherP->PostRequest();																		// Send the DMA request			
			//
			// Scatter: Write data from a preformatted local buffer to
			// a series of "buckets" in the external buffer such that
			// samples from multiple variable-length writes can be
			// re-constructed into the original order when retrieved
			dmaScatterP->SetFifoBuffer(reinterpret_cast<int8_t*> (extBufferP->mSample));	// Setup a pointer to the dst buffer
			dmaScatterP->SetFifoSize(cDemoGain_DMASG_ExternalBufferSize);					// Report the size of dst buffer
			dmaScatterP->SetLinearBuffer(reinterpret_cast<int8_t*> (localWriteBuffer_B));	// Setup a pointer to the src buffer
			dmaScatterP->SetOffsetTable(localDataP->mExtBufferOffsetTable);					// Setup a pointer to the table of offsets
			dmaScatterP->SetBaseOffset(localDataP->mCurExternalWriteOffset);				// Report the base offset in bytes
			dmaScatterP->SetNumOffsets(cDemoGain_DMASG_NumReadWritePoints);					// Report the number of offsets
			dmaScatterP->SetBurstLength(curBurstSize);										// Report the burst size in bytes
			dmaScatterP->PostRequest();														// Send the DMA request	
		}
		
		
		//--------- Store instance-specific information ---------//
		//
		{
			localDataP->mLastBufferLength = curBufferLength;
			localDataP->mCurLocalReadPosition = (curLocalReadPos + curBufferLength) & cDemoGain_DMASG_LocalReadBufferLengthMask;
			localDataP->mCurExternalWriteOffset = (localDataP->mCurExternalWriteOffset + curBurstSize) & cDemoGain_DMASG_ExternalBufferSegmentSizeMask;
			localDataP->mCurExternalReadOffset = (localDataP->mCurExternalReadOffset + curBurstSize) & cDemoGain_DMASG_ExternalBufferSegmentSizeMask;
		}
		
		
		
		//--------- TEST: Verify algorithmic delay ---------//
		//
#if DEMOGAIN_DMA_TEST_MODE
		{
			const uint32_t samplecount = localDataP->mTestModeSampleCount;
			if (samplecount >= cDemoGain_DMASG_LatencyContribution)
			{
				//const float* latentOutput = &localDataP->mTestModeOutputSamples[cDemoGain_DMASG_LatencyContribution];
				const uint32_t offset = (SDemoGain_DMASG_DMALocalData::TestModeBufferLength + samplecount - cDemoGain_DMASG_LatencyContribution) & ((SDemoGain_DMASG_DMALocalData::TestModeBufferLength)-1);
				for (int t = 0; t < curBufferLength; t++)
				{
					float curSamp = localDataP->mTestModeInputSamples[(offset + t) & ((SDemoGain_DMASG_DMALocalData::TestModeBufferLength)-1)];
					if (!bypass)
					{
						curSamp *= gain;
					}

					AAX_ASSERT (pdO[t] == curSamp);
				}
			}
			
			// Pass-through actual output
			for (int t = 0; t < curBufferLength; t++)
			{
				instance->mOutputPP [0][t] = instance->mInputPP [0][t];
			}
			
			localDataP->TestModeRecordSamples (pdI, curBufferLength);
		}
#endif
		
	} // End instance-iteration loop
}
