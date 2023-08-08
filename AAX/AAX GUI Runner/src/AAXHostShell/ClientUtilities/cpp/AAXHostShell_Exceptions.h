//
//  AAXHostShell_Exceptions.h
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 5/8/15.
//
//

/** @file Exception classes used by AAXHostShell client utilities
 
 @details
 Define @c AAXHOSTSHELL_EXCEPTIONS_INCLUDE_STACKTRACES as 1 to include a stack trace in each
 exception's context string. This requires linking to CFnd.
 
 @internal @note These classes use the same structure as the classes in AAXVal_Exceptions.h
 */
 

#ifndef __AAXHostShell_Exceptions__
#define __AAXHostShell_Exceptions__

// AAXHostShell Includes
#include "AAXHostShell_Types.h"

// AAX Includes
#include "AAX.h"

// Standard Includes
#include <string>
#include <cstdio>
#include <cstdarg>

// AAX Includes
#include "AAX_Errors.h"

// Includes for custom features
#if (defined(AAXHOSTSHELL_EXCEPTIONS_INCLUDE_STACKTRACES)) && (0 != AAXHOSTSHELL_EXCEPTIONS_INCLUDE_STACKTRACES)
#include "Cmn_StackDump.h"
#include <sstream>
#endif


//-----------------------------------------------------
#pragma mark - Macros

#define AAXHOSTSHELL_THROW_FAIL_FOR_FUNC(RESULT, FUNC) do { if (kAAXHShell_Success != RESULT) { throw(AAXHostShell::Exception::Factory<AAXHostShell::Exception::ResultError>::Format(__FUNCTION__, #FUNC, RESULT)); } } while(false)
#define AAXHOSTSHELL_THROW_FAIL(RESULT, ERR_WHAT) do { if (kAAXHShell_Success != RESULT) { throw(AAXHostShell::Exception::Factory<AAXHostShell::Exception::ResultError>::Format("%s - %s", __FUNCTION__, ERR_WHAT)); } } while(false)

#define AAXHOSTSHELL_DEFINE_BASIC_EXCEPTION(X) class X : public AAXHostShell::Exception::Any, public AAXHostShell::Exception::Factory<X> { public: virtual ~X() {} }
#define AAXHOSTSHELL_DEFINE_BASIC_EXCEPTION_SUB(X, SUP) class X : public SUP, public AAXHostShell::Exception::Factory<X> { public: virtual ~X() {} }


//-----------------------------------------------------
#pragma mark - Exception class declarations

namespace AAXHostShell
{
	namespace Exception
	{
		/** @brief Mix-in class template to provide standard constructors to all Exception classes
		 
			@details
			@c SUB should be set to the sub-classing type (CRTP) and must also be an @ref Exception::Any sub-class
		 */
		template <typename SUB> // CRTP
		class Factory
		{
		public:
			/* non-virtual destructor */   ~Factory() {}
			
			static SUB Format(const std::string& inWhat) { SUB ex; ex.SetWhat(inWhat.c_str()); return ex; }
			
#if AAXHOSTSHELL_USE_CPP11
			template<typename... Args>
			static SUB Format(const char* inFunction, const char* inFormat, Args... inArgs)
			{
				SUB ex;
				
				if (inFormat)
				{
					char buffer[1024];
					std::snprintf(buffer, 1024, inFormat, inArgs...);
					ex.SetWhat(buffer);
				}
				
				if (inFunction)
				{
					ex.SetFunction(inFunction);
				}
				
				return ex;
			}
#else
			static SUB Format(const char* inFunction, const char* inFormat, ...)
			{
				SUB ex;
				
				if (inFormat)
				{
					char buffer[1024];
					
					va_list args;
					va_start(args,inFormat);
					
					struct StArgEnder
					{
						explicit StArgEnder(va_list& inArgs) : mArgs(inArgs) {}
						~StArgEnder() { va_end(mArgs); }
					private:
						va_list& mArgs;
					} argEnder(args);
					
					if (512 > strlen(inFormat))
					{
						vsprintf(buffer, inFormat, args);
					}
					else if (1024 - 12 > strlen(inFormat))
					{
						sprintf(buffer, "[overflow] %s", inFormat);
					}
					else
					{
						sprintf(buffer, "[overflow]");
					}
					
					ex.SetWhat(buffer);
				}
				
				if (inFunction)
				{
					ex.SetFunction(inFunction);
				}
				
				return ex;
			}
#endif
		};
		
		/** @brief Base class for all AAXHostShell Client Utility exceptions
		 */
		class Any
		{
		public:
			virtual /* destructor */   ~Any() {}
			
			/* default constructor */  Any();
			explicit /* constructor */ Any(const char* inWhat);
			explicit /* constructor */ Any(const char* inFunction, const char* inWhat);
			
#if AAXHOSTSHELL_USE_CPP11
			/* copy constructor */     Any(const Any&) = default;
			/* move constructor */     Any(Any&&) = default;
			Any&                       operator=(const Any&) = default;
			Any&                       operator=(Any&&) = default;
#endif
			
		public:
			void SetWhat(const char* inWhat) { mWhat = inWhat; }
			void SetFunction(const char* inFunction) { mFunction = inFunction; }
			
			/// Returns the exception description
			const std::string& What() const { return mWhat; }
			
			/// Returns true if context information is available
			bool HasContext() const { return (false == Context().empty()); }
			
			/// Returns the throwing function name or a full stack trace, or an empty string if no context information is available
			std::string Context() const { return mStackTrace.empty() ? mFunction : mStackTrace; }
			
		private:
			static std::string GenerateStackTrace(); ///< Generates an empty string if AAXHOSTSHELL_EXCEPTIONS_INCLUDE_STACKTRACES is set to zero
			
		private:
			std::string mWhat;
			std::string mFunction;
			const std::string mStackTrace;
		};
		
		class ResultError : public Exception::Any, public Exception::Factory<ResultError>
		{
		public:
			using Exception::Factory<ResultError>::Format; // Un-hide inherited
			
			virtual /* destructor */   ~ResultError() {}
			
			static ResultError Format(AAXHShell_TResult inResult);
			static ResultError Format(const char* inCalledFunction, AAXHShell_TResult inResult);
			static ResultError Format(const char* inExecutingFunction, const char* inCalledFunction, AAXHShell_TResult inResult);
			
		private:
			static const char* AAXHostShellResultToString(AAXHShell_TResult inResult);
		};
		
		class AAXResultError : public Exception::Any, public Exception::Factory<AAXResultError>
		{
		public:
			using Exception::Factory<AAXResultError>::Format; // Un-hide inherited
			
			virtual /* destructor */   ~AAXResultError() {}
			
			static AAXResultError Format(AAX_Result inResult);
			static AAXResultError Format(const char* inCalledFunction, AAX_Result inResult);
			static AAXResultError Format(const char* inExecutingFunction, const char* inCalledFunction, AAX_Result inResult);
			
		private:
			static const char* AAXResultToString(AAX_Result inResult);
		};
		
		AAXHOSTSHELL_DEFINE_BASIC_EXCEPTION(UnsupportedValue);
		AAXHOSTSHELL_DEFINE_BASIC_EXCEPTION(InternalError); // Internal run-time error
		AAXHOSTSHELL_DEFINE_BASIC_EXCEPTION(InvalidParameter); // Invalid function parameter
	}
}


//-----------------------------------------------------
#pragma mark - Exception class implementation

//---
#pragma mark AAXHostShell::Exception::Any

inline AAXHostShell::Exception::Any::Any()
: mWhat()
, mFunction()
, mStackTrace(GenerateStackTrace())
{
}

inline AAXHostShell::Exception::Any::Any(const char* inWhat)
: mWhat(inWhat)
, mFunction()
, mStackTrace(GenerateStackTrace())
{
}

inline AAXHostShell::Exception::Any::Any(const char* inFunction, const char* inWhat)
: mWhat(inWhat)
, mFunction(inFunction)
, mStackTrace(GenerateStackTrace())
{
}

/* static */
inline std::string AAXHostShell::Exception::Any::GenerateStackTrace()
{
#if (defined(AAXHOSTSHELL_EXCEPTIONS_INCLUDE_STACKTRACES)) && (0 != AAXHOSTSHELL_EXCEPTIONS_INCLUDE_STACKTRACES)
	Cmn_StackDump sd;
	std::stringstream ss;
	sd.Print(ss, 5);
	return ss.str();
#else
	return std::string();
#endif
}


//---
#pragma mark AAXHostShell::Exception::ResultError

/* static */
inline AAXHostShell::Exception::ResultError AAXHostShell::Exception::ResultError::Format(AAXHShell_TResult inResult)
{
	return AAXHostShell::Exception::Factory<ResultError>::Format(NULL, "Encountered AAXHostShell error: %ld (%s)", (long int)inResult, AAXHostShellResultToString(inResult));
}

/* static */
inline AAXHostShell::Exception::ResultError AAXHostShell::Exception::ResultError::Format(const char* inCalledFunction, AAXHShell_TResult inResult)
{
	return AAXHostShell::Exception::Factory<ResultError>::Format(NULL, "Error returned from %s: %ld (%s)", inCalledFunction, (long int)inResult, AAXHostShellResultToString(inResult));
}

/* static */
inline AAXHostShell::Exception::ResultError AAXHostShell::Exception::ResultError::Format(const char* inExecutingFunction, const char* inCalledFunction, AAXHShell_TResult inResult)
{
	return AAXHostShell::Exception::Factory<ResultError>::Format(inExecutingFunction, "%s - Error returned from %s: %ld (%s)", inExecutingFunction, inCalledFunction, (long int)inResult, AAXHostShellResultToString(inResult));
}

/* static */
inline const char* AAXHostShell::Exception::ResultError::AAXHostShellResultToString(AAXHShell_TResult inResult)
{
#define DEFINE_AAXHOSTSHELL_ERROR_STRING(X) do { if (X == inResult) { return #X; } } while(false)
	
	DEFINE_AAXHOSTSHELL_ERROR_STRING(kAAXHShell_Success);
	DEFINE_AAXHOSTSHELL_ERROR_STRING(kAAXHShell_Fail);
	return "<unknown error code>";
}


//---
#pragma mark AAXHostShell::Exception::AAXResultError

/* static */
inline AAXHostShell::Exception::AAXResultError AAXHostShell::Exception::AAXResultError::Format(AAX_Result inResult)
{
	return AAXHostShell::Exception::Factory<AAXResultError>::Format(NULL, "Encountered AAX error: %ld (%s)", (long int)inResult, AAXResultToString(inResult));
}

/* static */
inline AAXHostShell::Exception::AAXResultError AAXHostShell::Exception::AAXResultError::Format(const char* inCalledFunction, AAX_Result inResult)
{
	return AAXHostShell::Exception::Factory<AAXResultError>::Format(NULL, "AAX error returned from %s: %ld (%s)", inCalledFunction, (long int)inResult, AAXResultToString(inResult));
}

/* static */
inline AAXHostShell::Exception::AAXResultError AAXHostShell::Exception::AAXResultError::Format(const char* inExecutingFunction, const char* inCalledFunction, AAX_Result inResult)
{
	return AAXHostShell::Exception::Factory<AAXResultError>::Format(inExecutingFunction, "%s - AAX error returned from %s: %ld (%s)", inExecutingFunction, inCalledFunction, (long int)inResult, AAXResultToString(inResult));
}

/* static */
inline const char* AAXHostShell::Exception::AAXResultError::AAXResultToString(AAX_Result inResult)
{
#define DEFINE_AAX_ERROR_STRING(X) do { if (X == inResult) { return #X; } } while(false)
	
	DEFINE_AAX_ERROR_STRING(AAX_SUCCESS);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_PARAMETER_ID);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_STRING_CONVERSION);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_METER_INDEX);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_NULL_OBJECT);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_OLDER_VERSION);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_CHUNK_INDEX);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_CHUNK_ID);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INCORRECT_CHUNK_SIZE);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_UNIMPLEMENTED);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_PARAMETER_INDEX);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_NOT_INITIALIZED);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_ACF_ERROR);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_METER_TYPE);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_CONTEXT_ALREADY_HAS_METERS);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_NULL_COMPONENT);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_PORT_ID_OUT_OF_RANGE);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_FIELD_TYPE_DOES_NOT_SUPPORT_DIRECT_ACCESS);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_DIRECT_ACCESS_OUT_OF_BOUNDS);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_FIFO_FULL);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INITIALIZING_PACKET_STREAM_THREAD);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_POST_PACKET_FAILED);
	DEFINE_AAX_ERROR_STRING(AAX_RESULT_PACKET_STREAM_NOT_EMPTY);
	DEFINE_AAX_ERROR_STRING(AAX_RESULT_ADD_FIELD_UNSUPPORTED_FIELD_TYPE);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_MIXER_THREAD_FALLING_BEHIND);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_FIELD_INDEX);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_MALFORMED_CHUNK);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_TOD_BEHIND);
	DEFINE_AAX_ERROR_STRING(AAX_RESULT_NEW_PACKET_POSTED);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_PLUGIN_NOT_AUTHORIZED);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_PLUGIN_NULL_PARAMETER);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_NOTIFICATION_FAILED);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_VIEW_SIZE);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_SIGNED_INT_OVERFLOW);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_NO_COMPONENTS);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_DUPLICATE_EFFECT_ID);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_DUPLICATE_TYPE_ID);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_EMPTY_EFFECT_NAME);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_UNKNOWN_PLUGIN);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_PROPERTY_UNDEFINED);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_PATH);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_UNKNOWN_ID);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_UNKNOWN_EXCEPTION);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_ARGUMENT);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_NULL_ARGUMENT);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_INVALID_INTERNAL_DATA);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_ARGUMENT_BUFFER_OVERFLOW);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_UNSUPPORTED_ENCODING);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_UNEXPECTED_EFFECT_ID);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_NO_ABBREVIATED_PARAMETER_NAME);
	DEFINE_AAX_ERROR_STRING(AAX_ERROR_ARGUMENT_OUT_OF_RANGE);
	
	return "<unknown error code>";
}


#endif /* defined(__AAXHostShell_Exceptions__) */
