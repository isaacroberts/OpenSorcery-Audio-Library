//
//  LottieButton.cpp
//  
//
//  Created by Isaac Roberts on 6/12/22.
//

// Created by Isaac Roberts


//#include "LottieButton.h"


LottieButton::LottieButton()
: Button("LottieButton"), Timer()
{
}

LottieButton::~LottieButton()
{
	
}

void LottieButton::loadLottie(const String& jsonCode, bool useOversampling)
{
	currentAnimation = std::make_unique<Lottie>(jsonCode);

	if (useOversampling)
		currentAnimation->setScaleFactor(2.0f);

	resized();
	repaint();
}


void LottieButton::timerCallback()
{
	if (currentAnimation != nullptr && currentAnimation->getNumFrames() > 0)
	{
		bool isDone = currentAnimation->advanceWithoutLooping();
		if (isDone)
		{
			stopTimer();
		}
		repaint();
	}
}

void LottieButton::setPlaying(bool set)
{
	if (set) play();
	else stop();
}
void LottieButton::stop()
{
	stopTimer();
	currentAnimation->setFrame(0);
	repaint();
}
void LottieButton::play()
{
	if (currentAnimation != nullptr)
	{
		auto frameRate = currentAnimation->getFrameRate();

		if (frameRate > 0.0)
			startTimer(1000.0 / frameRate);
		else jassertfalse;
	}
}


void LottieButton::paintButton(Graphics& g, bool highlight, bool down)
{
	if (bgColor.getAlpha()>0)
		g.fillAll(bgColor);

	if (currentAnimation != nullptr)
	{
		currentAnimation->render(g, 0, 0);
	}
}

void LottieButton::internalClickCallback (const ModifierKeys& mods)
{
	Button::internalClickCallback(mods);
	if (currentAnimation)
	{
		currentAnimation->setFrame(0);
		play();
	}
}


void LottieButton::resized()
{
	if (currentAnimation != nullptr)
	{
		currentAnimation->setSize(getWidth(), getHeight());
	}
}

void LottieButton::setBackgroundColour(Colour c)
{
	bgColor = c;
	setOpaque(c.getAlpha() == 0xff);
}
void LottieButton::setTransparentBackground()
{
	bgColor = Colours::transparentWhite;
	setOpaque(false);
}
