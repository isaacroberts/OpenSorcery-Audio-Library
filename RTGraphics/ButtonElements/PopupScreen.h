//
//  PopupScreen.h
//  
//
//  Created by Isaac Roberts on 8/10/21.
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

class PopupScreen : public Component
{
public:
	PopupScreen() : Component(),
	ownedComponent(NULL),
	desiredW(250), desiredH(100)
	{
//		setFocusContainer(true);
		setAlwaysOnTop(true);
		onHide = [&](){};
	}
	PopupScreen(Component* owned) : Component(),
	ownedComponent(owned),
	desiredW(250), desiredH(100)
	{
		addAndMakeVisible(ownedComponent);
//		setFocusContainer(true);
		setAlwaysOnTop(true);
		onHide = [&](){};
	}
	PopupScreen(Component& owned) : Component(),
	ownedComponent(&owned),
	desiredW(250), desiredH(100)
	{
		addAndMakeVisible(ownedComponent);
		setFocusContainerType(FocusContainerType::focusContainer);
		setAlwaysOnTop(true);
		onHide = [&](){};
	}

	void setOwnedComponent(Component* set) {
		ownedComponent = set;
		if (ownedComponent)
			addAndMakeVisible(ownedComponent);
	}
	void setDesiredBounds(float w, float h) {
		desiredW = w;
		desiredH = h;
		//Must call resize after this
	}
	
	void toggle_show()
	{
		if (isVisible())
			hide();
		else
			show();
	}
	void show()
	{
		setVisible(true);
	}
	void hide()
	{
		bool isVis = isVisible();
		setVisible(false);
		if (isVis) {
			onHide();
		}
	}
	void focusLost(FocusChangeType cause) override
	{
		hide();
	}
	void mouseDown(const MouseEvent &e) override
	{
		//Will only arrive at this method if it's in the margin zone
		onHide = [&](){};
		hide();
	}
	
	void resized() override {
		setAlwaysOnTop(true);

		float wmax = getWidth() * .98;
		float hmax = getHeight() * .8;
		
		float iw = std::min(wmax, desiredW);
		float ih = std::min(hmax, desiredH);
		
		float ratio = desiredW / desiredH;
		
		float rw = std::min(iw, ih*ratio);
		float rh = std::min(ih, iw/ratio);

		//Resized owned component first
		if (ownedComponent)
			ownedComponent->setBounds((getWidth()-rw)/2, (getHeight()-rh)/2, rw, rh);
	}
	
	virtual ~PopupScreen() {}
	
	std::function<void()> onHide;
	
private:
	Component* ownedComponent;
	float desiredW, desiredH;
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupScreen);

};
