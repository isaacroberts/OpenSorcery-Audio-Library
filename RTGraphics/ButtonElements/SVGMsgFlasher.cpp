//
//  SVGMsgFlasher.cpp
//  
//
//  Created by Isaac Roberts on 6/7/22.
//

// Created by Isaac Roberts


#include "SVGMsgFlasher.h"



void SVGMsgFlasher::SVGMsg::load(juce::String setmsg, const char* data, size_t size)
{
	txt = setmsg;
	if (data != NULL)
		svg = Drawable::createFromImageData(data, size);
}
SVGMsgFlasher::SVGMsgFlasher(int msgCt)
: msgAmt(msgCt), msgs(new SVGMsg[msgCt]),
maxTime(2.0f),
curMaxTime(1.0f),
startTime(0),
curMsg(0),
flashing(false),
lastOpacity(0),
nextOpacity(0),
font(),
textColor(Colours::white)
{
	setInterceptsMouseClicks(false, false);
}

void SVGMsgFlasher::flash(int msg, float time)
{
	jassert(isPositiveAndBelow(msg, msgAmt));
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
void SVGMsgFlasher::_startOnMessageThread()
{
	startTime = Time::currentTimeMillis();
	setVisible(true);
	startTimerHz(20);
	flashing=true;
}
void SVGMsgFlasher::stop()
{
	setVisible(false);
	flashing=false;
	stopTimer();
}
float SVGMsgFlasher::getElapsedTime()
{
	int64 t = Time::currentTimeMillis();
	return (t - startTime)/1000.f / curMaxTime;
}
void SVGMsgFlasher::timerCallback()
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
float SVGMsgFlasher::opacityCurve(float t)
{
	float opacity = 1- t;
	//Release after half 
	opacity = jlimit(0.f, 1.0f, opacity*2);
	//Attack time 
	opacity *= std::min(t*5, 1.0f);
	return opacity;
}
void SVGMsgFlasher::paint(Graphics& g)
{
	if (flashing)
	{
		g.setFont(font);
		g.setColour(textColor.withAlpha(nextOpacity));
		
//		g.drawRect(txtBounds);
//		g.drawRect(svgBounds);
		
		if (msgs[curMsg].svg)
		{
			g.drawFittedText(msgs[curMsg].txt, txtBounds, Justification::centred, 2);
			msgs[curMsg].svg->drawWithin (g, svgBounds, RectanglePlacement::centred, nextOpacity);
		}
		else
		{
			g.drawFittedText(msgs[curMsg].txt, getBounds(), Justification::centred, 2);
		}
		lastOpacity = nextOpacity;
	}
}
void SVGMsgFlasher::resized()
{
	float splt = getHeight() * .333;
	auto r = getBounds().withPosition(0,0);
	svgBounds = r.toFloat().withTop(splt);
	txtBounds = r.withBottom(splt);
}
						 
SVGMsgFlasher::~SVGMsgFlasher()
{
	delete[] msgs;
}

