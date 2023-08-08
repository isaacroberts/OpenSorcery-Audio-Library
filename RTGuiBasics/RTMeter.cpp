//
//  RTMeter.cpp
//  
//
//  Created by Isaac Roberts on 8/9/20.
//

// Created by Isaac Roberts


//#include "RTMeter.h"

RTMeter::RTMeter()
: Component(), Timer(),
min(-60), max(0),
callbackInRMS(false)
{
	level = 0 ;
	callback = [](){return 0.f;};
//	repaint();
	startTimerHz(60);
}
void RTMeter::startTimer()
{
	//???
}
void RTMeter::paint(Graphics& g)
{
	float relLevel = (level-min)/(max-min);
	if (relLevel < 0) relLevel = 0;
	else if (relLevel > 1) relLevel = 1;
	getLookAndFeel().drawMeter(g, 0, 0,
				   relLevel, level, min, max,
				   getWidth(), getHeight(),
				   this);
}
void RTMeter::timerCallback()
{
	float newLevel = callback();
	
	if (callbackInRMS)
		newLevel = 20 * log10(newLevel);
		
	//If level changed
	if (fabs(level - newLevel) > .0001)
	{
		//If both above max
		if ((newLevel > max) && (level > max) )
		{
			level = newLevel;
		}
		else if ((newLevel < min) && (level < min))
		{
			level = newLevel;
		}
		else
		{
			level = newLevel;
			repaint();
		}
	}
	//else don't update the level - this prevents creeping changes from being ignored
	
}

RTMeter::~RTMeter()
{
	
}



