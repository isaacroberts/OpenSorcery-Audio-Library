//
//  ShopifyEDPUnlockStatus.cpp
//  
//
//  Created by Isaac Roberts on 7/7/22.
//

// Created by Isaac Roberts


#include "ShopifyEDPUnlockStatus.h"

/*
	 https://app-easy-product-downloads.fr/api/get-license-key?license_key=KEY&api_token=TOKEN
 */

ShopifyEDPUnlockStatus::ShopifyEDPUnlockStatus()
{
}

ShopifyEDPUnlockStatus::~ShopifyEDPUnlockStatus()
{
	if (stream)
		stream->cancel();
}
void ShopifyEDPUnlockStatus::userCancelled()
{
	if (stream)
		stream->cancel();
}
String ShopifyEDPUnlockStatus::getResponse (const String& password, String request)
{
	//Response
	String r  = UnlockUtil::deob("sfrvftu");

	//license_key
	String lk = UnlockUtil::deob("mjdfotf`lfz");
	//api token
	String pi = "api_token";
	
	URL url (getServerAuthenticationURL()
			 .withNewSubPath("api/"+request)
			 .withParameter (lk, password)
			 .withParameter (pi, getAPIToken())
			 );
	
//	URL url(getServerAuthenticationURL() + String("/api/")+request+"?"+lk+"="+password+"&"+pi+getAPIToken()
//			);
//	url = url.
//			withPOSTData();
	
//	std::cout<<url.toString(true)<<std::endl;
	
//	std::cout<< "Trying _WCUnlockStatus_ to unlock via URL: " << url.toString (false)<<std::endl;

	{
		ScopedLock lock (streamCreationLock);
		stream.reset (new WebInputStream (url, true));
	}

	if (stream->connect (nullptr))
	{
		auto thread = Thread::getCurrentThread();
		MemoryOutputStream result;

		while (! (stream->isExhausted() || stream->isError()
					|| (thread != nullptr && thread->threadShouldExit())))
		{
			auto bytesRead = result.writeFromInputStream (*stream, 8192);

			if (bytesRead < 0)
				break;
		}

		return result.toString();
	}

	return "";
}
juce::var ShopifyEDPUnlockStatus::getResult(const String& password, String request)
{
	String reply = getResponse(password, request);
//	std::cout<<reply<<std::endl;
	return juce::JSON::parse(reply);
}
LicenseUnlock::UnlockResult ShopifyEDPUnlockStatus::attemptWebserverUnlock (const String& email, const String& password)
{
	
	LicenseUnlock::UnlockResult result;
	result.result= UnlockResult::Cracked;

	String request = "get-license-key";
	auto var = getResult(password, request);
	
	if (var.isVoid())
	{
		/*
		 ------
		 -- If no response
		 ------
		 */
		return handleFailedConnection();
	}

	
	if (var.hasProperty("status"))
	{
		if (var["status"] == "success")
		{
			//Could check the email here
			result.result= UnlockResult::Succeeded;
			return result;
		}
		else
		{
			result.result = UnlockResult::Failed;
			result.errorMessage = "Incorrect license key";
		}
	}
	else
	{
		//error
		String e = UnlockUtil::deob2("gttqt");
		//" Code: "
		String c1 = UnlockUtil::deob4("$Gshi>$");
		//code
		String c2 = UnlockUtil::deob3("frgh");

		result.result = UnlockResult::ServerFail;
		result.errorMessage = String(var[Identifier(e)].toString())
			+ c1 + String(var[Identifier(c2)].toString());
		result.urlToLaunch = getSupportURL();
		return result;

	}
	
	return result;
}


