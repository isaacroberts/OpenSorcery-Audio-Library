//
//
// Copyright 2015, 2017 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_Commands.m
//  AAXHostShell client utilities
//

// Self Include
#import "AAXHostShell_ObjC_Commands.h"

// AAXHostShell Include
#import <AAXHostShell/AAXHostShell.h>





//-------------------------------------------------------------------------------------------
#pragma mark - AAXHostShell_CommandCreateGUI
//-------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------
#pragma mark private category

@interface AAXHostShell_CommandCreateGUI ()
- (AAXHShell_TCommandListID)privateCreateIDForWindow:(NSWindow*)window forInstance:(AAXHShell_TInstanceID)instanceID ofPlugIn:(AAXHShell_TPlugInID)plugInID;
@property (nonatomic) AAXHShell_TCommandListID privateCommandListID;
@end

//-------------------------------------------------------------------------------------------
@implementation AAXHostShell_CommandCreateGUI
@synthesize privateCommandListID = _commandListID;


//-------------------------------------------------------------------------------------------
#pragma mark NSObject methods

- (void)dealloc
{
	AAXHShell_TCommandListID commandListID = [self identifier];
	if (kAAXHShell_IDUnset != commandListID)
	{
		AAXHShell_DeleteCommandList(commandListID);
	}
}

//-------------------------------------------------------------------------------------------
#pragma mark NSCopying methods

- (id)copyWithZone:(NSZone *)zone
{
	id copy = [[[self class] allocWithZone:zone] init];
	if (copy)
	{
		[copy setPrivateCommandListID:self.privateCommandListID];
	}
	return copy;
}


//-------------------------------------------------------------------------------------------
#pragma mark AAXHostShell_CommandProtocol methods

- (BOOL)execute
{
	BOOL result = false;
	AAXHShell_TCommandListID commandListID = [self identifier];
	if (kAAXHShell_IDUnset != commandListID)
	{
		result = AAXHShell_ExecuteCommandList(commandListID);
	}
	return result;
}

- (BOOL)undo
{
	BOOL result = false;
	AAXHShell_TCommandListID commandListID = [self identifier];
	if (kAAXHShell_IDUnset != commandListID)
	{
		result = AAXHShell_UndoCommandList(commandListID);
	}
	return result;
}

- (AAXHShell_TCommandListID)identifier
{
	return _commandListID;
}

//-------------------------------------------------------------------------------------------
#pragma mark public methods

- (id)initForWindow:(NSWindow*)window forInstanceNum:(NSNumber*)instanceIDNum ofPlugInNum:(NSNumber*)plugInIDNum
{
	self = [super init];
	if (self)
	{
		const AAXHShell_TInstanceID instanceID = [instanceIDNum unsignedLongValue];
		const AAXHShell_TPlugInID plugInID = [plugInIDNum unsignedLongValue];
		_commandListID = [self privateCreateIDForWindow:window forInstance:instanceID ofPlugIn:plugInID];
	}
	return self;
}

- (id)initForWindow:(NSWindow*)window forInstance:(AAXHShell_TInstanceID)instanceID ofPlugIn:(AAXHShell_TPlugInID)plugInID
{
	self = [super init];
	if (self)
	{
		_commandListID = [self privateCreateIDForWindow:window forInstance:instanceID ofPlugIn:plugInID];
	}
	return self;
}

//-------------------------------------------------------------------------------------------
#pragma mark private methods

- (AAXHShell_TCommandListID)privateCreateIDForWindow:(NSWindow*)window forInstance:(AAXHShell_TInstanceID)instanceID ofPlugIn:(AAXHShell_TPlugInID)plugInID
{
	assert(nil != window);
	assert(kAAXHShell_IDUnset != instanceID);
	assert(kAAXHShell_IDUnset != plugInID);
	
	// These values will be included in the returned dictionary
	AAXHShell_TCommandListID guiCreateCommandListID = kAAXHShell_IDUnset;
	
	// Create a new command list
	bool commandListResult = AAXHShell_CreateCommandList(&guiCreateCommandListID);
	
	// Allocate memory for parameters that will be passed along in the command list
	AAXHShell_TCommandListDataID plugInIDDataID = kAAXHShell_IDUnset;
	AAXHShell_TCommandListDataID instanceIDDataID = kAAXHShell_IDUnset;
	AAXHShell_TCommandListDataID viewIDDataID = kAAXHShell_IDUnset;
	AAXHShell_TCommandListDataID viewAddressDataID = kAAXHShell_IDUnset;
	
	const void* const viewAddress = CFBridgingRetain([window contentView]);
	
	commandListResult = commandListResult && AAXHShell_AddCommandListData(guiCreateCommandListID, &plugInID, sizeof(AAXHShell_TPlugInID), &plugInIDDataID);
	commandListResult = commandListResult && AAXHShell_AddCommandListData(guiCreateCommandListID, &instanceID, sizeof(AAXHShell_TInstanceID), &instanceIDDataID);
	commandListResult = commandListResult && AAXHShell_AddCommandListData(guiCreateCommandListID, 0, sizeof(AAXHShell_TViewID), &viewIDDataID);
	commandListResult = commandListResult && AAXHShell_AddCommandListData(guiCreateCommandListID, &viewAddress, sizeof(void*), &viewAddressDataID);
	
	// Construct the command list using the addresses of its internally-allocated data
	commandListResult = commandListResult && AAXHShell_PushCommandCreateEffectGUI(guiCreateCommandListID, plugInIDDataID, instanceIDDataID);
	commandListResult = commandListResult && AAXHShell_PushCommandCreateViewContainer(guiCreateCommandListID, viewAddressDataID, viewIDDataID);
	commandListResult = commandListResult && AAXHShell_PushCommandSetEffectInstanceView(guiCreateCommandListID, plugInIDDataID, instanceIDDataID, viewIDDataID);
	commandListResult = commandListResult && AAXHShell_PushCommandSetCurrentViewOnEffectGUI(guiCreateCommandListID, plugInIDDataID, instanceIDDataID);
	commandListResult = commandListResult && AAXHShell_PushCommandRunEffectGUI(guiCreateCommandListID, plugInIDDataID);
	
	if (false == commandListResult)
	{
		// TODO: Figure out how to unroll any set values in the event of a failure
		guiCreateCommandListID = kAAXHShell_IDUnset;
	}
	
	return guiCreateCommandListID;
}






@end // @implementation AAXHostShell_CommandCreateGUI




//-------------------------------------------------------------------------------------------
#pragma mark - AAXHostShell_CommandInstantiatePlugIn
//-------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------
#pragma mark private category

@interface AAXHostShell_CommandInstantiatePlugIn ()
- (NSDictionary*)privateCreateDataWithPath:(NSString*)path;
@property (nonatomic) NSDictionary* privateDataIDs;
@end

//-------------------------------------------------------------------------------------------
@implementation AAXHostShell_CommandInstantiatePlugIn
@synthesize privateDataIDs = _dataIDs;


//-------------------------------------------------------------------------------------------
#pragma mark NSObject methods

- (void)dealloc
{
	AAXHShell_TCommandListID commandListID = [self identifier];
	if (kAAXHShell_IDUnset != commandListID)
	{
		AAXHShell_DeleteCommandList(commandListID);
	}
}

//-------------------------------------------------------------------------------------------
#pragma mark NSCopying methods

- (id)copyWithZone:(NSZone *)zone
{
	id copy = [[[self class] allocWithZone:zone] init];
	if (copy)
	{
		[copy setPrivateDataIDs:[self.privateDataIDs copyWithZone:zone]];
	}
	return copy;
}

//-------------------------------------------------------------------------------------------
#pragma mark AAXHostShell_CommandProtocol methods

- (BOOL)execute
{
	BOOL result = false;
	AAXHShell_TCommandListID commandListID = [self identifier];
	if (kAAXHShell_IDUnset != commandListID)
	{
		result = AAXHShell_ExecuteCommandList(commandListID);
	}
	return result;
}

- (BOOL)undo
{
	BOOL result = false;
	AAXHShell_TCommandListID commandListID = [self identifier];
	if (kAAXHShell_IDUnset != commandListID)
	{
		result = AAXHShell_UndoCommandList(commandListID);
	}
	return result;
}

- (AAXHShell_TCommandListID)identifier
{
	return [[[self privateDataIDs] valueForKey:@"CommandListID"] unsignedLongLongValue];
}

//-------------------------------------------------------------------------------------------
#pragma mark public methods

- (id)initForURL:(NSURL*)pathURL
{
	self = [super init];
	if (self)
	{
		NSString* path = [pathURL absoluteString];
		_dataIDs = [self privateCreateDataWithPath:path];
	}
	return self;
}

- (id)initForPath:(NSString*) path
{
	self = [super init];
	if (self)
	{
		_dataIDs = [self privateCreateDataWithPath:path];
	}
	return self;
}

- (NSDictionary*)dataIDs
{
	return _dataIDs;
}

//-------------------------------------------------------------------------------------------
#pragma mark private methods

- (NSDictionary*)privateCreateDataWithPath:(NSString*)path;
{
	NSMutableDictionary *toReturn = [[NSMutableDictionary alloc] init];
	
	// These values will be included in the returned dictionary
	AAXHShell_TCommandListID instantiateCommandListID = kAAXHShell_IDUnset;
	AAXHShell_TCommandListDataID plugInIDDataID = kAAXHShell_IDUnset;
	AAXHShell_TCommandListDataID instanceIDDataID = kAAXHShell_IDUnset;
	
	bool result = AAXHShell_CreateCommandList(&instantiateCommandListID);
	if (true == result)
	{
		// Allocate memory for parameters that will be passed along in the command list
		AAXHShell_TCommandListDataID plugInPathDataID = kAAXHShell_IDUnset;
		AAXHShell_TCommandListDataID effectIndexIDDataID = kAAXHShell_IDUnset;
		
		const NSUInteger pathByteLength = 1+[path lengthOfBytesUsingEncoding:[NSString defaultCStringEncoding]];
		assert((uint32_t)0xFFFFFFFF >= pathByteLength);
		const char* const pathCStr = [path cStringUsingEncoding:[NSString defaultCStringEncoding]];
		
		result = result && AAXHShell_AddCommandListData(instantiateCommandListID, 0, sizeof(AAXHShell_TPlugInID), &plugInIDDataID);
		result = result && AAXHShell_AddCommandListData(instantiateCommandListID, 0, sizeof(AAXHShell_TInstanceID), &instanceIDDataID);
		result = result && AAXHShell_AddCommandListData(instantiateCommandListID, 0, sizeof(int32_t), &effectIndexIDDataID);
		result = result && AAXHShell_AddCommandListData(instantiateCommandListID, pathCStr, (uint32_t)pathByteLength, &plugInPathDataID);
		
		// Construct the command list using the addresses of its internally-allocated data
		result = result && AAXHShell_PushCommandLoadPlugIn(instantiateCommandListID, plugInPathDataID, plugInIDDataID);
		result = result && AAXHShell_PushCommandFindFirstNativeEffectIndex(instantiateCommandListID, plugInIDDataID, effectIndexIDDataID);
		result = result && AAXHShell_PushCommandInstantiateEffect(instantiateCommandListID, plugInIDDataID, effectIndexIDDataID, instanceIDDataID);
	}
	
	if (true == result)
	{
		[toReturn setValue:[NSNumber numberWithUnsignedLong:instantiateCommandListID] forKey:@"CommandListID"];
		[toReturn setValue:[NSNumber numberWithUnsignedLong:plugInIDDataID] forKey:@"PlugInIDDataID"];
		[toReturn setValue:[NSNumber numberWithUnsignedLong:instanceIDDataID] forKey:@"InstanceIDDataID"];
	}
	
	return toReturn;
}


@end // @implementation AAXHostShell_CommandInstantiatePlugIn
