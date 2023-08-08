//
//  PlayheadMonitor.h
//  
//
//  Created by Isaac Roberts on 6/10/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class PlayheadMonitor {
public:
	enum PlayMode {
		//DAW   = Actual position in DAW timeline
		//Extra = Position that continues since pause
//		DAWOnly=0,
////		DAWPlusExtra=1, DAWOrExtra=2,
////		ContinueAfterPause = 1,
////		FromZeroAfterPause = 2
//		StopOnPause,
//		PauseOnPause,
//		ContinueOnPause,
//		ContinueAndResetOnPause
		Default=0,
		PauseOnDAWPause = 1,
		ResetOnDAWPause = 2,
		MoveOnPausedDAWJump =4
		
		
	};
private:
	
public:
	class Listener {
	public:
		Listener(bool isGui);
		Listener(bool isGui, PlayheadMonitor*);
		Listener(bool isGui, PlayheadMonitor&);
		void setPlayhead(PlayheadMonitor* set);
		void setPlayhead(PlayheadMonitor& set);
		virtual ~Listener();
		
		//Getters
		float getPositionInBars(float modulus=-1);
		float getPositionInBeats(float modulus=-1);
		float getPositionInSeconds(float modulus=-1);
		float getBPM() { return monitor ? monitor->getBPM() : 120.f; }
		
		//Util
		int getMicrosecondsPerQuarterNote() { return monitor ? monitor->getMicrosecondsPerQuarterNote() : 0; }
		double getBarsPerSample() { return monitor ? monitor->getBarsPerSample() : .0001; }
		double getBarsPerSecond() { return monitor ? monitor->getBarsPerSecond() : 1.; }
		
		inline bool paused() { return !isPlaying(); }
		inline bool playing() { return isPlaying();}
		inline bool isPaused() { return !isPlaying(); }
		inline bool isPlaying() {
			return monitor ? monitor->isPlaying() : false;
		}
		//Setters
		void pause(bool reset=false, bool pauseDAWIfPossible=false) {setPlaying(false, reset, pauseDAWIfPossible); }
		void play(bool reset, bool playDAWIfPossible=false) {setPlaying(true, reset, playDAWIfPossible); }
		void setPlaying(bool set, bool reset=false, bool changeDAWIfPossible=false);
		void resetPosition() {if (monitor) monitor->resetPosition(this); }

		//Callbacks
		virtual void positionJumped(float from, float to) {}
		virtual void pauseChanged(bool isNowPlaying) {}
		virtual void bpmChanged(float set, int timeSigNum, int timeSigDen) {}
//		virtual PlayMode desiredMode() = 0;
		
		
		PlayheadMonitor* getMonitor() { return monitor; }
		bool isFromGui() { return isGui; }
		bool isFromBackend() { return !isGui; }
//	protected:
		//Setters
		
	private:
		void addThisAsListener();
	private:
		const bool isGui;
	protected:
		PlayheadMonitor* monitor;
	};
	PlayheadMonitor();
	virtual ~PlayheadMonitor();
	
	
	//Setters
	void pause(bool reset=false, bool pauseTransportIfPossible=false, Listener* src=NULL) { setPlaying(false, reset, pauseTransportIfPossible, src); }
	void play(bool reset, bool playTransportIfPossible=false, Listener* src=NULL) { setPlaying(true, reset, playTransportIfPossible, src); }
	void setPlaying(bool set, bool reset=false, bool changeTransportIfPossible=false, Listener* src=NULL);
	
	void resetPosition(Listener* src=NULL);
	
	//Getters
	float getPosition(); 
	float getPositionInBars();
	float getPositionInBeats();
	float getPositionInSeconds();
	float getBPM() { return bpm; }
	
	//Util
	int getMicrosecondsPerQuarterNote();
	float getMidiFactor(int ticks);
	double getBarsPerSample();
	double getBarsPerSecond();
	int getSamplesPerBar();
	
	double getBeatsPerSample();
	int getSamplesPerBeat();
	
	inline bool isPaused() { return !isPlaying(); }
	bool isPlaying();
	bool isInfoAvailable();
	
	int getTimeSigNum() { return timeSigNum; }
	int getTimeSigDen() { return timeSigDen; }
	
	float getTimeSigFraction();
	
	float getBeatLen();
	float getBarLen();
	
	float getCurrentBeat();
	float getCurrentBeatWithinBar();
	float getCurrentBar();
	float getBeatAtTime(float seconds);
	float getBarAtTime(float seconds);
	
	int getBeatsInBar();
	float getBeatFraction();
	float getBeatOutOfN(int n);
	
	void update(AudioPlayHead* playHead, int numFrames);
	
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
	void clearGuiListeners() { guiListeners.clear(); }
	void clearBackendListeners() { backendListeners.clear(); }
	void clearListeners() { guiListeners.clear(); backendListeners.clear(); }

	void setRate(float set) { rate = set; }
	void setPlayMode(PlayMode set) { playMode = set; }
	PlayMode getPlayMode() { return playMode; }

	bool isDawPlaying() { return dawPlaying; }
	bool isVirtualPlaying() { return virtualPlaying; }
	
protected:
	bool dawSigToTimeSig(AudioPlayHead::TimeSignature&);
	
	void updateBPMOnly(AudioPlayHead::PositionInfo&);
	void updateOrIncrementIfPaused(AudioPlayHead*, int numFrames);
	
	float ppqToBars(float ppq);
	float secToBars(float sec);

	//Different settings for different amounts of returned items
	//All 3
	bool updatePosSBP(int numFrames, int64 smp, int64 bars, double ppqPos);
	//Two of
	bool updatePosBP (int numFrames, int64 bars, double ppqPos);
	bool updatePosSP (int numFrames, int64 smp, double ppqPos);
	bool updatePosSB (int numFrames, int64 smp, int64 bars);
	//Only 1
	bool updatePosB  (int numFrames, int64 bars);
	bool updatePosP  (int numFrames, double ppqPos);
	bool updatePosS  (int numFrames, int64 smp);
	//Zero
	bool updatePosNa (int numFrames);

	
	
	void update(AudioPlayHead::PositionInfo&, int numFrames);
	void increment(int numFrames);

	void setDawPlaying(bool set, Listener* src=NULL);
	void setVirtualPlaying(bool set, Listener* src=NULL);
	void setPosition(float set, Listener* src=NULL);
//	void updatePlaying(Listener* src=NULL);
	
	void updatePlayingListeners(Listener* src=NULL);
	void inThread_updatePlaying(Listener* src=NULL);
	void updateBPM(Listener* src=NULL);
	void inThread_updateBPM(Listener* src=NULL);
	void updatePosition(float from, Listener* src=NULL);
	void inThread_updatePosition(float from, Listener* src=NULL);

	void updateTimePrecalcs();
	
protected:
	ListenerList<Listener> guiListeners;
	ListenerList<Listener> backendListeners;
	PlayMode playMode;

//	AudioPlayHead::PositionInfo currentPositionInfo;
	
	
	int64 samplePosition;
	int64 nextSamples;
	float barPosition;
	float bpm;
	int timeSigNum, timeSigDen;
	int origSigNum, origSigDen;
	bool playing;
	bool dawPlaying;
	bool virtualPlaying;
	int lastDawSamples;
	float rate;
	
	//Pre Calcs
	double barsPerSecond;
	double barsPerPpq;
	
//	int lastTptCtrl=-1;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayheadMonitor);

};
