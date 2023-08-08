//
//
// Copyright 2014 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_StringUtils.h
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 2/19/14.
//
//

#ifndef __AAXHostShell_CppUtilities_StringUtils_h__
#define __AAXHostShell_CppUtilities_StringUtils_h__

// AAXHostShell Includes
#include "AAXHostShell_Types.h"

// AAX Includes
#include "AAX_Enums.h"
#include "AAX_StringUtilities.h"

// Standard Includes
#include <sstream>

namespace AAXHostShell
{
	struct CString
	{
	public:
		~CString()
		{
			Clear();
		}
		
		explicit CString(size_t inByteSize)
		: mSize(inByteSize)
		, mBuffer(new char[mSize])
		{
		}
		
		explicit CString(const std::string& inStr)
		: mSize(inStr.length()+1)
		, mBuffer(new char[mSize])
		{
			strcpy(mBuffer, inStr.c_str());
		}
		
		CString(const CString& inOther)
		: mSize(0)
		, mBuffer(nullptr)
		{
			operator=(inOther);
		}
		
		CString(CString&& inOther)
		: mSize(inOther.mSize)
		, mBuffer(inOther.mBuffer)
		{
			inOther.mSize = 0;
			inOther.mBuffer = nullptr;
		}
		
		CString& operator=(const CString& inOther)
		{
			Clear();
			mSize = inOther.mSize;
			mBuffer = new char[mSize];
			memcpy(mBuffer, inOther.mBuffer, mSize);
			
			return *this;
		}
		
		CString& operator=(CString&& inOther)
		{
			Clear();
			mSize = inOther.mSize;
			mBuffer = inOther.mBuffer;
			inOther.mSize = 0;
			inOther.mBuffer = nullptr;
			
			return *this;
		}
		
		const char* Get() const { return mBuffer; }
		char* Get() { return mBuffer; }
		std::string AsStr() { return std::string(mBuffer); }
		size_t Size() { return mSize; }
		
	private:
		void Clear()
		{
			if (mBuffer)
			{
				delete[] mBuffer;
				mSize = 0;
			}
		}
		
	private:
		size_t mSize;
		char* mBuffer;
	};
	
	inline std::string AsStringFourChar(uint32_t inFourChar)
	{
		const uint32_t oneCharMask = 0x000000FF;
		const int16_t oneCharNumBits = 8;
		
		std::string resultStr("");
		for (int16_t i = 3; i >= 0; --i)
		{
			const char curChar = static_cast<char>((inFourChar >> (i*oneCharNumBits)) & oneCharMask);
			resultStr += ((0x20 <= curChar) && (0x7E >= curChar)) ? curChar : '?';
		}
		
		return resultStr;
	}

	inline uint32_t AsUint32(std::string str)
	{
		const int16_t oneCharNumBits = 8;
		const size_t charNum = 4;
		
		str.resize(charNum, '?');
		
		uint32_t result = 0;
		for (size_t i = 0; i < charNum; ++i)
		{
			result |= str[i] << (charNum - i - 1) * oneCharNumBits;
		}
		
		return result;
	}

	inline std::string AsStringPlatform(int32_t inPlatform)
	{
		std::string result("[unknown]");
		switch (inPlatform)
		{
			case kAAXHShell_Platform_Native:
				result = "native";		break;
			case kAAXHShell_Platform_TI:
				result = "dsp";			break;
			case kAAXHShell_Platform_None:
				result = "none";		break;
			case kAAXHShell_Platform_Any:
				result = "any";			break;
			default:
				result = "[invalid]";	break;
		}
		return result;
	}
	
	inline std::string AsStringSampleRate(int32_t inSampleRate)
	{
		std::string result("[unknown]");
		if (kAAXHShell_SampleRate_Any == inSampleRate)
		{
			result = "any";
		}
		else
		{
			std::stringstream ss;
			ss << inSampleRate;
			result = ss.str();
		}
		return result;
	}
	
	inline std::string AsStringBool(AAXHShell_TBoolean inBool)
	{
		return (0 == inBool) ? "false" : "true";
	}
	
	inline std::string AsStringID(uint64_t inID)
	{
		std::string result("[unknown]");
		
		std::stringstream ss;
		ss << inID;
		result = ss.str();
		
		return result;
	}
	
	inline std::string AsString(const AAXHShell_SHostContext& inContext)
	{
		std::string result("{");
		
		result += "'plat': " + AsStringPlatform(inContext.mPlatform);
		result += ", ";
		result += "'in': " + AAX::AsStringStemFormat(static_cast<AAX_EStemFormat>(inContext.mInputStemFormat));
		result += ", ";
		result += "'out': " + AAX::AsStringStemFormat(static_cast<AAX_EStemFormat>(inContext.mOutputStemFormat));
		result += ", ";
		result += "'rate': " + AsStringSampleRate(inContext.mSampleRate);
		result += ", ";
		result += "'alg': " + AsStringBool(inContext.mSupportsProcessing);
		
		return result + "}";
	}
}


#endif /* defined(__AAXHostShell_CppUtilities_StringUtils_h__) */
