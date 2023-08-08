/*================================================================================================*/
/*
 *	Copyright 2011-2015 by Avid Technology, Inc.
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
 *	\file   DemoGain_SliderCocoa.h
 *
 *	\brief	DemoGain_SliderCocoa class definition.
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/
#import <Cocoa/Cocoa.h>
#import <AppKit/NSSlider.h>

class AAX_IViewContainer;
class AAX_IEffectParameters;

@interface DemoGain_SliderCocoa : NSSlider
{
	AAX_IViewContainer *		mViewContainer;
	AAX_IEffectParameters *		mEffectParameters;
	
	boolean_t					mSetToDefault;
}

- (void)		SetViewContainer:(AAX_IViewContainer *) iViewContainer;
- (void)		SetEffectParameters:(AAX_IEffectParameters *) iEffectParameters;
- (void)		BeginControlEditEvent:(boolean_t) iDefaultCommandState;
- (void)		EndControlEditEvent;
- (boolean_t)	GetDefaultCommandState;
- (void)		SetDefaultCommandState:(boolean_t) iDefaultCommandState;
- (uint32_t)	GetAAXModifiersFromNSEvent:(NSEvent *) theEvent;

@end
