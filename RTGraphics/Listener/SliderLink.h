//
//  SliderLink.h
//  
//
//  Created by Isaac Roberts on 2/20/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class SliderLink : public juce::Slider::Listener
{
public:
	SliderLink()
	: sliders(),
	active(true)
	{}
	
	void addSlider(Slider *s) {
		s->addListener(this);
		sliders.push_back(s);
	}
	
	void setActive(bool set) {
		active = set;
	}
	
	void sliderValueChanged (Slider *s) override {doCallback(s);}
	void sliderDragStarted (Slider *s) override {doCallback(s);}
	void sliderDragEnded (Slider *s) override {doCallback(s);}

	virtual ~SliderLink() {}
	
private:
	
	void doCallback(Slider* caller)
	{
		if (active)
		{
			float value = caller->getValue();
			
			for (int n=0; n < sliders.size(); ++n)
			{
				if (sliders[n] != caller)
				{
					sliders[n]->setValue(value, dontSendNotification);
				}
			}
		}
	}
	
	std::vector<Slider*> sliders;
	bool active;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderLink);

};
