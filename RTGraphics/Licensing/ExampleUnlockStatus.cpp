//
//  ExampleUnlockStatus.cpp
//  
//
//  Created by Isaac Roberts on 5/19/22.
//

// Created by Isaac Roberts


#include "ExampleUnlockStatus.h"

ExampleUnlockStatus::ExampleUnlockStatus()
: LicenseUnlock()
{
}

ExampleUnlockStatus::~ExampleUnlockStatus()
{
	
}
String ExampleUnlockStatus::emailFormName()
{
	//Email Address
	return UnlockUtil::deob5("Jrfnq%Fiiwjxx");
}
String ExampleUnlockStatus::passwordFormName()
{
	//License Key
	return UnlockUtil::deob3("Olfhqvh#Nh|");
}

LicenseUnlock::UnlockResult ExampleUnlockStatus::attemptWebserverUnlock (const String& email, const String& key)
{
	LicenseUnlock::UnlockResult result;
	result.result = UnlockResult::Succeeded;
	return result;
}
