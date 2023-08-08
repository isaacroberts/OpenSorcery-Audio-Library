//
//
// Copyright 2015, 2017 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_Commands.h
//  AAXHostShell client utilities
//

#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>

// AAXHostShell Includes
#import <AAXHostShell/AAXHostShell_Types.h>

// OS X Framework Includes
#import <Cocoa/Cocoa.h>


@protocol AAXHostShell_CommandProtocol <NSObject, NSCopying>
- (BOOL)execute;
- (BOOL)undo;
- (AAXHShell_TCommandListID)identifier;
@end



@interface AAXHostShell_CommandCreateGUI : NSObject <AAXHostShell_CommandProtocol>
{
}

- (id)initForWindow:(NSWindow*)window forInstanceNum:(NSNumber*) instanceIDNum ofPlugInNum:(NSNumber*)plugInIDNum;
- (id)initForWindow:(NSWindow*)window forInstance:(AAXHShell_TInstanceID)instanceID ofPlugIn:(AAXHShell_TPlugInID)plugInID;

@end // @interface AAXHostShell_CommandCreateGUI


@interface AAXHostShell_CommandInstantiatePlugIn : NSObject <AAXHostShell_CommandProtocol>
{
}

- (id)initForURL:(NSURL*)path;
- (id)initForPath:(NSString*)path;

- (NSDictionary*)dataIDs;

@end // @interface AAXHostShell_CommandInstantiatePlugIn



