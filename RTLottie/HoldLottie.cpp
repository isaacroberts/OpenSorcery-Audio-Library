//
//  HoldLottie.cpp
//  
//
//  Created by Isaac Roberts on 6/6/22.
//

// Created by Isaac Roberts


//#include "HoldLottie.h"


HoldLottie::HoldLottie()
: RLottieComponent(),
callback([](){return true; })
{
	
}
HoldLottie::HoldLottie(std::function<bool(void)> cb)
: RLottieComponent(),
callback(cb)
{
}
HoldLottie::~HoldLottie()
{
	
}
void HoldLottie::showAndPlay(std::function<bool(void)> cb)
{
	callback = cb;
	showAndPlay();
}
void HoldLottie::timerCallback()
{
	if (callback())
		hideAndStop();
	else
		RLottieComponent::timerCallback();
}

HoldLottieButton::HoldLottieButton()
: LottieButton(),
shouldStopLottieCallback([]() { return true;})
{
	
}
HoldLottieButton::HoldLottieButton(std::function<bool(void)> cb)
: LottieButton(),
shouldStopLottieCallback(cb)
{
}
void HoldLottieButton::timerCallback()
{
	if (currentAnimation != nullptr && currentAnimation->getNumFrames() > 0)
	{
		if (shouldStopLottieCallback())
		{
			currentAnimation->setFrame(0);
			stopTimer();
		}
		else
		{
			currentAnimation->advanceFrame();
		}
		repaint();
	}
}
