//
//  FlatImagePresetBar.cpp
//  
//
//  Created by Isaac Roberts on 7/14/20.
//

// Created by Isaac Roberts


#include "FlatImageToolbar.h"


FlatImageToolbar::FlatImageToolbar(Image i)
: CommonLAF(),
image(i)
{
}

FlatImageToolbar::~FlatImageToolbar()
{
	
}
void FlatImageToolbar::paintToolbarBackground (Graphics &g, int width, int height, Toolbar &t)
{
//	std::cout<<"draw toolbar bg"<<std::endl;
	g.drawImageAt(image,0, 0);
}

void FlatImageToolbar::drawDrawableButton (Graphics& g, DrawableButton& button, bool highlighted, bool down)
{
//	std::cout<<"im drawing button"<<&button<<std::endl;
}
void FlatImageToolbar::drawImageButton (Graphics& g, Image* i, int imageX, int imageY, int imageW, int imageH, const Colour &overlayColour, float imageOpacity, ImageButton &)
{
//	std::cout<<"im drawing image button"<<std::endl;

}
void FlatImageToolbar::drawComboBox (Graphics& g, int w, int h, bool isButtonDown, int x, int y, int buttonW, int buttonH, ComboBox& comboBox)
{
	//Dropdown button for comboBox
	
//	std::cout<<"im drawing combo box"<<std::endl;
//	g.setColour(Colour(0,0,0));
//	g.fillRoundedRectangle(0, 0, comboBox.getWidth(), comboBox.getHeight(), 2.5);

}
Label* FlatImageToolbar::createComboBoxTextBox(ComboBox &c)
{
	Label* l = new Label(String(), String());
	
	l->setFont(getComboBoxFont(c));
	
	return l;
}
Font FlatImageToolbar::getFont()
{
//	std::cout<<"font"<<std::endl;
	return Font ("Sans-serif", 15, 0);
}
Font FlatImageToolbar::getComboBoxFont (ComboBox& comboBox)
{
	return getFont();
}
Font FlatImageToolbar::getMenuBarFont (MenuBarComponent &m, int itemIndex, const String &itemText)
{
	return getFont();
}
Font FlatImageToolbar::getLabelFont(Label& l)
{
	return getFont();
}
void FlatImageToolbar::positionComboBoxText (ComboBox& comboBox, Label &labelToPosition)
{//relative to comboBox
	Rectangle<int> bounds = Rectangle<int>();
	bounds.setX(0);
	bounds.setY(0);
	bounds.setHeight(comboBox.getHeight());
	bounds.setWidth(comboBox.getWidth());
	labelToPosition.setBounds(bounds);
}

void FlatImageToolbar::drawLabel (Graphics& g, Label &label)
{
	//cheat
//	g.setColour(Colour(0,0,0));
//	g.fillRoundedRectangle(0, 0, label.getWidth(), label.getHeight(), 2.5);
	
//	std::cout<<"im drawing label"<<std::endl;
	
	if (! label.isBeingEdited())
	{
		auto alpha = label.isEnabled() ? 1.0f : 0.5f;
		const Font font (getLabelFont (label));

		g.setColour (label.findColour (Label::textColourId).withMultipliedAlpha (alpha));
		g.setFont (font);

		Rectangle<int> textArea = getLabelBorderSize (label).subtractedFrom (label.getLocalBounds());

		textArea.setX(textArea.getX()+lTextOffset);
		textArea.setY(textArea.getY());
//		textArea.setHeight(50);
//		textArea.setWidth(300);
		
//		int x = label.
		
//		g.drawText(te)
		
		g.drawText (label.getText(), textArea, Justification::left);
	}
}
void FlatImageToolbar::drawPopupMenuItem(Graphics &g, const Rectangle<int> &area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const String &text, const String &shortcutKeyText, const Drawable *icon, const Colour *textColour)
{
	g.setColour(Colour(0,0,0));
	g.fillRect(area);
	if (textColour == NULL)
	{
		g.setColour(Colour(0,250,0));
	}
	else
		g.setColour(*textColour);
	g.setFont(getFont());
	g.drawText(text, area.getX()+lTextOffset, area.getY(), area.getWidth(), area.getHeight(), Justification::left);
}
//void FlatImageToolbar::getIdealPopupMenuItemSize (const String &text, bool isSeparator, int standardMenuItemHeight, int &idealWidth, int &idealHeight)
//{
////	std::cout<<"returnning "<<presetBox->getWidth()<<std::endl;
////	idealWidth = presetBox->getWidth();
////	idealHeight = presetBox->getHeight();
//}


//int FlatImagePresetBar::getMenuBarItemWidth(MenuBarComponent &m, int itemIndex, const String &itemText)
//{
//	return presetBox->getWidth();
//}
