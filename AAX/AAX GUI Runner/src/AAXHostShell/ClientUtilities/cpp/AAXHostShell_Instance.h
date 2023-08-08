//
//
// Copyright 2014 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_Instance.h
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 2/19/14.
//
//

#ifndef __AAXHostShell_CppUtilities_Instance__
#define __AAXHostShell_CppUtilities_Instance__

// AAXHostShell Include
#include "AAXHostShell_Types.h"
#include "AAXHostShell_StrongTypeDefs.h"

// AAX Includes
#include "AAX_Enums.h"
#include "AAX_GUITypes.h"
#include "AAX.h"

// Standard Includes
#include <memory>
#include <string>
#include <vector>
#include <map>


namespace AAXHostShell
{
	/**
	 @brief Utility class representing an effect instance
	 
	 @details
	 - The instance is deleted when its Instance object is destroyed.
	 - In general, the methods of this class will throw on failure and
	 are guaranteed not to return invalid data unless noted otherwise.
	 
	 @ingroup AAXHostShell_CppUtilities
	 */
	class Instance : public std::enable_shared_from_this<Instance>
	{
		// ---------------------------------------------------------------
		// Instance Interface
		// ---------------------------------------------------------------
	public:
		typedef std::shared_ptr<Instance> SmartPtr;
		typedef std::shared_ptr<const Instance> SmartPtrConst;
		
	public:
		/* non-virtual destructor */	~Instance();
		
		static SmartPtr CreateInstanceOfType(AAXHShell_TPlugInID inPlugInID, int32_t inType); // Create an instance of a specific type
		static SmartPtr CreateInstanceOfEffect(AAXHShell_TPlugInID inPlugInID, const TEffectIndex& inEffectIndex); // Create an instance using the current shell context
		static SmartPtr CreateInstanceOfEffectForContext(AAXHShell_TPlugInID inPlugInID, const TEffectIndex& inEffectIndex, const AAXHShell_SHostContext& inShellContext); // Create an instance using a specific shell context
		
	private:
		explicit /* constructor */		Instance(AAXHShell_TPlugInID inPlugInID, int32_t inIndex);
		explicit /* constructor */		Instance(AAXHShell_TPlugInID inPlugInID, const TEffectIndex& inEffectIndex, const AAXHShell_SHostContext* inShellContext);
		
	public:
		bool IsValid() const;
		AAXHShell_TInstanceID ID() const;
		AAXHShell_TPlugInID GetPlugInID() const;
		std::string GetEffectID() const;
        bool   GetIsAudioSuite() const; ///< \sa AAXHostShell::PlugIn::GetTypeIsAudioSuite
		
	public:
		struct Parameter
		{
		public:
			~Parameter();
			
#if AAXHOSTSHELL_USE_CPP11
			Parameter() = default; // Included for STL container compatibility
#else
			Parameter() : mInstance(), mIndex() {} // Included for STL container compatibility
#endif
			explicit Parameter(Instance::SmartPtr inInstance, const char* inID);
			explicit Parameter(Instance::SmartPtr inInstance, int32_t inIndex);
			
#if AAXHOSTSHELL_USE_CPP11
			Parameter(const Parameter& inOther) = default;
			Parameter(Parameter&& inOther) = default;
			
			Parameter& operator=(const Parameter& inOther) = default;
			Parameter& operator=(Parameter&& inOther) = default;
#endif
			
			/** \brief Get the parameter's abbreviated name of a given maximum length
			 
			 \param[in] inMaxLength
			 The maximum length of the parameter name, including the NULL terminating char
			 */
			std::string GetName(size_t inMaxLength = 32) const;
			std::string GetID() const;
			AAX_EParameterType GetType() const;
			bool IsAutomatable() const;
			int32_t GetNumSteps() const;

			std::string GetValueStr() const;
			void SetValue(const std::string& inValueString); // Automatically performs touch/release
			double GetNormalizedValue() const;
			void SetNormalizedValue(double inValue);
			
		private:
			void ValidateInternalData() const;
			
		private:
			struct StTouch;
			
			Instance::SmartPtrConst mInstance;
			int32_t mIndex;
		};
		
		typedef std::map<std::string, Parameter> ParameterMap;
		
		int32_t GetNumParameters() const;
		// The following methods MUST be called from a SmartPtr, at least on the first call. This is guaranteed
		// by the factory function mechanism, which guarantees that clients always generate a SmartPtr.
		const ParameterMap& GetAllParameters();
		const Parameter& GetParameter(const std::string& inID) const;
		Parameter GetParameter(const std::string& inID);
		// End of functions which MUST be called from a SmartPtr
		
	public:
		struct CurveData
		{
		public:
			~CurveData();
			explicit CurveData(Instance::SmartPtr inInstance, AAX_ECurveType inCurveType, uint32_t inNumValues);
			explicit CurveData(Instance::SmartPtr inInstance, AAX_ECurveType inCurveType);
			
			AAX_ECurveType GetCurveType() const { return mCurveType; }
			bool IsSupported() const { return !GetValues().empty(); }
			std::vector<AAX_Point> GetValues() const;
			AAX_Result GetMeterIds(uint32_t* outXMeterId, uint32_t* outYMeterId) const;
			AAX_Rect GetDisplayRange() const;
			
			static std::string AsASCIIArt(const std::vector<AAX_Point>& inValues, const AAX_Rect& inCustomDisplayRange = AAX_Rect());
			
		private:
			void InitializeValues();
			void ValidateInternalData() const;
			
		private:
			Instance::SmartPtr mInstance;
			AAX_ECurveType mCurveType;
			uint32_t mNumValues;
			float* const mXValues;
			float* const mYValues;
		};
		
		/**	Returns true only if EUCON curve types are supported for all categories
		 *	defined by the plug-in.
		 */
		bool GetEUCONCurveDataIsSupported();
		
		bool IsEffectInCategory(uint32_t inCategory);
		bool IsCurveDataSupported(AAX_ECurveType inCurveType);
		
		bool GetMeterIds(uint32_t* outXMeterId, uint32_t* outYMeterId);
		
		// ---------------------------------------------------------------
		// Instance Internal Data
		// ---------------------------------------------------------------
	private:
		void ValidateInternalData() const;
		
	private:
		class Scope;
		
		const std::unique_ptr<Scope> mSelf;
		const AAXHShell_TPlugInID mPlugInID;
		ParameterMap mParameters; // Lazily initialized. Never access directly (e.g. use GetAllParameters())
	};
}

#endif /* defined(__AAXHostShell_CppUtilities_Instance__) */
