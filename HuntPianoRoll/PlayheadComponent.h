//
//  PlayheadComponent.h
//  
//
//  Created by Isaac Roberts on 6/5/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class PlayheadComponent : public Component, Timer
{
public:
	PlayheadComponent(LiveMidi* midi);
	
	void setParentBounds(int x, int y, int w, int h);
	void setParentBounds(Rectangle<int> r) { setParentBounds(r.getX(), r.getY(), r.getWidth(), r.getHeight()); }

	void setColour(Colour set);
	void setColour(Colour playing, Colour paused);
	
	void setPixelsPerBar(float set) {
		pixelsPerBar = set;
		repaint();
	}
	
	void timerCallback() override;
	
	//Override these
	void paint(Graphics& g) override;
	//Used for repaint speedup
	virtual int getDesiredWidth() { return 2; }
	
	
	virtual ~PlayheadComponent();
	
protected:
	virtual void updateBounds();
	
protected:
	LiveMidi* midi;
	Colour active, paused;
	int parentX, parentY, parentW, parentH;
	float pixelsPerBar;
	float playheadX;
	bool wasPlaying;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayheadComponent);

};

class FlashingPlayheadComponent : public PlayheadComponent
{
public:
	FlashingPlayheadComponent(LiveMidi* midi) : PlayheadComponent(midi), flashColor(Colours::white), noteOn(false) {}
	
	void timerCallback() override;
	void paint(Graphics& g) override;
	int getDesiredWidth() override { return 2; }

	
	void setFlashColor(Colour f) { flashColor = f; }
protected:
//	void updateBounds() override;
	Colour flashColor;
	bool noteOn;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlashingPlayheadComponent);
};
