/*================================================================================================*/
/*
 *	Copyright 2016-2017, 2021 by Avid Technology, Inc.
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
#include "DemoGain_Parameters.h"

// AAX Components
#include "AAX_ICollection.h"
#include "AAX_IComponentDescriptor.h"
#include "AAX_IEffectDescriptor.h"
#include "AAX_IPropertyMap.h"
#include "AAX_Errors.h"
#include "AAX_Exception.h"
#include "AAX_Assert.h"

// Standard Includes
#include <vector>

namespace
{
	const AAX_CEffectID kEffectID_DemoGain	= "com.avid.aax.sdk.demogain.upmixer";

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

	// Structure representing a host context in which this
	// plug-in is supported. Note that this plug-in does
	// not register any offline (AudioSuite) support since
	// AAX does not allow width-changing AudioSuite plug-ins.
	struct SHostContext
	{
		AAX_EStemFormat mInputStemFormat;
		AAX_EStemFormat mOutputStemFormat;
		AAX_ETargetPlatform mTargetPlatform;
	};
	
	// ***************************************************************************
	// FUNCTION:	PlugInID
	// Generate a plug-in ID for a given configuration
	//
	// WARNING: Using algorithmic generation of type IDs.
	// The resulting IDs MUST NEVER CHANGE between versions
	// of this plug-in. Changes will result in compatibility
	// bugs with saved sessions that are very difficult to
	// resolve successfully.
	//
	// Whenever this approach is taken in production code
	// the resulting IDs should be tested at compile time
	// for consistency against known expected values.
	// ***************************************************************************
	AAX_Result PlugInID(const SHostContext& inHostContext, AAX_CTypeID& outTypeID, AAX_EProperty& outProperty)
	{
		AAX_Result result = AAX_SUCCESS;
		AAX_EProperty property = AAX_eProperty_NoID;
		
		uint8_t plat = 0;
		if (inHostContext.mTargetPlatform == kAAX_eTargetPlatform_Native) {
			plat = 'N';
			property = AAX_eProperty_PlugInID_Native;
		}
		else if (inHostContext.mTargetPlatform == kAAX_eTargetPlatform_TI) {
			plat = 'T';
			property = AAX_eProperty_PlugInID_TI;
		}
		else {
			result = AAX_ERROR_INVALID_ARGUMENT;
		}
		
		if (AAX_SUCCESS == result)
		{
			const uint8_t ifmt = 'A' + AAX_STEM_FORMAT_INDEX(inHostContext.mInputStemFormat);
			const uint8_t ofmt = 'A' + AAX_STEM_FORMAT_INDEX(inHostContext.mOutputStemFormat);
			
			outTypeID = 0x0;
			outTypeID |= (ofmt << 0);
			outTypeID |= (ifmt << 8);
			outTypeID |= ('_'  << 16);
			outTypeID |= (plat << 24);
			
			outProperty = property;
		}
		
		return result;
	}

	// ***************************************************************************
	// ROUTINE:	DescribeAlgorithmComponent
	// Algorithm component description
	// ***************************************************************************
	template<AAX_EStemFormat kInputStemFormat, AAX_EStemFormat kOutputStemFormat>
	AAX_Result DescribeAlgorithmComponent( AAX_IComponentDescriptor * outDesc )
	{
		AAX_CheckedResult err;
		const uint16_t cNumInChannels = AAX_STEM_FORMAT_CHANNEL_COUNT ( kInputStemFormat );
		const uint16_t cNumOutChannels = AAX_STEM_FORMAT_CHANNEL_COUNT ( kOutputStemFormat );
				
		// Describe algorithm's context structure
		//
		// Subscribe context fields to host-provided services or information
		err = outDesc->AddAudioIn (eAlgPortID_AudioIn);
		err = outDesc->AddAudioOut (eAlgPortID_AudioOut);
		err = outDesc->AddAudioBufferLength (eAlgPortID_BufferSize);

		// Register context fields as communications destinations (i.e. input)
		err = outDesc->AddDataInPort ( eAlgPortID_BypassIn, sizeof (int32_t) );
		err = outDesc->AddDataInPort ( eAlgPortID_CoefsGainIn, sizeof (SDemoGain_CoefsGain) );
		err = outDesc->AddDataInPort ( eAlgPortID_RoutingMapIn, sizeof (SDemoGain_RoutingMap) );
		
		// Meter section
		if (cMaxNumChannels >= cNumInChannels && cMaxNumChannels >= cNumOutChannels)
		{
			AAX_CTypeID	meterIDs[ cMaxNumMeters ];
			const uint16_t    cNumMeters = cNumInChannels + cNumOutChannels;
			uint16_t    index = 0;
			
			// Input meters
			uint16_t channelIndex = 0;
			while ( channelIndex < cNumInChannels )
				meterIDs[index++] = cDemoGain_InputMeterID[channelIndex++];
			
			// Output meters
			channelIndex = 0;
			while ( channelIndex < cNumOutChannels )
				meterIDs[index++] = cDemoGain_OutputMeterID[channelIndex++];

			err = outDesc->AddMeters ( eAlgPortID_Meters, meterIDs, cNumMeters );
		}
		else
		{
			err = outDesc->AddPrivateData ( eAlgPortID_Meters, 1 );
		}
		
		// Register processing callbacks
		//
		// Create a property map 
		AAX_IPropertyMap* properties = outDesc->NewPropertyMap();
		if ( !properties )
			err = AAX_ERROR_NULL_OBJECT;
		//
		// Required properties
		err = properties->AddProperty ( AAX_eProperty_ManufacturerID, cDemoGain_ManufactureID );
		err = properties->AddProperty ( AAX_eProperty_ProductID, cDemoGain_UpMixer_ProductID );
		err = properties->AddProperty ( AAX_eProperty_InputStemFormat, kInputStemFormat );
		err = properties->AddProperty ( AAX_eProperty_OutputStemFormat, kOutputStemFormat );
		
		// Optional properties
		err = properties->AddProperty ( AAX_eProperty_CanBypass, true );
		err = properties->AddProperty ( AAX_eProperty_UsesClientGUI, true );
		
		AAX_CTypeID plugInIDValue = AAX_eProperty_NoID;
		AAX_EProperty plugInIDProperty = AAX_eProperty_NoID;
		err = PlugInID( SHostContext({kInputStemFormat, kOutputStemFormat, kAAX_eTargetPlatform_Native}), plugInIDValue, plugInIDProperty );
		err = properties->AddProperty ( plugInIDProperty, plugInIDValue );
		
		// Register Native callback
		err = outDesc->AddProcessProc_Native(DemoGain_AlgorithmProcessFunction< cNumInChannels, cNumOutChannels >, properties);

		AAX_CTypeID plugInIDValue_TI = AAX_eProperty_NoID;
		AAX_EProperty plugInIDProperty_TI = AAX_eProperty_NoID;
		err = PlugInID( SHostContext({kInputStemFormat, kOutputStemFormat, kAAX_eTargetPlatform_TI}), plugInIDValue_TI, plugInIDProperty_TI );
		err = properties->AddProperty ( plugInIDProperty_TI, plugInIDValue_TI );

		// Register TI-specific properties
		properties->AddProperty(AAX_eProperty_DSP_AudioBufferLength, AAX_eAudioBufferLengthDSP_Default);
#ifndef AAX_TI_BINARY_IN_DEVELOPMENT // Define this macro when using a debug TI DLL to allocate only 1 instance per chip
		properties->AddProperty(AAX_eProperty_TI_InstanceCycleCount, cDemoGain_TI_Counts[cNumOutChannels - 1][0]);
		properties->AddProperty(AAX_eProperty_TI_SharedCycleCount, cDemoGain_TI_Counts[cNumOutChannels - 1][1]);
#endif
		// Register TI callback
		AAX_CString dllName("DemoGain_UpMixer_");
		dllName.AppendNumber(cNumInChannels);
		dllName.Append("ch_TI_Example.dll");
		AAX_CString algEntryName("AlgEntry_");
		algEntryName.AppendNumber(cNumInChannels);
		algEntryName.Append("_");
		algEntryName.AppendNumber(cNumOutChannels);
		err = outDesc->AddProcessProc_TI(dllName.CString(), algEntryName.CString(), properties);
		
		return err;
	}
	
	// ***************************************************************************
	// ROUTINE:	AddAlgorithmComponentDescription
	// Create algorithm component description and add to an effect description
	// ***************************************************************************
	template<AAX_EStemFormat kInputStemFormat, AAX_EStemFormat kOutputStemFormat>
	AAX_Result AddAlgorithmComponentDescription( AAX_IEffectDescriptor& outEffectDescriptor )
	{
		AAX_Result result = AAX_SUCCESS;
		AAX_CAPTURE_MULT(result,
			AAX_CheckedResult err;
			AAX_IComponentDescriptor* compDesc = outEffectDescriptor.NewComponentDescriptor ();
			if ( !compDesc )
				err = AAX_ERROR_NULL_OBJECT;
			
			err = compDesc->Clear();
			err = DescribeAlgorithmComponent<kInputStemFormat, kOutputStemFormat>(compDesc);
			err = outEffectDescriptor.AddComponent( compDesc );
		);
		return result;
	}
	
	// This example uses a bit of template metaprogramming in order to register the algorithm's
	// input and output stem formats using template parameters. This appraoch is used specifically
	// to ensure that a different entrypoint is used for each stem format in the TI DLL. Native
	// plug-ins can use run-time resolution.
	//
	// Usage example:
	//   AAX_IEffectDescriptor* desc = description_object;
	//   AAX_Result result;
	//   int numComponentsAdded = 0;
	//   numComponentsAdded += TMDoDescribeAlgorithmComponents<target_platform_1>(desc, result);
	//   numComponentsAdded += TMDoDescribeAlgorithmComponents<target_platform_2>(desc, result);
	//   if (0 == numComponentsAdded && AAX_SUCCESS != result)
	//     return result;
	//
	// TMStemFormat: Association of indices to stem formats
	//   Note: a better way to do this is with overloaded operator[] but Visual Studio 2013 does
	//   not support the constexpr keyword, which would be required for this use of operator[].
	template <int I> struct TMStemFormat {};
	template <> struct TMStemFormat<0>  { static const AAX_EStemFormat get = AAX_eStemFormat_Mono; };
	template <> struct TMStemFormat<1>  { static const AAX_EStemFormat get = AAX_eStemFormat_Stereo; };
	template <> struct TMStemFormat<2>  { static const AAX_EStemFormat get = AAX_eStemFormat_LCR; };
	template <> struct TMStemFormat<3>  { static const AAX_EStemFormat get = AAX_eStemFormat_LCRS; };
	template <> struct TMStemFormat<4>  { static const AAX_EStemFormat get = AAX_eStemFormat_Quad; };
	template <> struct TMStemFormat<5>  { static const AAX_EStemFormat get = AAX_eStemFormat_5_0; };
	template <> struct TMStemFormat<6>  { static const AAX_EStemFormat get = AAX_eStemFormat_5_1; };
	template <> struct TMStemFormat<7>  { static const AAX_EStemFormat get = AAX_eStemFormat_6_0; };
	template <> struct TMStemFormat<8>  { static const AAX_EStemFormat get = AAX_eStemFormat_6_1; };
	template <> struct TMStemFormat<9>  { static const AAX_EStemFormat get = AAX_eStemFormat_7_0_SDDS; };
	template <> struct TMStemFormat<10> { static const AAX_EStemFormat get = AAX_eStemFormat_7_1_SDDS; };
	template <> struct TMStemFormat<11> { static const AAX_EStemFormat get = AAX_eStemFormat_7_0_DTS; };
	template <> struct TMStemFormat<12> { static const AAX_EStemFormat get = AAX_eStemFormat_7_1_DTS; };
	template <> struct TMStemFormat<13> { static const AAX_EStemFormat get = AAX_eStemFormat_7_0_2; };
	template <> struct TMStemFormat<14> { static const AAX_EStemFormat get = AAX_eStemFormat_7_1_2; };
	template <> struct TMStemFormat<15> { static const AAX_EStemFormat get = AAX_eStemFormat_Ambi_1_ACN; };
	template <> struct TMStemFormat<16> { static const AAX_EStemFormat get = AAX_eStemFormat_Ambi_2_ACN; };
	template <> struct TMStemFormat<17> { static const AAX_EStemFormat get = AAX_eStemFormat_Ambi_3_ACN; };
	static const int TMStemFormatSize = 18; // simple approach
	//
	// TMP loop functions
	template <AAX_EStemFormat kInputStemFormat, int I=TMStemFormatSize>
	struct TMDoDescribeAlgorithmComponentsOfInputFormat {
		static void Run( AAX_IEffectDescriptor& outEffectDescriptor, AAX_AggregateResult& ioAggregateResult ) {
//            if (TMStemFormat<I-1>::get != kInputStemFormat) { // don't register non-converting combinations
                ioAggregateResult = AddAlgorithmComponentDescription<kInputStemFormat, TMStemFormat<I-1>::get>(outEffectDescriptor);
//            }
			
			// next
			TMDoDescribeAlgorithmComponentsOfInputFormat<kInputStemFormat, I-1>::Run(outEffectDescriptor, ioAggregateResult);
		}
	};
	template<AAX_EStemFormat kInputStemFormat>
	struct TMDoDescribeAlgorithmComponentsOfInputFormat<kInputStemFormat, 0> {
		static void Run( AAX_IEffectDescriptor&, AAX_AggregateResult& ) { /* terminal case: no action */ }
	};
	//
	template <int I=TMStemFormatSize>
	struct TMDoDescribeAlgorithmComponents {
		static void Run( AAX_IEffectDescriptor& outEffectDescriptor, AAX_AggregateResult& ioAggregateResult ) {
			TMDoDescribeAlgorithmComponentsOfInputFormat<TMStemFormat<I-1>::get>::Run(outEffectDescriptor, ioAggregateResult);
			TMDoDescribeAlgorithmComponents<I-1>::Run(outEffectDescriptor, ioAggregateResult);
		}
	};
	template<>
	struct TMDoDescribeAlgorithmComponents<0> {
		static void Run( AAX_IEffectDescriptor&, AAX_AggregateResult& ) { /* terminal case: no action */ }
	};
	
	// ***************************************************************************
	// ROUTINE:	DemoGain_GetPlugInDescription
	// ***************************************************************************
	AAX_Result DemoGain_GetPlugInDescription( AAX_IEffectDescriptor * outDescriptor )
	{
		AAX_CheckedResult err;
		
		// Effect identifiers
		//
		err = outDescriptor->AddName ( "Demo Gain (UpMixer)" );
		err = outDescriptor->AddName ( "DemoGain" );
		err = outDescriptor->AddName ( "DmGain" );
		err = outDescriptor->AddName ( "DGpr" );
		err = outDescriptor->AddName ( "Dn" );
		err = outDescriptor->AddCategory ( AAX_ePlugInCategory_Example );
		
		// Effect components
		//
		// Algorithm component
		//
		{
			// Register all input/output stem format combinations
			AAX_AggregateResult agg;
			TMDoDescribeAlgorithmComponents<>::Run(*outDescriptor, agg);
			
			// throw an exception using the last error if none
			// of the components were successfully registered
			if (0 == agg.NumSucceeded()) {
				err = agg.LastFailure();
			}
		}
		
		// Data model
		//
		err = outDescriptor->AddProcPtr( (void *) DemoGain_Parameters::Create, kAAX_ProcPtrID_Create_EffectParameters );
		outDescriptor->AddResourceInfo ( AAX_eResourceType_PageTable, "DemoGainPages.xml" );

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
		
			for (uint32_t channelIndex = 0 ; channelIndex < cMaxNumChannels; ++channelIndex)
			{
				err = outDescriptor->AddMeterDescription ( cDemoGain_OutputMeterID [ channelIndex ], cMeterNames_Outputs [ channelIndex ], outputMeterProperties );
			}
		}

		return err;
	}
} // end unnamed namespace

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
	err = outCollection->AddPackageName( "DemoGainUpMixer AAX SDK Example Plug-In" );
	err = outCollection->AddPackageName( "DemoGainUMixer" );
	err = outCollection->AddPackageName( "DemoGainUMix" );
	err = outCollection->AddPackageName( "DemoGainU" );
	err = outCollection->AddPackageName( "DmGU" );
	err = outCollection->SetPackageVersion( 1 );
	
	return err;
}
