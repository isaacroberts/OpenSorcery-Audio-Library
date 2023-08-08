//
//  WCUnlockForm.h
//  
//
//  Created by Isaac Roberts on 12/15/20.
//

// Created by Isaac Roberts 

#pragma once

#ifdef JUCE_CRYPTOGRAPHY_H_INCLUDED

#include <JuceHeader.h>

#include "UnlockForm.h"

using namespace juce;

class WCUnlockForm : public UnlockForm
{
public:
	WCUnlockForm (LicenseUnlock &, bool hasCancelButton=false, bool overlayHasCancelButton=false, bool hasLinkButton=true);
	
	virtual ~WCUnlockForm();
	
	class KeyFilter : public juce::TextEditor::InputFilter {
	public:
		KeyFilter(String prefix_) : InputFilter(),
		prefix(prefix_)
		{}
		virtual ~KeyFilter() {}
		virtual String filterNewText(TextEditor& editor, const String&) override;
		String prefix;
	};
	
protected:
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WCUnlockForm);

};



#endif
