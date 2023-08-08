//
//
// Copyright 2014 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_Instance.cpp
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 2/19/14.
//
//

// Self Include
#include "AAXHostShell_Instance.h"

// AAXHostShell Includes
#include "AAXHostShell.h"

// AAXHostShell Client Utility Includes
#include "AAXHostShell_ScopedCommandList.h"
#include "AAXHostShell_Exceptions.h"
#include "AAXHostShell_StringUtils.h"
#include "AAXHostShell_Utilities.h"

// AAX Includes
#include "AAX_UtilsNative.h"
#include "AAX.h"

// Standard Includes
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <cassert>


//----------------------------------------------------------
#pragma mark - AAXHostShell::Instance::Scope Declaration
//----------------------------------------------------------

/**
 @brief This class encapsulates an effect instance scope
 
 @details
 
 
 @pre The AAXHostShell must be initialized throughout the lifetime of all
 \ref Scope objects
 */
class AAXHostShell::Instance::Scope
{
public:
	/**
	 @brief Instantiate an effect of a specific Type
	 
	 @param[in] inPlugInID
	 @param[in] inType
	 
	 @details
	 The effect's instantiation may fail if it does not include any types matching \c inType.
	 Check \ref IsActive() to determine whether the instance was successfully created.
	 */
	explicit /* constructor */		Scope(AAXHShell_TPlugInID inPlugInID, int32_t inType);
	/**
	 @brief Instantiate an effect for a host context
	 
	 @param[in] inPlugInID
	 @param[in] inEffectIndex
	 @param[in] inShellContext
	 Optional; if this parameter is NULL then the current shell context will be used
	 
	 @details
	 The effect's instantiation may fail if it does not include any types that are compatible
	 with the gien shell context. Check \ref IsActive() to determine whether the instance was
	 successfully created.
	 */
	explicit /* constructor */		Scope(AAXHShell_TPlugInID inPlugInID, int32_t inEffectIndex, const AAXHShell_SHostContext* inShellContext);
	
	/**
	 @brief Uninstantiate the effect
	 */
	/* non-virtual destructor */	~Scope();
	
public:
	bool IsActive() const;
	AAXHShell_TInstanceID GetInstanceID() const;
	
private:
	void Teardown();
	
	AAXHShell_TCommandListDataID mInstanceIDDataID;
	ScopedCommandList mScopedCommandList;
};



//----------------------------------------------------------
#pragma mark - AAXHostShell::Instance Definition
//----------------------------------------------------------

/* static */
AAXHostShell::Instance::SmartPtr AAXHostShell::Instance::CreateInstanceOfType(AAXHShell_TPlugInID inPlugInID, int32_t inType)
{
	AAXHostShell::Instance::SmartPtr instance(new AAXHostShell::Instance(inPlugInID, inType));
	return instance->IsValid() ? instance : nullptr;
}

/* static */
AAXHostShell::Instance::SmartPtr AAXHostShell::Instance::CreateInstanceOfEffect(AAXHShell_TPlugInID inPlugInID, const TEffectIndex& inEffectIndex)
{
	AAXHostShell::Instance::SmartPtr instance(new AAXHostShell::Instance(inPlugInID, inEffectIndex, nullptr));
	return instance->IsValid() ? instance : nullptr;
}

/* static */
AAXHostShell::Instance::SmartPtr AAXHostShell::Instance::CreateInstanceOfEffectForContext(AAXHShell_TPlugInID inPlugInID, const TEffectIndex& inEffectIndex, const AAXHShell_SHostContext& inShellContext)
{
	AAXHostShell::Instance::SmartPtr instance(new AAXHostShell::Instance(inPlugInID, inEffectIndex, &inShellContext));
	return instance->IsValid() ? instance : nullptr;
}

AAXHostShell::Instance::Instance(AAXHShell_TPlugInID inPlugInID, int32_t inIndex)
: mSelf(new AAXHostShell::Instance::Scope(inPlugInID, inIndex))
, mPlugInID(inPlugInID)
{
	ValidateInternalData();
}

AAXHostShell::Instance::Instance(AAXHShell_TPlugInID inPlugInID, const TEffectIndex& inEffectIndex, const AAXHShell_SHostContext* inShellContext)
: mSelf(new AAXHostShell::Instance::Scope(inPlugInID, inEffectIndex, inShellContext))
, mPlugInID(inPlugInID)
{
	ValidateInternalData();
}

AAXHostShell::Instance::~Instance()
{
}

bool AAXHostShell::Instance::IsValid() const
{
	return (NULL != mSelf) && (mSelf->IsActive());
}

AAXHShell_TInstanceID AAXHostShell::Instance::ID() const
{
	ValidateInternalData();
	return mSelf->GetInstanceID();
}

AAXHShell_TPlugInID AAXHostShell::Instance::GetPlugInID() const
{
	return mPlugInID;
}

std::string AAXHostShell::Instance::GetEffectID() const
{
	ValidateInternalData();
	
	static const int32_t kBufferSize = 1024;
	char idCString[kBufferSize];
	
	const AAXHShell_TResult aaxhShellResult = AAXHShell_GetInstanceEffectID(mPlugInID, ID(), idCString, kBufferSize);
	AAXHOSTSHELL_THROW_FAIL(aaxhShellResult, "Encountered unknown failure while retrieving effect IDs");
	
	return std::string(idCString);
}

bool AAXHostShell::Instance::GetIsAudioSuite() const
{
    ValidateInternalData();
    AAXHShell_TBoolean isAudioSuite;
    const AAXHShell_TResult aaxShellResult = AAXHShell_GetEffectTypeIsAudioSuite(mPlugInID, mSelf->GetInstanceID(), &isAudioSuite); 
    AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetEffectTypeIsAudioSuite);

    return (bool)isAudioSuite;
}

int32_t AAXHostShell::Instance::GetNumParameters() const
{
	ValidateInternalData();
	
	int32_t numParameters;
	const AAXHShell_TResult aaxhShellResult = AAXHShell_GetNumberOfParameters(mPlugInID, ID(), &numParameters);
	AAXHOSTSHELL_THROW_FAIL(aaxhShellResult, "Encountered unknown failure while retrieving number of effect parameters");
	
	return numParameters;
}

const AAXHostShell::Instance::ParameterMap& AAXHostShell::Instance::GetAllParameters()
{
	ValidateInternalData();
	
	// NOTE: like most of the functions in these utility classes, this is NOT designed for concurrent clients
	if (true == mParameters.empty())
	{
		// Initialize mParameters with all parameters in the instance
		const int32_t numParameters = GetNumParameters();
		mParameters.clear();
		try
		{
			for (int32_t i = 0; i < numParameters; ++i)
			{
				Parameter param(shared_from_this(), i);
				const std::string paramID = param.GetID();
				assert(0 == mParameters.count(paramID));
				mParameters[paramID] = param;
			}
		}
		catch (std::bad_weak_ptr)
		{
			// This exception will be thrown by shared_from_this() if no
			// shared pointer to the Instance currently exists. In this
			// situation there is not much we can do except return an
			// empty map. However, this should never happen since the
			// Instance should have been called from a shared_ptr.
			mParameters.clear();
		}
	}
	
	return mParameters;
}

const AAXHostShell::Instance::Parameter& AAXHostShell::Instance::GetParameter(const std::string &inID) const
{
	ValidateInternalData();
	
	// const_cast is OK here since a) the reason we don't have a non-const version of
	// GetAllParameters() is that it is difficult to convert an STL container of
	// non-const objects to an STL container of const objects, and b) we are casting
	// the fetched object right back to const, so this method basically does what a
	// const version of GetAllParameters() would do anyway.
	return const_cast<Instance*>(this)->GetAllParameters().at(inID);
}

AAXHostShell::Instance::Parameter AAXHostShell::Instance::GetParameter(const std::string &inID)
{
	ValidateInternalData();
	return GetAllParameters().at(inID);
}

bool AAXHostShell::Instance::GetEUCONCurveDataIsSupported()
{
	bool isEQValidated = IsEffectInCategory(AAX_ePlugInCategory_EQ) ? IsCurveDataSupported(AAX_eCurveType_EQ) : true;
	bool isDynamicsValidated = IsEffectInCategory(AAX_ePlugInCategory_Dynamics)
	? (IsCurveDataSupported(AAX_eCurveType_Dynamics) && IsCurveDataSupported(AAX_eCurveType_Reduction))
	: true;
	return isEQValidated && isDynamicsValidated;
}

bool AAXHostShell::Instance::IsEffectInCategory(uint32_t inCategory)
{
	uint32_t categoryBits = 0;
	const AAXHShell_TResult aaxhShellResult = AAXHShell_GetEffectCategoryBits(mPlugInID, GetEffectID().c_str(), &categoryBits);
	AAXHOSTSHELL_THROW_FAIL(aaxhShellResult, "Failed to get effect category bits");
	
	return (categoryBits & inCategory);
}

bool AAXHostShell::Instance::IsCurveDataSupported(AAX_ECurveType inCurveType)
{
	CurveData curveData(shared_from_this(), inCurveType);
	return curveData.IsSupported();
}

bool AAXHostShell::Instance::GetMeterIds(uint32_t* outXMeterId, uint32_t* outYMeterId)
{
	if (!outXMeterId || !outYMeterId)
		return false;
	
	CurveData curveData(shared_from_this(), AAX_eCurveType_Dynamics);
	return (curveData.GetMeterIds(outXMeterId, outYMeterId) == AAX_SUCCESS);
}

void AAXHostShell::Instance::ValidateInternalData() const
{
	if (false == IsValid())
	{
		throw(AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Attempting operation on invalid AAXHostShell::PlugIn"));
	}
}


//----------------------------------------------------------
#pragma mark - AAXHostShell::Instance::Parameter Definition
//----------------------------------------------------------

static int32_t PV_GetParamterIndexForID(AAXHShell_TPlugInID inPlugInID, AAXHShell_TInstanceID inInstanceID, const char* inParameterID)
{
	int32_t numParams;
	AAXHShell_TResult aaxShellResult = kAAXHShell_Fail;
	
	aaxShellResult = AAXHShell_GetNumberOfParameters(inPlugInID, inInstanceID, &numParams);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetNumberOfParameters);
	
	int32_t paramIndex = 0;
	bool foundParamIndex = false;
	while ((false == foundParamIndex) && (paramIndex < numParams))
	{
		static const int32_t sBufferSize = 256;
		char buffer[sBufferSize];
		aaxShellResult = AAXHShell_GetParameterID(inPlugInID, inInstanceID, paramIndex, buffer, sBufferSize);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetParameterID);
		
		if (0x0 != AAX::IsParameterIDEqual(buffer, inParameterID))
		{
			foundParamIndex = true;
		}
		else
		{
			++paramIndex;
		}
	}
	
	if (false == foundParamIndex)
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Could not find a parameter with the specified ID: %s", inParameterID ? inParameterID : "NULL");
	}
	
	return paramIndex;
}

struct AAXHostShell::Instance::Parameter::StTouch
{
	~StTouch()
	{
		const AAXHShell_TResult aaxShellResult = AAXHShell_ReleaseParameter(mPlugInID, mInstanceID, mParameterIndex);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_ReleaseParameter);
	}
	
	explicit StTouch(const Parameter& inParameter)
	: mPlugInID(inParameter.mInstance->mPlugInID)
	, mInstanceID(inParameter.mInstance->ID())
	, mParameterIndex(inParameter.mIndex)
	{
		const AAXHShell_TResult aaxShellResult = AAXHShell_TouchParameter(mPlugInID, mInstanceID, mParameterIndex);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_TouchParameter);
	}
	
private:
	AAXHShell_TPlugInID mPlugInID;
	AAXHShell_TInstanceID mInstanceID;
	int32_t mParameterIndex;
};

AAXHostShell::Instance::Parameter::~Parameter()
{
}

AAXHostShell::Instance::Parameter::Parameter(Instance::SmartPtr inInstance, const char* inID)
: mInstance(inInstance)
, mIndex(PV_GetParamterIndexForID(mInstance->mPlugInID, mInstance->ID(), inID))
{
	ValidateInternalData();
}

AAXHostShell::Instance::Parameter::Parameter(Instance::SmartPtr inInstance, int32_t inIndex)
: mInstance(inInstance)
, mIndex(inIndex)
{
	ValidateInternalData();
}

std::string AAXHostShell::Instance::Parameter::GetName(size_t inMaxLength) const
{
	ValidateInternalData();
	
	AAXHostShell::CString buffer(inMaxLength);
	const AAXHShell_TResult aaxShellResult = AAXHShell_GetParameterNameOfLength(mInstance->mPlugInID, mInstance->ID(), mIndex, (int32_t)std::min<size_t>(INT32_MAX, buffer.Size()), buffer.Get());
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetParameterName);
	
	return buffer.AsStr();
}

std::string AAXHostShell::Instance::Parameter::GetID() const
{
	ValidateInternalData();
	
	AAXHostShell::CString buffer(64);
	const AAXHShell_TResult aaxShellResult = AAXHShell_GetParameterID(mInstance->mPlugInID, mInstance->ID(), mIndex, buffer.Get(), (int32_t)std::min<size_t>(INT32_MAX, buffer.Size()));
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetParameterID);
	
	return buffer.AsStr();
}

AAX_EParameterType AAXHostShell::Instance::Parameter::GetType() const
{
	ValidateInternalData();
	
	int32_t type;
	const AAXHShell_TResult aaxShellResult = AAXHShell_GetParameterType(mInstance->mPlugInID, mInstance->ID(), mIndex, &type);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetParameterType);
	
	return static_cast<AAX_EParameterType>(type);
}

bool AAXHostShell::Instance::Parameter::IsAutomatable() const
{
	ValidateInternalData();
	
	AAXHShell_TBoolean automatable;
	const AAXHShell_TResult aaxShellResult = AAXHShell_GetParameterIsAutomatable(mInstance->mPlugInID, mInstance->ID(), mIndex, &automatable);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetParameterIsAutomatable);
	
	return (0x0 != automatable);
}

int32_t AAXHostShell::Instance::Parameter::GetNumSteps() const
{
	ValidateInternalData();
	
	int32_t numSteps;
	const AAXHShell_TResult aaxShellResult = AAXHShell_GetParameterNumOfSteps(mInstance->mPlugInID, mInstance->ID(), mIndex, &numSteps);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetParameterNumOfSteps);
	
	return numSteps;
}

std::string AAXHostShell::Instance::Parameter::GetValueStr() const
{
	ValidateInternalData();
	
	AAXHostShell::CString buffer(64);
	const AAXHShell_TResult aaxShellResult = AAXHShell_GetParameterValueString(mInstance->mPlugInID, mInstance->ID(), mIndex, buffer.Get(), (int32_t)std::min<size_t>(INT32_MAX, buffer.Size()));
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetParameterValueString);
	
	return buffer.AsStr();
}

void AAXHostShell::Instance::Parameter::SetValue(const std::string& inValueString)
{
	ValidateInternalData();
	
	const AAXHostShell::CString buffer(inValueString);
	
	{
		StTouch touch(*this);
		const AAXHShell_TResult aaxShellResult = AAXHShell_SetParameterValueFromString(mInstance->mPlugInID, mInstance->ID(), mIndex, buffer.Get());
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_SetParameterValueFromString);
	}
}

double AAXHostShell::Instance::Parameter::GetNormalizedValue() const
{
	ValidateInternalData();
	
	double normalizedValue;
	const AAXHShell_TResult aaxShellResult = AAXHShell_GetParameterNormalizedValue(mInstance->mPlugInID, mInstance->ID(), mIndex, &normalizedValue);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetParameterNormalizedValue);
	
	return normalizedValue;
}

void AAXHostShell::Instance::Parameter::SetNormalizedValue(double inValue)
{
	ValidateInternalData();
	
	{
		StTouch touch(*this);
		const AAXHShell_TResult aaxShellResult = AAXHShell_SetParameterNormalizedValue(mInstance->mPlugInID, mInstance->ID(), mIndex, inValue);
		AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_SetParameterNormalizedValue);
	}
}

void AAXHostShell::Instance::Parameter::ValidateInternalData() const
{
	if (!mInstance)
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Attempting operation on NULL AAXHostShell::Instance. Parameter: %p", this);
	}
	
	mInstance->ValidateInternalData();
}

//----------------------------------------------------------
#pragma mark - AAXHostShell::Instance::CurveData Definition
//----------------------------------------------------------

static std::vector<float> PV_GetCurveInputTaper(AAX_ECurveType inCurveType, uint32_t inNumValues, float inStartVal, float inEndVal)
{
	std::vector<float> resultVec;
	
	switch (inCurveType)
	{
		case AAX_eCurveType_EQ:
		{
			double minLog = log(inStartVal);
			double maxLog = log(inEndVal);
			for (uint32_t i = 0; i < inNumValues; ++i)
			{
				const double normalizedValue = double(i)/inNumValues;
				double doubleRealValue = exp(normalizedValue * (maxLog - minLog) + minLog);
				
				if (doubleRealValue < inStartVal)
					doubleRealValue = inStartVal;
				if (doubleRealValue > inEndVal)
					doubleRealValue = inEndVal;
				
				resultVec.push_back((float)doubleRealValue);
			}
			
			break;
		}
		case AAX_eCurveType_Dynamics:
		case AAX_eCurveType_Reduction:
		{
			const float incr = (0 < inNumValues) ? (inEndVal - inStartVal) / inNumValues : 0;
			for (uint32_t i = 0; i < inNumValues; ++i)
			{
				resultVec.push_back(inStartVal + i*incr);
			}
			
			break;
		}
		default:
		{
			throw AAXHostShell::Exception::InvalidParameter::Format(__FUNCTION__, "Encountered an unexpected AAX_ECurveType: %lu", (long unsigned int)inCurveType);
		}
	}
	
	return resultVec;
}

static void PV_InitializeCurveValueArrays(AAX_ECurveType inCurveType, uint32_t inNumValues, const AAX_Rect& inCustomDisplayRange, float* ioXValues, float* ioYValues)
{
	float xMin = inCustomDisplayRange.left;
	float xMax = xMin + inCustomDisplayRange.width;
	
	std::vector<float> taper;
	switch (inCurveType)
	{
		case AAX_eCurveType_EQ:
		{
			if (AAX_Rect() == inCustomDisplayRange)
			{
				xMin = 20.0f;
				xMax = 20000.0f;
			}
			
			taper = PV_GetCurveInputTaper(inCurveType, inNumValues, xMin, xMax);
			break;
		}
		case AAX_eCurveType_Dynamics:
		case AAX_eCurveType_Reduction:
		{
			if (AAX_Rect() == inCustomDisplayRange)
			{
				xMin = -80.0f;
				xMax = 0.0f;
			}
			
			taper = PV_GetCurveInputTaper(inCurveType, inNumValues, xMin, xMax);
			break;
		}
		default:
		{
			throw AAXHostShell::Exception::InvalidParameter::Format(__FUNCTION__, "Encountered an unexpected AAX_ECurveType: %lu", (long unsigned int)inCurveType);
		}
	}
	
	for (uint32_t i = 0; i < inNumValues; ++i)
	{
		ioXValues[i] = taper.at(i);
		ioYValues[i] = 0.0f;
	}
}

AAXHostShell::Instance::CurveData::CurveData(Instance::SmartPtr inInstance, AAX_ECurveType inCurveType, uint32_t inNumValues)
: mInstance(inInstance)
, mCurveType(inCurveType)
, mNumValues(inNumValues)
, mXValues(new float[inNumValues])
, mYValues(new float[inNumValues])
{
	InitializeValues();
	ValidateInternalData();
}

AAXHostShell::Instance::CurveData::CurveData(Instance::SmartPtr inInstance, AAX_ECurveType inCurveType)
: mInstance(inInstance)
, mCurveType(inCurveType)
, mNumValues(160)
, mXValues(new float[160])
, mYValues(new float[160])
{
	InitializeValues();
	ValidateInternalData();
}

AAXHostShell::Instance::CurveData::~CurveData()
{
	delete[] mXValues;
	delete[] mYValues;
}

std::vector<AAX_Point> AAXHostShell::Instance::CurveData::GetValues() const
{
	ValidateInternalData();
	
	AAX_Result aaxResult = AAX_SUCCESS;
	const AAXHShell_TResult aaxShellResult = AAXHShell_GetInstanceCurveData(mInstance->mPlugInID, mInstance->ID(), mCurveType, mXValues, mNumValues, &aaxResult, mYValues);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxShellResult, AAXHShell_GetInstanceCurveData);
	
	std::vector<AAX_Point> resultVec;
	if (AAX_SUCCESS == aaxResult)
	{
		for (uint32_t i = 0; i < mNumValues; ++i)
		{
			resultVec.push_back(AAX_Point(mYValues[i], mXValues[i]));
		}
	}
	else if (AAX_ERROR_UNIMPLEMENTED == aaxResult)
	{
		// Return empty result vector
	}
	else
	{
		throw AAXHostShell::Exception::AAXResultError::Format(__FUNCTION__, "AAXHShell_GetInstanceCurveData", aaxResult);
	}
	
	return resultVec;
}

AAX_Result AAXHostShell::Instance::CurveData::GetMeterIds(uint32_t* outXMeterId, uint32_t* outYMeterId) const
{
	ValidateInternalData();
	AAX_Result aaxResult = AAX_SUCCESS;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetInstanceCurveDataMeterIds(mInstance->mPlugInID, mInstance->ID(), mCurveType, &aaxResult, outXMeterId, outYMeterId);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetInstanceCurveDataMeterIds);
	
	return aaxResult;
}

AAX_Rect AAXHostShell::Instance::CurveData::GetDisplayRange() const
{
	ValidateInternalData();
	AAX_Rect resultRect;
	
	float xMin, xMax, yMin, yMax;
	AAX_Result aaxResult = AAX_SUCCESS;
	AAXHShell_TResult aaxhShellResult = AAXHShell_GetInstanceCurveDataDisplayRange(mInstance->mPlugInID, mInstance->ID(), mCurveType, &aaxResult, &xMin, &xMax, &yMin, &yMax);
	AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(aaxhShellResult, AAXHShell_GetInstanceCurveDataDisplayRange);
	
	resultRect = AAX_Rect();
	if (AAX_SUCCESS == aaxResult)
	{
		resultRect.top = yMax;
		resultRect.left = xMin;
		resultRect.height = yMax - yMin;
		resultRect.width = xMax - xMin;
	}
	else if (AAX_ERROR_UNIMPLEMENTED == aaxResult)
	{
		// Return default AAX_Rect
	}
	else
	{
		throw AAXHostShell::Exception::AAXResultError::Format(__FUNCTION__, "AAXHShell_GetInstanceCurveDataDisplayRange", aaxResult);
	}
	
	return resultRect;
}

std::string AAXHostShell::Instance::CurveData::AsASCIIArt(const std::vector<AAX_Point>& inValues, const AAX_Rect& inCustomDisplayRange)
{
	if (inValues.empty())
		return std::string();
	
	const size_t cNumRows = 20;
	const size_t cNumColumns = inValues.size() / 2;
	
	float minX = inCustomDisplayRange.left;
	float maxX = minX + inCustomDisplayRange.width;
	float maxY = inCustomDisplayRange.top;
	float minY = maxY - inCustomDisplayRange.height;
	const bool useValueLimitsForDisplayRange = (AAX_Rect() == inCustomDisplayRange);
	
	std::vector<float> sortedXs;
	for (std::vector<AAX_Point>::const_iterator iter = inValues.begin(); iter != inValues.end(); ++iter)
	{
		if (true == useValueLimitsForDisplayRange)
		{
			minX = std::min(minX, iter->horz);
			maxX = std::max(maxX, iter->horz);
			minY = std::min(minY, iter->vert);
			maxY = std::max(maxY, iter->vert);
		}
		
		sortedXs.push_back(iter->horz);
	}
	std::sort(sortedXs.begin(), sortedXs.end());
	const float rowSize = (maxY - minY) / cNumRows;
	
	std::string emptyRow(cNumColumns, ' ');
	std::vector<std::string> rows(cNumRows, emptyRow);
	for (std::vector<AAX_Point>::const_iterator iter = inValues.begin(); iter != inValues.end(); ++iter)
	{
		const AAX_Point shiftedPoint(iter->vert - minY, iter->horz - minX);
		const int rowIdx = std::max<int>(0, std::min<int>(cNumRows-1, (0 != rowSize) ? (int)floor(shiftedPoint.vert / rowSize) : 0));
		
		// Find the appropriate column (input values may use a non-linear taper)
		int xIdx = 0;
		for (; (iter->horz > sortedXs.at(xIdx)) && (xIdx < sortedXs.size()); ++xIdx);
		const int columnIdx = std::max<int>(0, std::min<int>((int)cNumColumns-1, (int)(xIdx * (double(cNumColumns)/double(sortedXs.size())))));
		
		rows.at(rowIdx).at(columnIdx) = '*';
	}
	
	std::stringstream resultStream;
	const std::streamsize defaultWidth = resultStream.width();
	const std::streamsize leftWidth = 8;
	resultStream.setf(std::ios::right);
	resultStream.precision(2);
	
	// Rows are in lowest-to-highest order, so reverse_iterator is what we want in order to draw from top to bottom
	for (std::vector<std::string>::const_reverse_iterator iter = rows.rbegin(); iter != rows.rend(); ++iter)
	{
		// Header column
		if (rows.rbegin() == iter)
		{
			resultStream << std::setw(leftWidth-1) << maxY;
			resultStream << std::setw(1) << "|";
		}
		else if ((rows.rend()-1) == iter)
		{
			resultStream << std::setw(leftWidth-1) << minY;
			resultStream << std::setw(1) << "|";
		}
		else
		{
			resultStream << std::setw(leftWidth) << "|";
		}
		
		// Graph row
		resultStream << std::setw((int)defaultWidth) << *iter << std::endl;
	}
	
	{
		resultStream << std::string(cNumColumns+leftWidth, '-') << std::endl;
		resultStream << std::setw(leftWidth) << "|" << std::setw(4) << minX << std::string(cNumColumns-8, ' ') << std::setw(4) << maxX << std::endl;
	}
	
	return resultStream.str();
}

void AAXHostShell::Instance::CurveData::InitializeValues()
{
	const AAX_Rect customDisplayRange = GetDisplayRange();
	PV_InitializeCurveValueArrays(mCurveType, mNumValues, customDisplayRange, mXValues, mYValues);
}

void AAXHostShell::Instance::CurveData::ValidateInternalData() const
{
	if (!mInstance)
	{
		throw AAXHostShell::Exception::InternalError::Format(__FUNCTION__, "Attempting operation on NULL AAXHostShell::Instance. CurveData: %p", this);
	}
	
	mInstance->ValidateInternalData();
}


//----------------------------------------------------------
#pragma mark - AAXHostShell::Instance::Scope Definition
//----------------------------------------------------------

AAXHostShell::Instance::Scope::Scope(AAXHShell_TPlugInID inPlugInID, int32_t inIndex)
: mInstanceIDDataID(kAAXHShell_IDUnset)
, mScopedCommandList()
{
	AAXHShell_TCommandListID commandListID = kAAXHShell_IDUnset;
	AAXHShell_TResult result = AAXHShell_CreateCommandList(&commandListID);
	if (kAAXHShell_Success == result)
	{
		// Allocate memory for parameters that will be passed along in the command list
		AAXHShell_TCommandListDataID plugInIDDataID = kAAXHShell_IDUnset;
		AAXHShell_TCommandListDataID effectIndexIDDataID = kAAXHShell_IDUnset;
		
		result = result && AAXHShell_AddCommandListData(commandListID, &inPlugInID, sizeof(AAXHShell_TPlugInID), &plugInIDDataID);
		result = result && AAXHShell_AddCommandListData(commandListID, &inIndex, sizeof(int32_t), &effectIndexIDDataID);
		result = result && AAXHShell_AddCommandListData(commandListID, 0, sizeof(AAXHShell_TInstanceID), &mInstanceIDDataID);
		
		// Construct the command list using the addresses of its internally-allocated data
		result = result && AAXHShell_PushCommandInstantiateEffectByTypeIndex(commandListID, plugInIDDataID, effectIndexIDDataID, mInstanceIDDataID);
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

AAXHostShell::Instance::Scope::Scope(AAXHShell_TPlugInID inPlugInID, int32_t inEffectIndex, const AAXHShell_SHostContext* inShellContext)
: mInstanceIDDataID(kAAXHShell_IDUnset)
, mScopedCommandList()
{
	if (false == AAXHostShell::HostContextPtrIsValid(inShellContext))
	{
		throw AAXHostShell::Exception::InvalidParameter::Format(__FUNCTION__, "Encountered unsupported host context: %s", inShellContext ? AAXHostShell::AsString(*inShellContext).c_str() : "NULL");
	}
	
	AAXHShell_TCommandListID commandListID = kAAXHShell_IDUnset;
	AAXHShell_TResult result = AAXHShell_CreateCommandList(&commandListID);
	if (kAAXHShell_Success == result)
	{
		// Allocate memory for parameters that will be passed along in the command list
		AAXHShell_TCommandListDataID plugInIDDataID = kAAXHShell_IDUnset;
		AAXHShell_TCommandListDataID effectIndexIDDataID = kAAXHShell_IDUnset;
		
		result = result && AAXHShell_AddCommandListData(commandListID, &inPlugInID, sizeof(AAXHShell_TPlugInID), &plugInIDDataID);
		result = result && AAXHShell_AddCommandListData(commandListID, &inEffectIndex, sizeof(int32_t), &effectIndexIDDataID);
		result = result && AAXHShell_AddCommandListData(commandListID, 0, sizeof(AAXHShell_TInstanceID), &mInstanceIDDataID);
		
		// Construct the command list using the addresses of its internally-allocated data
		if (nullptr == inShellContext)
		{
			result = result && AAXHShell_PushCommandInstantiateEffect(commandListID, plugInIDDataID, effectIndexIDDataID, mInstanceIDDataID);
		}
		else
		{
			AAXHShell_TCommandListDataID shellContextDataID = kAAXHShell_IDUnset;
			result = result && AAXHShell_AddCommandListData(commandListID, inShellContext, sizeof(AAXHShell_SHostContext), &shellContextDataID);
			result = result && AAXHShell_PushCommandInstantiateEffectForContext(commandListID, plugInIDDataID, effectIndexIDDataID, shellContextDataID, mInstanceIDDataID);
		}
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

AAXHostShell::Instance::Scope::~Scope()
{
	Teardown();
}

bool AAXHostShell::Instance::Scope::IsActive() const
{
	return mScopedCommandList.IsValid();
}

AAXHShell_TInstanceID AAXHostShell::Instance::Scope::GetInstanceID() const
{
	AAXHShell_TInstanceID instanceID = kAAXHShell_IDUnset;
	AAXHShell_TResult result = AAXHShell_GetCommandListData(mScopedCommandList.ID(), mInstanceIDDataID, &instanceID, sizeof(AAXHShell_TInstanceID));
	return (kAAXHShell_Success == result) ? instanceID : kAAXHShell_IDUnset;
}

void AAXHostShell::Instance::Scope::Teardown()
{
	mScopedCommandList.Reset();
	mInstanceIDDataID = kAAXHShell_IDUnset;
}
