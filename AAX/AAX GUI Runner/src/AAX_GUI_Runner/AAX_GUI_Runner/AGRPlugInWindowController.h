//
//
// Copyright 2013, 2014, 2017 by Avid Technology, Inc.
//
//

//
//  AGRPlugInWindowController.h
//  AAX_GUI_Runner
//

#import <Cocoa/Cocoa.h>

// AAXHostShell Includes
#import <AAXHostShell/AAXHostShell.h>

@interface AGRPlugInWindowController : NSWindowController

- (id)init;
- (void)loadGUIForInstanceWithData:(NSDictionary*)data;

@end
