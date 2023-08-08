/*================================================================================================*/
/*
 *	Copyright 2010-2015 by Avid Technology, Inc.
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
 *	\file   DemoGain_TextField.h
 *
 */ 
/*================================================================================================*/
#import <Cocoa/Cocoa.h>
#import <AppKit/NSTextField.h>

@interface DemoGain_TextField : NSTextField
{	
	NSColor	* mOrigTextColor;
	NSColor	* mOrigBackgroundColor;
}

@property (nonatomic, retain) NSColor *	mOrigTextColor;
@property (nonatomic, retain) NSColor *	mOrigBackgroundColor;

@end
