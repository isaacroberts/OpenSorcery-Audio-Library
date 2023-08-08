//
//
// Copyright 2013, 2014, 2017 by Avid Technology, Inc.
//
//

//
//  AGRPlugInInstance.m
//  AAX_GUI_Runner
//

// Self Include
#import "AGRPlugInInstance.h"

// AGR Includes
#import "AGRPlugInWindowController.h"

// AAXHostShell Includes
#import "AAXHostShell_ObjC_Commands.h"
#import <AAXHostShell/AAXHostShell.h>


//-------------------------------------------------------------------------------------------
#pragma mark private interface

@interface AGRPlugInInstance ()
@property (nonatomic) AAXHostShell_CommandInstantiatePlugIn* privateCommandInstantiatePlugIn;
@property (nonatomic) NSDictionary* privateData;
@end


//-------------------------------------------------------------------------------------------
@implementation AGRPlugInInstance
@synthesize privateCommandInstantiatePlugIn = _commandInstantiatePlugIn;
@synthesize privateData = _privateData;


//-------------------------------------------------------------------------------------------
#pragma mark private methods

- (AAXHShell_TCommandListID)instantiatePlugInCommandListID
{
	return [[self privateCommandInstantiatePlugIn] identifier];
}

- (void)resetData
{
	[self.privateData setValue:[NSNumber numberWithUnsignedLong:kAAXHShell_IDUnset] forKey:@"PlugInID"];
	[self.privateData setValue:[NSNumber numberWithUnsignedLong:kAAXHShell_IDUnset] forKey:@"InstanceID"];
	[self setPrivateCommandInstantiatePlugIn:nil];
}

- (NSDictionary*)instanceDataFromCommandListDataIDs:(NSDictionary*)commandListDataIDs
{
	AAXHShell_TPlugInID plugInID = kAAXHShell_IDUnset;
	AAXHShell_TInstanceID instanceID = kAAXHShell_IDUnset;
	
	bool result = true;
	AAXHShell_TCommandListDataID plugInIDDataID = [[commandListDataIDs valueForKey:@"PlugInIDDataID"] unsignedLongValue];
	AAXHShell_TCommandListDataID instanceIDDataID = [[commandListDataIDs valueForKey:@"InstanceIDDataID"] unsignedLongValue];
	AAXHShell_TCommandListID inputCommandListID = [[commandListDataIDs valueForKey:@"CommandListID"] unsignedLongValue];
	result = result && AAXHShell_GetCommandListData(inputCommandListID, plugInIDDataID, &plugInID, sizeof(AAXHShell_TPlugInID));
	result = result && AAXHShell_GetCommandListData(inputCommandListID, instanceIDDataID, &instanceID, sizeof(AAXHShell_TInstanceID));
	
	return (false == result) ? nil : [NSDictionary dictionaryWithObjectsAndKeys:
									  [NSNumber numberWithUnsignedLong:plugInID], @"PlugInID",
									  [NSNumber numberWithUnsignedLong:instanceID], @"InstanceID",
									  nil];
}


//-------------------------------------------------------------------------------------------
#pragma mark public methods

- (id)initWithPath:(NSString*)path
{
	self = [super init];
	
	if (self)
	{
		[self resetData];
		[self setPrivateCommandInstantiatePlugIn: [[AAXHostShell_CommandInstantiatePlugIn alloc] initForPath:path]];
		
		// Execute the command list
		bool result = [self.privateCommandInstantiatePlugIn execute];
		if (true == result)
		{
			NSDictionary* commandListDataIDs = [self.privateCommandInstantiatePlugIn dataIDs];
			[self setPrivateData:[self instanceDataFromCommandListDataIDs:commandListDataIDs]];
		}
	}
		
	return self;
}

- (AAXHShell_TInstanceID)identifier
{
	NSNumber* idNum = [self identifierNum];
	assert(nil != idNum);
	return [idNum unsignedLongValue];
}

- (NSNumber*)identifierNum
{
	return [self.privateData objectForKey:@"InstanceID"];
}

- (NSDictionary*)data;
{
	return self.privateData;
}


//-------------------------------------------------------------------------------------------
#pragma mark inherited methods

- (id)copyWithZone:(NSZone *)zone
{
	id copy = [[[self class] allocWithZone:zone] init];
	if (copy)
	{
		[copy setPrivateCommandInstantiatePlugIn:[self.privateCommandInstantiatePlugIn copyWithZone:zone]];
		[copy setPrivateData:[self.privateData copyWithZone:zone]];
	}
	return copy;
}

-(void)dealloc
{
	AAXHShell_UndoAndDeleteCommandList([self instantiatePlugInCommandListID]);
}

@end
