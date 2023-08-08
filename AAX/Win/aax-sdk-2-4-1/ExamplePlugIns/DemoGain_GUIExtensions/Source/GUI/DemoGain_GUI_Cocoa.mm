/*================================================================================================*/
/*
 *	Copyright 2013-2015, 2019 by Avid Technology, Inc.
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
#import <Cocoa/Cocoa.h>

#include "DemoGain_ViewController.h"
#include "DemoGain_ViewControllerFactory.h"

#include "DemoGain_GUI_Cocoa.h"
#include "DemoGain_Defs.h"

#include "AAX_UtilsNative.h"


// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_IEffectGUI * AAX_CALLBACK DemoGain_GUI::Create ()
{
	AAX_IEffectGUI * effectGUI = new DemoGain_GUI;
	return effectGUI;
}

// *******************************************************************************
// METHOD:	DemoGain_GUI
// *******************************************************************************
DemoGain_GUI::DemoGain_GUI ()
:   mDemoGainViewController(0)
{
}

// *******************************************************************************
// METHOD:	~DemoGain_GUI
// *******************************************************************************
DemoGain_GUI::~DemoGain_GUI ()
{
}

// *******************************************************************************
// METHOD:	CreateViewContents
// *******************************************************************************
void DemoGain_GUI::CreateViewContents ()
{
	NSBundle *			demoGainBundle = [NSBundle bundleWithIdentifier:@"com.avid.aax-sdk-example.DemoGain-Cocoa"];
	if ( demoGainBundle )
	{
		// The plug-in's Cocoa elements are all contained in a separate
		// bundle, DemoGain_CocoaGUI.  Once the DemoGain_CocoaGUI bundle
		// is loaded here it is never unloaded, even if the host unloads
		// the main DemoGain bundle
		
		NSString* cocoaUIPath = [[demoGainBundle resourcePath] stringByAppendingPathComponent:@"DemoGain_CocoaGUI.bundle"];		
		NSBundle* cocoaUIBundle = [NSBundle bundleWithPath:cocoaUIPath];
		
		NSError* error;
		BOOL result = [cocoaUIBundle preflightAndReturnError:&error];
		NSLog(@"%d", result);
		
		DemoGain_ViewControllerFactory* factory = [[[cocoaUIBundle principalClass] alloc] init];
		mDemoGainViewController = (DemoGain_ViewController*)[factory viewControllerFromBundle:cocoaUIBundle
		                                                     effectParameters:this->GetEffectParameters()
		                                                     controller:this->GetController()];
		this->SetViewController ( mDemoGainViewController );
	}
}

// *******************************************************************************
// METHOD:	AddViewContents
// *******************************************************************************
void DemoGain_GUI::CreateViewContainer ()
{	
	AAX_CEffectGUI_Cocoa::CreateViewContainer(); // Call inherited
	[mDemoGainViewController SetViewContainerOnViews:this->GetViewContainer()];
	[mDemoGainViewController SetEffectParametersOnViews:this->GetEffectParameters()];
}

// *******************************************************************************
// METHOD:	ParameterUpdated
// *******************************************************************************
AAX_Result DemoGain_GUI::ParameterUpdated ( AAX_CParamID iParameterID )
{
	if ( mDemoGainViewController && AAX::IsParameterIDEqual( iParameterID, DemoGain_GainID ) )
		[mDemoGainViewController UpdateGainValues];
	
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	TimerWakeup
// *******************************************************************************
AAX_Result DemoGain_GUI::TimerWakeup (void)
{
	if ( mDemoGainViewController )
	{
		[mDemoGainViewController UpdateMeters];
	}
	
	return AAX_SUCCESS;
}
