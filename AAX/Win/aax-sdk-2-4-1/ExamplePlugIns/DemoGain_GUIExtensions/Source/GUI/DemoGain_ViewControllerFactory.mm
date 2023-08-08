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

#import "DemoGain_ViewControllerFactory.h"

#import "DemoGain_ViewController.h"

#import "AAX_IEffectParameters.h"
#import "AAX_IController.h"
#import "AAX_IViewContainer.h"

@implementation DemoGain_ViewControllerFactory

// will need PT controller and PT parameters as opaque arguments
- (NSViewController*)viewControllerFromBundle:(NSBundle*)bundle
							effectParameters:(AAX_IEffectParameters*)effectParameters
							controller:(AAX_IController*)controller
{
    DemoGain_ViewController* viewController = [[DemoGain_ViewController alloc] initWithNibName:@"DemoGain_View" bundle:bundle];
    assert(viewController != nil);
        
	[viewController SetParameters:effectParameters ];
	[viewController SetController:controller ];

    return viewController;
}

@end
