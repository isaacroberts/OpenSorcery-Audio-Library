//
//
// Copyright 2014 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_ClientCore.h
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 2/19/14.
//
//

#ifndef __AAXHostShell_CppUtilities_ClientCore__
#define __AAXHostShell_CppUtilities_ClientCore__

// AAXHostShell Include
#include "AAXHostShell_Types.h"
#include "AAXHostShell_PlugIn.h"

// Standard Includes
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stack>



// Forward declarations
struct AAXHShell_SHostContext;


namespace AAXHostShell
{
	/**
	 @brief Utility class for AAXHostShell management
	 
	 @details
	 - This is the highest-level utility interface for C++ clients of
	 AAXHostShell
	 - In general, the methods of this class will throw on failure and
	 are guaranteed not to return invalid data unless noted otherwise.
	 
	 @ingroup AAXHostShell_CppUtilities
	 */
	class ClientCore
	{
		// ---------------------------------------------------------------
		// ClientCore Interface
		// ---------------------------------------------------------------
	public:
		typedef std::shared_ptr<ClientCore> SmartPtr;
		typedef std::map<AAXHShell_TPlugInID, PlugIn::SmartPtr> TPlugInMap;
		
	public:
		/* default constructor */		ClientCore();
		/* non-virtual destructor */	~ClientCore();
		
	public:
		// Lifecycle management
		void Initialize();
		void Teardown();
		bool IsInitialized() const;
		
		// Host context
		void GetHostContext(AAXHShell_SHostContext& outShellContext) const;
		void SetHostContext(const AAXHShell_SHostContext& inShellContext);
		int32_t GetDebugLevel() const;
		void SetDebugLevel(int32_t inDebugBits);
		class SetHostContextScope
		{
		public:
			explicit SetHostContextScope(ClientCore::SmartPtr inCore, const AAXHShell_SHostContext& inShellContext);
			~SetHostContextScope();
		private:
			ClientCore::SmartPtr mCore;
			AAXHShell_SHostContext mContext;
		};
		
		// Plug-in management
		PlugIn::SmartPtr LoadPlugIn(const std::string& inFilePath);
		void UnloadPlugIn(AAXHShell_TPlugInID inPlugInID);
		const PlugIn::SmartPtrConst GetPlugIn(AAXHShell_TPlugInID inPlugInID) const;
		PlugIn::SmartPtr GetPlugIn(AAXHShell_TPlugInID inPlugInID);
		const TPlugInMap& GetLoadedPlugIns() const;
		
		
		// ---------------------------------------------------------------
		// ClientCore Internal Data
		// ---------------------------------------------------------------
	private:
		void ValidateInternalData() const;
		
	private:
		/**
		 @brief This class encapsulates an AAXHostShell initialization scope
		 
		 @details
		 The AAXHostShell is initialized upon construction of the first SharedInitScope
		 and torn down upon destruction of the last SharedInitScope.
		 
		 @todo This class uses a simple reference counting mechanism and is not currently
		 thread-safe. Clients should only instantiate objects of this class on the main
		 thread.
		 
		 */
		class SharedInitScope
		{
		public:
			SharedInitScope();
			SharedInitScope(const SharedInitScope&);
			~SharedInitScope();
		};
		
		std::stack<SharedInitScope> mInitScopes;
		TPlugInMap mPlugIns;
	};

}

#endif /* defined(__AAXHostShell_CppUtilities_ClientCore__) */
