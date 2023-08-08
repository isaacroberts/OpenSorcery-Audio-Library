//
//  LicenseUnlock.cpp
//  
//
//  Created by Isaac Roberts on 12/16/20.
//

// Created by Isaac Roberts


#include "LicenseUnlock.h"

#include "RT/Utilities/RestUtil.h"

#include <assert.h>

/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 6 End-User License
   Agreement and JUCE Privacy Policy (both effective as of the 16th June 2020).

   End User License Agreement: www.juce.com/juce-6-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

/* Note: There's a bit of light obfuscation in this code, just to make things
   a bit more annoying for crackers who try to reverse-engineer your binaries, but
   nothing particularly foolproof.
*/

#include <random>

//#include "../DebugWindow.h"

struct KeyFileUtils
{
	static XmlElement createKeyFileContent (const String& appName,
											const String& userEmail,
											const String& licenseKey,
											const int& machineID,
											bool expires)
	{
		XmlElement xml (UnlockUtil::deob5("pj~"));

		xml.setAttribute (UnlockUtil::deob4("iqemp"), userEmail);
		xml.setAttribute (UnlockUtil::deob1("mjdfotflfz"), licenseKey);
		xml.setAttribute (UnlockUtil::deob2("ocejkpg"), machineID);
		xml.setAttribute( UnlockUtil::deob4("|t"), expires ? 1 : 0);
		xml.setAttribute (UnlockUtil::deob1("bqq"), appName);
		xml.setAttribute (UnlockUtil::deob1("ebuf"), String::toHexString (Time::getCurrentTime().toMilliseconds()));

		return xml;
	}

	static String createKeyFileComment (const String& appName,
										const String& userEmail,
										int machineID)
	{
		String comment;
		comment << UnlockUtil::deob1("Lfzgjmf!gps!") << appName << newLine;

		comment << UnlockUtil::deob2("Gockn<\"") << userEmail << newLine
				<< UnlockUtil::deob5("Hwjfyji?%") << Time::getCurrentTime().toString (true, true);

		return comment;
	}

	//==============================================================================
	static String encryptXML (const XmlElement& xml, RSAKey privateKey)
	{
		MemoryOutputStream text;
		text << xml.toString (XmlElement::TextFormat().singleLine());

		BigInteger val;
		val.loadFromMemoryBlock (text.getMemoryBlock());

		privateKey.applyToValue (val);

		return val.toString (16);
	}

	static String createKeyFile (String comment,
								 const XmlElement& xml,
								 RSAKey rsaPrivateKey)
	{
		String asHex (UnlockUtil::deob4("'") + encryptXML (xml, rsaPrivateKey));

		StringArray lines;
		lines.add (comment);
		lines.add (String());

		const int charsPerLine = 70;
		while (asHex.length() > 0)
		{
			lines.add (asHex.substring (0, charsPerLine));
			asHex = asHex.substring (charsPerLine);
		}

		lines.add (String());

		return lines.joinIntoString ("\r\n");
	}

	//==============================================================================
	static XmlElement decryptXML (String hexData, RSAKey rsaPublicKey)
	{
		BigInteger val;
		val.parseString (hexData, 16);

		RSAKey key (rsaPublicKey);
		jassert (key.isValid());

		std::unique_ptr<XmlElement> xml;

		if (! val.isZero())
		{
			key.applyToValue (val);

			auto mb = val.toMemoryBlock();

			if (CharPointer_UTF8::isValidString (static_cast<const char*> (mb.getData()), (int) mb.getSize()))
				xml = parseXML (mb.toString());
		}
		//"key"
		String k = UnlockUtil::deob5("pj~");
		return xml != nullptr ? *xml : XmlElement(k);
	}

	static XmlElement getXmlFromKeyFile (String keyFileText, RSAKey rsaPublicKey)
	{
		return decryptXML (keyFileText.fromLastOccurrenceOf ("#", false, false).trim(), rsaPublicKey);
	}

	static StringArray getMachineNumbers (XmlElement xml, StringRef attributeName)
	{
		StringArray numbers;
		numbers.addTokens (xml.getStringAttribute (attributeName), ",; ", StringRef());
		numbers.trim();
		numbers.removeEmptyStrings();
		return numbers;
	}

	static String getEmail (const XmlElement& xml)          {
		//"email"
		return xml.getStringAttribute (UnlockUtil::deob5("jrfnq"));
	}
	static String getLicenseKey (const XmlElement& xml)          {
		//"licensekey"
		return xml.getStringAttribute (UnlockUtil::deob2("nkegpugmg{"));
	}
	static String getAppID (const XmlElement& xml)          {
		//"app"
		return xml.getStringAttribute (UnlockUtil::deob5("fuu"));
	}
	static int getMachineNo(const XmlElement& xml)			{
		//"machine"
		return xml.getIntAttribute(UnlockUtil::deob3("pdfklqh"));
	}
	static bool expires (const XmlElement& xml)          {
		//"xp"
		return xml.getIntAttribute (UnlockUtil::deob3("{s"));
	}

	struct KeyFileData
	{
		String email, appID;
		String licenseKey;
		int machineNumber;

		bool keyFileExpires;
		Time expiryTime;
	};

	static KeyFileData getDataFromKeyFile (XmlElement xml)
	{
		KeyFileData data;

//		data.licensee = getLicensee (xml);
		data.licenseKey = getLicenseKey(xml);
		data.email = getEmail (xml);
		data.appID = getAppID (xml);
		data.machineNumber = getMachineNo(xml);

		//expiryTime
		String e = UnlockUtil::deobfuscate("fyqjszUjnf");
		
		if (expires(xml) && xml.hasAttribute (e))
		{
			data.keyFileExpires = true;
			data.expiryTime = Time (xml.getStringAttribute (e).getHexValue64());
		}
		else
		{
			data.keyFileExpires = false;
		}

		return data;
	}
};

String LicenseUnlock::titleString()
{
	//Enter your license info.
	return UnlockUtil::deob1("Foufs!zpvs!mjdfotf!jogp/");
}
String LicenseUnlock::unlockButtonText()
{
	//Unlock
	return UnlockUtil::deob2("Wpnqem");
}
String LicenseUnlock::linkButtonName()
{
	//Get Key
	return UnlockUtil::deob3("Jhw#Nh|");
}

//==============================================================================
#if JUCE_MODULE_AVAILABLE_juce_data_structures
const char* LicenseUnlock::unlockedProp = "u";
const char* LicenseUnlock::expiryTimeProp = "t";
static const char* stateTagName = "stn";
static const char* userNameProp = "unp";
static const char* userKeyProp = "ukp";
static const char* keyfileDataProp = "kfdp";

/*
static var machineNumberAllowed (StringArray numbersFromKeyFile,
								 StringArray localMachineNumbers)
{
	var result;

	for (int i = 0; i < localMachineNumbers.size(); ++i)
	{
		auto localNumber = localMachineNumbers[i].trim();

		if (localNumber.isNotEmpty())
		{
			for (int j = numbersFromKeyFile.size(); --j >= 0;)
			{
				var ok (localNumber.trim().equalsIgnoreCase (numbersFromKeyFile[j].trim()));
				result.swapWith (ok);

				if (result)
					break;
			}

			if (result)
				break;
		}
	}

	return result;
}
*/
//==============================================================================
LicenseUnlock::LicenseUnlock()  : status (stateTagName),
keyFileExpiryState(ExpiryCracked)
{
	machine_number = MachineIDUtilities::getMachineHash();
}

bool LicenseUnlock::start()
{
	String keyFilePath = getKeyFileLocation();
	File keyFile (keyFilePath);
		
	keyFileExpiryState= DoesNotExist;
	
	if (keyFile.existsAsFile())
	{
		MemoryBlock data;
		bool loaded = keyFile.loadFileAsData(data);
		
		if (!loaded)
			return false;
		
		String decrypted = decrypt(data, machine_number);
		
		if (decrypted != "")
		{
			ExpiryState result = applyKeyFile(decrypted);
			keyFileExpiryState=result;
			
			if (expiryFailed(result))
				return false;
			else if (expirySucceeded(result))
				return true;
			else if (result==HasExpired || result==NearExpiry)
			{
				String email = getUserEmail();
				String key = getUserKey();
				
				UnlockResult unlock;
				unlock.result = UnlockResult::Failed;
				
				if ((!hasEmailField() || email != "") && key != "")
				{
					unlock = runUnlock(email, key);
				}
				
				if (UnlockResult::failsBeforeExpiry(unlock.result))
				{
					//Delete key file?
					return false;
				}
				else if (result == NearExpiry)
				{
					return true;
				}
				else if (result == HasExpired)
				{
					return UnlockResult::unlocks(unlock.result);
				}
			}
		}
		else
		{
			keyFileExpiryState=MaybeMaliciousFail;
			keyFile.deleteFile();
		}
	}
	return false;
}

void LicenseUnlock::saveFile(const String& email, const String& password)
{
	
	String keyFilePath = getKeyFileLocation();
	
//	std::cout<<"KeyFile path = "<<keyFilePath<<std::endl;
	
	bool expires = doesFileExpire();
	
	String appName = getProductID();
//	int machineID = MachineIDUtilities::getMachineHash();
	
	
	XmlElement xml = KeyFileUtils::createKeyFileContent (appName, email, password, machine_number, expires);
	
	if (expires)
	{
		Time expiryTime = Time::getCurrentTime();
		expiryTime += getFileDuration();
		//expiryTime
		String e = UnlockUtil::deob5("j}unw~Ynrj");
		xml.setAttribute(e, String::toHexString (expiryTime.toMilliseconds()));
	}
	
	String str = xml.toString();
	MemoryBlock output = encrypt(str, machine_number);
	
//	decrypt(output, machine_number);
	
	File keyFile (keyFilePath);
	
	keyFile.replaceWithData(output.getData(), output.getSize());
}


#define KL 40

key_type LicenseUnlock::getFileEncryptionKey()
{
	int sr = getSRand();
	
	std::random_device rd;   // non-deterministic generator
	std::mt19937 gen(sr);
	
	unsigned long long key = 0 ;
	
	for (int i = 0 ; i < KL; ++i)
	{
		key = key << 2;
		key += gen()%256;
	}
	
//	std::cout<<"encryption key = \""<<key<<"\""<<std::endl;
	
	return key;
}
char deref(key_type key, int amt)
{
	key = key >> (amt * 2);
	return char(key & 127);
}

MemoryBlock LicenseUnlock::encrypt(String input, int machineNo)
{
	key_type key = getFileEncryptionKey();
	
	MemoryBlock block(input.length());
	
	auto string = input.getCharPointer().getAddress();
	
	for (int i=0; i < input.length(); ++i)
	{
		int kn = i % KL;
		block[i] = (string[i] + deref(key, kn));
	}
	
	return block;
}
String LicenseUnlock::decrypt(MemoryBlock& string, int machineNo)
{
	auto key = getFileEncryptionKey();
	
//	Debug::print("File Key =",key);
	
	String output = "";
	
	for (int i=0; i < string.getSize(); ++i)
	{
		int kn = i % KL;
		char c = string[i] - deref(key, kn);
	
		output += c;
	}
	
	return output;
}

LicenseUnlock::~LicenseUnlock()
{
}

void LicenseUnlock::load()
{
	MemoryBlock mb;
	String string_to_encode = "";
	mb.fromBase64Encoding (string_to_encode);

	if (mb.getSize() > 0)
		status = ValueTree::readFromGZIPData (mb.getData(), mb.getSize());
	else
		status = ValueTree (stateTagName);
	
	if (machine_number == -1)
		status.removeProperty (unlockedProp, nullptr);

	KeyFileUtils::KeyFileData data;
	data = KeyFileUtils::getDataFromKeyFile (KeyFileUtils::getXmlFromKeyFile (status[keyfileDataProp], getPublicKey()));

	if (data.keyFileExpires)
	{
		if (! doesProductIDMatch (data.appID))
			status.removeProperty (expiryTimeProp, nullptr);

		if (data.machineNumber == machine_number)
			status.removeProperty (expiryTimeProp, nullptr);
	}
	else
	{
		if (! doesProductIDMatch (data.appID))
			status.removeProperty (unlockedProp, nullptr);

		if (data.machineNumber == machine_number)
			status.removeProperty (unlockedProp, nullptr);
	}
}

void LicenseUnlock::save()
{
	MemoryOutputStream mo;

	{
		GZIPCompressorOutputStream gzipStream (mo, 9);
		status.writeToStream (gzipStream);
	}

//	saveState (mo.getMemoryBlock().toBase64Encoding());
}

char LicenseUnlock::MachineIDUtilities::getPlatformPrefix()
{
   #if JUCE_MAC
	return 'M';
   #elif JUCE_WINDOWS
	return 'W';
   #elif JUCE_LINUX
	return 'L';
   #elif JUCE_IOS
	return 'I';
   #elif JUCE_ANDROID
	return 'A';
   #endif
}

int LicenseUnlock::MachineIDUtilities::getMachineHash()
{
	auto ids = getLocalMachineIDs();
	
	int numID = 0;
	
	if (ids.size()==0)
	{
//		std::cout<<"No machine ID's!"<<std::endl;
		jassert(false);
		return -1;
	}
	for (String id : ids)
		numID += id.hashCode();

	return abs(numID);
}

String LicenseUnlock::MachineIDUtilities::getEncodedIDString (const String& input)
{
	auto platform = String::charToString (static_cast<juce_wchar> (getPlatformPrefix()));

	//salt_1
	String s1 = UnlockUtil::deobfuscate("tbmu`2");
	
	return platform + MD5 ((input + s1 + platform).toUTF8())
						.toHexString().substring (0, 9).toUpperCase();
}

bool LicenseUnlock::MachineIDUtilities::addFileIDToList (StringArray& ids, const File& f)
{
	if (auto num = f.getFileIdentifier())
	{
		ids.add (getEncodedIDString (String::toHexString ((int64) num)));
		return true;
	}

	return false;
}

void LicenseUnlock::MachineIDUtilities::addMACAddressesToList (StringArray& ids)
{
	for (auto& address : MACAddress::getAllAddresses())
		ids.add (getEncodedIDString (address.toString()));
}

StringArray LicenseUnlock::MachineIDUtilities::getLocalMachineIDs()
{
	auto identifiers = SystemStats::getDeviceIdentifiers();

	for (auto& identifier : identifiers)
		identifier = getEncodedIDString (identifier);

	return identifiers;
}

StringArray LicenseUnlock::getLocalMachineIDs()
{
	return MachineIDUtilities::getLocalMachineIDs();
}

void LicenseUnlock::userCancelled()
{
}

void LicenseUnlock::setUserEmail (const String& usernameOrEmail)
{
	status.setProperty (userNameProp, usernameOrEmail, nullptr);
}
void LicenseUnlock::setUserKey (const String& key)
{
	status.setProperty (userKeyProp, key, nullptr);
}

String LicenseUnlock::getUserEmail() const
{
	return status[userNameProp].toString();
}
String LicenseUnlock::getUserKey() const
{
	return status[userKeyProp].toString();
}

LicenseUnlock::ExpiryState LicenseUnlock::applyKeyFile (String keyFileContent)
{
	KeyFileUtils::KeyFileData data;

	auto xml = parseXML(keyFileContent);

	if (xml==nullptr)
	{
		//this tells whether decryption failed
		return MaybeMaliciousFail;
	}

	data = KeyFileUtils::getDataFromKeyFile (*xml);
	
	if ((!hasEmailField() || data.email.isNotEmpty()) && data.licenseKey.isNotEmpty() && doesProductIDMatch (data.appID))
	{
		if (hasEmailField())
			setUserEmail (data.email);
		setUserKey( data.licenseKey);
		status.setProperty (keyfileDataProp, keyFileContent, nullptr);
		status.removeProperty (data.keyFileExpires ? expiryTimeProp : unlockedProp, nullptr);

		//dummy result is obfuscation
		var actualResult (0), dummyResult (1.0);
		
		auto localMachine = MachineIDUtilities::getMachineHash();
		
//		var v (machineNumberAllowed (data.machineNumbers, getLocalMachineIDs()));
		var v (data.machineNumber == localMachine);
		
		actualResult.swapWith (v);
		v = var(localMachine == -1);
		dummyResult.swapWith (v);
		jassert (! dummyResult);
		
		if (doesFileExpire())
		{
			 if (!data.keyFileExpires)
			 {
				 //can't get by it that easily
				 return HasExpired;
			 }
			 
			 if ((! dummyResult) && actualResult)
				 status.setProperty (expiryTimeProp, data.expiryTime.toMilliseconds(), nullptr);
			 

			 int64 remaining = data.expiryTime.toMilliseconds() - Time::getCurrentTime().toMilliseconds();
			 
			 if (remaining <= 0)
				 return HasExpired;
			 
			 int64 length = getFileDuration().inMilliseconds();
			 
			 if (remaining < length / 2)
			 {
				 return NearExpiry;
			 }
			 else
			 {
				 return Success;
			 }
		 }
		

		if ((! dummyResult) && actualResult)
			status.setProperty (unlockedProp, actualResult, nullptr);

		if (isUnlocked())
			return Success;
	}

	return MaliciousFail;
}

LicenseUnlock::UnlockResult LicenseUnlock::handleXmlReply (XmlElement xml)
{
	UnlockResult r;

	r.result = UnlockResult::Cracked;
//	if (auto keyNode = xml.getChildByName ("KEY"))
//	{
//		//Code doesn't reach here unless you've failed to override everything
//		const String keyText (keyNode->getAllSubText().trim());
//		assert(false);
////		r.succeeded = keyText.length() > 10 && applyKeyFile (keyText);
//	}
//	else
//	{
//		r.succeeded = false;
//	}
//
//	if (xml.hasTagName ("MESSAGE"))
//		r.informativeMessage = xml.getStringAttribute ("message").trim();
//
//	if (xml.hasTagName ("ERROR"))
//		r.errorMessage = xml.getStringAttribute ("error").trim();
//
//	if (xml.getStringAttribute ("url").isNotEmpty())
//		r.urlToLaunch = xml.getStringAttribute ("url").trim();
//
//	if (r.errorMessage.isEmpty() && r.informativeMessage.isEmpty() && r.urlToLaunch.isEmpty() && ! r.succeeded)
//		r.errorMessage = getMessageForUnexpectedReply();

	return r;
}

LicenseUnlock::UnlockResult LicenseUnlock::handleFailedConnection()
{
	UnlockResult r;
	r.result = UnlockResult::NoConnection;
	r.errorMessage = getMessageForConnectionFailure (RestUtil::hasInternetAccess());
	return r;
}

String LicenseUnlock::getMessageForConnectionFailure (bool isInternetConnectionWorking)
{
	//Couldn't connect to
	String nocon = UnlockUtil::deobfuscate("Dpvmeo(u!dpoofdu!up!");
	
	
	String message = nocon + getWebsiteName() + UnlockUtil::deob2("000")+"\n\n";

	if (isInternetConnectionWorking)
	{
		/*
		message << TRANS("Your internet connection seems to be OK, but our webserver "
						 "didn't respond... This is most likely a temporary problem, so try "
						 "again in a few minutes, but if it persists, please contact us for support!");
		*/
		message << UnlockUtil::deob4( "]syv$mrxivrix$gsrrigxmsr$wiiqw$xs$fi$SO0$fyx$syv${ifwivziv$hmhr+x$viwtsrh222$Xlmw$mw$qswx$pmoip}$e$xiqtsvev}$tvsfpiq0$ws$xv}$ekemr$mr$e$ji{$qmryxiw0$fyx$mj$mx$tivwmwxw0$tpiewi$gsrxegx$yw$jsv$wyttsvx%");
	}
	else
	{
		/*
		message << TRANS("No internet sites seem to be accessible from your computer.. Before trying again, "
						 "please check that your network is working correctly, and make sure "
						 "that any firewall/security software installed on your machine isn't "
						 "blocking your web connection.");
		 
		 No internet sites seem to be accessible from your computer.. Before trying again, please check that your network is working correctly, and make sure that any firewall/security software installed on your machine isn't blocking your web connection.
		 */
		message << UnlockUtil::deob3( "Qr#lqwhuqhw#vlwhv#vhhp#wr#eh#dffhvvleoh#iurp#|rxu#frpsxwhu11#Ehiruh#wu|lqj#djdlq/#sohdvh#fkhfn#wkdw#|rxu#qhwzrun#lv#zrunlqj#fruuhfwo|/#dqg#pdnh#vxuh#wkdw#dq|#iluhzdoo2vhfxulw|#vriwzduh#lqvwdoohg#rq#|rxu#pdfklqh#lvq*w#eorfnlqj#|rxu#zhe#frqqhfwlrq1");
	}

	return message;
}

String LicenseUnlock::getMessageForUnexpectedReply()
{
	//"Unexpected or corrupted reply from "
	//"Please try again in a few minutes, and contact us for support if this message appears again."
	return UnlockUtil::deobfuscate("Vofyqfdufe!ps!dpssvqufe!sfqmz!gspn!") + getWebsiteName()
			+ UnlockUtil::deob1("///")+"\n\n"
			+ UnlockUtil::deob5( "Uqjfxj%yw~%flfns%ns%f%kj|%rnszyjx1%fsi%htsyfhy%zx%ktw%xzuutwy%nk%ymnx%rjxxflj%fuujfwx%flfns3");
}

LicenseUnlock::UnlockResult LicenseUnlock::runUnlock (const String& in_email, const String& password)
{
	String email;
	if (hasEmailField())
		email = in_email.trim();
	else
		email = "none";
	
	UnlockResult result = attemptWebserverUnlock(email, password);
	
	if (UnlockResult::unlocks(result.result))
	{
		saveFile(email, password);
	}
	
	result.succeeded = UnlockResult::unlocks(result.result);
	
	return result;
}


LicenseUnlock::UnlockResult LicenseUnlock::attemptWebserverUnlock (const String& email,
																			 const String& password)
{
	// This method will block while it contacts the server, so you must run it on a background thread!
	jassert (! MessageManager::getInstance()->isThisTheMessageThread());

	auto reply = readReplyFromWebserver (email, password);

	//"Reply from server: "
	String r = UnlockUtil::deob("Sfqmz!gspn!tfswfs;!");
	DBG (r << reply);

	if (auto xml = parseXML (reply))
		return handleXmlReply (*xml);

	return handleFailedConnection();
}

#endif // JUCE_MODULE_AVAILABLE_juce_data_structures

//==============================================================================
String LicenseUnlock::KeyGen::generateKeyFile (const String& appName,
									   const String& userEmail,
									   const String& licenseKey,
									   const int& machineID,
									   const RSAKey& privateKey)
{
	auto xml = KeyFileUtils::createKeyFileContent (appName, userEmail, licenseKey, machineID, false);
	auto comment = KeyFileUtils::createKeyFileComment (appName, userEmail, machineID);

	return KeyFileUtils::createKeyFile (comment, xml, privateKey);
}

String LicenseUnlock::KeyGen::generateExpiringKeyFile (const String& appName,
											   const String& userEmail,
											   const String& licenseKey,
											   const int& machineID,
											   const Time expiryTime,
											   const RSAKey& privateKey)
{
	//expiryTime
	String e = UnlockUtil::deob3("h{slu|Wlph");
	
	//"Expires: "
	String e2 = UnlockUtil::deob1("Fyqjsft;!");
	
	auto xml = KeyFileUtils::createKeyFileContent (appName, userEmail, licenseKey, machineID, true);
	xml.setAttribute (e, String::toHexString (expiryTime.toMilliseconds()));

	auto comment = KeyFileUtils::createKeyFileComment (appName, userEmail, machineID);
	comment << newLine << e2 << expiryTime.toString (true, true);

	return KeyFileUtils::createKeyFile (comment, xml, privateKey);
}



