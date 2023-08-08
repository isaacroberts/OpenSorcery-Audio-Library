//
//  AboutScreen.h
//  Tremelon - Shared Code
//
//  Created by Isaac Roberts on 6/10/20.
//

// Created by Isaac Roberts 

#ifndef AboutScreen_h
#define AboutScreen_h

#include <stdio.h>

#include <JuceHeader.h>


class AboutScreen : public ImageComponent
{
public:
	AboutScreen();
//	AboutScreen(const AboutScreen& other);
//	AboutScreen& operator=(const AboutScreen& other);
	
	
	virtual ~AboutScreen();
	
	void toggle_show();
	void show();
	void hide();
	
//	void focusGained(FocusChangeType cause)override;
	void focusLost(FocusChangeType cause)override;
	
	void mouseDown(const MouseEvent & e)override;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AboutScreen);

	
	Rectangle<int> imgBounds;
};

#endif /* AboutScreen_h */
