/*================================================================================================*/
/*
 *	Copyright 2013-2015, 2017, 2019 by Avid Technology, Inc.
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
#include "HostProcessor_Comp_Describe.h"
#include "DemoDelay_Describe.h"
#include "DemoDelay_HostProcessor_Comp.h"
#include "DemoDelay_Parameters.h"
#include "DemoDelay_Defs.h"

// AAX Includes
#include "AAX_ICollection.h"
#include "AAX_IEffectDescriptor.h"
#include "AAX_IPropertyMap.h"
#include "AAX_Exception.h"
#include "AAX_Assert.h"


// ***************************************************************************
// ROUTINE:	DelayComp_GetPlugInDescription
// ***************************************************************************
static AAX_Result DelayComp_GetPlugInDescription( AAX_IEffectDescriptor * outDescriptor )
{
	AAX_CheckedResult err;
	
    AAX_IPropertyMap* const properties = outDescriptor->NewPropertyMap();
    if (!properties)
    	err = AAX_ERROR_NULL_OBJECT;
    
	err = properties->AddProperty ( AAX_eProperty_ManufacturerID, cDemoDelay_ManufactureID );
	err = properties->AddProperty ( AAX_eProperty_ProductID, cDemoDelay_ProductID_Comp );
	err = properties->AddProperty ( AAX_eProperty_PlugInID_AudioSuite, cDemoDelay_PlugInID_Comp_AudioSuite );
	err = properties->AddProperty ( AAX_eProperty_NumberOfInputs, AAX_eMaxAudioSuiteTracks );
	err = properties->AddProperty ( AAX_eProperty_NumberOfOutputs, AAX_eMaxAudioSuiteTracks );
   	err = properties->AddProperty ( AAX_eProperty_UsesRandomAccess, true );
    err = properties->AddProperty ( AAX_eProperty_UsesClientGUI, true );
	
	err = outDescriptor->AddName ( "Demo Delay (HostProcessor Delay Comp)" );
	err = outDescriptor->AddName ( "Demo Delay (HP Delay Comp)" );
	err = outDescriptor->AddName ( "Demo Delay (HP Cmp)" );
	err = outDescriptor->AddName ( "DmDly HP Cmp" );
	err = outDescriptor->AddName ( "DmDly Cmp" );
	err = outDescriptor->AddName ( "DC" );
	
	err = outDescriptor->AddCategory ( AAX_ePlugInCategory_Example );
    
    err = outDescriptor->AddProcPtr(reinterpret_cast<void*>(DemoDelay_Parameters::Create), kAAX_ProcPtrID_Create_EffectParameters);
    err = outDescriptor->AddProcPtr(reinterpret_cast<void*>(DemoDelay_HostProcessor_Comp::Create), kAAX_ProcPtrID_Create_HostProcessor);
    
    err = outDescriptor->SetProperties(properties);
    
    return err;
    
}

// ***************************************************************************
// ROUTINE:	HostProcessor_Comp_GetEffectDescriptions
// ***************************************************************************
AAX_Result HostProcessor_Comp_GetEffectDescriptions( AAX_ICollection * outCollection )
{
	AAX_CheckedResult err;
	static const AAX_CEffectID effectID_DemoDelay_Comp = "com.avid.aax.sdk.demodelay.hostprocessor.comp";
	AAX_IEffectDescriptor* const plugInDescriptor = outCollection->NewDescriptor();
	if ( plugInDescriptor )
	{
		AAX_SWALLOW_MULT(
			err = DelayComp_GetPlugInDescription( plugInDescriptor );
			err = outCollection->AddEffect( effectID_DemoDelay_Comp, plugInDescriptor );
		);
	}
	else
		err = AAX_ERROR_NULL_OBJECT;
    
    return err;
}
