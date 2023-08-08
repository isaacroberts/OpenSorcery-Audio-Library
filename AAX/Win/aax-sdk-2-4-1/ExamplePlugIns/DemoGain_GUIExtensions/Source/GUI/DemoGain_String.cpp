/*================================================================================================*/
/*
 *	Copyright 2009-2015 by Avid Technology, Inc.
 *	All rights reserved.
 *
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 */
/*================================================================================================*/
#include "DemoGain_String.h"


DemoGain_String::DemoGain_String() :
	mString("")
{
}

uint32_t DemoGain_String::Length () const
{
	return uint32_t(mString.length());
}

uint32_t DemoGain_String::MaxLength () const
{
	return kMaxStringLength;
}

const char * DemoGain_String::Get ()	const
{
	return mString.c_str();
}

void DemoGain_String::Set ( const char * iString )
{
	mString = iString;
}

AAX_IString & DemoGain_String::operator=(const AAX_IString & iOther)
{	
	mString = iOther.Get();
	return *this;
}

AAX_IString & DemoGain_String::operator=(const char * iString)
{
	mString = iString;
	return *this;
}
