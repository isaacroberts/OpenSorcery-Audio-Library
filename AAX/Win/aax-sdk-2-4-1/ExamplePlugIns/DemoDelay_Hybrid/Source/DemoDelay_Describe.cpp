/*================================================================================================*/
/*
 *	Copyright 2013-2015, 2017 by Avid Technology, Inc.
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

// DemoDelay Includes
#include "DemoDelay_Describe.h"
#include "DemoDelay_Defs.h"
#include "DemoDelay_Alg.h"
#include "DemoDelay_Parameters.h"

// AAX Includes
#include "AAX_ICollection.h"
#include "AAX_IComponentDescriptor.h"
#include "AAX_IEffectDescriptor.h"
#include "AAX_IPropertyMap.h"
#include "AAX_Exception.h"
#include "AAX_Errors.h"
#include "AAX_Assert.h"

static const AAX_CEffectID kEffectID_DemoDelay = "com.avid.aax.sdk.demodelay.hybrid";


// ***************************************************************************
// ROUTINE:	DescribeAlgorithmComponent
// Algorithm component description
// ***************************************************************************
static void DescribeAlgorithmComponent( AAX_IComponentDescriptor * outDesc )
{
	AAX_CheckedResult err;
		
	// Subscribe context fields to host-provided services or information
	err = outDesc->AddAudioIn (AAX_FIELD_INDEX (SDemoDelayHybrid_LowLatency_Context, mInputPP));
	err = outDesc->AddAudioOut (AAX_FIELD_INDEX (SDemoDelayHybrid_LowLatency_Context, mOutputPP));
	err = outDesc->AddAudioBufferLength (AAX_FIELD_INDEX (SDemoDelayHybrid_LowLatency_Context, mBufferLengthP));
	static_assert( eMeterTap_Count == sizeof(cDemoDelay_MeterID)/sizeof(AAX_CTypeID), "unexpected meter tap array size" );
	err = outDesc->AddMeters ( AAX_FIELD_INDEX (SDemoDelayHybrid_LowLatency_Context, mMeterOutputPP), cDemoDelay_MeterID, eMeterTap_Count );
    
	// Register context fields as communications destinations (i.e. input)
	err = outDesc->AddDataInPort ( AAX_FIELD_INDEX (SDemoDelayHybrid_LowLatency_Context, mCoefsDelayP), sizeof (SDemoDelay_Coefs) );
		
	// Create a property map 
	AAX_IPropertyMap* const properties = outDesc->NewPropertyMap();
	if ( !properties )
		err = AAX_ERROR_NULL_OBJECT;
	
	// Generic properties
	err = properties->AddProperty ( AAX_eProperty_ManufacturerID, cDemoDelay_ManufactureID );
	err = properties->AddProperty ( AAX_eProperty_ProductID, cDemoDelay_ProductID );
	err = properties->AddProperty ( AAX_eProperty_CanBypass, true );
	err = properties->AddProperty ( AAX_eProperty_UsesClientGUI, true ); // Uses auto-GUI
	
	// Stem format -specific properties
	err = properties->AddProperty ( AAX_eProperty_InputStemFormat, AAX_eStemFormat_Mono );
	err = properties->AddProperty ( AAX_eProperty_OutputStemFormat, AAX_eStemFormat_Mono );
	err = properties->AddProperty ( AAX_eProperty_HybridInputStemFormat, AAX_eStemFormat_Mono );
	err = properties->AddProperty ( AAX_eProperty_HybridOutputStemFormat, AAX_eStemFormat_Mono );
	
	// ID properties
	err = properties->AddProperty ( AAX_eProperty_PlugInID_Native, cDemoDelay_PlugInID_Native );
	err = properties->AddProperty ( AAX_eProperty_PlugInID_TI, cDemoDelay_PlugInID_TI );
	
	// Resource requirements
#ifndef AAX_TI_BINARY_IN_DEVELOPMENT // Define this macro when using a debug TI DLL to allocate only 1 instance per chip
	err = properties->AddProperty ( AAX_eProperty_TI_InstanceCycleCount, 170 );
	err = properties->AddProperty ( AAX_eProperty_TI_SharedCycleCount, 90 );
#endif
    
    // Register Native Callback
	err = outDesc->AddProcessProc_Native ( DemoDelay_AlgorithmProcessFunction, properties );
	
	// Register TI Callback
	err = properties->AddProperty ( AAX_eProperty_DSP_AudioBufferLength, AAX_eAudioBufferLengthDSP_Default );
	err = outDesc->AddProcessProc_TI ("DemoDelayHybridAlgMono.dll", "AlgEntry", properties );
}

// ***************************************************************************
// ROUTINE:	DescribeEffect
// ***************************************************************************
static AAX_Result DescribeEffect( AAX_IEffectDescriptor * outDescriptor )
{
	AAX_CheckedResult err;
	AAX_IComponentDescriptor* const compDesc = outDescriptor->NewComponentDescriptor ();
	if ( !compDesc )
		err = AAX_ERROR_NULL_OBJECT;

	// Effect identifiers
	//
	err = outDescriptor->AddName ( "Demo Delay (Hybrid)" );
	err = outDescriptor->AddName ( "Demo Delay (Hyb)" );
	err = outDescriptor->AddName ( "Demo Dly Hyb" );
	err = outDescriptor->AddName ( "DmDly Hy" );
	
	err = outDescriptor->AddCategory ( AAX_ePlugInCategory_Example );
	
	// Effect components
	//
	// Algorithm component
	DescribeAlgorithmComponent( compDesc );
	err = outDescriptor->AddComponent( compDesc );
	//
	// Data model
	err = outDescriptor->AddProcPtr( (void *) DemoDelay_Parameters::Create, kAAX_ProcPtrID_Create_EffectParameters );
	err = outDescriptor->AddResourceInfo ( AAX_eResourceType_PageTable, "DemoDelayPages.xml" );

	// Effect's meter display properties
	//
	// Input
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor->NewPropertyMap();
		if ( !meterProperties ) return AAX_ERROR_NULL_OBJECT;
		
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Input );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor->AddMeterDescription( cDemoDelay_MeterID[eMeterTap_Input], "Input", meterProperties );
	}
    // Output
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor->NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
		
		err = err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Output );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor->AddMeterDescription( cDemoDelay_MeterID[eMeterTap_Output], "Output", meterProperties );
	}
	
	return err;
}

// ***************************************************************************
// ROUTINE:	GetEffectDescriptions
// ***************************************************************************
AAX_Result GetEffectDescriptions( AAX_ICollection * outCollection )
{
	AAX_CheckedResult err = AAX_SUCCESS;
	AAX_IEffectDescriptor* const plugInDescriptor = outCollection->NewDescriptor();
	if ( plugInDescriptor )
	{
		AAX_SWALLOW_MULT(
			err = DescribeEffect( plugInDescriptor );
			err = outCollection->AddEffect( kEffectID_DemoDelay, plugInDescriptor );
		);
	}
	else
		err = AAX_ERROR_NULL_OBJECT;
	
	err = outCollection->SetManufacturerName( "Avid" );
	err = outCollection->AddPackageName( "DemoDelay (Hybrid) AAX SDK Example Plug-In" );
	err = outCollection->AddPackageName( "DemoDelay (Hybrid)" );
	err = outCollection->AddPackageName( "DemoDelay Hybrid" );
	err = outCollection->AddPackageName( "DemoDelay" );
	err = outCollection->AddPackageName( "DmDly" );
	err = outCollection->SetPackageVersion( 1 );
	
	return err;
}
