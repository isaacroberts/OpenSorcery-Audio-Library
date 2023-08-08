//
//  PlayheadComponent.cpp
//  
//
//  Created by Isaac Roberts on 6/5/22.
//

// Created by Isaac Roberts


#include "PlayheadComponent.h"


PlayheadComponent::PlayheadComponent(LiveMidi* m)
: Component(),
Timer(),
midi(m)
{
	startTimerHz(30);
	wasPlaying=false;
//	setOpaque(true);
}
void PlayheadComponent::setParentBounds(int x, int y, int w, int h)
{
	parentX = x;
	parentY = y;
	parentW = w;
	parentH = h;
	updateBounds();
}

void PlayheadComponent::timerCallback()
{
	if (midi->isPlaying())
	{
		wasPlaying=true;
		updateBounds();
	}
	else
	{
		if (wasPlaying)
		{
			wasPlaying=false;
			repaint();
		}
	}
}
void PlayheadComponent::updateBounds()
{
	playheadX = midi->getPosition() * pixelsPerBar;
	if (playheadX < parentW)
	{
		int w= getDesiredWidth();
		setBounds(parentX + playheadX-w/2, parentY, w, parentH);
	}
}
void PlayheadComponent::paint(Graphics& g)
{
//	const float playheadThickness = wasPlaying ? 2.0f : 2.0f;
	g.setColour(wasPlaying ? active : paused);
//	g.fillRect(0, 0, getWidth(), getHeight());
	g.fillAll();
}
void PlayheadComponent::setColour(Colour set)
{
	setColour(set, Colours::transparentWhite);
}
void PlayheadComponent::setColour(Colour on, Colour disabled)
{
	active = on;
	paused = disabled;
	if (!wasPlaying)
		repaint();
}
PlayheadComponent::~PlayheadComponent()
{
	
}




void FlashingPlayheadComponent::timerCallback()
{
	if (midi->isPlaying())
	{
		wasPlaying=true;
		noteOn = midi->wasNoteOnWithin(.05f);
		updateBounds();
	}
	else
	{
		if (wasPlaying)
		{
			wasPlaying=false;
			repaint();
		}
	}
}

//void FlashingPlayheadComponent::updateBounds()
//{
//	playheadX = midi->getPosition() * pixelsPerBar;
//	if (playheadX < parentW)
//	{
//		int w= getDesiredWidth();
//		setBounds(parentX + playheadX-w+1, parentY, w, parentH);
//	}
//}
void FlashingPlayheadComponent::paint(Graphics& g)
{
//	int playheadThickness = (wasPlaying && noteOn) ? 3 : 3;
	g.setColour(wasPlaying ? (noteOn ? flashColor : active) : paused);
//	int x = getWidth()/2;
//	g.fillRect(x-playheadThickness/2, 0, playheadThickness, getHeight());
	g.fillAll();
//	const int w = getWidth();
////	const int i = w/2;
//	const int h = getHeight();
//	if (wasPlaying && noteOn)
//	{
//		const int i2 = w-1;
////		const int w2 = w-1;
//		for (int n=0; n < i2; ++n)
//		{
////			float f1 = float(n)/(w);
//			float f2 = float(n+1)/(w);
//
//			g.setColour(active.withAlpha(.25f + .75f * f2));
//			g.drawVerticalLine(n, 0, h);
////			g.drawVerticalLine(w2-n, 0, h);
//		}
//		g.setColour(active);
//		g.drawVerticalLine(i2-1, 0, h);
////		g.setColour(Colours::white);
//		g.drawVerticalLine(i2, 0, h);
//	}
//	else
//	{
//		g.setColour(wasPlaying ? active : paused);
//		g.fillRect(w-2, 0, 2, h);
//	}
}
