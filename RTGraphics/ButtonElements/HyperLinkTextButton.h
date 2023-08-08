//
//  HyperLinkTextButton.h
//  
//
//  Created by Isaac Roberts on 11/10/21.
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

class HyperLinkTextButton : public TextButton
{
public:
	HyperLinkTextButton(String text)
	:
	TextButton(text),
	url()
	{
//		onClick = [&]() {clicked();};
	}
	~HyperLinkTextButton()
	{}
	
	void setURL(String set)
	{
		url = URL(set);
	}
	void clicked() {
		if (url.isWellFormed())
			url.launchInDefaultBrowser();
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HyperLinkTextButton);
	
	URL url;
};
