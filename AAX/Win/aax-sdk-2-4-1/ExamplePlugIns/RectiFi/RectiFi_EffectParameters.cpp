/*================================================================================================*/
/*
 *	Copyright 2010-2015, 2017, 2021 by Avid Technology, Inc.
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

//AAX includes
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CStateTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CStateDisplayDelegate.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"
#include "AAX_SliderConversions.h"
#include "AAX_CLogTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CBinaryTaperDelegate.h"

#include "AAX_Assert.h"
#include "AAX_EndianSwap.h"

//RectiFi includes
#include "RectiFi_UnitDisplayDecorator.h"
#include "RectiFi_EffectParameters.h"
#include "RectiFi_Defs.h"
#include "RectiFi_Alg.h"

// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_CEffectParameters *AAX_CALLBACK RectiFi_EffectParameters::Create()
{
	AAX_CEffectParameters *effectParameters = new RectiFi_EffectParameters();
	return effectParameters;
}
// *******************************************************************************
// METHOD:	EffectInit
// *******************************************************************************
RectiFi_EffectParameters::RectiFi_EffectParameters () : AAX_CEffectParameters() 
{
}

// *******************************************************************************
// METHOD:	EffectInit
// *******************************************************************************
AAX_Result RectiFi_EffectParameters::EffectInit()
{	
	// Bypass
	AAX_CString bypassID =cDefaultMasterBypassID;
	AAX_IParameter * masterBypass = new AAX_CParameter<bool>(bypassID.CString(), AAX_CString("Master Bypass"), false,
		AAX_CBinaryTaperDelegate<bool>(), AAX_CBinaryDisplayDelegate<bool>("bypass", "on"), true);
	masterBypass->SetNumberOfSteps( 2 );
	masterBypass->SetType( AAX_eParameterType_Discrete );
	masterBypass->AddShortenedName(AAX_CString("MByp"));
	mParameterManager.AddParameter( masterBypass );  

	Controller()->GetSampleRate(&sampleRateContr);	

	AAX_IParameter * parameterPreFilter = new AAX_CParameter<double>( RectiFi_ID_PreFilter, AAX_CString( RectiFi_ID_PreFilter ), 204.85950699477874,
		AAX_CLogTaperDelegate<double, 0>(kMinFilterFrequency, kMaxFilterFrequency),
		RectiFi_UnitDisplayDelegateDecorator<double>( AAX_CNumberDisplayDelegate<double,0>(), "Hz" ), true);
	parameterPreFilter->SetNumberOfSteps ( 1001 );
	parameterPreFilter->AddShortenedName(AAX_CString("PreFiltr"));
	parameterPreFilter->AddShortenedName(AAX_CString("PreFil"));
	parameterPreFilter->AddShortenedName(AAX_CString("PreF"));	
	parameterPreFilter->AddShortenedName(AAX_CString("PrF"));	
	mParameterManager.AddParameter( parameterPreFilter );
	
	const char * sRectiFiTypes[] = { "Positive", "Negative", "Alternate", "Alt-Max", 0 };
	mParameterRectification = new AAX_CParameter<int32_t>( RectiFi_ID_Rectification, AAX_CString( RectiFi_ID_Rectification ), 1,
		AAX_CStateTaperDelegate<int32_t>(-1, 2), AAX_CStateDisplayDelegate<int32_t>( sRectiFiTypes, -1 ), true);
	mParameterRectification->SetNumberOfSteps ( 4 );
	mParameterRectification->SetType ( AAX_eParameterType_Discrete );
	mParameterRectification->AddShortenedName(AAX_CString("Rectifi"));
	mParameterRectification->AddShortenedName(AAX_CString("Rectfi"));
	mParameterRectification->AddShortenedName(AAX_CString("Rect"));	
	mParameterRectification->AddShortenedName(AAX_CString("Rct"));
	mParameterManager.AddParameter( mParameterRectification );
	
	mParameterGain = new AAX_CParameter<double>( RectiFi_ID_Gain, AAX_CString( RectiFi_ID_Gain ), 0.0,
		AAX_CLinearTaperDelegate<double, 0>( -18.0, 18.0),
		AAX_CUnitDisplayDelegateDecorator<double>( AAX_CNumberDisplayDelegate<double,1>(), "dB" ), true);
	mParameterGain->SetNumberOfSteps ( 361 );
	mParameterGain->AddShortenedName(AAX_CString("Gn"));
	mParameterManager.AddParameter( mParameterGain );

	AAX_CParameter<double> * parameterPostFilter = new AAX_CParameter<double>(RectiFi_ID_PostFilter, AAX_CString( RectiFi_ID_PostFilter ), 204.85950699477874,
		AAX_CLogTaperDelegate<double, 0>(kMinFilterFrequency, kMaxFilterFrequency),
		RectiFi_UnitDisplayDelegateDecorator<double>( AAX_CNumberDisplayDelegate<double,0>(), "Hz" ), true);
	parameterPostFilter->SetNumberOfSteps ( 1001 );
	parameterPostFilter->AddShortenedName(AAX_CString("PostFilt"));
	parameterPostFilter->AddShortenedName(AAX_CString("PostFl"));
	parameterPostFilter->AddShortenedName(AAX_CString("PstF"));	
	parameterPostFilter->AddShortenedName(AAX_CString("PsF"));
	mParameterManager.AddParameter( parameterPostFilter );
	
	AAX_CParameter<double> * parameterMix = new AAX_CParameter<double>( RectiFi_ID_Mix, AAX_CString( RectiFi_ID_Mix ), 50.0,
		AAX_CLinearTaperDelegate<double, 0>(0.0, 100.0),
		AAX_CUnitDisplayDelegateDecorator<double>( AAX_CNumberDisplayDelegate<double, 0>(), "%" ), true);
	parameterMix->SetNumberOfSteps ( 101 );
	mParameterManager.AddParameter( parameterMix );
	
	// register packets
	//
	// one parameter - one packet - one handler
	mPacketDispatcher.RegisterPacket(bypassID.CString(), eAlgPortID_MasterBypassControlIn);
	mPacketDispatcher.RegisterPacket(RectiFi_ID_Gain, eAlgPortID_GainIn, this, &RectiFi_EffectParameters::Gain_ProcessFunction);
	mPacketDispatcher.RegisterPacket(RectiFi_ID_Rectification, eAlgPortID_RectiTypeIn);
	
	// one parameter - two packets - one handler
	mPacketDispatcher.RegisterPacket(RectiFi_ID_PreFilter, eAlgPortID_CoefsPreFiltIn, this, &RectiFi_EffectParameters::PreFilt_ProcessFunction);
	mPacketDispatcher.RegisterPacket(RectiFi_ID_PostFilter, eAlgPortID_CoefsPostFiltIn, this, &RectiFi_EffectParameters::PostFilt_ProcessFunction);
	
	// two parameters - one packet - one handler
	mPacketDispatcher.RegisterPacket(RectiFi_ID_Mix, eAlgPortID_CoefsEffectMixIn, this, &RectiFi_EffectParameters::Mix_ProcessFunction);
	
	mPacketDispatcher.RegisterPacket(bypassID.CString(), eAlgPortID_CoefsEffectMixIn, this, &RectiFi_EffectParameters::Mix_ProcessFunction);
	
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	PreFilt_ProcessFunction
// *******************************************************************************
AAX_Result RectiFi_EffectParameters::PreFilt_ProcessFunction( AAX_CPacket& ioPacket )
{
	AAX_Result result = AAX_ERROR_NULL_OBJECT;

	AAX_IParameter*	 parameter = mParameterManager.GetParameterByID( RectiFi_ID_PreFilter );
	if (parameter)
	{
		double cutoffFrequency;
		if (parameter->GetValueAsDouble(&cutoffFrequency))
		{		
			SRectiFiCoefs_PreFilter*  messagePayloadP = ioPacket.GetPtr<SRectiFiCoefs_PreFilter>();

			// Retrieve control values
			//
			double sampleRate = static_cast<double>(sampleRateContr);
			double omega = (cutoffFrequency / sampleRate * 6.28);
			omega = 2.0 * tan(omega / 2.0);
			double omegaSquared = omega * omega;
			double normalizer = 1.0 / (4.0 + omegaSquared + 2.0 * omega);
			
			// Populate messagePayloadP
			//
			messagePayloadP->mCoeffPreA2stage1 = (-2.0 * omega + 2.0 + omegaSquared) * normalizer / 4.0;
			messagePayloadP->mCoeffPreA0stage1 = (2.0 + omegaSquared + 2.0 * omega) * normalizer / 4.0;
			messagePayloadP->mCoeffPreA1stage1 = (2.0 * omegaSquared - 4.0) * normalizer / 4.0;
			messagePayloadP->mCoeffPreB2stage1 = (-2.0 * omega + 4.0 + omegaSquared) * normalizer / 2.0;
			messagePayloadP->mCoeffPreB1stage1 = (2.0 * omegaSquared - 8.0) * normalizer / 2.0;
			messagePayloadP->mCoeffPreA0stage2 = -2.0 / (2.0 + omega);
			messagePayloadP->mCoeffPreA1stage2 = 2.0 / (2.0 + omega);
			messagePayloadP->mCoeffPreB1stage2 = (omega - 2.0) / (2.0 + omega) / 2.0;
			//
			// if max sample rate don't filter
			int32_t controlValue = LogDoubleToLongControl(cutoffFrequency, kMinFilterFrequency, kMaxFilterFrequency);
			if (LongControlToNewRange(controlValue, 0, 1000) == 1000)
			{
				// Zero'ing coefficients A0_2 and A1_2 will result in "no filtering"
				messagePayloadP->mCoeffPreA0stage2 = 0.0;
				messagePayloadP->mCoeffPreA1stage2 = 0.0;
			}
			
			result = AAX_SUCCESS;
		}
	}
	return result;
}
// *******************************************************************************
// METHOD:	PostFilt_ProcessFunction
// *******************************************************************************
AAX_Result RectiFi_EffectParameters::PostFilt_ProcessFunction( AAX_CPacket& ioPacket )
{
	AAX_Result result = AAX_ERROR_NULL_OBJECT;

	AAX_IParameter*	 parameter = mParameterManager.GetParameterByID( RectiFi_ID_PostFilter );
	if (parameter)
	{
		double cutoffFrequency;
		if (parameter->GetValueAsDouble(&cutoffFrequency))
		{		
			SRectiFiCoefs_PostFilter*  messagePayloadP = ioPacket.GetPtr<SRectiFiCoefs_PostFilter>();

			// Retrieve control values
			//
			double sampleRate = static_cast<double>(sampleRateContr);
			double omega = (cutoffFrequency / sampleRate * 6.28);
			omega = 2.0 * tan(omega / 2.0);
			double omegaSquared = omega * omega;
			double normalizer = 1.0 / (4.0 + omegaSquared + 2.0 * omega);

			// Populate messagePayloadP
			//
			messagePayloadP->mCoeffPostA2stage1 = (-2.0 * omega + 2.0 + omegaSquared) * normalizer / 4.0;
			messagePayloadP->mCoeffPostA0stage1 = (2.0 + omegaSquared + 2.0 * omega) * normalizer / 4.0;
			messagePayloadP->mCoeffPostA1stage1 = (2.0 * omegaSquared - 4.0) * normalizer / 4.0;
			messagePayloadP->mCoeffPostB2stage1 = (-2.0 * omega + 4.0 + omegaSquared) * normalizer / 2.0;
			messagePayloadP->mCoeffPostB1stage1 = (2.0 * omegaSquared - 8.0) * normalizer / 2.0;
			messagePayloadP->mCoeffPostA0stage2 = -2.0 / (2.0 + omega);
			messagePayloadP->mCoeffPostA1stage2 = 2.0 / (2.0 + omega);
			messagePayloadP->mCoeffPostB1stage2 = (omega - 2.0) / (2.0 + omega) / 2.0;
			//
			// if max sample rate don't filter
			int32_t controlValue = LogDoubleToLongControl(cutoffFrequency, kMinFilterFrequency, kMaxFilterFrequency);
			if (LongControlToNewRange(controlValue, 0, 1000) == 1000)
			{
				// Zero'ing coefficients A0_2 and A1_2 will result in "no filtering"
				messagePayloadP->mCoeffPostA0stage2 = 0.0;
				messagePayloadP->mCoeffPostA1stage2 = 0.0;
			}

			result = AAX_SUCCESS;
		}
	}
	return result;
}

// *******************************************************************************
// METHOD:	Gain_ProcessFunction
// *******************************************************************************
AAX_Result RectiFi_EffectParameters::Gain_ProcessFunction( AAX_CPacket& ioPacket )
{
	AAX_Result result = AAX_ERROR_NULL_OBJECT;

	AAX_IParameter*	 parameter = mParameterManager.GetParameterByID( RectiFi_ID_Gain );
	if (parameter)
	{
		double gainValue;
		if (parameter->GetValueAsDouble(&gainValue))
		{		
			float*  messagePayloadP = ioPacket.GetPtr<float>();
			*messagePayloadP = static_cast<float>(pow (10.0, (gainValue / 20.0)));//used to be float (tick count check is needed)
			
			result = AAX_SUCCESS;
		}	
	}		
	return result;
}

// *******************************************************************************
// METHOD:	Gain_ProcessFunction
// *******************************************************************************
AAX_Result RectiFi_EffectParameters::Mix_ProcessFunction( AAX_CPacket& ioPacket )
{
	AAX_Result result = AAX_ERROR_NULL_OBJECT;
	
	bool bypass = false;
	mParameterManager.GetParameterByID(cDefaultMasterBypassID)->GetValueAsBool(&bypass);
	
	AAX_IParameter*	 parameter = mParameterManager.GetParameterByID( RectiFi_ID_Mix );
	if (parameter)
	{
		double mixValue;
		if (parameter->GetValueAsDouble(&mixValue))
		{		
			SRectiFiCoefs_EffectMix*  messagePayloadP = ioPacket.GetPtr<SRectiFiCoefs_EffectMix>();
			
			// Retrieve control values
			double effectMix = mixValue;
			effectMix /= 100.0;
			
			// Populate messagePayloadP
			if (bypass)
			{
				messagePayloadP->mDryMix	= 1.0f;
				messagePayloadP->mRectiMix	= 0.0f;
			}
			else if (effectMix < 0.5)
			{
				messagePayloadP->mDryMix = static_cast<float>(1.0 - 0.8*effectMix);	// [1.0, 0.6] 
				messagePayloadP->mRectiMix = static_cast<float>(1.2*effectMix);		// [0.0, 0.6]
			}
			else
			{
				messagePayloadP->mDryMix   = static_cast<float>(1.2 - 1.2*effectMix);	// [0.6, 0.0]
				messagePayloadP->mRectiMix = static_cast<float>(0.2 + 0.8*effectMix);	// [0.6, 1.0] 
			}
			
			result = AAX_SUCCESS;
		}
	}
	return result;
}


//===================================================================================================
//	Chunks
//===================================================================================================
void RectiFi_EffectParameters::ByteSwapChunk(SCompleteRectiFiChunk* out, const SCompleteRectiFiChunk* in) const
{
	*out = *in;

	AAX_BigEndianNativeSwapInPlace(&out->fPreFilter);
	AAX_BigEndianNativeSwapInPlace(&out->fRectiType);
	AAX_BigEndianNativeSwapInPlace(&out->fEffectLevel);
	AAX_BigEndianNativeSwapInPlace(&out->fPostFilter);
	AAX_BigEndianNativeSwapInPlace(&out->fEffectMix);
}

//==============================================================================
// GetNumberOfChunks
//==============================================================================
AAX_Result RectiFi_EffectParameters::GetNumberOfChunks ( int32_t * numChunks ) const
{
	*numChunks = 1;
	return AAX_SUCCESS;
}

//==============================================================================
// GetChunkIDFromIndex
//==============================================================================
AAX_Result RectiFi_EffectParameters::GetChunkIDFromIndex ( int32_t index, AAX_CTypeID * chunkID ) const
{
	if (index == 0)
	{
		*chunkID = kCompleteStateChunk;
		return AAX_SUCCESS;
	}
	return AAX_ERROR_INVALID_CHUNK_INDEX;
}


//==============================================================================
// GetChunkSize
//==============================================================================
AAX_Result RectiFi_EffectParameters::GetChunkSize ( AAX_CTypeID chunkID, uint32_t * size ) const
{
	if (chunkID == kCompleteStateChunk)
	{
		*size = sizeof(SCompleteRectiFiChunk);
		return AAX_SUCCESS;
	}
	return AAX_ERROR_INVALID_CHUNK_ID;
}

//==============================================================================
// GetChunk
//==============================================================================
AAX_Result RectiFi_EffectParameters::GetChunk ( AAX_CTypeID chunkID, AAX_SPlugInChunk * chunk ) const
{
	if (chunkID == kCompleteStateChunk)
	{
		SCompleteRectiFiChunk	state;

		chunk->fSize = sizeof(SCompleteRectiFiChunk);
		chunk->fVersion = 1;
		chunk->fManufacturerID = cManufacturerID;
		chunk->fProductID = kProductID;
		//chunk->fPlugInID = cRectiFiMSMonoProcessTypeID;
		chunk->fChunkID = kCompleteStateChunk;
		chunk->fName[0] = 0;

		state.fPreFilter = GetChunkValueForParameter(RectiFi_ID_PreFilter);
		state.fRectiType = GetChunkValueForParameter(RectiFi_ID_Rectification);
		state.fEffectLevel = GetChunkValueForParameter(RectiFi_ID_Gain);
		state.fPostFilter = GetChunkValueForParameter(RectiFi_ID_PostFilter);
		state.fEffectMix = GetChunkValueForParameter(RectiFi_ID_Mix);

		ByteSwapChunk(reinterpret_cast<SCompleteRectiFiChunk*>(chunk->fData), &state);

		return AAX_SUCCESS;
	}

	return AAX_ERROR_INVALID_CHUNK_ID;
}

//==============================================================================
// SetChunk
//==============================================================================
AAX_Result RectiFi_EffectParameters::SetChunk( AAX_CTypeID chunkID, const AAX_SPlugInChunk * chunk)
{
	if (chunkID == kCompleteStateChunk)
	{
		SCompleteRectiFiChunk state;
		const SCompleteRectiFiChunk * inState = reinterpret_cast<const SCompleteRectiFiChunk*>(chunk->fData);
		ByteSwapChunk(&state, inState);

		this->SetParameterNormalizedValue(RectiFi_ID_PreFilter, Int32ToNormalized(state.fPreFilter));
		this->SetParameterNormalizedValue(RectiFi_ID_Rectification, Int32ToNormalized(state.fRectiType));
		this->SetParameterNormalizedValue(RectiFi_ID_Gain, Int32ToNormalized(state.fEffectLevel));
		this->SetParameterNormalizedValue(RectiFi_ID_PostFilter, Int32ToNormalized(state.fPostFilter));
		this->SetParameterNormalizedValue(RectiFi_ID_Mix, Int32ToNormalized(state.fEffectMix));

		return AAX_SUCCESS;
	}
	
	return AAX_ERROR_INVALID_CHUNK_ID;
}

//==============================================================================
// CompareActiveChunk
//==============================================================================
AAX_Result RectiFi_EffectParameters::CompareActiveChunk( const AAX_SPlugInChunk * aChunkP, AAX_CBoolean * aIsEqualP ) const
{
	// Assume that it is not active.	
	*aIsEqualP = false;

	// insure chunk is valid for this plugIn
	if (aChunkP->fChunkID == kCompleteStateChunk)
	{
		// get the chunk
		SCompleteRectiFiChunk state;
		const SCompleteRectiFiChunk * inData = reinterpret_cast<const SCompleteRectiFiChunk*>(aChunkP->fData);
		ByteSwapChunk(&state, inData);

		int32_t iValue;
		
		iValue = GetChunkValueForParameter(RectiFi_ID_PreFilter);
		if(iValue != state.fPreFilter) return AAX_SUCCESS;

        if (mParameterRectification)
        {
            int32_t rValue = mParameterRectification->GetValue();
            double dValue = Int32ToNormalized(state.fRectiType);
            iValue = mParameterRectification->TaperDelegate()->NormalizedToReal(dValue);
            if(rValue!=iValue) return AAX_SUCCESS;
        }

        iValue = GetChunkValueForParameter(RectiFi_ID_Gain);
		if(iValue != state.fEffectLevel) return AAX_SUCCESS;

		iValue = GetChunkValueForParameter(RectiFi_ID_PostFilter);
		if(iValue != state.fPostFilter) return AAX_SUCCESS;

		iValue = GetChunkValueForParameter(RectiFi_ID_Mix);
		if(iValue != state.fEffectMix) return AAX_SUCCESS;

		*aIsEqualP = true;

		return AAX_SUCCESS;
	}

	return AAX_ERROR_INVALID_CHUNK_ID;
}

//==============================================================================
// GetChunkValueForParameter
//==============================================================================
int32_t RectiFi_EffectParameters::GetChunkValueForParameter(const char* identifier) const
{
	double normalizedValue;
	const AAX_IParameter * parameter;
	parameter = mParameterManager.GetParameterByID(identifier);		AAX_ASSERT(parameter);
	normalizedValue = parameter->GetNormalizedValue();
	
	return NormalizedToInt32(normalizedValue);
}

//==============================================================================
// METHOD:	ResetFieldData
//==============================================================================
AAX_Result RectiFi_EffectParameters::ResetFieldData (AAX_CFieldIndex inFieldIndex, void * inData, uint32_t inDataSize) const
{
	if( inFieldIndex == eAlgFieldIndex_State )
	{
		SRectiFiStateBlock *	state = static_cast <SRectiFiStateBlock *> (inData);

		memset (state, 0, sizeof (*state));
		state->mRectiPositive[0] = (state->mRectiType != 0);
		state->mRectiPositive[1] = (state->mRectiType != 0);

		return AAX_SUCCESS;
	}

	return AAX_CEffectParameters::ResetFieldData(inFieldIndex, inData, inDataSize);
}
