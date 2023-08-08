//
//
// Copyright 2014 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_ClientCore.cpp
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 2/19/14.
//
//

// Self Include
#include "AAXHostShell_ClientCore.h"

// AAXHostShell Include
#include "AAXHostShell.h"
#include "AAXHostShell_Utilities.h"

// AAXHostShell Client Utility Includes
#include "AAXHostShell_StringUtils.h"
#include "AAXHostShell_Exceptions.h"

// Standard Includes
#include <algorithm>
#include <stdexcept>


//----------------------------------------------------------
#pragma mark - AAXHostShell::ClientCore
//----------------------------------------------------------

AAXHostShell::ClientCore::ClientCore()
{
	Initialize();
}

AAXHostShell::ClientCore::~ClientCore()
{
	Teardown();
}


//----------------------------------------------------------
#pragma mark Lifecycle management

void AAXHostShell::ClientCore::Initialize()
{
	SharedInitScope initScope;
	mInitScopes.push(initScope);
}

void AAXHostShell::ClientCore::Teardown()
{
	mPlugIns.clear();
	
	while (false == mInitScopes.empty())
	{
		mInitScopes.pop();
	}
}

bool AAXHostShell::ClientCore::IsInitialized() const
{
	return false == mInitScopes.empty();
}


//----------------------------------------------------------
#pragma mark Host context

void AAXHostShell::ClientCore::GetHostContext(AAXHShell_SHostContext& outShellContext) const
{
	const AAXHShell_TResult success = AAXHShell_GetHostContext(&outShellContext);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(success, AAXHShell_GetHostContext);
}

void AAXHostShell::ClientCore::SetHostContext(const AAXHShell_SHostContext& inShellContext)
{
	if (false == AAXHostShell::HostContextPtrIsValid(&inShellContext))
	{
		throw AAXHostShell::Exception::InvalidParameter::Format(__FUNCTION__, "Encountered unsupported host context: %s", AAXHostShell::AsString(inShellContext).c_str());
	}
	
	const AAXHShell_TResult success = AAXHShell_SetHostContext(&inShellContext);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(success, AAXHShell_SetHostContext);
}

int32_t AAXHostShell::ClientCore::GetDebugLevel() const
{
	int32_t result = 0x0;
	const AAXHShell_TResult success = AAXHShell_GetDebugLevel(&result);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(success, AAXHShell_GetDebugLevel);
	return result;
}

void AAXHostShell::ClientCore::SetDebugLevel(int32_t inDebugBits)
{
	const AAXHShell_TResult success = AAXHShell_SetDebugLevel(inDebugBits);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(success, AAXHShell_SetDebugLevel);
}

AAXHostShell::ClientCore::SetHostContextScope::SetHostContextScope(ClientCore::SmartPtr inCore, const AAXHShell_SHostContext& inShellContext)
: mCore(inCore)
, mContext(AAXHShell_kHostContext_Any)
{
	if (NULL != mCore.get())
	{
		mCore->GetHostContext(mContext);
		mCore->SetHostContext(inShellContext);
	}
}

AAXHostShell::ClientCore::SetHostContextScope::~SetHostContextScope()
{
	if (NULL != mCore.get() && (false == AAXHostShell::HostContextPtrIsAny(&mContext)))
	{
		mCore->SetHostContext(mContext);
	}
}


//----------------------------------------------------------
#pragma mark Plug-in management

static bool PV_IsRawAAXPlugInBundleName(const std::string& inString)
{
	const std::string::difference_type numPeriods = std::count(inString.begin(), inString.end(), '.');
	const std::string::difference_type numSlashes = std::count(inString.begin(), inString.end(), '/');
	const std::string::difference_type numBackslashes = std::count(inString.begin(), inString.end(), '\\');
	const std::string::size_type aaxpluginPos = inString.find(".aaxplugin");
	
	// Note that this algorithm accepts neither escaped characters nor a trailing slash
	return (1 == numPeriods && 0 == numSlashes && 0 == numBackslashes && inString.npos != aaxpluginPos);
}

AAXHostShell::PlugIn::SmartPtr AAXHostShell::ClientCore::LoadPlugIn(const std::string& inFilePath)
{
	ValidateInternalData();
	
	// Copy input path to a non-const std::string
	std::string filePath(inFilePath);
	
	// If the "path" is a raw .aaxplugin then prepend the default plug-ins directory
	if (true == PV_IsRawAAXPlugInBundleName(filePath))
	{
		const size_t bufferSize = 2048;
		char defaultPlugInsDirectory[bufferSize];
		const AAXHShell_TResult success = AAXHShell_GetDefaultPlugInsDirectory(defaultPlugInsDirectory, bufferSize);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(success, AAXHShell_GetDefaultPlugInsDirectory);
		
		filePath = std::string(defaultPlugInsDirectory) + filePath;
	}
	
	for (TPlugInMap::const_iterator iter = mPlugIns.begin(); iter != mPlugIns.end(); ++iter)
	{
		const PlugIn& curPlugIn = *iter->second;
		if (curPlugIn.GetFilePath() == filePath)
		{
			throw AAXHostShell::Exception::InvalidParameter::Format(__FUNCTION__, "Encountered redundant plug-in file path: %s", inFilePath.c_str());
		}
	}
	
	PlugIn::SmartPtr newPlugIn(new PlugIn(filePath));
	
	if (NULL == newPlugIn.get())
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "std::shared_ptr creation failed");
	}
	else if (false == newPlugIn->IsValid())
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Plug-in loading failed");
	}
	
	const AAXHShell_TPlugInID plugInID = newPlugIn->ID();
	
	if (0 != mPlugIns.count(plugInID))
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Encountered redundant AAXHShell_TPlugInID: %llu", (long long unsigned int)plugInID);
	}
	else if (kAAXHShell_IDUnset == plugInID)
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Encountered invalid AAXHShell_TPlugInID");
	}
	else
	{
		mPlugIns[plugInID] = newPlugIn;
	}
	
	return newPlugIn;
}

void AAXHostShell::ClientCore::UnloadPlugIn(AAXHShell_TPlugInID inPlugInID)
{
	mPlugIns.erase(inPlugInID);
}

const AAXHostShell::PlugIn::SmartPtrConst AAXHostShell::ClientCore::GetPlugIn(AAXHShell_TPlugInID inPlugInID) const
{
	return mPlugIns.at(inPlugInID);
}

AAXHostShell::PlugIn::SmartPtr AAXHostShell::ClientCore::GetPlugIn(AAXHShell_TPlugInID inPlugInID)
{
	return mPlugIns.at(inPlugInID);
}

const AAXHostShell::ClientCore::TPlugInMap& AAXHostShell::ClientCore::GetLoadedPlugIns() const
{
	ValidateInternalData();
	return mPlugIns;
}

void AAXHostShell::ClientCore::ValidateInternalData() const
{
	for (TPlugInMap::const_iterator iter = mPlugIns.begin(); iter != mPlugIns.end(); ++iter)
	{
		if (NULL == iter->second.get())
		{
			throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Encountered NULL AAXHostShell::PlugIn at ID: %llu", (long long unsigned int)iter->first);
		}
	}
}


//----------------------------------------------------------
#pragma mark - AAXHostShell::ClientCore::SharedInitScope
//----------------------------------------------------------

enum EInitScopeAction
{
	eInitScopeAction_Init,
	eInitScopeAction_Teardown
};

static void PV_DoAction(EInitScopeAction inAction)
{
	static int32_t sRefCount = 0;
	
	const int32_t newRefCount =
	(eInitScopeAction_Init == inAction) ? ++sRefCount :
	(eInitScopeAction_Teardown == inAction) ? --sRefCount :
	sRefCount;
	
	if (eInitScopeAction_Init == inAction && 1 == newRefCount)
	{
		AAXHShell_Initialize();
	}
	else if (eInitScopeAction_Teardown == inAction && 0 == newRefCount)
	{
		AAXHShell_Teardown();
	}
}

AAXHostShell::ClientCore::SharedInitScope::SharedInitScope()
{
	PV_DoAction(eInitScopeAction_Init);
}

AAXHostShell::ClientCore::SharedInitScope::SharedInitScope(const SharedInitScope&)
{
	PV_DoAction(eInitScopeAction_Init);
}

AAXHostShell::ClientCore::SharedInitScope::~SharedInitScope()
{
	PV_DoAction(eInitScopeAction_Teardown);
}
