/*================================================================================================*/
/*
 *	Copyright 2009-2017, 2019 by Avid Technology, Inc.
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

#include "AAX_ICollection.h"
#include "AAX_IComponentDescriptor.h"
#include "AAX_IEffectDescriptor.h"
#include "AAX_IPropertyMap.h"
#include "AAX_Assert.h"

// Component headers
#include "RectiFi_Defs.h"
#include "RectiFi_Describe.h"
#include "RectiFi_Alg.h"
#include "RectiFi_AlgProc.cpp" // Needed due to templatized function
#include "RectiFi_EffectParameters.h"

static const AAX_CEffectID kEffectID_RectiFI = "com.avid.aax.rectifi.example";

void DescribeAlgComponent ( AAX_EStemFormat inStemFormat, AAX_IComponentDescriptor & algDesc);

/*===================================================================================================*/
AAX_Result GetEffectDescriptions ( AAX_ICollection * outDescriptions )
{
	AAX_Result				result = AAX_SUCCESS;
	AAX_IEffectDescriptor * aDesc = outDescriptions->NewDescriptor();
	
	if ( aDesc )
	{
		result = Rectifi_GetPlugInDescription( *aDesc );
		if ( result == AAX_SUCCESS )
		{
			outDescriptions->AddEffect(kEffectID_RectiFI, aDesc);
			outDescriptions->SetManufacturerName( "Avid" );
			outDescriptions->AddPackageName( "Recti-Fi Example" );
		}
	}
	else result = AAX_ERROR_NULL_OBJECT;

	return result;
}

// *******************************************************************************
// ROUTINE:	RectiFi_EffectParameters_Create
// *******************************************************************************
static AAX_CEffectParameters *AAX_CALLBACK RectiFi_EffectParameters_Create()
{
	AAX_CEffectParameters *effectParameters = new RectiFi_EffectParameters();
	return effectParameters;
}

/*===================================================================================================*/
AAX_Result Rectifi_GetPlugInDescription(AAX_IEffectDescriptor &outDescriptor)
{
	int							err;
	AAX_IComponentDescriptor *	compDesc = outDescriptor.NewComponentDescriptor ();
	
	// Add component descriptors: 
	//
	// Alg component
	DescribeAlgComponent ( AAX_eStemFormat_Mono, *compDesc);
	err = outDescriptor.AddComponent ( compDesc );			AAX_ASSERT (err == 0);
	
	compDesc->Clear ();
	DescribeAlgComponent ( AAX_eStemFormat_Stereo, *compDesc);
	err = outDescriptor.AddComponent ( compDesc );			AAX_ASSERT (err == 0);


	AAX_IPropertyMap *	properties = outDescriptor.NewPropertyMap ();
	properties->AddProperty ( AAX_eProperty_ManufacturerID, cManufacturerID );
	properties->AddProperty ( AAX_eProperty_ProductID, kProductID );
	properties->AddProperty ( AAX_eProperty_UsesClientGUI, true ); // Use auto-generated GUI for this example
	outDescriptor.AddProcPtr( (void *) RectiFi_EffectParameters_Create, kAAX_ProcPtrID_Create_EffectParameters );
	
	outDescriptor.AddCategory( AAX_ePlugInCategory_Example );
	outDescriptor.AddCategory( AAX_ePlugInCategory_Harmonic );
	
	outDescriptor.AddName( "Recti-Fi (Example)" );
	outDescriptor.AddName( "Recti-Fi" );
	outDescriptor.AddName( "ReFi" );
	outDescriptor.AddName( "RF" );
	outDescriptor.SetProperties(properties);
	
	// Meter properties
	AAX_IPropertyMap* inputMeterProperties = outDescriptor.NewPropertyMap();
	if ( !inputMeterProperties ) return AAX_ERROR_NULL_OBJECT;

	inputMeterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Input );
	inputMeterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );

	AAX_IPropertyMap* outputMeterProperties = outDescriptor.NewPropertyMap();
	if ( !outputMeterProperties ) return AAX_ERROR_NULL_OBJECT;

	outputMeterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Output );
	outputMeterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );

	outDescriptor.AddMeterDescription( cInputMeterVal, RectiFi_Name_InputMeterL, inputMeterProperties );
	outDescriptor.AddMeterDescription( cInputMeterValStereo, RectiFi_Name_InputMeterR, inputMeterProperties );
	outDescriptor.AddMeterDescription( cOutputMeterVal, RectiFi_Name_OutputMeterL, outputMeterProperties );
	outDescriptor.AddMeterDescription( cOutputMeterValStereo, RectiFi_Name_OutputMeterR, outputMeterProperties );

	return AAX_SUCCESS;}

//==============================================================================
//	AAX: Component description: Algorithm component
//==============================================================================
void DescribeAlgComponent( AAX_EStemFormat inStemFormat, AAX_IComponentDescriptor &	outDesc)
{
	int					err;
	AAX_IPropertyMap *	properties = outDesc.NewPropertyMap ();

	// Subscribe context fields to host-provided services or information
	//
	err = outDesc.AddAudioIn (eAlgFieldIndex_AudioIn);							AAX_ASSERT (err == 0);
	err = outDesc.AddAudioOut (eAlgFieldIndex_AudioOut);						AAX_ASSERT (err == 0);
	err = outDesc.AddAudioBufferLength(eAlgFieldIndex_BufferSize);																		AAX_ASSERT (err == 0);
	err = outDesc.AddDataInPort (eAlgPortID_MasterBypassControlIn, sizeof(int32_t));													AAX_ASSERT (err == 0);

	// Register context fields as communications destinations
	//
	err = outDesc.AddDataInPort (eAlgPortID_CoefsPreFiltIn, sizeof (SRectiFiCoefs_PreFilter));																AAX_ASSERT (err == 0);
	err = outDesc.AddDataInPort (eAlgPortID_RectiTypeIn, sizeof (int32_t));							AAX_ASSERT (err == 0);
	err = outDesc.AddDataInPort (eAlgPortID_GainIn, sizeof (float));								AAX_ASSERT (err == 0);
	err = outDesc.AddDataInPort (eAlgPortID_CoefsPostFiltIn, sizeof (SRectiFiCoefs_PostFilter));															AAX_ASSERT (err == 0);
	err = outDesc.AddDataInPort (eAlgPortID_CoefsEffectMixIn, sizeof (SRectiFiCoefs_EffectMix));	AAX_ASSERT (err == 0);

	// Register private data fields
	//
	err = outDesc.AddPrivateData (
		eAlgFieldIndex_State,
		sizeof (SRectiFiStateBlock),
		AAX_ePrivateDataOptions_DefaultOptions);																			AAX_ASSERT (err == 0);
	

	switch( inStemFormat )
	{
		case AAX_eStemFormat_Mono:
		{
			// Register Meters
			AAX_ASSERT( SMeterTapsM::eMeterTap_Count == sizeof(cMeterIDs_Mono)/sizeof(AAX_CTypeID) );
			err = outDesc.AddMeters( eAlgFieldID_Meters, cMeterIDs_Mono, SMeterTapsM::eMeterTap_Count );

			properties->AddProperty (AAX_eProperty_InputStemFormat, AAX_eStemFormat_Mono );
			properties->AddProperty (AAX_eProperty_OutputStemFormat, AAX_eStemFormat_Mono );
			properties->AddProperty ( AAX_eProperty_CanBypass, true );
			properties->AddProperty ( AAX_eProperty_PlugInID_Native, kRTASMToMProcessTypeID );
			properties->AddProperty ( AAX_eProperty_PlugInID_AudioSuite, kASMToMProcessTypeID );

			err = outDesc.AddProcessProc_Native ( RectiFi_Algorithm_ProcessFunction <1, 1, 32>, properties);		AAX_ASSERT (err == 0);
			properties->AddProperty (AAX_eProperty_AudioBufferLength, AAX_eAudioBufferLengthDSP_Default);
			properties->AddProperty ( AAX_eProperty_PlugInID_TI, kMMProcessTypeID );

			properties->AddProperty ( AAX_eProperty_TI_InstanceCycleCount, 742 );
			properties->AddProperty ( AAX_eProperty_TI_SharedCycleCount, 276 );

			err = outDesc.AddProcessProc_TI ("RectiFiAlgMono_Example.dll", "AlgEntry", properties, "");																AAX_ASSERT (err == 0);
		} break;
		
		case AAX_eStemFormat_Stereo:
		{
			// Register Meters
			AAX_ASSERT( SMeterTapsS::eMeterTap_Count == sizeof(cMeterIDs_Stereo)/sizeof(AAX_CTypeID) );
			err = outDesc.AddMeters( eAlgFieldID_Meters, cMeterIDs_Stereo, SMeterTapsS::eMeterTap_Count );

			properties->AddProperty (AAX_eProperty_InputStemFormat, AAX_eStemFormat_Stereo );
			properties->AddProperty (AAX_eProperty_OutputStemFormat, AAX_eStemFormat_Stereo );
			properties->AddProperty ( AAX_eProperty_CanBypass, true );
			properties->AddProperty ( AAX_eProperty_PlugInID_Native, kRTASSToSProcessTypeID );
			err = outDesc.AddProcessProc_Native (RectiFi_Algorithm_ProcessFunction <2, 2, 32>, properties);		AAX_ASSERT (err == 0);

			properties->AddProperty (AAX_eProperty_AudioBufferLength, AAX_eAudioBufferLengthDSP_Default);
			properties->AddProperty ( AAX_eProperty_PlugInID_TI, kSSProcessTypeID );
			
			properties->AddProperty ( AAX_eProperty_TI_InstanceCycleCount, 1116 );
			properties->AddProperty ( AAX_eProperty_TI_SharedCycleCount, 332 );

			err = outDesc.AddProcessProc_TI ("RectiFiAlgStereo_Example.dll", "AlgEntry", properties);																AAX_ASSERT (err == 0);
		} break;
			
		default:
			AAX_ASSERT(false);
			break;
	};
}
