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
 *	\file   DemoGain_ViewControllerFactory.h
 *
 *	\brief  DemoGain_ViewControllerFactory class definition
 *
 *  \author David Hearst
 */ 
/*================================================================================================*/
class AAX_IEffectParameters;
class AAX_IController;
class AAX_IViewContainer;

@interface DemoGain_ViewControllerFactory : NSObject
{
}

- (NSViewController*)viewControllerFromBundle:(NSBundle*)bundle effectParameters:(AAX_IEffectParameters*)effectParameters
																controller:(AAX_IController*)controller;

@end
