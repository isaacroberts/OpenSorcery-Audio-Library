/*================================================================================================*/
/*
 *	Copyright 2010-2015, 2017, 2019 by Avid Technology, Inc.
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
 *	\file   RectiFi_EffectParameters.h
 *
 *	\brief  RectiFi_EffectParameters class declaration.
 *
 */ 
/*================================================================================================*/ 

#ifndef RECTIFI_EFFECTPARAMETERS_H
#define RECTIFI_EFFECTPARAMETERS_H

#include "AAX_CEffectParameters.h"


#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_HOST
#include AAX_ALIGN_FILE_END
	struct SCompleteRectiFiChunk
	{
		int32_t					fPreFilter;
		int32_t					fRectiType;
		int32_t					fEffectLevel;
		int32_t					fPostFilter;
		int32_t					fEffectMix;
	};
#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_RESET
#include AAX_ALIGN_FILE_END


// *******************************************************************************
// CLASS:	RectiFi_EffectParameters
// *******************************************************************************
class RectiFi_EffectParameters : public AAX_CEffectParameters
{
public:
	//Constructor
	RectiFi_EffectParameters ();
	
	static AAX_CEffectParameters *AAX_CALLBACK Create();
	//Overrides from AAX_CEffectParameters
	AAX_Result			EffectInit();

protected:
	AAX_Result	PreFilt_ProcessFunction(  AAX_CPacket& ioPacket );
	AAX_Result	PostFilt_ProcessFunction( AAX_CPacket& ioPacket );
	AAX_Result	Gain_ProcessFunction( AAX_CPacket& ioPacket );
	AAX_Result	Mix_ProcessFunction( AAX_CPacket& ioPacket );
private:
	AAX_Result SetChunk( AAX_CTypeID chunkID, const AAX_SPlugInChunk * chunk);
	void       ByteSwapChunk(SCompleteRectiFiChunk* out, const SCompleteRectiFiChunk* in) const;
	AAX_Result CompareActiveChunk( const AAX_SPlugInChunk * aChunkP, AAX_CBoolean * aIsEqualP ) const;
	AAX_Result GetNumberOfChunks ( int32_t * numChunks ) const;
	AAX_Result GetChunkIDFromIndex ( int32_t index, AAX_CTypeID * chunkID ) const;
	AAX_Result GetChunkSize ( AAX_CTypeID chunkID, uint32_t * size ) const;
	AAX_Result GetChunk ( AAX_CTypeID chunkID, AAX_SPlugInChunk * chunk ) const;
	int32_t    GetChunkValueForParameter(const char* identifier) const;
	AAX_Result ResetFieldData (AAX_CFieldIndex inFieldIndex, void * inData, uint32_t inDataSize) const;

private:	
	AAX_CSampleRate sampleRateContr;
    AAX_CParameter<double> * mParameterGain;
    AAX_CParameter<int32_t> * mParameterRectification;
};


#endif
