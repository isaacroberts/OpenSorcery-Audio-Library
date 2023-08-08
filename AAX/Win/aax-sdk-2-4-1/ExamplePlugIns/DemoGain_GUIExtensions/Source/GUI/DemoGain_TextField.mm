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
/*================================================================================================*/
#import  "DemoGain_TextField.h"


@implementation DemoGain_TextField

@synthesize	mOrigTextColor;
@synthesize	mOrigBackgroundColor;

- (void)awakeFromNib
{
	mOrigTextColor = [[self textColor] copy];
	mOrigBackgroundColor = [[self backgroundColor] copy];
}

- (BOOL) becomeFirstResponder
{
	BOOL result = [super becomeFirstResponder];
	if ( result )
	{
		[self setTextColor:[NSColor blackColor]];
		[self setBackgroundColor:[NSColor whiteColor]];
	}
		
	return result;
}

- (void)textDidEndEditing:(NSNotification *)notification
{
	[super textDidEndEditing:notification];

	[self setTextColor:mOrigTextColor];
	[self setBackgroundColor:mOrigBackgroundColor];

}

@end

