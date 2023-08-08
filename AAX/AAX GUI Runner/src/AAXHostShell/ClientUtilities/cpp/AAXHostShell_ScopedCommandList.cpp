//
//
// Copyright 2014 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_LoadPlugInScope.cpp
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 2/19/14.
//
//

// Self Include
#include "AAXHostShell_ScopedCommandList.h"

// AAXHostShell Include
#include "AAXHostShell.h"

// AAXHostShell Client Utility Includes
#include "AAXHostShell_Exceptions.h"

// Standard Includes
#include <stdexcept>


AAXHostShell::ScopedCommandList::ScopedCommandList()
: mDeleteOnReset(false)
, mCommandListID(kAAXHShell_IDUnset)
{
}

AAXHostShell::ScopedCommandList::ScopedCommandList(AAXHShell_TCommandListID inCommandListID, bool inTakeOwnership)
: mDeleteOnReset(inTakeOwnership)
, mCommandListID(kAAXHShell_IDUnset)
{
	Execute();
}

AAXHostShell::ScopedCommandList::~ScopedCommandList()
{
	Reset();
}

void AAXHostShell::ScopedCommandList::Reset(AAXHShell_TCommandListID inCommandListID, bool inTakeOwnership)
{
	Reset();
	
	mDeleteOnReset = inTakeOwnership;
	mCommandListID = inCommandListID;
	
	Execute();
}

void AAXHostShell::ScopedCommandList::Reset()
{
	if (kAAXHShell_IDUnset != mCommandListID)
	{
		if (true == mDeleteOnReset)
		{
			const AAXHShell_TResult result = AAXHShell_UndoAndDeleteCommandList(mCommandListID);
			AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(result, AAXHShell_UndoAndDeleteCommandList);
		}
		else
		{
			const AAXHShell_TResult result = AAXHShell_UndoCommandList(mCommandListID);
			AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(result, AAXHShell_UndoCommandList);
		}
	}
	
	SetInternalDataToDefault();
}

bool AAXHostShell::ScopedCommandList::IsValid() const
{
	return (kAAXHShell_IDUnset != mCommandListID);
}

AAXHShell_TCommandListID AAXHostShell::ScopedCommandList::ID() const
{
	return mCommandListID;
}

void AAXHostShell::ScopedCommandList::Execute()
{
	if (false == AAXHShell_ExecuteCommandList(mCommandListID))
	{
		// Execution failure is a little different that a simple reset since
		// AAXHShell_ExecuteCommandList() is guaranteed to have already undone
		// everything that it can before returning a failure code.
		if (mDeleteOnReset)
		{
			AAXHShell_DeleteCommandList(mCommandListID);
		}
		
		SetInternalDataToDefault();
	}
}

void AAXHostShell::ScopedCommandList::SetInternalDataToDefault()
{
	mDeleteOnReset = false;
	mCommandListID = kAAXHShell_IDUnset;
}
