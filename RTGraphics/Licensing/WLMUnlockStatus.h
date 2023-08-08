//
//  WLMUnlockStatus.h
//  
//
//  Created by Isaac Roberts on 10/16/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class WLMUnlockStatus {
	/*
		For Wordpress "License Manager" plugin by wpexpertsio
	 
		Keys can be a custom length, number of blocks, length of blocks, and character set
		Keys can include a prefix or suffix. (Only prefixes are written right now
	 */
public:
	WLMUnlockStatus(int numBlocks, int blockLength);
	
	virtual String getPrefix() = 0;
	virtual String getConsumerKey() override = 0;
	virtual String getConsumerSecret() override=0;
	
	virtual String getWebsiteName() override = 0;
	virtual URL getServerAuthenticationURL() override = 0;

	
	
	virtual ~WLMUnlockStatus();
	
	virtual String getKeyFileLocation() override;

	
	
	virtual String emailFormName() override;
	virtual String passwordFormName() override;

	
	
	String returnCryptedData() override { return String(machine_number);}
	void setCryptedData(String s) override { jassert (String(machine_number) == s);}
	
	
	//============ WP Methods ====================================================
	
	 /** Subclasses that talk to a particular web-store will implement this method
		 to contact their webserver and attempt to unlock the current machine for
		 the given username and password. The return value is the XML text from the
		 server which contains error information and/or the encrypted keyfile.
	 */
	String readReplyFromWebserver (const String& email, const String& password) override {return "";}
	
	String call(String urlExtra, StringPairArray&);
	
	juce::var activate(const String& email, const String& password);
	
	String getResponse (const String& email, const String& password, String request);
	juce::var getResult (const String& email, const String& password, String request);
	
	virtual UnlockResult attemptWebserverUnlock (const String& email, const String& password) override;
	

protected:
	String allowedChars;
	char sepChar;
private:
	int numBlocks;
	int blockLength;
	
private:
	CriticalSection streamCreationLock;
	std::unique_ptr<WebInputStream> stream;

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WLMUnlockStatus);

};
