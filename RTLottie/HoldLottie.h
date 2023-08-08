//
//  HoldLottie.h
//  
//
//  Created by Isaac Roberts on 6/6/22.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>

using namespace juce;

class HoldLottie : public RLottieComponent {
public:
	HoldLottie();
	HoldLottie(std::function<bool(void)>);

	void showAndPlay(std::function<bool(void)>);
	void play(std::function<bool(void)>);
	
	void showAndPlay() { RLottieComponent::showAndPlay(); }
	void play() { RLottieComponent::play(); }

	
//	void start();
	
	virtual ~HoldLottie();
	
	
	
protected:
	std::function<bool(void)> callback;
	
	void timerCallback() override;

	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HoldLottie);

};


class HoldLottieButton : public LottieButton
{
public:
	HoldLottieButton();
	HoldLottieButton(std::function<bool(void)>);

	std::function<bool(void)> shouldStopLottieCallback;

protected:
	void timerCallback() override;

};
