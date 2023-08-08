//
//  HyperlinkImageButton.h
//  Tremelon - Shared Code
//
//  Created by Isaac Roberts on 6/12/20.
//

// Created by Isaac Roberts 

#ifndef HyperlinkImageButton_h
#define HyperlinkImageButton_h

#include <stdio.h>

#include <JuceHeader.h>

class HyperLinkToolbarButton : public ToolbarButton
{
public:
	HyperLinkToolbarButton(int idd, const String& filename, std::unique_ptr<Drawable> d, std::unique_ptr<Drawable> d2)
	:
	ToolbarButton(idd, filename, std::move(d), std::move(d2)),
	url()
	{
		onClick = [&]() {clicked();};
	}
	~HyperLinkToolbarButton()
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
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HyperLinkToolbarButton);
	
	URL url;
};

#endif /* HyperlinkImageButton_h */
