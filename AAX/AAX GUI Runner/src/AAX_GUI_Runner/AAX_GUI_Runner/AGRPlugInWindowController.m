//
//
// Copyright 2013, 2014, 2017 by Avid Technology, Inc.
//
//

//
//  AGRPlugInWindowController.m
//  AAX_GUI_Runner
//

// Self Include
#import "AGRPlugInWindowController.h"

// AGR Includes
#import "AGRPlugInInstance.h"

// AAXHostShell Includes
#import "AAXHostShell_ObjC_Commands.h"
#import <AAXHostShell/AAXHostShell.h>



//-------------------------------------------------------------------------------------------
#pragma mark private interface

@interface AGRPlugInWindowController ()
@property (nonatomic) AAXHostShell_CommandCreateGUI* privateCommandCreateGUI;
@property (nonatomic) NSNumber* privateInstanceIDNum;
@end


//-------------------------------------------------------------------------------------------
@implementation AGRPlugInWindowController
@synthesize privateCommandCreateGUI = _commandCreateGUI;
@synthesize privateInstanceIDNum = _instanceIDNum;


//-------------------------------------------------------------------------------------------
#pragma mark private methods


//-------------------------------------------------------------------------------------------
#pragma mark public methods

- (id)init
{
    self = [super initWithWindowNibName:@"AGRPlugInWindowController"];
	if (self)
	{
		[self setPrivateCommandCreateGUI:nil];
	}
	return self;
}

- (void)loadGUIForInstanceWithData:(NSDictionary*)data
{
	NSNumber* plugInID = [data valueForKey:@"PlugInID"];
	NSNumber* instanceID = [data valueForKey:@"InstanceID"];
	[self setPrivateCommandCreateGUI:[[AAXHostShell_CommandCreateGUI alloc] initForWindow:self.window forInstanceNum:instanceID ofPlugInNum:plugInID]];
	
	if (self.privateCommandCreateGUI && true == [self.privateCommandCreateGUI execute])
	{
		// Notify observers that the plug-in GUI has been loaded
		[self setPrivateInstanceIDNum:instanceID];
		[[NSNotificationCenter defaultCenter] postNotificationName:@"AGRPlugInWindowDidAddGUI" object:self.privateInstanceIDNum];
	}
	else
	{
		//TODO: What to do if executing the load command fails?
		assert(false);
	}
}


//-------------------------------------------------------------------------------------------
#pragma mark NSWindowDelegate methods

- (void)windowWillClose:(NSNotification *)notification
{	
	if ([notification object] == [self window])
	{
		// Close out the plug-in GUI
		[self.privateCommandCreateGUI undo];
		
		// Notify observers that the plug-in GUI has been removed
		[[NSNotificationCenter defaultCenter] postNotificationName:@"AGRPlugInWindowDidRemoveGUI" object:self.privateInstanceIDNum];
	}
}

@end
