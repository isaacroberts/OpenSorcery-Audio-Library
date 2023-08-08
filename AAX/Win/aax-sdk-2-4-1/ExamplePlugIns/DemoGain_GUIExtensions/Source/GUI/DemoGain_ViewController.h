/*================================================================================================*/
/*
 *	Copyright 2009-2015 by Avid Technology, Inc.
 *	All rights reserved.
 *
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 */

/**  
 *	\file   DemoGain_ViewController.h
 *
 *	\brief  DemoGain_ViewController class definition.
 */ 
/*================================================================================================*/
#import <Cocoa/Cocoa.h>

#import "DemoGain_SliderCocoa.h"

#import <AppKit/NSTextField.h>
#import <AppKit/NSLevelIndicator.h>

class AAX_IEffectParameters;
class AAX_IController;
class AAX_IViewContainer;

// *******************************************************
// INTERFACE:	DemoGain_ViewController
// *******************************************************
@interface DemoGain_ViewController : NSViewController <NSTextFieldDelegate>
{	
	IBOutlet DemoGain_SliderCocoa *	mGainSlider;
	IBOutlet NSTextField *		mGainText;
	IBOutlet NSLevelIndicator * mInputLevelIndicator;
	IBOutlet NSLevelIndicator * mOutputLevelIndicator;
	
	AAX_IEffectParameters *		mEffectParameters;
	AAX_IController *			mController;
}

@property (nonatomic, retain) DemoGain_SliderCocoa *		mGainSlider;
@property (nonatomic, retain) NSTextField *			mGainText;
@property (nonatomic, retain) NSLevelIndicator *	mInputLevelIndicator;
@property (nonatomic, retain) NSLevelIndicator *	mOutputLevelIndicator;

- (void)		SetParameters:(AAX_IEffectParameters *) iEffectParameters;
- (void)		SetController:(AAX_IController *) iController;
- (void)		SetViewContainerOnViews:(AAX_IViewContainer *) iViewContainer; // Should be called after views have been created
- (void)		SetEffectParametersOnViews:(AAX_IEffectParameters *) iEffectParameters; // Should be called after views have been created
- (void)		UpdateGainValues;
- (void)		UpdateMeters;
- (void)		controlTextDidEndEditing:(NSNotification *)obj;
- (IBAction)	ChangeGainSlider:(id) iSender;
//- (IBAction)	ChangeGainText:(id) iSender;

@end
