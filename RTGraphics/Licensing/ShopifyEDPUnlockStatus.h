//
//  ShopifyEDPUnlockStatus.h
//  
//
//  Created by Isaac Roberts on 7/7/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "LicenseUnlock.h"
#include "UnlockForm.h"

using namespace juce;

/*
	Easy Digital Products
	Shopify Plugin
 
 
	app-easy-product-downloads.fr
 
 
 
 https://app-easy-product-downloads.fr/api/get-license-key?license_key=KEY&api_token=TOKEN

 */

class ShopifyEDPUnlockStatus : public LicenseUnlock
{
public:
	ShopifyEDPUnlockStatus();
	
	
	virtual String getAPIToken() = 0;
	/** Returns the name of the web-store website, not for communication, but for
		presenting to the user.
	*/
//	virtual String getWebsiteName() = 0;

//	virtual bool doesFileExpire() = 0;
//	virtual RelativeTime getFileDuration() { return RelativeTime(); }

	
	
	bool hasEmailField() override { return false; }

	//TODO: Obfuscate
	String emailFormName() override {
		return String();
	}
	String passwordFormName() override {
		return "License Key";
	}
	URL getServerAuthenticationURL() override {
		return URL("https://app-easy-product-downloads.fr/api/");
//		return URL("https://lemonaid.ai/api");

	}
	
	void userCancelled();

	UnlockResult attemptWebserverUnlock (const String& email, const String& password) override;

	String readReplyFromWebserver (const String& email, const String& password) override { return ""; }

	virtual ~ShopifyEDPUnlockStatus();
protected:
	String getResponse (const String& password, String request);
	juce::var getResult (const String& password, String request);

	CriticalSection streamCreationLock;
	std::unique_ptr<WebInputStream> stream;

	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShopifyEDPUnlockStatus);

};



class ShopifyEDPUnlockForm : public UnlockForm
{
public:
	ShopifyEDPUnlockForm (ShopifyEDPUnlockStatus & u, bool hasCancelButton=false, bool overlayHasCancelButton=false, bool hasLinkButton=true)
	: UnlockForm(u, hasCancelButton, overlayHasCancelButton, hasLinkButton)
	{
		passwordBox.setPasswordCharacter(0);
	}
	
	virtual ~ShopifyEDPUnlockForm()
	{
		
	}
	
//	class KeyFilter : public juce::TextEditor::InputFilter {
//	public:
//		KeyFilter(String prefix_) : InputFilter(),
//		prefix(prefix_)
//		{}
//		virtual ~KeyFilter() {}
//		virtual String filterNewText(TextEditor& editor, const String&) override;
//		String prefix;
//	};
	
protected:
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShopifyEDPUnlockForm);

};

