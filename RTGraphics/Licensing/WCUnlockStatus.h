//
//  WCUnlockStatus.h
//  
//
//  Created by Isaac Roberts on 12/14/20.
//

// Created by Isaac Roberts 

#pragma once

#ifdef JUCE_CRYPTOGRAPHY_H_INCLUDED

#include <JuceHeader.h>

#include "LicenseUnlock.h"

using namespace juce;

/*
 This can be used for the WooCommerce Software Add-on.
 */

class WCUnlockStatus : public LicenseUnlock {
public:
	WCUnlockStatus();
	

	virtual String emailFormName() override;
	virtual String passwordFormName() override;

	
	//========== App-specific Methods ==================================================
	 /** This must return your product's ID, as allocated by the store. */
	 virtual String getProductID() override = 0;

		/** This must return the RSA public key for authenticating responses from
		 the server for this app. You can get this key from your marketplace
		 account page.
	 */
	 virtual RSAKey getPublicKey() override = 0;
	
	/** Returns the name of the web-store website, not for communication, but for
		presenting to the user.
	*/
	virtual String getWebsiteName() override = 0;
	
	/** Returns the URL of the authentication API. */
	virtual URL getServerAuthenticationURL() override = 0;

	
	
	virtual String getKeyFileLocation() override = 0;
	
//	String returnCryptedData() override { return String(machine_number);}
//	void setCryptedData(String s) override { jassert (String(machine_number) == s);}
	
	
	bool hasEmailField() override { return true; }
	//============ WP Methods ====================================================

	 /** Subclasses that talk to a particular web-store will implement this method
		 to contact their webserver and attempt to unlock the current machine for
		 the given username and password. The return value is the XML text from the
		 server which contains error information and/or the encrypted keyfile.
	 */
	String readReplyFromWebserver (const String& email, const String& password) override {return "";}
	
	String getResponse (const String& email, const String& password, String request);
	juce::var getResult (const String& email, const String& password, String request);
	
	virtual UnlockResult attemptWebserverUnlock (const String& email, const String& password) override;
	
	virtual ~WCUnlockStatus();
protected:

	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WCUnlockStatus);

	CriticalSection streamCreationLock;
	std::unique_ptr<WebInputStream> stream;

	
	
};

#endif
