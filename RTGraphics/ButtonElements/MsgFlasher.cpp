//
//  MsgFlasher.cpp
//  
//
//  Created by Isaac Roberts on 5/9/23.
//

// Created by Isaac Roberts


#include "MsgFlasher.h"



MsgFlasher::MsgFlasher()
: curMsg(""),
maxTime(2.0f),
curMaxTime(1.0f),
startTime(0),
flashing(false),
lastOpacity(0),
nextOpacity(0),
font(),
textColor(Colours::white)
{
	setInterceptsMouseClicks(false, false);
}

void MsgFlasher::flash(String msg, float time)
{
	curMsg = msg;
//	jassert(msgs[curMsg].svg);
	curMaxTime = time;
	if (!flashing)
	{
		if (MessageManager::existsAndIsCurrentThread())
			_startOnMessageThread();
		else
			MessageManager::callAsync([&]() { _startOnMessageThread(); });
	}
	else
	{
		startTime = Time::currentTimeMillis();
	}
}
void MsgFlasher::_startOnMessageThread()
{
	startTime = Time::currentTimeMillis();
	setVisible(true);
	startTimerHz(20);
	flashing=true;
}
void MsgFlasher::stop()
{
	curMsg = "";
	setVisible(false);
	flashing=false;
	stopTimer();
}
float MsgFlasher::getElapsedTime()
{
	int64 t = Time::currentTimeMillis();
	return (t - startTime)/1000.f / curMaxTime;
}
void MsgFlasher::timerCallback()
{
	if (flashing)
	{
		float time = getElapsedTime();
		
		if (time > 1)
			stop();
		else
		{
			nextOpacity = opacityCurve(time);
			if (fabs(nextOpacity-lastOpacity)>.01)
			{
//				lastOpacity = nextOpacity;
				repaint();
			}
		}
	}
	else
		stop();
}
float MsgFlasher::opacityCurve(float t)
{
	float opacity = 1- t;
	//Release after half
	opacity = jlimit(0.f, 1.0f, opacity*2);
	//Attack time
	opacity *= std::min(t*5, 1.0f);
	return opacity;
}
void MsgFlasher::paint(Graphics& g)
{
	if (flashing)
	{
		g.setFont(font);
		g.setColour(textColor.withAlpha(nextOpacity));
		
		g.drawFittedText(curMsg, getBounds(), Justification::centred, 2);
		lastOpacity = nextOpacity;
	}
}
void MsgFlasher::resized()
{
}
						 
MsgFlasher::~MsgFlasher()
{
}

