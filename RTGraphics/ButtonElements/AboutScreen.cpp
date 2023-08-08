//
//  AboutScreen.cpp
//  Tremelon - Shared Code
//
//  Created by Isaac Roberts on 6/10/20.
//

// Created by Isaac Roberts

#include "AboutScreen.h"



AboutScreen::AboutScreen()
:
ImageComponent(),
imgBounds(6, 98, 468, 578)
{
	setFocusContainerType(FocusContainerType::focusContainer);
	setAlwaysOnTop(true);
}
void AboutScreen::toggle_show()
{
	if (isVisible())
		hide();
	else
		show();
}
void AboutScreen::show()
{
	setVisible(true);
//	setEnabled(true);
}
void AboutScreen::hide()
{
	setVisible(false);
//	setEnabled(false);
}
void AboutScreen::focusLost(FocusChangeType cause)
{
	hide();
}
void AboutScreen::mouseDown(const MouseEvent &e)
{
	Point<int> p = e.getMouseDownPosition();
	
	if (! imgBounds.contains(p))
	{
		hide();
	}
}
//Destructor
AboutScreen::~AboutScreen()
{
	
}

