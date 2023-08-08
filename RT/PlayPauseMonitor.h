//
//  PlayPauseMonitor.h
//  
//
//  Created by Isaac Roberts on 10/19/21.
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

class PlayPauseMonitor {
public:
	class Listener {
	public:
		Listener() {}
		virtual void playPauseChanged(bool playing) = 0;
		virtual ~Listener() {}
	};

	PlayPauseMonitor()
	: prevPlaying(false)
	{
	}
	
	void update(AudioPlayHead* playHead) {
		if (playHead)
		{
			Optional<AudioPlayHead::PositionInfo> temp = playHead->getPosition();
			
			if (temp)
			{
				if (prevPlaying != (*temp).getIsPlaying())
				{
					prevPlaying = (*temp).getIsPlaying();
					updateListeners();
				}
			}
		}
	}
	
	virtual ~PlayPauseMonitor() {}
	
	void addGuiListener(Listener* listener) {
		guiListeners.add(listener);
	}
	void removeGuiListener(Listener* listener) {
		guiListeners.remove(listener);
	}

	void addBackendListener(Listener* listener) {
		backendListeners.add(listener);
	}
	void removeBackendListener(Listener* listener) {
		backendListeners.remove(listener);
	}
protected:
	ListenerList<Listener> guiListeners;
	ListenerList<Listener> backendListeners;
	
	bool prevPlaying=false;
	
	void updateListeners()
	{
		backendListeners.call([&](Listener& l) { l.playPauseChanged(prevPlaying); } );
		if (!guiListeners.isEmpty())
		{
			MessageManager::callAsync([&](){inThread_updateListeners(); });
		}
	}
	void inThread_updateListeners()
	{
		guiListeners.call([&](Listener& l) { l.playPauseChanged(prevPlaying); });
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayPauseMonitor);

};
