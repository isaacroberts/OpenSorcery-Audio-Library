/*================================================================================================*/
/*
 *	Copyright 2016-2017 by Avid Technology, Inc.
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

#include "DemoGain_Describe.h"
#include "DemoGain_Defs.h"
#include "DemoGain_Alg.h"

//AAX Components
#include "DemoGain_Alg.h"
#include "DemoGain_Parameters.h"

#include "AAX_ICollection.h"
#include "AAX_IComponentDescriptor.h"
#include "AAX_IEffectDescriptor.h"
#include "AAX_IPropertyMap.h"
#include "AAX_Exception.h"
#include "AAX_Errors.h"
#include "AAX_Assert.h"


namespace
{
	AAX_CEffectID kEffectID_DemoGain	= "com.avid.aax.sdk.demogain.multichannel";

	const char * cMeterNames_Inputs[cMaxNumChannels]  = 
	{
		"Input Meter 01",
		"Input Meter 02",
		"Input Meter 03",
		"Input Meter 04",
		"Input Meter 05",
		"Input Meter 06",
		"Input Meter 07",
		"Input Meter 08",
		"Input Meter 09",
		"Input Meter 10",
		"Input Meter 11",
		"Input Meter 12",
		"Input Meter 13",
		"Input Meter 14",
		"Input Meter 15",
		"Input Meter 16"
	};

	const char * cMeterNames_Outputs[cMaxNumChannels] =
	{
		"Output Meter 01",
		"Output Meter 02",
		"Output Meter 03",
		"Output Meter 04",
		"Output Meter 05",
		"Output Meter 06",
		"Output Meter 07",
		"Output Meter 08",
		"Output Meter 09",
		"Output Meter 10",
		"Output Meter 11",
		"Output Meter 12",
		"Output Meter 13",
		"Output Meter 14",
		"Output Meter 15",
		"Output Meter 16"
	};

	const char * cMeterName_GR_CmpLim = "Cmp/Lim Gain Reduction Meter";
	const char * cMeterName_GR_ExpGate = "Exp/Gate Gain Reduction Meter";
}

// ***************************************************************************
// ROUTINE:	DescribeAlgorithmComponent
// Algorithm component description
// ***************************************************************************
template <uint32_t kStemFormat>
static void DescribeAlgorithmComponent( AAX_IComponentDescriptor* outDesc )
{
	AAX_CheckedResult err;
	const uint16_t cNumChannels = AAX_STEM_FORMAT_CHANNEL_COUNT ( kStemFormat );
	
	// Describe algorithm's context structure
	//
	// Subscribe context fields to host-provided services or information
	err = outDesc->AddAudioIn (eAlgPortID_AudioIn);
	err = outDesc->AddAudioOut (eAlgPortID_AudioOut);
	err = outDesc->AddAudioBufferLength (eAlgPortID_BufferSize);

	// Register context fields as communications destinations (i.e. input)
	err = outDesc->AddDataInPort ( eAlgPortID_BypassIn, sizeof (int32_t) );
	err = outDesc->AddDataInPort ( eAlgPortID_CoefsGainIn, sizeof (SDemoGain_CoefsGain) );
	
    // Meter section
	if (cMaxNumChannels >= cNumChannels)
	{
		AAX_CTypeID	meterIDs[ cMaxNumMeters ];
		const uint16_t    cNumMeters = cNumChannels * 2 + 2;
		uint16_t    index = 0;
		
		// Input meters
		uint16_t channelIndex = 0;
		while ( channelIndex < cNumChannels )
			meterIDs[index++] = cDemoGain_InputMeterID[channelIndex++];
		
		// Output meters
		channelIndex = 0;
		while ( channelIndex < cNumChannels )
			meterIDs[index++] = cDemoGain_OutputMeterID[channelIndex++];

		// Cmp/Lim Gain reduction meter
		meterIDs[index++] = cDemoGain_CmpLim_GRMeterID;

		// Exp/Gate Gain reduction meter
		meterIDs[index++] = cDemoGain_ExpGate_GRMeterID;

		err = outDesc->AddMeters ( eAlgPortID_Meters, meterIDs, cNumMeters );
	}
	else
	{
		err = outDesc->AddPrivateData ( eAlgPortID_Meters, 1 );
	}
    
	// Register processing callbacks
	//
	// Create a property map 
	AAX_IPropertyMap* const properties = outDesc->NewPropertyMap();
	if ( !properties )
		err = AAX_ERROR_NULL_OBJECT;
	//
	// Required properties
	err = properties->AddProperty ( AAX_eProperty_ManufacturerID, cDemoGain_ManufactureID );
	err = properties->AddProperty ( AAX_eProperty_ProductID, cDemoGain_ProductID );
	err = properties->AddProperty ( AAX_eProperty_InputStemFormat, kStemFormat );
	err = properties->AddProperty ( AAX_eProperty_OutputStemFormat, kStemFormat );
	
	// Optional properties
	err = properties->AddProperty ( AAX_eProperty_CanBypass, true );
	err = properties->AddProperty ( AAX_eProperty_UsesClientGUI, true );
	
	AAX_CTypeID nativeType = AAX_eProperty_NoID;
	AAX_CTypeID audioSuiteType = AAX_eProperty_NoID;
	AAX_CTypeID dspType = AAX_eProperty_NoID;
    switch ( kStemFormat )
    {
        case AAX_eStemFormat_Mono:
            nativeType = cDemoGain_PlugInID_Native;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite;
			dspType = cDemoGain_PlugInID_TI;
            break;
            
        case AAX_eStemFormat_Stereo:
            nativeType = cDemoGain_PlugInID_Native_Stereo;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_Stereo;
			dspType = cDemoGain_PlugInID_TI_Stereo;
            break;
            
        case AAX_eStemFormat_LCR:
            nativeType = cDemoGain_PlugInID_Native_LCR;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_LCR;
			dspType = cDemoGain_PlugInID_TI_LCR;
            break;
            
        case AAX_eStemFormat_Quad:
            nativeType = cDemoGain_PlugInID_Native_QUAD;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_QUAD;
			dspType = cDemoGain_PlugInID_TI_QUAD;
            break;
            
        case AAX_eStemFormat_LCRS:
            nativeType = cDemoGain_PlugInID_Native_LCRS;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_LCRS;
			dspType = cDemoGain_PlugInID_TI_LCRS;
            break;
            
        case AAX_eStemFormat_5_0:
            nativeType = cDemoGain_PlugInID_Native_5_0;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_5_0;
			dspType = cDemoGain_PlugInID_TI_5_0;
            break;
            
        case AAX_eStemFormat_5_1:
            nativeType = cDemoGain_PlugInID_Native_5_1;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_5_1;
			dspType = cDemoGain_PlugInID_TI_5_1;
            break;
            
        case AAX_eStemFormat_6_0:
            nativeType = cDemoGain_PlugInID_Native_6_0;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_6_0;
			dspType = cDemoGain_PlugInID_TI_6_0;
            break;
            
        case AAX_eStemFormat_6_1:
            nativeType = cDemoGain_PlugInID_Native_6_1;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_6_1;
			dspType = cDemoGain_PlugInID_TI_6_1;
            break;
            
        case AAX_eStemFormat_7_0_SDDS:
            nativeType = cDemoGain_PlugInID_Native_7_0_SDDS;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_7_0_SDDS;
			dspType = cDemoGain_PlugInID_TI_7_0_SDDS;
            break;
            
        case AAX_eStemFormat_7_1_SDDS:
            nativeType = cDemoGain_PlugInID_Native_7_1_SDDS;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_7_1_SDDS;
			dspType = cDemoGain_PlugInID_TI_7_1_SDDS;
            break;
            
       	case AAX_eStemFormat_7_0_DTS:
            nativeType = cDemoGain_PlugInID_Native_7_0_DTS;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_7_0_DTS;
			dspType = cDemoGain_PlugInID_TI_7_0_DTS;
            break;
            
       	case AAX_eStemFormat_7_1_DTS:
            nativeType = cDemoGain_PlugInID_Native_7_1_DTS;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_7_1_DTS;
			dspType = cDemoGain_PlugInID_TI_7_1_DTS;
            break;
           
        case AAX_eStemFormat_7_0_2:
            nativeType = cDemoGain_PlugInID_Native_7_0_2;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_7_0_2;
            dspType = cDemoGain_PlugInID_TI_7_0_2;
            break;
            
        case AAX_eStemFormat_7_1_2:
            nativeType = cDemoGain_PlugInID_Native_7_1_2;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_7_1_2;
			dspType = cDemoGain_PlugInID_TI_7_1_2;
            break;
			
		case AAX_eStemFormat_Ambi_1_ACN:
            nativeType = cDemoGain_PlugInID_Native_1_ACN;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_1_ACN;
			dspType = cDemoGain_PlugInID_TI_1_ACN;
			break;
			
		case AAX_eStemFormat_Ambi_2_ACN:
            nativeType = cDemoGain_PlugInID_Native_2_ACN;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_2_ACN;
			dspType = cDemoGain_PlugInID_TI_2_ACN;
			break;
			
		case AAX_eStemFormat_Ambi_3_ACN:
            nativeType = cDemoGain_PlugInID_Native_3_ACN;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_3_ACN;
			dspType = cDemoGain_PlugInID_TI_3_ACN;
			break;
			
		// TEST CASES
		case AAX_STEM_FORMAT ( (AAX_eStemFormatNum+0), 1 ):
            nativeType = cDemoGain_PlugInID_Native_3_ACN+1;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_3_ACN+1;
			dspType = cDemoGain_PlugInID_TI_3_ACN+1;
			break;
			
		case AAX_STEM_FORMAT ( (AAX_eStemFormatNum+1), 20 ):
            nativeType = cDemoGain_PlugInID_Native_3_ACN+2;
            audioSuiteType = cDemoGain_PlugInID_AudioSuite_3_ACN+2;
			dspType = cDemoGain_PlugInID_TI_3_ACN+2;
			break;
		// ~TEST CASES
			
        default:
			AAX_ASSERT(false);
            break;
    }
	
	if (AAX_eProperty_NoID != nativeType) {
		err = properties->AddProperty ( AAX_eProperty_PlugInID_Native, nativeType ); }
	if (AAX_eProperty_NoID != audioSuiteType) {
		err = properties->AddProperty ( AAX_eProperty_PlugInID_AudioSuite, audioSuiteType ); }
	if (AAX_eProperty_NoID != dspType) {
		err = properties->AddProperty ( AAX_eProperty_PlugInID_TI, dspType ); }
	
	// Register Native/AudioSuite callback
	err = outDesc->AddProcessProc_Native(DemoGain_AlgorithmProcessFunction< cNumChannels >, properties);

	// TI-specific properties
	if (AAX_eProperty_NoID != dspType)
	{
#ifndef AAX_TI_BINARY_IN_DEVELOPMENT // Define this macro when using a debug TI DLL to allocate only 1 instance per chip
		properties->AddProperty(AAX_eProperty_TI_InstanceCycleCount, cDemoGain_TI_Counts[cNumChannels - 1][0]);
		properties->AddProperty(AAX_eProperty_TI_SharedCycleCount, cDemoGain_TI_Counts[cNumChannels - 1][1]);
#endif
		properties->AddProperty(AAX_eProperty_DSP_AudioBufferLength, AAX_eAudioBufferLengthDSP_Default);

		// Register TI callback
		AAX_CString algEntryName("AlgEntry_");
		algEntryName.AppendNumber(cNumChannels);
		err = outDesc->AddProcessProc_TI("DemoGain_Multichannel_TI_Example.dll", algEntryName.CString(), properties);
	}
}

// ***************************************************************************
// ROUTINE:	DemoGain_GetPlugInDescription
// ***************************************************************************
static AAX_Result DemoGain_GetPlugInDescription( AAX_IEffectDescriptor * outDescriptor )
{
	AAX_CheckedResult err;
	AAX_IComponentDescriptor *	compDesc = outDescriptor->NewComponentDescriptor ();
	if ( !compDesc )
		err = AAX_ERROR_NULL_OBJECT;

	// Effect identifiers
	//
	err = outDescriptor->AddName ( "Demo Gain (Multichannel)" );
	err = outDescriptor->AddName ( "DemoGain" );
	err = outDescriptor->AddName ( "DmGain" );
	err = outDescriptor->AddName ( "DGpr" );
	err = outDescriptor->AddName ( "Dn" );
	err = outDescriptor->AddCategory ( AAX_ePlugInCategory_Example );
	
	// Effect components
	//
	// Algorithm component
	{
		AAX_AggregateResult agg;
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_Mono >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_Stereo >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_LCR >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_LCRS >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_Quad >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_5_0 >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_5_1 >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_6_0 >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_6_1 >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_7_0_SDDS >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_7_1_SDDS >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_7_0_DTS >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_7_1_DTS >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_7_0_2 >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
						 );
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_7_1_2 >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_Ambi_1_ACN >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_Ambi_2_ACN >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_eStemFormat_Ambi_3_ACN >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
	
		// TEST CASES
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_STEM_FORMAT ( AAX_eStemFormatNum+0, 1 ) >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		AAX_CAPTURE_MULT(agg,
			compDesc->Clear();
			DescribeAlgorithmComponent< AAX_STEM_FORMAT ( AAX_eStemFormatNum+1, 20 ) >(compDesc);
			err = outDescriptor->AddComponent( compDesc );
		);
		// ~TEST CASES
	}

	//
	// Data model
	err = outDescriptor->AddProcPtr( (void *) DemoGain_Parameters::Create, kAAX_ProcPtrID_Create_EffectParameters );
	err = outDescriptor->AddResourceInfo ( AAX_eResourceType_PageTable, "DemoGainPages.xml" );

	// Effect's meter display properties
	//
    // Input meters
    {
		AAX_IPropertyMap* const inputMeterProperties = outDescriptor->NewPropertyMap();
		if (!inputMeterProperties)
			err = AAX_ERROR_NULL_OBJECT;
		
		err = inputMeterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Input );
		err = inputMeterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		for (uint32_t channelIndex = 0; channelIndex < cMaxNumChannels; ++channelIndex)
		{
			err = outDescriptor->AddMeterDescription ( cDemoGain_InputMeterID [ channelIndex ], cMeterNames_Inputs [ channelIndex ], inputMeterProperties );
		}
	}
    // Output meters
    {
		AAX_IPropertyMap* const outputMeterProperties = outDescriptor->NewPropertyMap();
		if (!outputMeterProperties)
			err = AAX_ERROR_NULL_OBJECT;
		
		err = outputMeterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_Output );
		err = outputMeterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_Default );
		for (uint32_t channelIndex = 0; channelIndex < cMaxNumChannels; ++channelIndex)
		{
			err = outDescriptor->AddMeterDescription ( cDemoGain_OutputMeterID [ channelIndex ], cMeterNames_Outputs [ channelIndex ], outputMeterProperties );
		}
	}
    // Cmp/Lim Gain reduction meter
    {
		AAX_IPropertyMap* const cmpLimGRMeterProperties = outDescriptor->NewPropertyMap();
		if (!cmpLimGRMeterProperties)
			err = AAX_ERROR_NULL_OBJECT;
		
		err = cmpLimGRMeterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_CLGain );
		err = cmpLimGRMeterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_TopRight );
		err = outDescriptor->AddMeterDescription ( cDemoGain_CmpLim_GRMeterID, cMeterName_GR_CmpLim, cmpLimGRMeterProperties );
	}
    // Exp/Gate Gain reduction meter
    {
		AAX_IPropertyMap* const expGateGRMeterProperties = outDescriptor->NewPropertyMap();
		if (!expGateGRMeterProperties)
			err = AAX_ERROR_NULL_OBJECT;
		
		err = expGateGRMeterProperties->AddProperty ( AAX_eProperty_Meter_Type, AAX_eMeterType_EGGain );
		err = expGateGRMeterProperties->AddProperty ( AAX_eProperty_Meter_Orientation, AAX_eMeterOrientation_TopRight );
		err = outDescriptor->AddMeterDescription ( cDemoGain_ExpGate_GRMeterID, cMeterName_GR_ExpGate, expGateGRMeterProperties );
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
		AAX_SWALLOW_MULT(
			err = DemoGain_GetPlugInDescription( plugInDescriptor );
			err = outCollection->AddEffect( kEffectID_DemoGain, plugInDescriptor );
		);
	}
	else
		err = AAX_ERROR_NULL_OBJECT;
	
	err = outCollection->SetManufacturerName( "Avid" );
	err = outCollection->AddPackageName( "DemoGainMultichannel AAX SDK Example Plug-In" );
	err = outCollection->AddPackageName( "DemoGainMultichannel Plug-In" );
	err = outCollection->AddPackageName( "DemoGainMultich Plug-In" );
	err = outCollection->AddPackageName( "DemoGainM" );
	err = outCollection->AddPackageName( "DmGM" );
	err = outCollection->SetPackageVersion( 1 );
	
	return err;
}
