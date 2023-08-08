/*================================================================================================*/
/*
 *	Copyright 2009-2015, 2017 by Avid Technology, Inc.
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

// DemoDist Includes
#include "DemoDist_Describe.h"
#include "DemoDistDefs.h"
#include "DemoDist_Alg.h"
#include "DemoDist_AlgProc.cpp" // Needed due to function template
#include "DemoDist_Parameters.h"

// AAX Includes
#include "AAX_ICollection.h"
#include "AAX_IComponentDescriptor.h"
#include "AAX_IEffectDescriptor.h"
#include "AAX_IPropertyMap.h"
#include "AAX_Exception.h"
#include "AAX_Errors.h"
#include "AAX_Assert.h"


static const AAX_CEffectID kEffectID_DemoDist = "com.avid.aax.sdk.demodist";


// ***************************************************************************
// ROUTINE:	DescribeAlgorithmComponent
// Algorithm component description
// ***************************************************************************
template <AAX_EStemFormat StemFormat>
static void DescribeAlgComponent (AAX_IComponentDescriptor& outDesc)
{
	static_assert (StemFormat == AAX_eStemFormat_Mono || StemFormat == AAX_eStemFormat_Stereo, "unsupported stem format");
	
	AAX_CheckedResult err;
	
	// Describe algorithm's context structure
	//
	// Subscribe context fields to host-provided services or information
	err = outDesc.AddAudioIn (eDemoDist_AlgFieldID_AudioIn);
	err = outDesc.AddAudioOut (eDemoDist_AlgFieldID_AudioOut);
	err = outDesc.AddAudioBufferLength (eDemoDist_AlgFieldID_BufferLength);
	//
	// Register meter fields
	switch( StemFormat )
	{
		case AAX_eStemFormat_Mono:
		{
			static_assert( SMeterTapsM::eMeterTap_Count == sizeof(cDemoDist_MeterID_Mono)/sizeof(AAX_CTypeID), "invalid number of meter taps" );
			err = outDesc.AddMeters ( eAlgFieldID_MeterTaps, cDemoDist_MeterID_Mono, SMeterTapsM::eMeterTap_Count );
		} break;
		case AAX_eStemFormat_Stereo:
		{
			static_assert( SMeterTapsS::eMeterTap_Count == sizeof(cDemoDist_MeterID_Stereo)/sizeof(AAX_CTypeID), "invalid number of meter taps" );
			err = outDesc.AddMeters ( eAlgFieldID_MeterTaps, cDemoDist_MeterID_Stereo, SMeterTapsS::eMeterTap_Count );			
		} break;
		default:
			break;
	}
	//
	// Register context fields as communications destinations (i.e. input)
	err = outDesc.AddDataInPort (eAlgPortID_BypassIn, sizeof (int32_t));
	err = outDesc.AddDataInPort (eAlgPortID_MixIn, sizeof (float));
	err = outDesc.AddDataInPort (eAlgPortID_InpGIn, sizeof (float));
	err = outDesc.AddDataInPort (eAlgPortID_OutGIn, sizeof (float));
	err = outDesc.AddDataInPort (eAlgPortID_CoefsDistIn, sizeof (SDemoDist_DistCoefs));
	err = outDesc.AddDataInPort (eAlgPortID_CoefsFilterIn, sizeof (SDemoDist_FiltCoefs));
	//
	// Register private data fields
	const int32_t dataBlockSize = sizeof (CSimpleBiquad) * AAX_STEM_FORMAT_CHANNEL_COUNT(StemFormat);
	err = outDesc.AddPrivateData (eAlgFieldID_Biquads, dataBlockSize,
                                  AAX_ePrivateDataOptions_DefaultOptions);
	
	
	// Register processing callbacks
	//
	// Create a property map
	AAX_IPropertyMap* const properties = outDesc.NewPropertyMap();
	if ( !properties )
		err = AAX_ERROR_NULL_OBJECT;
	//
	// Generic properties	
	err = properties->AddProperty ( AAX_eProperty_ManufacturerID, cDemoDist_ManufactureID );
	err = properties->AddProperty ( AAX_eProperty_ProductID, cDemoDist_ProductID );
	err = properties->AddProperty ( AAX_eProperty_InputStemFormat, StemFormat );
	err = properties->AddProperty ( AAX_eProperty_OutputStemFormat, StemFormat );
	err = properties->AddProperty ( AAX_eProperty_CanBypass, true );
	err = properties->AddProperty ( AAX_eProperty_UsesClientGUI, true ); 	// Register for auto-GUI
	//
	// Stem format -specific properties
	switch( StemFormat )
	{
		case AAX_eStemFormat_Mono:
		{
			// IDs
			err = properties->AddProperty ( AAX_eProperty_PlugInID_Native, cDemoDist_TypeID_MonoNative );
			err = properties->AddProperty ( AAX_eProperty_PlugInID_AudioSuite, cDemoDist_TypeID_AS );
			err = properties->AddProperty ( AAX_eProperty_PlugInID_TI, cDemoDist_TypeID_MonoTI );
				
			// Resource requirements
#ifndef AAX_TI_BINARY_IN_DEVELOPMENT // Define this macro when using a debug TI DLL to allocate only 1 instance per chip
			err = properties->AddProperty ( AAX_eProperty_TI_InstanceCycleCount, 692 );
			err = properties->AddProperty ( AAX_eProperty_TI_SharedCycleCount, 156 );
#endif
			
			// Register callbacks
			//
			// Native (Native and AudioSuite)
			err = outDesc.AddProcessProc_Native (DemoDist_AlgorithmProcessFunction <1, 1>, properties);
			//
			// TI (DSP)
			err = properties->AddProperty (AAX_eProperty_DSP_AudioBufferLength, AAX_eAudioBufferLengthDSP_Default);
			err = outDesc.AddProcessProc_TI ("DemoDist_MM_TI_Example.dll", "AlgEntry", properties);
		} break;
			
		case AAX_eStemFormat_Stereo:
		{
			// IDs
			err = properties->AddProperty ( AAX_eProperty_PlugInID_Native, cDemoDist_TypeID_StereoNative );
			err = properties->AddProperty ( AAX_eProperty_PlugInID_TI, cDemoDist_TypeID_StereoTI );
			
			// Resource requirements
#ifndef AAX_TI_BINARY_IN_DEVELOPMENT // Define this macro when using a debug TI DLL to allocate only 1 instance per chip
			err = properties->AddProperty ( AAX_eProperty_TI_InstanceCycleCount, 1121 );
			err = properties->AddProperty ( AAX_eProperty_TI_SharedCycleCount, 236 );
#endif
			
			// Register callbacks
			//
			// Native (Native and AudioSuite)
			err = outDesc.AddProcessProc_Native (DemoDist_AlgorithmProcessFunction<2, 2>, properties);
			//
			// TI (DSP)
			err = properties->AddProperty (AAX_eProperty_DSP_AudioBufferLength, AAX_eAudioBufferLengthDSP_Default);
			err = outDesc.AddProcessProc_TI ("DemoDist_SS_TI_Example.dll", "AlgEntry", properties);
		} break;
			
		default:
			break;
	}		
}

// ***************************************************************************
// ROUTINE:	DescribeEffectMeters
// ***************************************************************************
static void DescribeEffectMeters(AAX_IEffectDescriptor& outDescriptor)
{
	AAX_CheckedResult err;
	
	// Effect's meter display properties
	//
	// Input (mono)
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor.NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
		
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Input );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor.AddMeterDescription( cDemoDist_MeterID_In, "Input", meterProperties );
	}
	//
	// Input (left)
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor.NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
		
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Input );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor.AddMeterDescription( cDemoDist_MeterID_InL, "LeftIn", meterProperties );
	}
	//
	// Input (right)
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor.NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
		
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Input );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor.AddMeterDescription( cDemoDist_MeterID_InR, "RightIn", meterProperties );
	}
	//
	// Output (mono)
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor.NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
		
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Output );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor.AddMeterDescription( cDemoDist_MeterID_DistOut, "Output", meterProperties );
	}
	//
	// Output (left)
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor.NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
		
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Output );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor.AddMeterDescription( cDemoDist_MeterID_DistL, "LeftOut", meterProperties );
	}
	//
	// Output (right)
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor.NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
		
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Output );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor.AddMeterDescription( cDemoDist_MeterID_DistR, "RightOut", meterProperties );
	}
}

// ***************************************************************************
// ROUTINE:	DescribeEffect
// ***************************************************************************
static AAX_Result DescribeEffect(AAX_IEffectDescriptor& outDescriptor)
{
	AAX_CheckedResult err;
	AAX_IComponentDescriptor* const compDesc = outDescriptor.NewComponentDescriptor ();
	if ( !compDesc )
		err = AAX_ERROR_NULL_OBJECT;
	
	// Effect identifiers
	//
	err = outDescriptor.AddName ( "Demo Dist" );
	err = outDescriptor.AddName ( "DemoDist" );
	err = outDescriptor.AddName ( "DmD" );
	err = outDescriptor.AddCategory ( AAX_ePlugInCategory_Example );
	
	// Effect components
	//
	// Algorithm components
	{
		AAX_AggregateResult agg;
		
		// Mono
		AAX_CAPTURE_MULT( agg,
			err = compDesc->Clear();
			DescribeAlgComponent<AAX_eStemFormat_Mono> (*compDesc );
			err = outDescriptor.AddComponent ( compDesc );
		);
		
		// Stereo
		AAX_CAPTURE_MULT( agg,
			err = compDesc->Clear();
			DescribeAlgComponent<AAX_eStemFormat_Stereo> (*compDesc );
			err = outDescriptor.AddComponent ( compDesc );
		);
	}
	//
	// Data model
	err = outDescriptor.AddProcPtr( (void *) DemoDist_Parameters::Create, kAAX_ProcPtrID_Create_EffectParameters );
	err = outDescriptor.AddResourceInfo ( AAX_eResourceType_PageTable, "DemoDist.xml" );
	
	// Meters
	DescribeEffectMeters(outDescriptor);

	return err;
}


// ***************************************************************************
// ROUTINE:	GetEffectDescriptions
// ***************************************************************************
AAX_Result GetEffectDescriptions ( AAX_ICollection * outCollection )
{
	AAX_CheckedResult err;
	
	AAX_IEffectDescriptor* const plugInDescriptor = outCollection->NewDescriptor();
	if ( plugInDescriptor )
	{
		AAX_SWALLOW_MULT(
			err = DescribeEffect(*plugInDescriptor);
			err = outCollection->AddEffect ( kEffectID_DemoDist, plugInDescriptor );
		);
	}
	else
		err = AAX_ERROR_NULL_OBJECT;
	
	err = outCollection->SetManufacturerName( "Avid" );
	err = outCollection->AddPackageName( "DemoDist AAX SDK Example Plug-In" );
	err = outCollection->AddPackageName( "DemoDist Plug-In" );
	err = outCollection->AddPackageName( "DemoDist" );
	err = outCollection->AddPackageName( "DmDi" );
	err = outCollection->SetPackageVersion( 1 );
	
	return err;
}
