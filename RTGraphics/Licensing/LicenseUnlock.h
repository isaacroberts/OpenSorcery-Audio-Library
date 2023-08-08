//
//  LicenseUnlock.h
//  
//
//  Created by Isaac Roberts on 12/16/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#ifdef JUCE_CRYPTOGRAPHY_H_INCLUDED

#include "UnlockUtil.h"

typedef unsigned long long key_type;

using namespace juce;

/**
	A base class for online unlocking systems.

	This class stores information about whether your app has been unlocked for the
	current machine, and handles communication with a web-store to perform the
	unlock procedure.

	You probably won't ever use this base class directly, but rather a store-specific
	subclass such as TracktionMarketplaceStatus, which knows how to talk to the particular
	online store that you're using.

	To use it, you create a subclass which implements all the pure virtual methods
	(see their comments to find out what you'll need to make them do).

	Then you can create an instance of your subclass which will hold the registration
	state. Typically, you'll want to just keep a single instance of the class around for
	the duration of your app. You can then call its methods to handle the various
	registration tasks.

	Areas of your code that need to know whether the user is registered (e.g. to decide
	whether a particular feature is available) should call isUnlocked() to find out.

	If you want to create a GUI that allows your users to enter their details and
	register, see the OnlineUnlockForm class.

	@see OnlineUnlockForm, KeyGeneration

	@tags{ProductUnlocking}
*/


class LicenseUnlock
{
public:
	LicenseUnlock();

	virtual bool start(); // returns whether unlocked
		
	void saveFile(const String& email, const String& password);
	
	key_type getFileEncryptionKey();
	
	MemoryBlock encrypt(String, int machineNo);
	String decrypt(MemoryBlock&, int machineNo);
	
	/** Destructor. */
	virtual ~LicenseUnlock();

	virtual bool hasEmailField() = 0;
	
	virtual String titleString();
	virtual String unlockButtonText();
	
	virtual String emailFormName() = 0;
	virtual String passwordFormName() = 0;
	
	virtual String linkButtonName();
	virtual String linkButtonURL() { return String(); }
	
	//==============================================================================
	
	/** This must return your product's ID, as allocated by the store.
		Even if your store doesn't use one, it's used for the license key 
	 */
	virtual String getProductID() = 0;

	virtual String getPrefix() { return ""; }
	virtual String getSuffix() { return ""; }
	
	/** This must check whether a product ID string that the server returned is OK for
		unlocking the current app.
	*/
	virtual bool doesProductIDMatch (const String& returnedIDFromServer) {
		return returnedIDFromServer == getProductID();
	}

	/** This must return the RSA public key for authenticating responses from
		the server for this app. You can get this key from your marketplace
		account page.
	*/
	virtual RSAKey getPublicKey() = 0;

	/** Returns the name of the web-store website, not for communication, but for
		presenting to the user.
	*/
	virtual String getWebsiteName() = 0;

	/**
		Customizables
	 */
	virtual String getMessageForConnectionFailure (bool isInternetConnectionWorking);
	virtual String getSupportURL()=0;
	
	/** Returns the URL of the authentication API. */
	virtual URL getServerAuthenticationURL() = 0;

	virtual String getKeyFileLocation() = 0;
	
	virtual void save_to_crypted_file() {}
	virtual void read_crypted_file() {}
	
//	virtual String returnCryptedData() = 0;
//	virtual void setCryptedData(String) = 0;
	
	virtual bool doesFileExpire() = 0;
	virtual RelativeTime getFileDuration() { return RelativeTime(); }
	
	virtual bool encryptSpecific() { return false;}
	virtual bool tagSpecificWithKey() { return true; }
	virtual int getSRand() = 0;
	
	
	/** Subclasses that talk to a particular web-store will implement this method
		to contact their webserver and attempt to unlock the current machine for
		the given username and password. The return value is the XML text from the
		server which contains error information and/or the encrypted keyfile.
	*/
	virtual String readReplyFromWebserver (const String& email, const String& password) = 0;

	/** Returns a list of strings, any of which should be unique to this
		physical computer.

		When testing whether the user is allowed to use the product on this
		machine, this list of tokens is compared to the ones that were stored
		on the webserver.

		The default implementation of this method will simply call
		MachineIDUtilities::getLocalMachineIDs(), which provides a default
		version of this functionality.
	*/
	virtual StringArray getLocalMachineIDs();

	/** This method will be called if the user cancels the connection to the webserver
		by clicking the cancel button in OnlineUnlockForm::OverlayComp.

		The default implementation of this method does nothing but you should use it to
		cancel any WebInputStreams that may be connecting.
	*/
	void userCancelled();

	virtual String getMessageForUnexpectedReply();

	//==============================================================================
	// The following methods can be called by your app:

	/** Returns true if the product has been successfully authorised for this machine.

		The reason it returns a variant rather than a bool is just to make it marginally
		more tedious for crackers to work around. Hopefully if this method gets inlined
		they'll need to hack all the places where you call it, rather than just the
		function itself.

		Bear in mind that each place where you check this return value will need to be
		changed by a cracker in order to unlock your app, so the more places you call this
		method, the more hassle it will be for them to find and crack them all.
	*/
	inline var isUnlocked() const               { return status[unlockedProp]; }

	/** Returns the Time when the keyfile expires.

		If a the key file obtained has an expiry time, isUnlocked will return false and this
		will return a non-zero time. The interpretation of this is up to your app but could
		be used for subscription based models or trial periods.
	*/
	inline Time getExpiryTime() const           { return Time (static_cast<int64> (status[expiryTimeProp])); }

	/** Optionally allows the app to provide the user's email address if
		it is known.
		You don't need to call this, but if you do it may save the user
		typing it in.
	*/
	void setUserEmail (const String& usernameOrEmail);
	void setUserKey (const String& key);

	/** Returns the user's email / key if known. */
	String getUserEmail() const;
	String getUserKey() const;

	/** Attempts to perform an unlock using a block of key-file data provided.
		You may wish to use this as a way of allowing a user to unlock your app
		by drag-and-dropping a file containing the key data, or by letting them
		select such a file. This is often needed for allowing registration on
		machines without internet access.
	*/
	
	enum ExpiryState {
		ExpiryCracked=0, BenignFail=1, Cracked1=2, Success=3, Cracked2,  NearExpiry, HasExpired, Cracked3,
		DoesNotExist, MaybeMaliciousFail, MaliciousFail, Cracked4
	};
	
	inline bool expiryFailed(ExpiryState& e) {
		return !(e==Success || e==NearExpiry || e==HasExpired);
	}
	inline bool expirySucceeded(ExpiryState& e) {
		return e==Success || e==NearExpiry;
	}
	inline bool expiring(ExpiryState& e) {
		return (e==HasExpired || e==NearExpiry);
	}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
/* Your code here */
	inline bool expiryCracked(ExpiryState& e) {
		if (!(e==ExpiryCracked || e==Cracked1 || e==Cracked2 || e==Cracked3 || e==Cracked4))
			return false;
		//Intentionally no return func; provides less reliability for crackers
	}
#pragma GCC diagnostic pop
	
	ExpiryState applyKeyFile (String data);

	/** This provides some details about the reply that the server gave in a call
		to attemptWebserverUnlock().
	*/
	struct UnlockResult
	{
		enum Result {
			NotRun, Cracked, NoConnection, ServerFail, Succeeded, Failed
		};
		
		static bool failsBeforeExpiry(Result& r) {
			return r==Cracked || r==Failed;
		}
		static bool unlocks(Result& r) {
			return r==Succeeded;
		}
		
		
		/** If an unlock operation fails, this is the error message that the webserver
			supplied (or a message saying that the server couldn't be contacted)
		*/
		String errorMessage;

		/** This is a message that the webserver returned, and which the user should
			be shown.

			It's not necessarily an error message, e.g. it might say that there's a
			new version of the app available or some other status update.
		*/
		String informativeMessage;

		/** If the webserver wants the user to be directed to a web-page for further
			information, this is the URL that it would like them to go to.
		*/
		String urlToLaunch;

		/** If the unlock operation succeeded, this will be set to true. */
		Result result = NotRun;
		bool succeeded=false;
	};

	/** Contacts the webserver and attempts to perform a registration with the
		given user details.

		The return value will either be a success, or a failure with an error message
		from the server, so you should show this message to your user.

		Because this method blocks while it contacts the server, you must run it on
		a background thread, not on the message thread. For an easier way to create
		a GUI to do the unlocking, see OnlineUnlockForm.
	*/
	
	UnlockResult runUnlock (const String& email, const String& password);
	virtual UnlockResult attemptWebserverUnlock (const String& email, const String& password);

	/** Attempts to load the status from the state retrieved by getState().
		Call this somewhere in your app's startup code.
	 */
	void load();

	/** Triggers a call to saveState which you can use to store the current unlock status
		in your app's settings.
	 */
	void save();

	/** This class contains some utility functions that might help with machine ID generation. */
	struct MachineIDUtilities
	{
		static int getMachineHash();
		
		/** Returns a character that represents the current OS.
			E.g. 'M' for Mac, 'W' for windows, etc
		*/
		static char getPlatformPrefix();

		/** Returns an encoded hash string from the given input string, prefixing it with
			a letter to represent the current OS type.
		*/
		static String getEncodedIDString (const String& inputString);

		/** Utility function that you may want to use in your machine-ID generation code.
			This adds an ID string to the given array which is a hash of the filesystem ID of the
			given file.
		*/
		static bool addFileIDToList (StringArray& result, const File& file);

		/** Utility function that you may want to use in your machine-ID generation code.
			This adds some ID strings to the given array which represent each MAC address of the machine.
		*/
		static void addMACAddressesToList (StringArray& result);

		/** This method calculates some machine IDs based on things like network
			MAC addresses, hard-disk IDs, etc, but if you want, you can overload
			it to generate your own list of IDs.

			The IDs that are returned should be short alphanumeric strings
			without any punctuation characters. Since users may need to type
			them, case is ignored when comparing them.

			Note that the first item in the list is considered to be the
			"main" ID, and this will be the one that is displayed to the user
			and registered with the marketplace webserver. Subsequent IDs are
			just used as fallback to avoid false negatives when checking for
			registration on machines which have had hardware added/removed
			since the product was first registered.
		*/
		static StringArray getLocalMachineIDs();
	};
	struct KeyGen {
			/**
		  Generates the content of a key-file which can be sent to a user's machine to
		  unlock a product.

		  The returned value is a block of text containing an RSA-encoded block, followed
		  by some human-readable details. If you pass this block of text to
		  OnlineUnlockStatus::applyKeyFile(), it will decrypt it, and if the
		  key matches and the machine numbers match, it will unlock that machine.

		  Typically the way you'd use this on a server would be to build a small executable
		  that simply calls this method and prints the result, so that the webserver can
		  use this as a reply to the product's auto-registration mechanism. The
		  keyGenerationAppMain() function is an example of how to build such a function.

		  @see OnlineUnlockStatus
	  */
	  static String JUCE_CALLTYPE generateKeyFile (const String& appName,
												   const String& userEmail,
												   const String& licenseKey,
												   const int& machineID,
												   const RSAKey& privateKey);

	  /** Similar to the above key file generation method but with an expiry time.
		  You must supply a Time after which this key file should no longer be considered as active.

		  N.B. when an app is unlocked with an expiring key file, OnlineUnlockStatus::isUnlocked will
		  still return false. You must then check OnlineUnlockStatus::getExpiryTime to see if this
		  expiring key file is still in date and act accordingly.

		  @see OnlineUnlockStatus
	  */
	  static String JUCE_CALLTYPE generateExpiringKeyFile (const String& appName,
														   const String& userEmail,
														   const String& licenseKey,
														   const int& machineID,
														   const Time expiryTime,
														   const RSAKey& privateKey);
	};

protected:
	ValueTree status;
	
	int machine_number = -1;

	UnlockResult handleXmlReply (XmlElement);
//	virtual UnlockResult handleJsonReply (juce::var) {}
	UnlockResult handleFailedConnection();

	static const char* unlockedProp;
	static const char* expiryTimeProp;
	
	ExpiryState keyFileExpiryState;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LicenseUnlock)
};

#else
#warning "Include cryptography to use LicenseUnlock"

#endif  
