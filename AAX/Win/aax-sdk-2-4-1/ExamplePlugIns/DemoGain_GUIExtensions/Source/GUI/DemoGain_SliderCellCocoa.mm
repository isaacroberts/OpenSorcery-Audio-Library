/*================================================================================================*/
/*
 *	Copyright 2012-2015 by Avid Technology, Inc.
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

#import "DemoGain_SliderCellCocoa.h"

#include "DemoGain_SliderCocoa.h"

@implementation DemoGain_SliderCellCocoa

- (BOOL)continueTracking:(NSPoint)lastPoint at:(NSPoint)currentPoint inView:(NSView *)controlView
{
	// HACK? We all know that this is the right class for
	// this object, and other solutions seem too obtuse.
	DemoGain_SliderCocoa* sliderView;
	sliderView = (DemoGain_SliderCocoa* )[self controlView];
	if ([sliderView GetDefaultCommandState])
	{
		return YES;
	}
	else
	{
		return [super continueTracking:lastPoint at:currentPoint inView:controlView];
	}
}

- (void)stopTracking:(NSPoint)lastPoint at:(NSPoint)stopPoint inView:(NSView *)controlView mouseIsUp:(BOOL)flag
{
	// Call inherited first in order to release the control after the final update has been submitted
	[super stopTracking:lastPoint at:stopPoint inView:controlView mouseIsUp:flag];
	
	// HACK? We all know that this is the right class for
	// this object, and other solutions seem too obtuse.
	DemoGain_SliderCocoa* sliderView;
	sliderView = (DemoGain_SliderCocoa* )[self controlView];
	[sliderView EndControlEditEvent];
}

@end
