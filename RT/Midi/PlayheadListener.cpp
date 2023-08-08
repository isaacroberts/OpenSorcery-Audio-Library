//
//  PlayheadListener.cpp
//  
//
//  Created by Isaac Roberts on 6/10/22.
//

#include "PlayheadMonitor.h"


PlayheadMonitor::Listener::Listener(bool isGui_)
: isGui(isGui_),
monitor(NULL)
{
	
}
PlayheadMonitor::Listener::Listener(bool isGui_, PlayheadMonitor* m)
: isGui(isGui_),
monitor(m)
{
	addThisAsListener();
}
PlayheadMonitor::Listener::Listener(bool isGui_, PlayheadMonitor& m)
: isGui(isGui_),
monitor(&m)
{
	addThisAsListener();
}
void PlayheadMonitor::Listener::setPlayhead(PlayheadMonitor* set)
{
	monitor = set;
	addThisAsListener();
}
void PlayheadMonitor::Listener::setPlayhead(PlayheadMonitor& set)
{
	monitor = &set;
	addThisAsListener();
}
PlayheadMonitor::Listener::~Listener()
{
	if (monitor)
	{
		if (isGui)
			monitor->removeGuiListener(this);
		else
			monitor->removeBackendListener(this);
	}
}
void PlayheadMonitor::Listener::addThisAsListener()
{
	jassert(monitor);
	if (isGui)
		monitor->addGuiListener(this);
	else
		monitor->addBackendListener(this);
}


void PlayheadMonitor::Listener::setPlaying(bool set, bool reset, bool changeDAWIfPossible) {
	if (monitor)
	{
		monitor->setPlaying(set, reset, changeDAWIfPossible, this);
		pauseChanged(set);
	}
}


float PlayheadMonitor::Listener::getPositionInBars(float modulus)
{
	if (!monitor) return 0;
	float pos = monitor->getPositionInBars();
	if (modulus>0)
	{
		if (modulus==1.f)
			pos -= int(pos);
		else
			pos = fmod(pos, modulus);
	}
	return pos;
}
float PlayheadMonitor::Listener::getPositionInBeats(float modulus)
{
	if (!monitor) return 0;
	float pos = monitor->getPositionInBeats();
	if (modulus>0)
	{
		if (modulus==1.f)
			pos -= int(pos);
		else
			pos = fmod(pos, modulus);
	}
	return pos;
}
float PlayheadMonitor::Listener::getPositionInSeconds(float modulus)
{
	if (!monitor) return 0;
	float pos = monitor->getPositionInSeconds();
	if (modulus>0)
	{
		if (modulus==1.f)
			pos -= int(pos);
		else
			pos = fmod(pos, modulus);
	}
	return pos;
}
