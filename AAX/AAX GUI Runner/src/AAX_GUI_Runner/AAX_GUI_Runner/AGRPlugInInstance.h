//
//
// Copyright 2013, 2014, 2017 by Avid Technology, Inc.
//
//

//
//  AGRPlugInInstance.h
//  AAX_GUI_Runner
//

#import <Cocoa/Cocoa.h>

// AGR Includes
#import "AGRPlugInWindowController.h"

// AAXHostShell Includes
#import <AAXHostShell/AAXHostShell.h>


@interface AGRPlugInInstance : NSObject <NSCopying>
{
}

- (id)initWithPath:(NSString*)path;

- (AAXHShell_TInstanceID)identifier;
- (NSNumber*)identifierNum;
- (NSDictionary*)data;

@end
