//
//
// Copyright 2014 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_PlugIn.cpp
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 2/19/14.
//
//

// Self Include
#include "AAXHostShell_PlugIn.h"

// AAXHostShell Client Utilities Include
#include "AAXHostShell_ScopedCommandList.h"
#include "AAXHostShell_StringUtils.h"
#include "AAXHostShell_Utilities.h"
#include "AAXHostShell_Exceptions.h"

// AAXHostShell Include
#include "AAXHostShell.h"

// boost Includes
#include "boost/scoped_array.hpp"
#include "boost/property_tree/xml_parser.hpp"

// Standard Includes
#include <sstream>
#include <stdexcept>
#include <algorithm>


//----------------------------------------------------------
#pragma mark - Private utilities
//----------------------------------------------------------

static AAXHShell_SHostContext PV_GetCurrentHostContext()
{
	AAXHShell_SHostContext context;
	AAXHShell_TResult success = AAXHShell_GetHostContext(&context);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(success, AAXHShell_GetHostContext);
	return context;
}


//----------------------------------------------------------
#pragma mark - AAXHostShell::PlugIn::Scope Declaration
//----------------------------------------------------------

/**
 @brief This class encapsulates a plug-in load scope
 
 @details
 An .aaxplugin bundle is loaded upon construction of an object of this class
 and unloaded upon destruction.
 
 @pre The AAXHostShell must be initialized throughout the lifetime of all
 \ref Scope objects
 */
class AAXHostShell::PlugIn::Scope
{
public:
	/**
	 @brief Load an .aaxplugin bundle
	 */
	explicit /* constructor */		Scope(const std::string& inFullPath);
	
	/**
	 @brief Unload the .aaxplugin bundle
	 */
	/* non-virtual destructor */	~Scope();
	
public:
	bool IsActive() const;
	AAXHShell_TPlugInID GetPlugInID() const;
	
private:
	void Teardown();
	
	AAXHShell_TCommandListDataID mPlugInIDDataID;
	ScopedCommandList mScopedCommandList;
};


//----------------------------------------------------------
#pragma mark - AAXHostShell::PlugIn Definition
//----------------------------------------------------------

AAXHostShell::PlugIn::PlugIn(const std::string& inFilePath)
: mInstances()
, mValidationResults()
, mSelf(new Scope(inFilePath))
{
}

AAXHostShell::PlugIn::~PlugIn()
{
	mInstances.clear();
}

bool AAXHostShell::PlugIn::IsValid() const
{
	return (NULL != mSelf) && (mSelf->IsActive());
}

AAXHShell_TPlugInID AAXHostShell::PlugIn::ID() const
{
	ValidateInternalData();
	return mSelf->GetPlugInID();
}

std::string AAXHostShell::PlugIn::GetFilePath() const
{
	ValidateInternalData();
	
	static const size_t kPathBufferSize = 2048;
	char pathCString[kPathBufferSize];
	AAXHShell_TResult result = AAXHShell_GetPlugInPath(ID(), pathCString, kPathBufferSize);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(result, AAXHShell_GetPlugInPath);
	
	return std::string(pathCString);
}

std::string AAXHostShell::PlugIn::GetManufacturerName() const
{
	ValidateInternalData();

	static const size_t kManufacturerNameBufferSize = 2048;
	char manufacturerNameCString[kManufacturerNameBufferSize];

	AAXHShell_TResult result = AAXHShell_GetManufacturerName(ID(), manufacturerNameCString, kManufacturerNameBufferSize);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(result, AAXHShell_GetManufacturerName);

	return std::string(manufacturerNameCString);
}

std::string AAXHostShell::PlugIn::GetPackageName() const
{
	ValidateInternalData();

	static const size_t kPackageNameBufferSize = 2048;
	char packageNameCString[kPackageNameBufferSize];

	AAXHShell_TResult result = AAXHShell_GetPackageName(ID(), packageNameCString, kPackageNameBufferSize);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(result, AAXHShell_GetPackageName);

	return std::string(packageNameCString);
}

std::vector<std::string> AAXHostShell::PlugIn::GetEffectIDs() const
{
	ValidateInternalData();
	
	std::vector<std::string> resultIDs;
	
	static const int32_t kBufferSize = 1024;
	char idCString[kBufferSize];
	
	AAXHShell_TResult aaxhShellResult = kAAXHShell_Success;
	
	int32_t numEffects = 0;
	aaxhShellResult = AAXHShell_GetNumberOfEffects(ID(), &numEffects);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetNumberOfEffects);
	
	for (int32_t i = 0; i < numEffects && true == aaxhShellResult; ++i)
	{
		aaxhShellResult = AAXHShell_GetEffectIDByIndex(ID(), i, idCString, kBufferSize);
		if (kAAXHShell_Success != aaxhShellResult)
		{
			throw AAXHostShell::Exception::ResultError::Format(__FUNCTION__, "Encountered failure when calling AAXHShell_GetEffectIDByIndex with plug-in ID: %llu, index: %ld", (long long unsigned int)ID(), (long int)i);
		}
		
		resultIDs.push_back(std::string(idCString));
	}
	
	return resultIDs;
}

int32_t AAXHostShell::PlugIn::GetNumEffectIDs() const
{
	ValidateInternalData();
	
	int32_t numEffects = 0;
	const AAXHShell_TResult aaxhShellResult = AAXHShell_GetNumberOfEffects(ID(), &numEffects);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetNumberOfEffects);
	
	return numEffects;
}

std::map<std::string, std::string> AAXHostShell::PlugIn::GetEffectIDsAndNames() const
{
	std::map<std::string, std::string> idToNameMap;
	
	static const int32_t kBufferSize = 128;
	char bufferCString[kBufferSize];
	
	const int32_t numEffectIDs = GetNumEffectIDs();
	for (int32_t i = 0; i < numEffectIDs; ++i)
	{
		AAXHShell_TResult aaxhShellResult = kAAXHShell_Success;
		
		aaxhShellResult = AAXHShell_GetEffectIDByIndex(ID(), i, bufferCString, kBufferSize);
		if (kAAXHShell_Success != aaxhShellResult)
		{
			throw AAXHostShell::Exception::ResultError::Format(__FUNCTION__, "Encountered failure when calling AAXHShell_GetEffectIDByIndex with plug-in ID: %llu, index: %ld", (long long unsigned int)ID(), (long int)i);
		}
		const std::string idStr = bufferCString;
		if (0 < idToNameMap.count(idStr))
		{
			throw AAXHostShell::Exception::UnsupportedValue::Format(__FUNCTION__, "Encountered redundant effect ID '%s' when calling AAXHShell_GetEffectIDByIndex with plug-in ID: %llu, index: %ld", idStr.c_str(), (long long unsigned int)ID(), (long int)i);
		}
		
		aaxhShellResult = AAXHShell_GetEffectNameByIndex(ID(), i, bufferCString, kBufferSize);
		if (kAAXHShell_Success != aaxhShellResult)
		{
			throw AAXHostShell::Exception::ResultError::Format(__FUNCTION__, "Encountered failure when calling AAXHShell_GetEffectNameByIndex with plug-in ID: %llu, index: %ld", (long long unsigned int)ID(), (long int)i);
		}
		const std::string nameStr = bufferCString;
		
		idToNameMap.insert(std::make_pair(idStr, nameStr));
	}
	
	return idToNameMap;
}

std::string AAXHostShell::PlugIn::GetEffectDescriptionXMLByIndex(const TEffectIndex& inIndex) const
{
	ValidateInternalData();
	
	int32_t bufferSize;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetEffectDescriptionSize(ID(), inIndex, &bufferSize);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectDescriptionSize);
	
	boost::scoped_array<char> idStringBuffer(new char[bufferSize]);
	aaxhShellResult = AAXHShell_GetEffectDescription(ID(), inIndex, idStringBuffer.get(), bufferSize);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectDescription);
	
	return std::string(idStringBuffer.get());
}

void* AAXHostShell::PlugIn::GetProcPtr(const TEffectIndex& inIndex, AAXHShel_ProcType inProcID)
{
    ValidateInternalData();
    void* outproc = nullptr;
    AAXHShell_TResult aaxhShellResult = AAXHShell_GetProcPtr(ID(),inIndex, inProcID, &outproc);
    AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult,AAXHShell_GetProcPtr);
    return outproc;
}

boost::property_tree::ptree AAXHostShell::PlugIn::GetEffectDescriptionPTreeByIndex(const TEffectIndex& inIndex) const
{
	using namespace boost::property_tree;
	std::stringstream xmlStream(GetEffectDescriptionXMLByIndex(inIndex));
	basic_ptree<std::string, std::string> pTree;
	read_xml(xmlStream, pTree, xml_parser::no_comments);
	return pTree;
}

bool AAXHostShell::PlugIn::GetEffectSupportsContextByIndex(const TEffectIndex& inIndex, const AAXHShell_SHostContext& inShellContext) const
{
	ValidateInternalData();
	
	if (false == AAXHostShell::HostContextPtrIsValid(&inShellContext))
	{
		throw AAXHostShell::Exception::InvalidParameter::Format(__FUNCTION__, "Encountered unsupported host context: %s", AAXHostShell::AsString(inShellContext).c_str());
	}
	
	AAXHShell_TBoolean result = 0x0;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetEffectSupportsHostContext(ID(), inIndex, &inShellContext, &result);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectSupportsHostContext);
	
	return (result != 0x0);
}

bool AAXHostShell::PlugIn::GetTypeIsAudioSuite(const TTypeIndex& inTypeIndex) const
{
	ValidateInternalData();
    AAXHShell_TBoolean isAudioSuite;
    const AAXHShell_TResult aaxShellResult = AAXHShell_GetEffectTypeIsAudioSuite(ID(), inTypeIndex, &isAudioSuite);
    AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetEffectTypeIsAudioSuite);
	
    return (bool)isAudioSuite;
}

const AAXHostShell::Instance::SmartPtrConst AAXHostShell::PlugIn::GetInstance(AAXHShell_TInstanceID inInstanceID) const
{
	ValidateInternalData();
	return mInstances.at(inInstanceID);
}

AAXHostShell::Instance::SmartPtr AAXHostShell::PlugIn::GetInstance(AAXHShell_TInstanceID inInstanceID)
{
	ValidateInternalData();
	return mInstances.at(inInstanceID);
}

const AAXHostShell::PlugIn::TInstanceMap& AAXHostShell::PlugIn::GetInstances() const
{
	ValidateInternalData();
	return mInstances;
}

AAXHostShell::Instance::SmartPtr AAXHostShell::PlugIn::InstantiateEffectByIndex(const TEffectIndex& inIndex)
{
	return InstantiateEffectByIndex(inIndex, PV_GetCurrentHostContext());
}

AAXHostShell::Instance::SmartPtr AAXHostShell::PlugIn::InstantiateEffectByTypeIndex(const TTypeIndex& inIndex)
{
	ValidateInternalData();
	
	Instance::SmartPtr newInstance(AAXHostShell::Instance::CreateInstanceOfType(ID(), inIndex));
	if (!newInstance->IsValid())
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Effect instantiation failed for index: %ld", (long int)inIndex);
	}
	
	const AAXHShell_TInstanceID instanceID = newInstance->ID();
	
	if (0 != mInstances.count(instanceID))
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Encountered redundant AAXHShell_TInstanceID: %llu", (long long unsigned int)instanceID);
	}
	else if (kAAXHShell_IDUnset == instanceID)
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Encountered invalid AAXHShell_TInstanceID");
	}
	else
	{
		mInstances[instanceID] = newInstance;
	}
	
	return newInstance;
}

AAXHostShell::Instance::SmartPtr AAXHostShell::PlugIn::InstantiateEffectByIndex(const TEffectIndex& inIndex, const AAXHShell_SHostContext& inShellContext)
{
	ValidateInternalData();
	
	if (false == AAXHostShell::HostContextPtrIsValid(&inShellContext))
	{
		throw AAXHostShell::Exception::InvalidParameter::Format(__FUNCTION__, "Encountered unsupported host context: %s", AAXHostShell::AsString(inShellContext).c_str());
	}
	
	Instance::SmartPtr newInstance(AAXHostShell::Instance::CreateInstanceOfEffectForContext(ID(), inIndex, inShellContext));
	
	if (NULL == newInstance.get())
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "std::shared_ptr creation failed");
	}
	else if (false == newInstance->IsValid())
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Effect instantiation failed for index: %ld, shell context: %s", (long int)inIndex, AAXHostShell::AsString(inShellContext).c_str());
	}
	
	const AAXHShell_TInstanceID instanceID = newInstance->ID();
	
	if (0 != mInstances.count(instanceID))
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Encountered redundant AAXHShell_TInstanceID: %llu", (long long unsigned int)instanceID);
	}
	else if (kAAXHShell_IDUnset == instanceID)
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Encountered invalid AAXHShell_TInstanceID");
	}
	else
	{
		mInstances[instanceID] = newInstance;
	}
	
	return newInstance;
}

AAXHostShell::Instance::SmartPtr AAXHostShell::PlugIn::InstantiateEffectByID(const std::string& inID)
{
	return InstantiateEffectByID(inID, PV_GetCurrentHostContext());
}

AAXHostShell::Instance::SmartPtr AAXHostShell::PlugIn::InstantiateEffectByID(const std::string& inID, const AAXHShell_SHostContext& inShellContext)
{
	if (false == AAXHostShell::HostContextPtrIsValid(&inShellContext))
	{
		throw AAXHostShell::Exception::InvalidParameter::Format(__FUNCTION__, "Encountered unsupported host context: %s", AAXHostShell::AsString(inShellContext).c_str());
	}
	
	const std::vector<std::string> effectIDs = GetEffectIDs();
	std::vector<std::string>::const_iterator iter = std::find(effectIDs.begin(), effectIDs.end(), inID);
	
	if (effectIDs.end() == iter)
	{
		throw AAXHostShell::Exception::InvalidParameter::Format(__FUNCTION__, "Unable to locate desired effect ID: %s, context: %s", inID.c_str(), AAXHostShell::AsString(inShellContext).c_str());
	}
	
	const auto index = std::distance(effectIDs.begin(), iter);
	assert(INT32_MAX >= index);
	return InstantiateEffectByIndex(static_cast<TEffectIndex>(int32_t(index)), inShellContext);
}

void AAXHostShell::PlugIn::DestroyInstance(AAXHShell_TInstanceID inInstanceID)
{
	mInstances.erase(inInstanceID);
}

AAXHostShell::PlugIn::TTypeTriadMap AAXHostShell::PlugIn::GetTypes() const
{
	AAXHostShell::PlugIn::TTypeTriadMap typeMap;
	
	int32_t numTypes;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetNumberOfTypes(ID(), &numTypes);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetNumberOfTypes);
	
	for (int32_t i = 0; i < numTypes && (kAAXHShell_Success == aaxhShellResult); ++i)
	{
		TTypeIndex curIndex(i);
		AAX_SPlugInIdentifierTriad& curTriad = typeMap[curIndex];
		aaxhShellResult = AAXHShell_GetEffectTypeTriad(ID(), i, &curTriad.mManufacturerID, &curTriad.mProductID, &curTriad.mPlugInID);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectTypeTriad);
	}
	
	return typeMap;
}

AAXHostShell::PlugIn::TTypeTriadMap AAXHostShell::PlugIn::GetTypesForEffect(const std::string& inEffectID) const
{
	TTypeTriadMap typesForEffect;
	const TTypeTriadMap types = GetTypes();
	
	const int32_t cBufferSize = 512;
	char buffer[cBufferSize];
	
	for (TTypeTriadMap::const_iterator iter = types.begin(); iter != types.end(); ++iter)
	{
		const TTypeIndex& typeIndex = iter->first;
		const bool success = AAXHShell_GetEffectTypeEffectID(ID(), typeIndex, buffer, cBufferSize);
		if (true == success && inEffectID == std::string(buffer))
		{
			typesForEffect.insert(*iter);
		}
	}
	
	return typesForEffect;
}

AAXHostShell::PlugIn::TTypePlatformMap AAXHostShell::PlugIn::GetTypePlatforms() const
{
	AAXHostShell::PlugIn::TTypePlatformMap typeMap;
	
	int32_t numTypes;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetNumberOfTypes(ID(), &numTypes);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetNumberOfTypes);
	
	for (int32_t i = 0; i < numTypes && (kAAXHShell_Success == aaxhShellResult); ++i)
	{
		AAXHShell_TBoolean isAudioSuite = 0x0;
		aaxhShellResult = AAXHShell_GetEffectTypeIsAudioSuite(ID(), i, &isAudioSuite);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectTypeIsAudioSuite);
		
		AAXHShell_TBoolean isNative = 0x0;
		aaxhShellResult = AAXHShell_GetEffectTypeIsNative(ID(), i, &isNative);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectTypeIsNative);
		
		AAXHShell_TBoolean isTI = 0x0;
		aaxhShellResult = AAXHShell_GetEffectTypeIsTI(ID(), i, &isTI);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectTypeIsTI);
		
		AAXHostShell::PlugIn::TTypePlatformMap::value_type::second_type value;
		
		value.second = (0x0 == isAudioSuite); // second: online
		value.first = ((0x0 == isNative) ?
					   ((0x0 == isTI) ?
						kAAXHShell_Platform_None :
						kAAXHShell_Platform_TI
						) :
					   ((0x0 == isTI) ?
						kAAXHShell_Platform_Native :
						kAAXHShell_Platform_Any
						)
					   );
		
		typeMap[TTypeIndex(i)] = value;
	}
	
	return typeMap;
}

AAXHostShell::PlugIn::TPropertyPair AAXHostShell::PlugIn::GetPropertyForEffect(const TEffectIndex& inEffectIndex, AAX_EProperty inProperty) const
{
	AAXHShell_TBoolean propertyIsDefined = 0x0;
	AAX_CPropertyValue propertyValue = 0x0;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetEffectProperty(ID(), inEffectIndex, inProperty, &propertyIsDefined, &propertyValue);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectProperty);
	
	return std::make_pair(0x0 != propertyIsDefined, propertyValue);
}

AAXHostShell::PlugIn::TPropertyPair AAXHostShell::PlugIn::GetPropertyForType(const TTypeIndex& inTypeIndex, int64_t inProperty) const
{
	AAXHShell_TBoolean propertyIsDefined = 0x0;
	AAX_CPropertyValue propertyValue = 0x0;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetEffectTypeProperty(ID(), inTypeIndex, inProperty, &propertyIsDefined, &propertyValue);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectTypeProperty);
	
	return std::make_pair(0x0 != propertyIsDefined, propertyValue);
}

const AAXHostShell::PlugIn::TValidationResultVecMap& AAXHostShell::PlugIn::GetValidationResults() const
{
	if (true == mValidationResults.empty())
	{
		AAXHostShell::PlugIn::TValidationResultVecMap tempMap;
		
		int32_t resultType[] = {/*kAAXHShell_ResultType_Success, */kAAXHShell_ResultType_Warning, kAAXHShell_ResultType_Error};
		for (int i = 0; i < sizeof(resultType)/sizeof(int32_t); ++i)
		{
			if (0 < tempMap.count(resultType[i]))
			{
				throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Encountered a redundant result type key while initializing a validation result cache: %ld", (long int)resultType[i]);
			}
			
			// Add an empty vector to the map for this index
			tempMap[resultType[i]] = AAXHostShell::PlugIn::TValidationResultVecMap::mapped_type();
			
			int32_t numResults = 0, maxResultCStringSize = 0;
			AAXHShell_TResult aaxhShellResult = AAXHShell_GetDescriptionValidationInfo(ID(), resultType[i], &numResults, &maxResultCStringSize);
			AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetDescriptionValidationInfo);
			
			boost::scoped_array<char> idStringBuffer(new char[maxResultCStringSize]);
			for (int32_t resultIdx = 0; resultIdx < numResults; ++resultIdx)
			{
				std::pair<int32_t, std::string> resultInfoVec;
				aaxhShellResult = AAXHShell_GetDescriptionValidationMessage(ID(), resultType[i], resultIdx, &resultInfoVec.first, idStringBuffer.get(), maxResultCStringSize);
				AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetDescriptionValidationMessage);
				
				resultInfoVec.second = idStringBuffer.get();
				tempMap.at(resultType[i]).push_back(resultInfoVec);
			}
		}
		
		std::swap(mValidationResults, tempMap);
	}
	
	return mValidationResults;
}

void AAXHostShell::PlugIn::ValidateInternalData() const
{
	if (!IsValid())
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Attempting operation on invalid AAXHostShell::PlugIn at %p", this);
	}
}

void AAXHostShell::PlugIn::LoadPageTable(const TTypeIndex& inTypeIndex)
{
	const AAXHShell_TResult aaxhShellResult = AAXHShell_LoadPageTable(ID(), inTypeIndex);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_LoadPageTable);
}

bool AAXHostShell::PlugIn::IsPageTableLoaded(const TTypeIndex& inTypeIndex) const
{
	uint32_t result = 0;
	const AAXHShell_TResult aaxhShellResult = AAXHShell_IsPageTableLoaded(ID(), inTypeIndex, &result);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_IsPageTableLoaded);
	return result;
}

std::string AAXHostShell::PlugIn::GetPageTablesPath(const TTypeIndex& inTypeIndex) const
{
	std::string path;
	
	int32_t size = 0;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetPageTablesPathSize(ID(), inTypeIndex, &size);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetPageTablesPathSize);
	
	if (0 < size)
	{
		boost::scoped_array<char> buffer(new char[size]);
		aaxhShellResult = AAXHShell_GetPageTablesPath(ID(), inTypeIndex, size, buffer.get());
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetPageTablesPath);
		path = buffer.get();
	}
	
	return path;
}

bool AAXHostShell::PlugIn::HasCustomizedPageTable(const TTypeIndex& inTypeIndex, uint32_t inPageTableType, int32_t inPageTableSize) const
{
	uint32_t result = 0;
	const AAXHShell_TResult aaxhShellResult = AAXHShell_HasCustomizedPageTable(ID(), inTypeIndex, inPageTableType, inPageTableSize, &result);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_HasCustomizedPageTable);
	return result;
}
void AAXHostShell::PlugIn::GetPlugInControlForPage(const TTypeIndex& inTypeIndex, uint32_t inPageTableType, int32_t inPageTableSize,
							 int16_t inPageIndex, int16_t inControllerIndex, std::string& outControl)
{
	boost::scoped_array<char> buff(new char[64]);
	AAXHShell_GetPlugInControlForPage(ID(), inTypeIndex, inPageTableType, inPageTableSize,
									  inPageIndex, inControllerIndex, 64, buff.get());
	outControl = buff.get();
}

bool AAXHostShell::PlugIn::GetPageTableTypesForPlugInType(const TTypeIndex& inTypeIndex, std::vector<AAXHostShell::SPageTableType>& outPageTableTypes) const
{
	int32_t num = 0;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetNumSupportedPageTableTypes(ID(), inTypeIndex, &num);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetNumSupportedPageTableTypes);
	
	outPageTableTypes.clear();
	for (int32_t i = 0; i < num; ++i)
	{
		AAXHostShell::SPageTableType type;
		aaxhShellResult = AAXHShell_GetSupportedPageTableType(ID(), inTypeIndex, i, &type.mType, &type.mSize);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetSupportedPageTableType);
		outPageTableTypes.push_back(type);
	}
	
	return true;
}

bool AAXHostShell::PlugIn::GetControlsForPageTableType(const TTypeIndex& inTypeIndex,
													   uint32_t inPageTableType, int32_t inPageTableSize, std::vector<std::string>& outControls) const
{
	const size_t BUFFER_SIZE = 64;

	int32_t numPages = 0;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetNumPages(ID(), inTypeIndex, inPageTableType, inPageTableSize, &numPages);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetNumPages);

	
	int32_t numControls = 0;
	aaxhShellResult = AAXHShell_GetStandardNumControlsForPageTableType(inPageTableType, &numControls);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetStandardNumControlsForPageTableType);
	
	boost::scoped_array<char> controlsBuff(new char[BUFFER_SIZE]);
	for (int32_t pageIndex = 0; pageIndex < numPages; ++pageIndex)
	{
		for (int32_t controlIndex = 0; controlIndex < numControls; ++controlIndex)
		{
			aaxhShellResult = AAXHShell_GetControlForPageTableType(ID(), inTypeIndex, inPageTableType, inPageTableSize, pageIndex, controlIndex, BUFFER_SIZE, controlsBuff.get());
			AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetControlForPageTableType);
			
			outControls.push_back(controlsBuff.get());
			controlsBuff[0] = '\0';
		}
	}
	
	return true;
}

bool AAXHostShell::PlugIn::GetPageTableControlIdentifiers(const TTypeIndex& inTypeIndex, std::vector<std::string>* outControls) const
{
	if (inTypeIndex < 0 || !outControls)
		return false;
	
	int32_t numControls = 0;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetNumPageTableControlIdentifiers(ID(), inTypeIndex, &numControls);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetNumControlNamesForId);
	
	outControls->clear();
	
	const size_t BUFFER_SIZE = 64;
	boost::scoped_array<char> buffer(new char[BUFFER_SIZE]);
	for (int32_t i = 0; i < numControls; ++i)
	{
		buffer[0] = '\0';
		aaxhShellResult = AAXHShell_GetPageTableControlIdentifierByIndex(ID(), inTypeIndex, i, BUFFER_SIZE, buffer.get());
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetControlNameForIdByIndex);
		outControls->push_back(buffer.get());
	}
	
	return true;
}

std::multimap<uint32_t, int32_t> AAXHostShell::PlugIn::GetSupportedPageTables(const TTypeIndex& inTypeIndex) const
{
	std::multimap<uint32_t, int32_t> result;

    int32_t num = 0;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetNumSupportedPageTableTypes(ID(), inTypeIndex, &num);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetNumSupportedPageTableTypes);
	
	AAXHostShell::SPageTableType cursor;
	for (int32_t i = 0; i < num; ++i)
	{
		aaxhShellResult = AAXHShell_GetSupportedPageTableType(ID(), inTypeIndex, i, &cursor.mType, &cursor.mSize);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetSupportedPageTableType);
		
		result.insert(std::make_pair(cursor.mType, cursor.mSize));
	}
    
    return result;
}

uint32_t AAXHostShell::PlugIn::GetEffectCategoryBits(const TEffectIndex& inEffectIndex) const
{
	static const int32_t BUFFER_SIZE = 64;
	boost::scoped_array<char> buffer(new char[BUFFER_SIZE]);
	
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetEffectIDByIndex(ID(), inEffectIndex, buffer.get(), BUFFER_SIZE);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectIDByIndex);
	
	uint32_t bitmask = 0;
	aaxhShellResult = AAXHShell_GetEffectCategoryBits(ID(), buffer.get(), &bitmask);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectCategoryBits);
	
	return bitmask;
}

std::string AAXHostShell::PlugIn::GetCategoryBypassParameterID(const TEffectIndex& inEffectIndex, uint32_t inCategory) const
{
	static const int32_t BUFFER_SIZE = 64;
	boost::scoped_array<char> effectIDBuffer(new char[BUFFER_SIZE]);
	
	const AAXHShell_TResult aaxhShellResult = AAXHShell_GetEffectIDByIndex(ID(), inEffectIndex, effectIDBuffer.get(), BUFFER_SIZE);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectIDByIndex);
	
	boost::scoped_array<char> paramIDBuffer(new char[BUFFER_SIZE]);
	return AAXHShell_GetCategoryBypassParameterID(ID(), effectIDBuffer.get(), inCategory, BUFFER_SIZE, paramIDBuffer.get())
		 ? paramIDBuffer.get() : kAAXHShell_CategoryBypassParameterIDUnset;
}

bool AAXHostShell::PlugIn::GetEffectSupportsGainReductionMetering(const TEffectIndex& inEffectIndex) const
{
	AAXHShell_TBoolean itDoes = 0x0;
	const AAXHShell_TResult aaxhShellResult = AAXHShell_GetEffectSupportsGainReductionMetering(ID(), inEffectIndex, &itDoes);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetEffectSupportsGainReductionMetering);
	return (0x0 != itDoes);
}

//----------------------------------------------------------
#pragma mark - AAXHostShell::PlugIn::Scope Definition
//----------------------------------------------------------

AAXHostShell::PlugIn::Scope::Scope(const std::string& inFullPath)
: mPlugInIDDataID(kAAXHShell_IDUnset)
, mScopedCommandList()
{
	AAXHShell_TCommandListID commandListID = kAAXHShell_IDUnset;
	AAXHShell_TResult result = AAXHShell_CreateCommandList(&commandListID);
	if (kAAXHShell_Success == result)
	{
		// Allocate memory for parameters that will be passed along in the command list
		AAXHShell_TCommandListDataID plugInPathDataID = kAAXHShell_IDUnset;
		
		const size_t pathByteLength = inFullPath.length() + 1; // Include NULL terminating char
		assert((uint32_t)0xFFFFFFFF >= pathByteLength);
		const char* const pathCStr = inFullPath.c_str();
		
		result = result && AAXHShell_AddCommandListData(commandListID, 0, sizeof(AAXHShell_TPlugInID), &mPlugInIDDataID);
		result = result && AAXHShell_AddCommandListData(commandListID, pathCStr, (uint32_t)pathByteLength, &plugInPathDataID);
		
		// Construct the command list using the addresses of its internally-allocated data
		result = result && AAXHShell_PushCommandLoadPlugIn(commandListID, plugInPathDataID, mPlugInIDDataID);
	}
	
	// Execute the command list
	if (kAAXHShell_Success == result)
	{
		mScopedCommandList.Reset(commandListID, true);
		result = mScopedCommandList.IsValid();
	}
	
	if (kAAXHShell_Success != result)
	{
		Teardown();
	}
}

AAXHostShell::PlugIn::Scope::~Scope()
{
	Teardown();
}

bool AAXHostShell::PlugIn::Scope::IsActive() const
{
	return mScopedCommandList.IsValid();
}

AAXHShell_TPlugInID AAXHostShell::PlugIn::Scope::GetPlugInID() const
{
	AAXHShell_TPlugInID plugInID = kAAXHShell_IDUnset;
	AAXHShell_TResult result = AAXHShell_GetCommandListData(mScopedCommandList.ID(), mPlugInIDDataID, &plugInID, sizeof(AAXHShell_TPlugInID));
	return (kAAXHShell_Success == result) ? plugInID : kAAXHShell_IDUnset;
}

void AAXHostShell::PlugIn::Scope::Teardown()
{
	mScopedCommandList.Reset();
	mPlugInIDDataID = kAAXHShell_IDUnset;
}
