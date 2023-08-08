//
//
// Copyright 2014 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_PlugIn.h
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 2/19/14.
//
//

#ifndef __AAXHostShell_CppUtilities_PlugIn__
#define __AAXHostShell_CppUtilities_PlugIn__

// AAXHostShell Include
#include "AAXHostShell_Types.h"
#include "AAXHostShell_Instance.h"
#include "AAXHostShell_StrongTypeDefs.h"
#include "AAXHostShell_PageTableUtils.h"

// AAX Include
#include "AAX.h"

// Boost Includes
#include "boost/property_tree/ptree.hpp"

// Standard Includes
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>


namespace AAXHostShell
{
	/**
	 @brief Utility class representing a loaded plug-in
	 
	 @details
	 - The plug-in is unloaded when its PlugIn object is destroyed.
	 - In general, the methods of this class will throw on failure and
	 are guaranteed not to return invalid data unless noted otherwise.
	 
	 @ingroup AAXHostShell_CppUtilities
	 */
	class PlugIn
	{
		// ---------------------------------------------------------------
		// PlugIn Interface
		// ---------------------------------------------------------------
	public:
		typedef std::shared_ptr<PlugIn> SmartPtr;
		typedef std::shared_ptr<const PlugIn> SmartPtrConst;
		typedef std::map<AAXHShell_TInstanceID, AAXHostShell::Instance::SmartPtr> TInstanceMap;
		typedef std::map<TTypeIndex, AAX_SPlugInIdentifierTriad> TTypeTriadMap;
		typedef std::map<TTypeIndex, std::pair<int32_t, bool> > TTypePlatformMap; // Value is <platform, online>
		typedef std::pair<bool, AAX_CPropertyValue> TPropertyPair; ///< first: true if the property is defined, second: the property value
		typedef std::map<int32_t, std::vector<std::pair<int32_t, std::string> > > TValidationResultVecMap; ///< first: validation result type, second: vector of pairs of result code and message
		
	public:
		explicit /* constructor */		PlugIn(const std::string& inFilePath); //TODO: Make this private so that only ClientCore can create PlugIn objects (guarantees that the AAXHostShell has been initialized)
		/* non-virtual destructor */	~PlugIn();
		
	public:
		bool IsValid() const;
		AAXHShell_TPlugInID ID() const;
		std::string GetFilePath() const;
		std::string GetPackageName() const;
		std::string GetManufacturerName() const;

		std::vector<std::string> GetEffectIDs() const;
		int32_t GetNumEffectIDs() const;
		std::map<std::string, std::string> GetEffectIDsAndNames() const; ///< first: Effect ID, second: Effect name
		std::string GetEffectDescriptionXMLByIndex(const TEffectIndex& inIndex) const;
        void* GetProcPtr(const TEffectIndex& inIndex, AAXHShel_ProcType inProcID);
		boost::property_tree::ptree GetEffectDescriptionPTreeByIndex(const TEffectIndex& inIndex) const;
		bool GetEffectSupportsContextByIndex(const TEffectIndex& inIndex, const AAXHShell_SHostContext& inShellContext) const;
        bool GetTypeIsAudioSuite(const TTypeIndex& inTypeIndex) const;
		
		const Instance::SmartPtrConst GetInstance(AAXHShell_TInstanceID inInstanceID) const;
		Instance::SmartPtr GetInstance(AAXHShell_TInstanceID inInstanceID);
		const TInstanceMap& GetInstances() const;
		AAXHostShell::Instance::SmartPtr InstantiateEffectByIndex(const TEffectIndex& inIndex);
		AAXHostShell::Instance::SmartPtr InstantiateEffectByIndex(const TEffectIndex& inIndex, const AAXHShell_SHostContext& inShellContext);
		AAXHostShell::Instance::SmartPtr InstantiateEffectByTypeIndex(const TTypeIndex& inIndex);
		AAXHostShell::Instance::SmartPtr InstantiateEffectByID(const std::string& inID);
		AAXHostShell::Instance::SmartPtr InstantiateEffectByID(const std::string& inID, const AAXHShell_SHostContext& inShellContext);
		void DestroyInstance(AAXHShell_TInstanceID inInstanceID);
		
		TTypeTriadMap GetTypes() const;
		TTypeTriadMap GetTypesForEffect(const std::string& inEffectID) const;
		TTypePlatformMap GetTypePlatforms() const;
		TPropertyPair GetPropertyForEffect(const TEffectIndex& inEffectIndex, AAX_EProperty inProperty) const;
		TPropertyPair GetPropertyForType(const TTypeIndex& inTypeIndex, int64_t inProperty) const;
		
		const TValidationResultVecMap& GetValidationResults() const;
		
		// PageTables
		void LoadPageTable(const TTypeIndex& inTypeIndex);
		bool IsPageTableLoaded(const TTypeIndex& inTypeIndex) const;
		std::string GetPageTablesPath(const TTypeIndex& inTypeIndex) const; ///< \returns Empty std::string if no page table resource is registered
		
		bool HasCustomizedPageTable(const TTypeIndex& inTypeIndex, uint32_t inPageTableType, int32_t inPageTableSize) const;
		void GetPlugInControlForPage(const TTypeIndex& inTypeIndex, uint32_t inPageTableType, int32_t inPageTableSize,
									 int16_t inPageIndex, int16_t inControllerIndex, std::string& outControl);
		
		bool GetPageTableTypesForPlugInType(const TTypeIndex& inTypeIndex, std::vector<AAXHostShell::SPageTableType>& outPageTableTypes) const;
		
		bool GetControlsForPageTableType(const TTypeIndex& inTypeIndex,
										 uint32_t inPageTableType, int32_t inPageTableSize, std::vector<std::string>& outControls) const;
		
		bool GetPageTableControlIdentifiers(const TTypeIndex& inTypeIndex, std::vector<std::string>* ioList) const;
		
		// Map of table type / size. All customized table types are included
		std::multimap<uint32_t, int32_t> GetSupportedPageTables(const TTypeIndex& inTypeIndex) const;
		
		// Categories
		uint32_t GetEffectCategoryBits(const TEffectIndex& inEffectIndex) const;
		std::string GetCategoryBypassParameterID(const TEffectIndex& inEffectIndex, uint32_t inCategory) const;
		
		// Meters
		bool GetEffectSupportsGainReductionMetering(const TEffectIndex& inEffectIndex) const;
		
		// ---------------------------------------------------------------
		// PlugIn Internal Data
		// ---------------------------------------------------------------
	private:
		void ValidateInternalData() const;
		
	private:
		class Scope;
		
		TInstanceMap mInstances;
		mutable TValidationResultVecMap mValidationResults;      // lazily initialized cache
		const std::unique_ptr<Scope> mSelf;
	};
}

#endif /* defined(__AAXHostShell_CppUtilities_PlugIn__) */
