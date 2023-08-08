//
//  WCUnlockStatus.cpp
//  
//
//  Created by Isaac Roberts on 12/14/20.
//

// Created by Isaac Roberts


#include "WCUnlockStatus.h"

#include "RTGraphics/DebugWindow.h"


WCUnlockStatus::WCUnlockStatus()
: LicenseUnlock()
{
//	std::cout<<"Machine id: "<<machine_number<<std::endl;
//	Debug::print("Machine id:", machine_number);
}

WCUnlockStatus::~WCUnlockStatus()
{
	
}
String WCUnlockStatus::emailFormName()
{
	//Email Address
	return UnlockUtil::deob("Fnbjm!Beesftt");
}
String WCUnlockStatus::passwordFormName()
{
	//License Key
	return UnlockUtil::deob("Mjdfotf!Lfz");
}

/** Subclasses that talk to a particular web-store will implement this method
	to contact their webserver and attempt to unlock the current machine for
	the given username and password. The return value is the XML text from the
	server which contains error information and/or the encrypted keyfile.
*/
String WCUnlockStatus::getResponse (const String& email, const String& password, String request)
{
	//Response
	String r  = UnlockUtil::deob("sfrvftu");
	//Email
	String e  = UnlockUtil::deob("fnbjm");
	//license_key
	String lk = UnlockUtil::deob("mjdfotf`lfz");
	//product_id
	String pi = UnlockUtil::deob("qspevdu`je");
	//instance
	String i  = UnlockUtil::deob("jotubodf");
	
	URL url (getServerAuthenticationURL()
			 .withParameter (r, request)
			 .withParameter (e, email)
			 .withParameter (lk, password)
			 .withParameter (pi, getProductID()));
	if (machine_number != -1)
		url = URL(url
				.withParameter(i, String(machine_number)));
	
	std::cout<<url.toString(true)<<std::endl;
	
//	std::cout<< "Trying _WCUnlockStatus_ to unlock via URL: " << url.toString (true)<<std::endl;

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
juce::var WCUnlockStatus::getResult(const String& email, const String& password, String request)
{
	String reply = getResponse(email, password, request);
	
	return juce::JSON::parse(reply);
}
LicenseUnlock::UnlockResult WCUnlockStatus::attemptWebserverUnlock (const String& email, const String& key)
{
//	bool activated = false;
	
	LicenseUnlock::UnlockResult result;
	
	result.result= UnlockResult::Cracked;
	
	
//	if (machine_number != -1)
//	{
//		juce::var data = getResult(email, key, "check");
//
//		if (data.isVoid())
//		{
//			/*
//				------
//				-- If no response
//				------
//			 */
//			std::cout<<"connection failed"<<std::endl;
//			return handleFailedConnection();
//		}
//
//
//	//	{"success":true,"timestamp":1607988039,"remaining":4,"activations":
//		if (data["success"])
//		{
//			auto *act = data["activations"].getArray();
//			if (!act)
//			{
//				/*
//				  ------
//				  -- If no "activation" array in response
//				  ------
//			   */
//
//
//			}
//			else if (act->size() > 0)
//			{
//
//				for (int i = 0 ; i < act->size(); ++i )
//				{
//	//				{"activation_id":"1","instance":"1607987807","activation_platform":"",
////					 "activation_time":"2020-12-14 20:16:47"}],"sig":"ff0a15bb4ae21546c16ce37bda6a1700"}
//					int inst_num = int((*act)[i]["instance"]);
//
//					if (inst_num == machine_number)
//					{
//						/*
//							------
//							-- If this machine has already been activated
//							------
//						 */
////						result.informativeMessage = "Already activated! Success!";
//						result.succeeded = true;
//						return result;
//					}
//				}
//			}
//		}
//	}
	
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
	a = UnlockUtil::deob5("fhyn{fyji");
	if (data[Identifier(a)])
	{
		//"instance"
		String i = UnlockUtil::deobfuscate("jotubodf");
		int returned_number = data[Identifier(i)];
		
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
		String e = UnlockUtil::deob2("gttqt");
		//" Code: "
		String c1 = UnlockUtil::deob4("$Gshi>$");
		//code
		String c2 = UnlockUtil::deob3("frgh");
		result.result = UnlockResult::ServerFail;
		result.errorMessage = String(data[Identifier(e)].toString())
			+ c1 + String(data[Identifier(c2)].toString());
		result.urlToLaunch = getSupportURL();
		return result;
	}
}
