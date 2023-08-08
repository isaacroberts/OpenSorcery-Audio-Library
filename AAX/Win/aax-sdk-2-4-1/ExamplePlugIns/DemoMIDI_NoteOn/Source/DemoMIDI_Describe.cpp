/*================================================================================================*/
/*
 *	Copyright 2013-2017 by Avid Technology, Inc.
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

// DemoMIDI Includes
#include "DemoMIDI_Describe.h"
#include "DemoMIDI_Defs.h"
#include "DemoMIDI_Alg.h"
#include "DemoMIDI_AlgProc.cpp" //Need due to function template
#include "DemoMIDI_Parameters.h"

// AAX Includes
#include "AAX_ICollection.h"
#include "AAX_IComponentDescriptor.h"
#include "AAX_IEffectDescriptor.h"
#include "AAX_IPropertyMap.h"
#include "AAX_Exception.h"
#include "AAX_Errors.h"
#include "AAX_Assert.h"


static const AAX_CEffectID kEffectID_DemoMIDI = "com.avid.aax.sdk.demomidi";


// ***************************************************************************
// ROUTINE:	DescribeAlgorithmComponent
// Algorithm component description
// ***************************************************************************
static void DescribeAlgorithmComponent( AAX_IComponentDescriptor * outDesc )
{
	AAX_CheckedResult err;
	
	// Describe the fields in the algorithm's context structure
	//
	// Subscribe context fields to host-provided services or information
	err = outDesc->AddAudioIn(eAlgFieldID_AudioIn);
	err = outDesc->AddAudioOut(eAlgFieldID_AudioOut);
	err = outDesc->AddAudioBufferLength(eAlgFieldID_BufferSize);
    //
	// Register context fields as communications destinations (i.e. input)
	err = outDesc->AddDataInPort ( eAlgPortID_BypassIn,		sizeof (int32_t) );
	err = outDesc->AddDataInPort ( eAlgPortID_CoefsGainIn,	sizeof (float) );
	err = outDesc->AddDataInPort ( eAlgPortID_HoldIn,	sizeof (int32_t) );
	//
	// Register MIDI nodes
	err = outDesc->AddMIDINode(eAlgPortID_MIDINodeIn, AAX_eMIDINodeType_LocalInput, "DemoMIDI", 0x0001);
	err = outDesc->AddMIDINode(eAlgPortID_MIDINodeOutPT, AAX_eMIDINodeType_LocalOutput, "DemoMIDI Out", 0x0001);
	//
	// Register private data
	err = outDesc->AddPrivateData(eAlgPortID_IsOn, sizeof(SDemoMIDI_NoteBits));
	//
	// Register meters
	static_assert( eMeterTap_Count == sizeof(cDemoMIDI_MeterID)/sizeof(AAX_CTypeID), "unexpected meter tap array size" );
	err = outDesc->AddMeters ( eAlgFieldID_Meters, cDemoMIDI_MeterID, eMeterTap_Count );
	
	// Register processing callbacks
	//
	// Create a property map 
	AAX_IPropertyMap* const properties = outDesc->NewPropertyMap();
	if ( !properties )
		err = AAX_ERROR_NULL_OBJECT;
	//
	// Generic properties
	err = properties->AddProperty ( AAX_eProperty_ManufacturerID, cDemoMIDI_ManufactureID );
	err = properties->AddProperty ( AAX_eProperty_ProductID, cDemoMIDI_ProductID );
	err = properties->AddProperty ( AAX_eProperty_CanBypass, true );
	err = properties->AddProperty ( AAX_eProperty_UsesClientGUI, true ); // Uses auto-GUI
	//
	// Stem format -specific properties
	err = properties->AddProperty ( AAX_eProperty_InputStemFormat, AAX_eStemFormat_Mono );
	err = properties->AddProperty ( AAX_eProperty_OutputStemFormat, AAX_eStemFormat_Mono );
	//
	// ID properties
	err = properties->AddProperty ( AAX_eProperty_PlugInID_Native, cDemoMIDI_PlugInID_Native );
	err = properties->AddProperty ( AAX_eProperty_PlugInID_AudioSuite, cDemoMIDI_PlugInID_AudioSuite );
	//
    // Register callbacks
    //
    // AAX Native
	err = outDesc->AddProcessProc_Native ( DemoMIDI_AlgorithmProcessFunction <1, 1>, properties );
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
	err = outDescriptor->AddName ( "Demo MIDI NoteOn" );
	err = outDescriptor->AddName ( "DemoMIDI NoteOn" );
	err = outDescriptor->AddName ( "Demo MIDI" );
	err = outDescriptor->AddName ( "DmMIDI" );
	err = outDescriptor->AddName ( "DmMD" );
	err = outDescriptor->AddName ( "DM" );
	err = outDescriptor->AddCategory ( AAX_ePlugInCategory_Example );
	err = outDescriptor->AddCategory ( AAX_ePlugInCategory_SWGenerators );
	
	// Effect components
	//
	// Algorithm component
	DescribeAlgorithmComponent( compDesc );
	err = outDescriptor->AddComponent( compDesc );
	//
	// Data model
	err = outDescriptor->AddProcPtr( (void *) DemoMIDI_Parameters::Create, kAAX_ProcPtrID_Create_EffectParameters );
	err = outDescriptor->AddResourceInfo(AAX_eResourceType_PageTable, "DemoMIDIPages.xml");
	
	// Effect's meter display properties
	//
	// Input
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor->NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
		
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Input );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor->AddMeterDescription( cDemoMIDI_MeterID[eMeterTap_PreGain], "Input", meterProperties );
	}
    // Output
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor->NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
		
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Output );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor->AddMeterDescription( cDemoMIDI_MeterID[eMeterTap_PostGain], "Output", meterProperties );
	}
	
	return err;
}

// ***************************************************************************
// ROUTINE:	GetEffectDescriptions
// ***************************************************************************
AAX_Result GetEffectDescriptions( AAX_ICollection * outCollection )
{
	AAX_CheckedResult err;
	AAX_IEffectDescriptor* const plugInDescriptor = outCollection->NewDescriptor();
	if ( plugInDescriptor )
	{
		err = DescribeEffect( plugInDescriptor );
		err = outCollection->AddEffect( kEffectID_DemoMIDI, plugInDescriptor );
	}
	else
		err = AAX_ERROR_NULL_OBJECT;
	
	err = outCollection->SetManufacturerName( "Avid" );
	err = outCollection->AddPackageName( "DemoMIDI AAX SDK Example Plug-In" );
	err = outCollection->AddPackageName( "DemoMIDI" );
	err = outCollection->AddPackageName( "DmMD" );
	err = outCollection->SetPackageVersion( 1 );
	
	return err;
}

