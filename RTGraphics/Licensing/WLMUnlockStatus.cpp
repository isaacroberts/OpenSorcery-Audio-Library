//
//  WLMUnlockStatus.cpp
//  
//
//  Created by Isaac Roberts on 10/16/21.
//

// Created by Isaac Roberts


#include "WLMUnlockStatus.h"
#include "../FileLoad.h"

WLMUnlockStatus::WLMUnlockStatus(int nBlocks, int blockLen)
: LicenseUnlock(),
numBlocks(nBlocks), blockLength(blockLen)
{
}


WLMUnlockStatus::~WLMUnlockStatus()
{
	
}
String WLMUnlockStatus::emailFormName()
{
	//Email Address
	return UnlockUtil::deob("Fnbjm!Beesftt");
}
String WLMUnlockStatus::passwordFormName()
{
	//License Key
	return UnlockUtil::deob("Mjdfotf!Lfz");
}

/** Subclasses that talk to a particular web-store will implement this method
	to contact their webserver and attempt to unlock the current machine for
	the given username and password. The return value is the XML text from the
	server which contains error information and/or the encrypted keyfile.
*/

String WLMUnlockStatus::call(String urlExtra, StringPairArray& params)
{
	String ck = "consumer_key";
	String cs = "consumer_secret";
	
	URL url (getServerAuthenticationURL()+urlExtra);
	
	url = url.withParameters(params);

	url = url.withParameter(ck, getConsumerKey());
	url = url.withParameter(ck, getConsumerSecret());
	
	
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
}

juce::var activate(const String& email, const String& password)
{
	/*
	 $license = null;
	 if (!empty($license_key)) {
		 $response = $this->call("licenses/activate/{$license_key}");
		 if (isset($response['success']) && $response['success'] === true) {
			 $license = $response['data'];
		 } else {
			 $this->valid_status['is_valid'] = false;
			 $this->valid_status['error'] = $response['message'];
			 $this->valid_status['nextValidation'] = time();
			 update_option($this->valid_object, $this->valid_status);
			 throw new ErrorException($response['message']);
		 }
	 }

	 return $license;
	 */
	
	StringPairArray params;
	
	String urlExtra= "/activate/";
	urlExtra += password;
	
	String result = call(urlExtra, params);
	
	juce::var json = juce::JSON::parse(reply);
	
	
}



juce::var WLMUnlockStatus::getResult(const String& email, const String& password, String request)
{
	String reply = getResponse(email, password, request);
	
	return juce::JSON::parse(reply);
}
LicenseUnlock::UnlockResult WLMUnlockStatus::attemptWebserverUnlock (const String& email, const String& key)
{
//	bool activated = false;
	
	LicenseUnlock::UnlockResult result;
	
	result.result= UnlockResult::Cracked;
	
	//"activation"
	String a = UnlockUtil::deob("bdujwbujpo");
	juce::var data = getResult(email, key, a);
	
	if (data.isVoid())
	{
		/*
		 ------
		 -- If no response
		 ------
		 */
		return handleFailedConnection();
	}
	
	//"activated"
	a = UnlockUtil::deob("bdujwbufe");
	if (data[a])
	{
		//"instance"
		String i = UnlockUtil::deobfuscate("jotubodf");
		int returned_number = data[i];
		
		if (returned_number != machine_number)
		{
			//				std::cout<<"Returned instance number different from machine number; returned= "<<returned_number<<"; machine_no= "<<machine_number<<std::endl;
		}
		
		result.result= UnlockResult::Succeeded;
		return result;
	}
	else
	{
		/*
		 -----
		 -- General error message - we could check for which error here
		 -----
		 */
		//error
		String e = UnlockUtil::deobfuscate("fssps");
		//" Code: "
		String c1 = UnlockUtil::deobfuscate("!Dpef;!");
		//code
		String c2 = UnlockUtil::deobfuscate("dpef");
		result.result = UnlockResult::ServerFail;
		result.errorMessage = String(data[error].toString())
			+ c1 + String(data[c2].toString());
		result.urlToLaunch = getSupportURL();
		return result;
	}
}


