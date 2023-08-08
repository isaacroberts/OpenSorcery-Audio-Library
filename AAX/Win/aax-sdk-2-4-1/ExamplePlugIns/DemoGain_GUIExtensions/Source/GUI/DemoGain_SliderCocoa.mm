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
/*================================================================================================*/

#import "DemoGain_SliderCocoa.h"

#include "DemoGain_SliderCellCocoa.h"
#include "DemoGain_Defs.h"

#include "AAX_IViewContainer.h"
#include "AAX_IEffectParameters.h"
#include "AAX.h"

@implementation DemoGain_SliderCocoa

// *******************************************************
// METHOD:	awakeFromNib
// *******************************************************
- (void)awakeFromNib
{
//	[DemoGain_SliderCocoa setCellClass:[DemoGain_SliderCellCocoa class]];
//	[self setCell:[[[DemoGain_SliderCellCocoa alloc] init] autorelease]];
	
	// General initialization
	// TODO: Is this the right place?
	[self SetDefaultCommandState:false];
}

// *******************************************************
// METHOD:	SetViewContainer
// *******************************************************
- (void) SetViewContainer:(AAX_IViewContainer *) iViewContainer
{
  	mViewContainer = iViewContainer;
}

// *******************************************************
// METHOD:	SetEffectParameters
// *******************************************************
- (void) SetEffectParameters:(AAX_IEffectParameters *) iEffectParameters
{
  	mEffectParameters = iEffectParameters;
}

// *******************************************************
// METHOD:	mouseDown
// *******************************************************
- (void)mouseDown:(NSEvent *)theEvent
{
	AAX_Result result = AAX_ERROR_UNIMPLEMENTED;
	
	uint32_t modifiers = 0;
	if (mViewContainer)
	{
		modifiers = [self GetAAXModifiersFromNSEvent:theEvent];
		result = mViewContainer->HandleParameterMouseDown (DemoGain_GainID, modifiers);
	}
	
	if (result != AAX_SUCCESS)
	{
		// Check for if we need to set the control to its default value.
		boolean_t settingToDefault = (modifiers == AAX_eModifiers_Option);
		[self BeginControlEditEvent:settingToDefault];
		
		[super mouseDown:theEvent];
	}
}

// *******************************************************
// METHOD:	rightMouseDown
// *******************************************************
- (void)rightMouseDown:(NSEvent *)theEvent
{
	AAX_Result result = AAX_ERROR_UNIMPLEMENTED;
	
	uint32_t modifiers = 0;
	if (mViewContainer)
	{
		modifiers = [self GetAAXModifiersFromNSEvent:theEvent];
		result = mViewContainer->HandleParameterMouseDown (DemoGain_GainID, modifiers);
	}
	
	if (result != AAX_SUCCESS)
	{	
		// Check for if we need to set the control to its default value.
		boolean_t settingToDefault = (modifiers == AAX_eModifiers_Option);
		[self BeginControlEditEvent:settingToDefault];
		
		[super mouseDown:theEvent];
	}
}

// *******************************************************
// METHOD:	BeginControlEditEvent
// *******************************************************
- (void)BeginControlEditEvent:(boolean_t)iDefaultCommandState
{
	[self SetDefaultCommandState:iDefaultCommandState];
	
	if (mViewContainer && mEffectParameters)
	{
		mEffectParameters->TouchParameter(DemoGain_GainID);
	}
}

// *******************************************************
// METHOD:	EndControlEditEvent
// *******************************************************
- (void)EndControlEditEvent
{
	mSetToDefault = false;
	
	if (mViewContainer && mEffectParameters)
	{
		mEffectParameters->ReleaseParameter(DemoGain_GainID);
	}
}

// *******************************************************
// METHOD:	GetDefaultCommandState
// *******************************************************
- (boolean_t)GetDefaultCommandState
{
	return mSetToDefault;
}

// *******************************************************
// METHOD:	SetDefaultCommandState
// *******************************************************
- (void)SetDefaultCommandState:(boolean_t)iDefaultCommandState
{
	mSetToDefault = iDefaultCommandState;
}

// *******************************************************
// METHOD:	GetAAXModifiersFromNSEvent
// *******************************************************
- (uint32_t)GetAAXModifiersFromNSEvent:(NSEvent *)theEvent
{
	uint32_t modifiers = 0;

	if (theEvent)
	{
		if ([theEvent modifierFlags] & NSControlKeyMask)
		{
			modifiers |= AAX_eModifiers_Control;
		}
		
		if ([theEvent modifierFlags] & NSAlternateKeyMask)
		{
			modifiers |= AAX_eModifiers_Option;
		}
		
		if ([theEvent modifierFlags] & NSCommandKeyMask)
		{
			modifiers |= AAX_eModifiers_Command;
		}
		
		if ([theEvent type] == NSRightMouseUp ||
			[theEvent type] == NSRightMouseDragged ||
			[theEvent type] == NSRightMouseDown)
		{
			modifiers |= AAX_eModifiers_SecondaryButton;
		}
		
		// It is best practice to always query the host as
		// well, since the host's key handler may have
		// prevented some modifier key states from reaching
		// the plug-in.
		if (mViewContainer)
		{
			uint32_t aaxViewMods = 0;
			mViewContainer->GetModifiers (&aaxViewMods);
			modifiers |= aaxViewMods;
		}
	}
			
	return modifiers;
}

@end
