//
//
// Copyright 2013, 2014, 2017 by Avid Technology, Inc.
//
//

//
//  AGRAppDelegate.mm
//  AAX GUI Runner
//

// Self Include
#import "AGRAppDelegate.h"

// AGR Includes
#import "AGRPlugInInstance.h"

// AAXHostShell Includes
#import <AAXHostShell/AAXHostShell.h>




//-------------------------------------------------------------------------------------------
#pragma mark private interface

@interface AGRAppDelegate ()
@property (nonatomic) NSThread* privateApplicationIdleThread;
@property (nonatomic) NSMutableDictionary* privatePlugIns;
@property (nonatomic) NSMutableDictionary* privatePlugInWindows;
@end



//-------------------------------------------------------------------------------------------
@implementation AGRAppDelegate
@synthesize privateApplicationIdleThread = _applicationIdleThread;
@synthesize privatePlugIns = _plugIns;
@synthesize privatePlugInWindows = _plugInWindows;



//-------------------------------------------------------------------------------------------
#pragma mark private methods

- (void)openPlugInAtURL:(NSURL*)url
{
	NSNumber* instanceIDNum = [self createPlugInWithURL:url];
	if (nil != instanceIDNum)
	{
		AGRPlugInWindowController* wc = [self createPlugInWindow];
		assert(nil != wc);
		[self loadGUIForInstance:instanceIDNum intoWindow:wc];
		[wc showWindow:self];
		
		// Add this URL to the "Open Recent..." menu
		[[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:url];
	}
}

- (NSURL*)getPlugInURL
{
	NSURL* resultURL = nil;
	
	char buffer[512];
	if (true == AAXHShell_GetDefaultPlugInsDirectory(buffer, 512))
	{
		NSString* plugInFolderString = [[NSString alloc] initWithUTF8String:buffer];
		
		NSOpenPanel* openPanel = [NSOpenPanel openPanel];
		NSArray* fileTypes = [NSArray arrayWithObjects:@"aaxplugin", nil];
		[openPanel setAllowsMultipleSelection:NO];
		[openPanel setMessage:@"Choose an AAX plug-in to load:"];
		[openPanel setAllowedFileTypes:fileTypes];
		[openPanel setDirectoryURL:[NSURL fileURLWithPath:plugInFolderString]];
		NSInteger panelResult = [openPanel runModal];
		
		if (NSOKButton == panelResult && [[openPanel URL] isFileURL])
		{
			resultURL = [openPanel URL];
		}
	}

	return resultURL;
}

- (NSNumber*)createPlugInWithURL:(NSURL*)url
{
	NSNumber* instanceIDNum = nil;
	if ([url isFileURL])
	{
		// Create the plug-in instance
		AGRPlugInInstance* newInstance = [[AGRPlugInInstance alloc] initWithPath:[url path]];
		if (newInstance)
		{
			instanceIDNum = [newInstance identifierNum];
			if (kAAXHShell_IDUnset != [instanceIDNum unsignedLongValue])
			{
				// Add the instance
				[self.privatePlugIns setObject:newInstance forKey:instanceIDNum];
			}
			else
			{
				// Do nothing if the instance ID is unset; instance will be released on return
				instanceIDNum = nil;
			}
		}
	}
	return instanceIDNum;
}

- (AGRPlugInWindowController*)createPlugInWindow
{
	// Create the plug-in window
	return [[AGRPlugInWindowController alloc] init];
}

- (void)loadGUIForInstance:(NSNumber*)instanceIDNum intoWindow:(AGRPlugInWindowController*)windowController
{
	AGRPlugInInstance* instance = [self.privatePlugIns objectForKey:instanceIDNum];
	[windowController loadGUIForInstanceWithData:[instance data]];
	[self.privatePlugInWindows setObject:windowController forKey:instanceIDNum];
}

- (void)applicationMainThreadIdleEvent
{
	AAXHShell_DoMainThreadIdle();
}

- (void)applicationIdle
{
	while (NO == [[NSThread currentThread] isCancelled])
	{
		usleep(30*1000);
		[self performSelectorOnMainThread:@selector(applicationMainThreadIdleEvent) withObject:nil waitUntilDone:true];
	}
}


//-------------------------------------------------------------------------------------------
#pragma mark public methods

- (void)receivedNotification:(NSNotification *) notification
{
	NSString* notificationName = [notification name];
	if ([notificationName isEqualToString:@"AGRPlugInWindowDidAddGUI"])
	{
		// No action
	}
	else if ([notificationName isEqualToString:@"AGRPlugInWindowDidRemoveGUI"])
	{
		NSNumber* instanceID = notification.object;
		assert (instanceID != nil);
		assert (nil != [self.privatePlugIns objectForKey:instanceID]);
		assert (nil != [self.privatePlugInWindows objectForKey:instanceID]);
		
		// Remove the plug-in window, since we are now done with it
		// (TODO: Observe the actual "did close" notification and only remove the window at that point)
		[[self privatePlugInWindows] removeObjectForKey:instanceID];
		
//		//REM: Don't request that the instance be removed when its window is about to close. This leads to a double
//		// deletion: nobody is retaining a reference to the instance anymore, so it gets removed (good) but the
//		// instance object is what owns the window. Therefore nobody is retaining a reference to the window either
//		// the window is deleted by the instance dealloc. Then the window's in-flight close operation continues,
//		// although the window has already been deleted (bad).
//		//
//		// If we want to remove the plug-in instance whenever its window is closed then we need something else to
//		// own the window object.
//		
//		// Remove the plug-in instance, since we are now done with it
//		[[self privatePlugIns] removeObjectForKey:instanceID];
	}
}



//-------------------------------------------------------------------------------------------
#pragma mark inherited methods

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
	@autoreleasepool {
		// Close all open plug-in windows manually here to ensure that we don't
		// end up with in-flight messages during teardown
		[self.privatePlugInWindows enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
			AGRPlugInWindowController* wc = obj;
			[wc close];
		}];
		
		[self.privateApplicationIdleThread cancel];
		[self.privatePlugIns removeAllObjects];
		[self.privatePlugInWindows removeAllObjects];
	}
	
	AAXHShell_Teardown();
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// App delegate internal initialization
	[self setPrivatePlugIns: [[NSMutableDictionary alloc] init]];
	[self setPrivatePlugInWindows: [[NSMutableDictionary alloc] init]];
	
	// Application framework initialization
	AAXHShell_Initialize();
	
	// Application framework post-init setup
	AAXHShell_SHostContext hostContext = AAXHShell_kHostContext_Any;
	hostContext.mPlatform = kAAXHShell_Platform_Native;
	hostContext.mSupportsProcessing = true;
	AAXHShell_SetHostContext(&hostContext);
	
	// Begin listening to notifications
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(receivedNotification:)
												 name:@"AGRPlugInWindowDidAddGUI"
											   object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(receivedNotification:)
												 name:@"AGRPlugInWindowDidRemoveGUI"
											   object:nil];
	
	// Bring up the Open dialog
	// TODO: Move this to a notification handler thread?
	[self openDocument:nil];
	
	// Kick off the main application idle
	// TODO: Move this to the AAXHostShell framework
	[self setPrivateApplicationIdleThread:[[NSThread alloc] initWithTarget:self selector:@selector(applicationIdle) object:nil]];
	[[self privateApplicationIdleThread] start];
}

// This method is invoked when "Open..." is used
- (IBAction)openDocument:(id)sender
{
	NSURL* plugInURL = [self getPlugInURL];
	[self openPlugInAtURL:plugInURL];
}

// This method is invoked when "Open Recent..." is used
- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	NSURL* plugInURL = [NSURL fileURLWithPath:filename];
	[self openPlugInAtURL:plugInURL];
	return YES;
}

@end
