//
//  FlatImagePresetBar.h
//  
//
//  Created by Isaac Roberts on 7/14/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "../CommonLAF.h"

using namespace juce;

class FlatImageToolbar : public CommonLAF
{
public:
	FlatImageToolbar(Image i);

	void drawDrawableButton (Graphics &, DrawableButton &, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
	void drawImageButton (Graphics &, Image *, int imageX, int imageY, int imageW, int imageH, const Colour &overlayColour, float imageOpacity, ImageButton &)override;
	void drawComboBox (Graphics &, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox &)override;

	Font getFont();
	Font getLabelFont (Label &)override;
	
	void paintToolbarBackground (Graphics &, int width, int height, Toolbar &) override;
	
	Label* createComboBoxTextBox(ComboBox& c)override;
	void positionComboBoxText (ComboBox &, Label &labelToPosition)override;
	Font getComboBoxFont (ComboBox &)override;
//	void paintToolbarButtonLabel (Graphics &, int x, int y, int width, int height, const String &text, ToolbarItemComponent &)override;

	void drawLabel (Graphics &, Label &)override;
	
	void drawPopupMenuItem (Graphics &, const Rectangle< int > &area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const String &text, const String &shortcutKeyText, const Drawable *icon, const Colour *textColour)override;
	
	Font getMenuBarFont (MenuBarComponent &, int itemIndex, const String &itemText)override;

//	int getMenuBarItemWidth (MenuBarComponent &, int itemIndex, const String &itemText)override;
//	void getIdealPopupMenuItemSize (const String &text, bool isSeparator, int standardMenuItemHeight, int &idealWidth, int &idealHeight)override;

	
	virtual ~FlatImageToolbar();
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlatImageToolbar);
	
	const int lTextOffset=3;
	
	Image image;
};
