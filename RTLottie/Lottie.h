//
//  Lottie.h
//  
//	MIT-Licensed HISE Lottie, edited to use static library since the 2019 RLottie MIT Release
//
//  Edited by Isaac Roberts on 6/6/22.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>

using namespace juce;

class Lottie {
public:
	Lottie(juce::String data);
	
	void render(Graphics& g, int x, int y);
	
	/** Sets the size of the internal canvas. Call this before rendering. */
	void setSize(int width, int height);

	/** Returns the number of frames in the animation. */
	int getNumFrames() const { return frames; }

	/** Returns the framerate of the given animation. */
	double getFrameRate() const { return rate; }
	
	/** Checks whether the animation could be parsed correctly. */
	bool isValid() const;

	/** Sets the frame. Call this before calling render. */
	void setFrame(int frameNumber);
	void setFrameNormalized(float n);
	
	void advanceFrame();
	//Returns isDone
	bool advanceWithoutLooping();
	
	float getCurrentFrameNormalized() const;
	
	/** Returns the current frame. */
	int getCurrentFrame() const;

	/** Set a scale factor that is applied to the internal canvas. */
	void setScaleFactor(float newScaleFactor);
	
	
	virtual ~Lottie();
	
protected:
	Image canvas;
	Lottie_Animation* anim;
	int origW, origH;
	float scale;
	
	int frames;
	int lastFrame;
	int curFrame;
	double rate;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Lottie);

};
