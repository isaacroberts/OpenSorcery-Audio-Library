//
//  BPMListener.h
//  
//
//  Created by Isaac Roberts on 8/20/20.
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

class BPMMonitor {
public:
	class Listener {
	public:
		Listener() {}
		
		virtual void bpmChanged(double set, int timeSig, int timeDen) = 0;
		
		virtual ~Listener() {}
		
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Listener);
		
	};
	
	BPMMonitor();
	
	double getBPM() { return bpm; }
	int getMicrosecondsPerQuarterNote();
	
	void update(AudioPlayHead* playHead);
	void update(double bpm, int timeNum=4, int timeDen=4);
	void increment(int numFrames, float rate);
	void updateOrIncrement(AudioPlayHead* playHead, int numFrames, float rate);
	
//	void update();
	
	bool isInfoAvailable();
	
	int getTimeSigNum() { return currentPositionInfo.timeSigNumerator; }
	int getTimeSigDen() { return currentPositionInfo.timeSigDenominator; }
	
	float getTimeSigFraction();
	
	float getBeatLen();
	float getBarLen();
	
	float getCurrentBeat();
	float getCurrentBeatWithinBar();
	float getCurrentBar();
	float getBeatAtTime(float seconds);
	float getBarAtTime(float seconds);

	double getBeatsPerSample(float rate);
	double getBarsPerSample(float rate);
	
	int getBeatsInBar();
	float getBeatFraction();
	float getBeatOutOfN(int n);
	
	void clear() { listeners.clear();}
	
	void addListener(Listener* listener);
	void removeListener(Listener* remove);
	
	~BPMMonitor() { }
	
private:
	void updateBPMOnly(AudioPlayHead::CurrentPositionInfo&);
	
	void updateOrIncrementIfPaused(AudioPlayHead*, int numFrames, float rate);
	
	void updateBPM();
	void inThread_updateBPM();

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BPMMonitor);
	
	AudioPlayHead::CurrentPositionInfo currentPositionInfo;
	ListenerList<Listener> listeners;
	double bpm ;
	int timeSigNum, timeSigDen;
};
