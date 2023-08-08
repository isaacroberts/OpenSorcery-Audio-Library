//
//  WCUnlockForm.cpp
//  
//
//  Created by Isaac Roberts on 12/15/20.
//

// Created by Isaac Roberts


#include "WCUnlockForm.h"


WCUnlockForm::WCUnlockForm (LicenseUnlock &status, bool hasCancelButton, bool overlayHasCancelButton, bool hasLinkButton)
: UnlockForm(status, hasCancelButton, overlayHasCancelButton, hasLinkButton)
{
	passwordBox.setPasswordCharacter(0);
	
	passwordBox.setInputFilter(new KeyFilter(status.getPrefix()), true);
}

WCUnlockForm::~WCUnlockForm()
{
	
}




String WCUnlockForm::KeyFilter::filterNewText(TextEditor& editor, const String& intext)
{
	//SPBK 917ad46f-554f-4b91-a53c-644a51141121	
	int lenPrefix = prefix.length();
	const int slot1 = 8 + lenPrefix;
	const int slot2 = 12 + lenPrefix;
	const int slot3 = 16 + lenPrefix;
	const int slot4 = 20 + lenPrefix;
	const int length = 32 + 4 + lenPrefix;
	
	String text = editor.getText() + intext;
	
	text = text.removeCharacters("\r\n ");
	
	text = text.removeCharacters("-");
	
	if (! text.startsWith(prefix))
	{
		if (text.startsWithIgnoreCase(prefix))
		{
			text = prefix + text.substring(lenPrefix);
		}
		else
			text = prefix + text;
	}
	
	if (text.length() > slot4)
		text = text.replaceSection(slot4, 0, "-");
	if (text.length() > slot3)
		text = text.replaceSection(slot3, 0, "-");
	if (text.length() > slot2)
		text = text.replaceSection(slot2, 0, "-");
	if (text.length() > slot1)
		text = text.replaceSection(slot1, 0, "-");
	
	text = text.substring(0, length);
	
	editor.setText(text, false);
	
	return "";
}

