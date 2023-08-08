//
//
// Copyright 2014 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_ScopedCommandList.h
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 2/19/14.
//
//

#ifndef __AAXHostShell_CppUtilities_ScopedCommandList__
#define __AAXHostShell_CppUtilities_ScopedCommandList__

// AAXHostShell Includes
#include "AAXHostShell_Types.h"

// Standard Includes
#include <string>

namespace AAXHostShell
{
	/**
	 @brief This class encapsulates a single command list
	 
	 @details
	 The command list is executed upon construction and is undone upon destruction
	 
	 @note This class is not synchronized. Multi-threaded clients must provide
	 synchronization for access to this class.
	 
	 @pre The AAXHostShell must be initialized throughout the lifetime of all
	 \ref ScopedCommandList objects
	 */
	class ScopedCommandList
	{
	public:
		/**
		 @brief Create an empty command list scope
		 */
		/* default constructor */		ScopedCommandList();
		
		/**
		 @brief Execute a command list
		 
		 @details
		 
		 @param[in] inCommandListID
		 @param[in] inTakeOwnership		If \c true, the command list and all associated data will be deleted upon \ref ScopedCommandList destruction
		 */
		explicit /* constructor */		ScopedCommandList(AAXHShell_TCommandListID inCommandListID, bool inTakeOwnership);
		
		/**
		 @brief Reset the command list.
		 */
		/* non-virtual destructor */	~ScopedCommandList();
		
		/**
		 @brief Reset any previous command list and execute a newly provided command list
		 
		 @details
		 
		 @param[in] inCommandListID
		 @param[in] inTakeOwnership		If \c true, the command list and all associated data will be deleted upon \ref ScopedCommandList destruction
		 */
		void Reset(AAXHShell_TCommandListID inCommandListID, bool inTakeOwnership);
		
		/**
		 @brief Undo the command list and delete if the list is owned by this \ref ScopedCommandList
		 */
		void Reset();
		
		/**
		 @brief Returns \c true if a valid command list is currently set
		 */
		bool IsValid() const;
		
		/**
		 @brief Returns the command list ID
		 */
		AAXHShell_TCommandListID ID() const;
		
	private:
		void Execute();
		void SetInternalDataToDefault();
		
		bool mDeleteOnReset;
		AAXHShell_TCommandListID mCommandListID;
	};
}

#endif /* defined(__DSH_AAXH__AAXHostShell_CppUtilities_ScopedCommandList__) */
