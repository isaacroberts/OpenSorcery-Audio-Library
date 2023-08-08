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
#include "DemoGain_AlgProc_ScatterGather.h"
#include "DemoGain_AlgProc_Burst.h"
#include "DemoGain_AlgCommon.h"
#include "DemoGain_Parameters.h"
#include "DemoGain_Defs.h"

// AAX Includes
#include "AAX_ICollection.h"
#include "AAX_IComponentDescriptor.h"
#include "AAX_IEffectDescriptor.h"
#include "AAX_IPropertyMap.h"
#include "AAX_Exception.h"
#include "AAX_Errors.h"
#include "AAX_Assert.h"


// ***************************************************************************
// ROUTINE:	AddCommonAlgProperties
// Common DemoGain_DMA algorithm component properties description
// ***************************************************************************
static void AddCommonAlgProperties (AAX_IPropertyMap* outProperties)
{
	AAX_CheckedResult err;
	
	// Generic properties
	//
	// IDs
	err = outProperties->AddProperty ( AAX_eProperty_ManufacturerID, cDemoGain_ManufactureID );
	err = outProperties->AddProperty ( AAX_eProperty_ProductID, cDemoGain_ProductID );
	//
	// Feature support
	err = outProperties->AddProperty ( AAX_eProperty_CanBypass, true );
	err = outProperties->AddProperty ( AAX_eProperty_UsesClientGUI, true );
	err = outProperties->AddProperty ( AAX_eProperty_LatencyContribution, cDemoGain_DMA_LatencyContribution );
	//
	// Stem format
	err = outProperties->AddProperty ( AAX_eProperty_InputStemFormat, AAX_eStemFormat_Mono );
	err = outProperties->AddProperty ( AAX_eProperty_OutputStemFormat, AAX_eStemFormat_Mono );
}

// ***************************************************************************
// ROUTINE:	DescribeAlgorithmComponent_Common
// Common DemoGain_DMA algorithm component description
// ***************************************************************************
static void DescribeAlgorithmComponent_Common ( AAX_IComponentDescriptor* outDesc )
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
	err = outDesc->AddDataInPort ( eAlgPortID_CoefsGainIn, sizeof (SDemoGain_DMA_CoefsGain) );
}

// ***************************************************************************
// ROUTINE:	DescribeAlgorithmComponent_ScatterGather
// DemoGain_DMA Scatter/Gather algorithm component description
// ***************************************************************************
static void DescribeAlgorithmComponent_ScatterGather( AAX_IComponentDescriptor* outDesc )
{
	AAX_CheckedResult err;
	
	DescribeAlgorithmComponent_Common (outDesc);
	
	// Describe algorithm's context structure
	//
	// Subscribe context fields to host-provided services or information
	err = outDesc->AddDmaInstance (eAlgFieldID_ScatterGather_DmaScatter, AAX_IDma::eMode_Scatter);
	err = outDesc->AddDmaInstance (eAlgFieldID_ScatterGather_DmaGather, AAX_IDma::eMode_Gather);
	//
	// Register private data fields
	// TODO: Create TI- and Native-specific versions of these fields to allocate
	// only the amount of memory required by the min/max buffer sizes of the 
	// platform.  Cannot currently do this because of PTSW-156195
	err = outDesc->AddPrivateData (eAlgFieldID_ScatterGather_DMAAudioBuffer,
								   sizeof(SDemoGain_DMASG_RWBuffer),
								   AAX_ePrivateDataOptions_External);
	err = outDesc->AddPrivateData (eAlgFieldID_ScatterGather_DMALocalData,
								   sizeof(SDemoGain_DMASG_DMALocalData));	
	err = outDesc->AddPrivateData (eAlgFieldID_ScatterGather_ScratchBuffer,
								   sizeof(SDemoGain_DMASG_ScratchBuffer));
	
	// Register processing callbacks
	//
	// Create a property map 
	AAX_IPropertyMap* const properties = outDesc->NewPropertyMap();
	if ( !properties )
		err = AAX_ERROR_NULL_OBJECT;
	//
	// Common properties
	AddCommonAlgProperties (properties);
	//
	// Scatter/Gather properties
	err = properties->AddProperty ( AAX_eProperty_PlugInID_Native, cDemoGain_DMASG_PlugInID_Native );
	err = properties->AddProperty ( AAX_eProperty_PlugInID_AudioSuite, cDemoGain_DMASG_PlugInID_AudioSuite ); // support offline processing
	err = properties->AddProperty ( AAX_eProperty_PlugInID_TI, cDemoGain_DMASG_PlugInID_TI );
	
    // Register callbacks
    //
    // Native
	err = outDesc->AddProcessProc_Native ( DemoGain_DMASG_AlgorithmProcessFunction, properties );
	//
	// TI
	err = properties->AddProperty ( AAX_eProperty_DSP_AudioBufferLength, cDemoGain_DMA_TIBufferSizeProperty );
	err = properties->AddProperty ( AAX_eProperty_TI_MaxInstancesPerChip, 1 ); // this example code uses one instance per chip for simplicity
	err = outDesc->AddProcessProc_TI ("DemoGain_MM_DMA_Example.dll", "AlgEntry_ScatterGather", properties );
}

// ***************************************************************************
// ROUTINE:	DescribeAlgorithmComponent_Burst
// DemoGain_DMA Burst algorithm component description
// ***************************************************************************
static void DescribeAlgorithmComponent_Burst( AAX_IComponentDescriptor * outDesc )
{
	AAX_CheckedResult err;
	
	DescribeAlgorithmComponent_Common (outDesc);
	
	// Describe algorithm's context structure
	//
	// Subscribe context fields to host-provided services or information
	err = outDesc->AddDmaInstance (eAlgFieldID_Burst_DmaBurst, AAX_IDma::eMode_Burst);
	//
	// Register private data fields
	// TODO: Create TI- and Native-specific versions of these fields to allocate
	// only the amount of memory required by the min/max buffer sizes of the 
	// platform.  Cannot currently do this because of PTSW-156195
	err = outDesc->AddPrivateData (eAlgFieldID_Burst_DMAAudioBuffer,
								   sizeof(SDemoGain_DMABurst_RWBuffer),
								   AAX_ePrivateDataOptions_External);
	err = outDesc->AddPrivateData (eAlgFieldID_Burst_DMALocalData,
								   sizeof(SDemoGain_Burst_DMALocalData));	
	
	// Register processing callbacks
	//
	// Create a property map 
	AAX_IPropertyMap* const properties = outDesc->NewPropertyMap();
	if ( !properties )
		err = AAX_ERROR_NULL_OBJECT;
	//
	// Common properties
	AddCommonAlgProperties (properties);
	//
	// Burst properties
	err = properties->AddProperty ( AAX_eProperty_PlugInID_Native, cDemoGain_DMABurst_PlugInID_Native );
	err = properties->AddProperty ( AAX_eProperty_PlugInID_AudioSuite, cDemoGain_DMABurst_PlugInID_AudioSuite ); // support offline processing
	err = properties->AddProperty ( AAX_eProperty_PlugInID_TI, cDemoGain_DMABurst_PlugInID_TI );
	
    // Register callbacks
    //
    // Native
	err = outDesc->AddProcessProc_Native ( DemoGain_DMABurst_AlgorithmProcessFunction, properties );
	//
	// TI
	err = properties->AddProperty ( AAX_eProperty_DSP_AudioBufferLength, cDemoGain_DMA_TIBufferSizeProperty );
	err = properties->AddProperty ( AAX_eProperty_TI_MaxInstancesPerChip, 1 ); // this example code uses one instance per chip for simplicity
	err = outDesc->AddProcessProc_TI ("DemoGain_MM_DMA_Example.dll", "AlgEntry_Burst", properties );
}

// ***************************************************************************
// ROUTINE:	DemoGain_DMA_GetEffectDescription
// Common DemoGain_DMA Effect description
// ***************************************************************************
static AAX_Result DemoGain_DMA_GetEffectDescription( AAX_IEffectDescriptor * outDescriptor )
{
	AAX_CheckedResult err;
	
	// Effect identifiers
	//
	err = outDescriptor->AddName ( "Demo Gain DMA" );
	err = outDescriptor->AddName ( "Demo Gain" );
	err = outDescriptor->AddName ( "DemoGain" );
	err = outDescriptor->AddName ( "DmGain" );
	err = outDescriptor->AddName ( "DGpr" );
	err = outDescriptor->AddName ( "Dn" );
	err = outDescriptor->AddCategory ( AAX_ePlugInCategory_Example );

	// Effect components
	//
	// Data model
	err = outDescriptor->AddProcPtr( (void *) DemoGain_Parameters::Create, kAAX_ProcPtrID_Create_EffectParameters );
	err = outDescriptor->AddResourceInfo ( AAX_eResourceType_PageTable, "DemoGainPages.xml" );
	
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
// ROUTINE:	DemoGain_DMASG_GetEffectDescription
// DemoGain_DMA Scatter/Gather Effect description
// ***************************************************************************
static AAX_Result DemoGain_DMASG_GetEffectDescription( AAX_IEffectDescriptor * outDescriptor )
{
	AAX_CheckedResult err;
	AAX_IComponentDescriptor* const compDesc = outDescriptor->NewComponentDescriptor ();
	if ( !compDesc )
		err = AAX_ERROR_NULL_OBJECT;
	
	// Common Effect description
	//
	err = DemoGain_DMA_GetEffectDescription (outDescriptor);
	
	// Effect identifiers
	//
	err = outDescriptor->AddName ( "Demo Gain (DMA S/G)" );
	
	// Effect components
	//
	// Algorithm component
	DescribeAlgorithmComponent_ScatterGather( compDesc );
	err = outDescriptor->AddComponent( compDesc );
		
	return err;
}

// ***************************************************************************
// ROUTINE:	DemoGain_DMABurst_GetEffectDescription
// DemoGain_DMA Burst Effect description
// ***************************************************************************
static AAX_Result DemoGain_DMABurst_GetEffectDescription( AAX_IEffectDescriptor * outDescriptor )
{
	AAX_CheckedResult err;
	AAX_IComponentDescriptor* const compDesc = outDescriptor->NewComponentDescriptor ();
	if ( !compDesc )
		err = AAX_ERROR_NULL_OBJECT;
	
	// Common Effect description
	//
	err = DemoGain_DMA_GetEffectDescription (outDescriptor);
	
	// Effect identifiers
	//
	err = outDescriptor->AddName ( "Demo Gain (DMA Burst)" );
	
	// Effect components
	//
	// Algorithm component
	DescribeAlgorithmComponent_Burst( compDesc );
	err = outDescriptor->AddComponent( compDesc );

	return err;
}

// ***************************************************************************
// ROUTINE:	GetEffectDescriptions
// ***************************************************************************
AAX_Result GetEffectDescriptions( AAX_ICollection * outCollection )
{
	AAX_CheckedResult err;
	
	AAX_IEffectDescriptor* const plugInDescriptor_DMASG = outCollection->NewDescriptor();
	if (plugInDescriptor_DMASG)
	{
		AAX_SWALLOW_MULT(
			err = DemoGain_DMASG_GetEffectDescription( plugInDescriptor_DMASG );
			err = outCollection->AddEffect( kEffectID_DemoGain_DMASG, plugInDescriptor_DMASG );
		);
	}
	else
		err = AAX_ERROR_NULL_OBJECT;
	
	AAX_IEffectDescriptor* plugInDescriptor_DMABurst = outCollection->NewDescriptor();
	if (plugInDescriptor_DMABurst)
	{
		AAX_SWALLOW_MULT(
			err = DemoGain_DMABurst_GetEffectDescription( plugInDescriptor_DMABurst );
			err = outCollection->AddEffect( kEffectID_DemoGain_DMABurst, plugInDescriptor_DMABurst );
		);
	}
	else
		err = AAX_ERROR_NULL_OBJECT;
	
	err = outCollection->SetManufacturerName( "Avid" );
	err = outCollection->AddPackageName( "DemoGain (DMA) AAX SDK Example Plug-In" );
	err = outCollection->AddPackageName( "DemoGain (DMA)" );
	err = outCollection->AddPackageName( "DemoGain DMA" );
	err = outCollection->AddPackageName( "DemoGain" );
	err = outCollection->AddPackageName( "DmGi" );
	err = outCollection->SetPackageVersion( 1 );
	
	return err;
}
