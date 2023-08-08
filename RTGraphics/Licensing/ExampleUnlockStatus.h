//
//  ExampleUnlockStatus.h
//  
//
//  Created by Isaac Roberts on 5/19/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>
#ifdef JUCE_CRYPTOGRAPHY_H_INCLUDED


#include "LicenseUnlock.h"

//using namespace juce;

class ExampleUnlockStatus : public LicenseUnlock {
public:
	ExampleUnlockStatus();
	
		
	virtual String emailFormName() override;
	virtual String passwordFormName() override;

	
	//========== App-specific Methods ==================================================
	 /** This must return your product's ID, as allocated by the store. */
	virtual String getProductID() override = 0;

		/** This must return the RSA public key for authenticating responses from
		 the server for this app. You can get this key from your marketplace
		 account page.
	 */
	virtual RSAKey getPublicKey() override {return RSAKey(""); }
	
	/** Returns the name of the web-store website, not for communication, but for
		presenting to the user.
	*/
	virtual String getWebsiteName() override = 0;
	
	/** Returns the URL of the authentication API. */
	virtual URL getServerAuthenticationURL() override { return URL(UnlockUtil::deob5("myyux?44fqltwnymrnh2xtky|fwj3htr"));}

	String readReplyFromWebserver (const String& email, const String& password) override {
		return String();
	}

	
	virtual String getKeyFileLocation() override = 0;
	
//	String returnCryptedData() override { return String(machine_number);}
//	void setCryptedData(String s) override { jassert (String(machine_number) == s);}
	
	
	virtual UnlockResult attemptWebserverUnlock (const String& email, const String& password) override;
	
	virtual ~ExampleUnlockStatus();
protected:

	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExampleUnlockStatus);
};

#endif
