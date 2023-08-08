/*================================================================================================*/
/*
 *	Copyright 2014-2015, 2017 by Avid Technology, Inc.
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

// DemoGain Includes
#include "DemoGain_Describe.h"
#include "DemoGain_Defs.h"
#include "DemoGain_Alg.h"
#include "DemoGain_Parameters.h"

#if PLUGGUI==1
#include "DemoGain_GUI.h" // Win32 or Cocoa GUI
#elif PLUGGUI==2
#include "DemoGain_GUI_Juce.h" // Juce
#elif PLUGGUI==3
#include "DemoGain_GUI_VST.h" // VSTGUI
#elif PLUGGUI==4
#include "DemoGain_GUI.h" // Cocoa GUI in a separate bundle
#endif // PLUGGUI

//AAX Includes
#include "AAX_ICollection.h"
#include "AAX_IComponentDescriptor.h"
#include "AAX_IEffectDescriptor.h"
#include "AAX_IPropertyMap.h"
#include "AAX_Exception.h"
#include "AAX_Errors.h"
#include "AAX_Assert.h"

#if PLUGGUI==1
static AAX_CEffectID kEffectID_DemoGain	= "com.avid.aax.sdk.demogain.guiextensions.native";
#elif PLUGGUI==2
static AAX_CEffectID kEffectID_DemoGain	= "com.avid.aax.sdk.demogain.guiextensions.juce";
#elif PLUGGUI==3
static AAX_CEffectID kEffectID_DemoGain	= "com.avid.aax.sdk.demogain.guiextensions.vst";
#elif PLUGGUI==4
static AAX_CEffectID kEffectID_DemoGain	= "com.avid.aax.sdk.demogain.guiextensions.cocoa.sepbun";
#endif // PLUGGUI

// ***************************************************************************
// ROUTINE:	DescribeAlgorithmComponent
// Algorithm component description
// ***************************************************************************
static void DescribeAlgorithmComponent( AAX_IComponentDescriptor* outDesc )
{
	AAX_CheckedResult err;
	
	// Describe algorithm's context structure
	//
	// Subscribe context fields to host-provided services or information
	err = outDesc->AddAudioIn (eAlgFieldID_AudioIn);
	err = outDesc->AddAudioOut (eAlgFieldID_AudioOut);
	err = outDesc->AddAudioBufferLength (eAlgFieldID_BufferSize);
	static_assert(eMeterTap_Count == sizeof(cDemoGain_MeterID)/sizeof(AAX_CTypeID), "unexpected meter tap array size");
	err = outDesc->AddMeters ( eAlgFieldID_Meters, cDemoGain_MeterID, eMeterTap_Count );
    //
	// Register context fields as communications destinations (i.e. input)
	err = outDesc->AddDataInPort ( eAlgPortID_BypassIn, sizeof (int32_t) );
	err = outDesc->AddDataInPort ( eAlgPortID_CoefsGainIn, sizeof (SDemoGain_CoefsGain) );
	
	// Register processing callbacks
	//
	// Create a property map 
	AAX_IPropertyMap* properties = outDesc->NewPropertyMap();
	if ( !properties )
		err = AAX_ERROR_NULL_OBJECT;
	//
	// Generic properties
	err = properties->AddProperty ( AAX_eProperty_ManufacturerID, cDemoGain_ManufactureID );
	err = properties->AddProperty ( AAX_eProperty_ProductID, cDemoGain_ProductID );
	err = properties->AddProperty ( AAX_eProperty_CanBypass, true );
	//
	// Stem format -specific properties
	err = properties->AddProperty ( AAX_eProperty_InputStemFormat, AAX_eStemFormat_Mono );
	err = properties->AddProperty ( AAX_eProperty_OutputStemFormat, AAX_eStemFormat_Mono );
	//
	// ID properties
	err = properties->AddProperty ( AAX_eProperty_PlugInID_Native, cDemoGain_PlugInID_Native );
	err = properties->AddProperty ( AAX_eProperty_PlugInID_AudioSuite, cDemoGain_PlugInID_AudioSuite );	// for offline processing
	err = properties->AddProperty ( AAX_eProperty_PlugInID_TI, cDemoGain_PlugInID_TI );

    // Register Native callback
	err = outDesc->AddProcessProc_Native ( DemoGain_AlgorithmProcessFunction, properties );
	
	// TI-specific properties
#ifndef AAX_TI_BINARY_IN_DEVELOPMENT // Define this macro when using a debug TI DLL to allocate only 1 instance per chip
	err = properties->AddProperty ( AAX_eProperty_TI_InstanceCycleCount, 102 );
	err = properties->AddProperty ( AAX_eProperty_TI_SharedCycleCount, 80 );
#endif
	err = properties->AddProperty ( AAX_eProperty_DSP_AudioBufferLength, AAX_eAudioBufferLengthDSP_Default );
	
	// Register TI callback
#if PLUGGUI==0
	// No plug-in
#elif PLUGGUI==1
#	if defined (WINDOWS_VERSION)
	err = outDesc->AddProcessProc_TI ("DemoGain_GUIExWin32_MM_TI_Example.dll", "AlgEntry", properties );
#	else
	err = outDesc->AddProcessProc_TI ("DemoGain_GUIExCocoa_MM_TI_Example.dll", "AlgEntry", properties );
#	endif // WINDOWS_VERSION
#elif PLUGGUI==2
	err = outDesc->AddProcessProc_TI ("DemoGain_GUIExJuce_MM_TI_Example.dll", "AlgEntry", properties );
#elif PLUGGUI==3
	err = outDesc->AddProcessProc_TI ("DemoGain_GUIExVST_MM_TI_Example.dll", "AlgEntry", properties );
#elif PLUGGUI==4
	err = outDesc->AddProcessProc_TI ("DemoGain_GUIExCocoa_sb_MM_TI_Example.dll", "AlgEntry", properties );
#endif // PLUGGUI
}

// ***************************************************************************
// ROUTINE:	DescribeEffect
// ***************************************************************************
static AAX_Result DescribeEffect( AAX_IEffectDescriptor* outDescriptor )
{
	AAX_CheckedResult err;
	AAX_IComponentDescriptor* const compDesc = outDescriptor->NewComponentDescriptor ();
	if ( !compDesc )
		err = AAX_ERROR_NULL_OBJECT;

	// Effect identifiers
	//
#if PLUGGUI==0
	// No plug-in
#elif PLUGGUI==1
#	if defined (WINDOWS_VERSION)
	err = outDescriptor->AddName ( "Demo Gain (Win32 GUI Example)" );
	err = outDescriptor->AddName ( "Demo Gain (Win32)" );
#	else
	err = outDescriptor->AddName ( "Demo Gain (Cocoa GUI Example)" );
	err = outDescriptor->AddName ( "Demo Gain (Cocoa)" );
#	endif // WINDOWS_VERSION
#elif PLUGGUI==2
	err = outDescriptor->AddName ( "Demo Gain (Juce GUI Example)" );
	err = outDescriptor->AddName ( "Demo Gain (Juce)" );
#elif PLUGGUI==3
	err = outDescriptor->AddName ( "Demo Gain (VSTGUI GUI Example)" );
	err = outDescriptor->AddName ( "Demo Gain (VSTGUI)" );
#elif PLUGGUI==4
	err = outDescriptor->AddName ( "Demo Gain (Separated Cocoa GUI Example)" );
	err = outDescriptor->AddName ( "Demo Gain (Sep Cocoa)" );
#endif // PLUGGUI
	err = outDescriptor->AddName ( "Demo Gain" );
	err = outDescriptor->AddName ( "DemoGain" );
	err = outDescriptor->AddName ( "DmGain" );
	err = outDescriptor->AddName ( "DGpr" );
	err = outDescriptor->AddName ( "Dn" );
	err = outDescriptor->AddCategory ( AAX_ePlugInCategory_Example );
	
	// Effect components
	//
	// Algorithm component
	DescribeAlgorithmComponent( compDesc );
	err = outDescriptor->AddComponent( compDesc );
	//
	// Data model
	err = outDescriptor->AddProcPtr( (void *) DemoGain_Parameters::Create, kAAX_ProcPtrID_Create_EffectParameters );
	err = outDescriptor->AddResourceInfo ( AAX_eResourceType_PageTable, "DemoGainPages.xml" );
	// GUI
	err = outDescriptor->AddProcPtr( (void *) DemoGain_GUI::Create, kAAX_ProcPtrID_Create_EffectGUI );
	
	// Effect's meter display properties
	//
	// Input meter
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor->NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
			
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Input );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor->AddMeterDescription( cDemoGain_MeterID[eMeterTap_PreGain], "Input", meterProperties );
	}
	// Output meter
	{
		AAX_IPropertyMap* const meterProperties = outDescriptor->NewPropertyMap();
		if ( !meterProperties )
			err = AAX_ERROR_NULL_OBJECT;
			
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Output );
		err = meterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		err = outDescriptor->AddMeterDescription( cDemoGain_MeterID[eMeterTap_PostGain], "Output", meterProperties );
	}
	
	return err;
}

// ***************************************************************************
// ROUTINE:	GetEffectDescriptions
// ***************************************************************************
AAX_Result GetEffectDescriptions( AAX_ICollection * outCollection )
{
	AAX_CheckedResult err;
	AAX_IEffectDescriptor *	plugInDescriptor = outCollection->NewDescriptor();
	if ( plugInDescriptor )
	{
		AAX_SWALLOW_MULT(
			err = DescribeEffect( plugInDescriptor );
			err = outCollection->AddEffect( kEffectID_DemoGain, plugInDescriptor );
		);
	}
	else
		err = AAX_ERROR_NULL_OBJECT;
	
	err = outCollection->SetManufacturerName( "Avid" );
	err = outCollection->AddPackageName( "DemoGain GUI AAX SDK Example Plug-In" );
	err = outCollection->AddPackageName( "DemoGain GUI" );
	err = outCollection->AddPackageName( "DemoGain" );
	err = outCollection->AddPackageName( "DmGi" );
	err = outCollection->SetPackageVersion( 1 );
	
	return err;
}
