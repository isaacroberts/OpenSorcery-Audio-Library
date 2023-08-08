//
//  LottieButton.h
//  
//
//  Created by Isaac Roberts on 6/12/22.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>

using namespace juce;

class LottieButton : public juce::Button, public juce::Timer
{
public:
	/*
			Shows frame 0 when not clicked
				Plays lottie when clicked
			Does not toggle
	 
	 */
	
	LottieButton();
	
	void loadLottie(const String& jsonCode, bool useOversampling=true);
	void timerCallback() override;
	
	/** Sets the background colour. If you pass in a opaque colour it might improve the rendering performance. */
	void setBackgroundColour(Colour c);
	void setTransparentBackground();
protected:
	void paintButton (Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
	void internalClickCallback (const ModifierKeys& mods) override;

	void setPlaying(bool set);
	
	/** Starts playing the animation with the given framerate. */
	void play();

	/** Stops the playback. */
	void stop();

public:
	/** Load an animation from the JSON code. */

	/** @internal */
	void resized() override;

	/** @internal */
//	void paint(Graphics& g) override;

	
	virtual ~LottieButton();
	
protected:
	std::unique_ptr<Lottie> currentAnimation;
	juce::Colour bgColor;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LottieButton);

};
