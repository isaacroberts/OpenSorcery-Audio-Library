//
//  BPMListener.cpp
//  
//
//  Created by Isaac Roberts on 8/20/20.
//

#include <stdio.h>

#include "BPMMonitor.h"
#include "RTGraphics/DebugWindow.h"

BPMMonitor::BPMMonitor()
{
	currentPositionInfo.resetToDefault();
	bpm = currentPositionInfo.bpm;
	timeSigNum = currentPositionInfo.timeSigNumerator;
	timeSigDen = currentPositionInfo.timeSigDenominator;
}
bool BPMMonitor::isInfoAvailable()
{
	//TODO: find out if info is available
	return true;
}
float BPMMonitor::getTimeSigFraction()
{
	return float(getTimeSigNum()) / getTimeSigDen();
}
int BPMMonitor::getMicrosecondsPerQuarterNote()
{
	return (int)(60000000./bpm);
}

float BPMMonitor::getBeatLen()
{
	return 60.f / bpm;
}
float BPMMonitor::getBarLen()
{
	return getBeatLen() * getBeatsInBar();
}
float BPMMonitor::getCurrentBeat()
{
	float ppq = currentPositionInfo.ppqPosition;
//	Get out of quarter-notes
	ppq /= 4;
	ppq *= getTimeSigDen();
	//modulus
//	ppq -= int(std::floor(ppq));
	return ppq;
}
float BPMMonitor::getCurrentBeatWithinBar()
{
	//Position in quarter notes
	float ppq = currentPositionInfo.ppqPosition - currentPositionInfo.ppqPositionOfLastBarStart;
	//Get out of quarter-notes
	ppq /= 4;
	//Get into current denom
	ppq *= getTimeSigNum();
	return ppq;
}
float BPMMonitor::getCurrentBar()
{
	float ppq = currentPositionInfo.ppqPosition;
	ppq /= 4;
	ppq *= getTimeSigDen();
	ppq /= getTimeSigNum();
	return ppq;
}
float BPMMonitor::getBeatAtTime(float seconds)
{
	//Seconds / 60(seconds/minute) * Bpm(beats/minute) = beats
	return seconds / 60. * bpm;
}
float BPMMonitor::getBarAtTime(float seconds)
{
	return getBeatAtTime(seconds)/ getBeatsInBar();
}
int BPMMonitor::getBeatsInBar()
{
	return getTimeSigNum();
}
float BPMMonitor::getBeatFraction()
{
	return getCurrentBeatWithinBar() / getBeatsInBar();
}
float BPMMonitor::getBeatOutOfN(int n)
{
	float ppq = currentPositionInfo.ppqPosition - currentPositionInfo.ppqPositionOfLastBarStart;
	//Some grace for early notes
//	if (currentPositionInfo.ppqPositionOfLastBarStart > 0)
//		ppq -= .1;
	//Get out of quarter-notes
	ppq /= 4;
	//modulus
	ppq -= int(std::floor(ppq));
	ppq *= n;
	return ppq;
}

double BPMMonitor::getBeatsPerSample(float rate)
{
	//seconds = (sample/rate)
	//beatN =  seconds / 60. * bpm;
	//beatN =  (sample/rate) / 60. * bpm;
	//beatN/sample = 1*bpm/(rate * 60)
	return bpm / (rate * 60.);
}
double BPMMonitor::getBarsPerSample(float rate)
{
	//seconds = (sample/rate)
	//beatN =  seconds / 60. * bpm;
	//barN = beatN / getBeatsInBar()
	
	//barN = (((sample/rate)) / 60. * bpm) / getBeatsInBar()
	//barN/sample = (((1/rate)) / 60. * bpm) / getBeatsInBar()
	//barN/sample = bpm/(rate*60*getBeatsInBar())
	
	return bpm/(rate * 60.  * getBeatsInBar());
}
void BPMMonitor::addListener(Listener* listener)
{
	listeners.add(listener);
	if (bpm > 0 ) {
		listener->bpmChanged(bpm, timeSigNum, timeSigDen);
	}
}
void BPMMonitor::removeListener(Listener* listener)
{
	listeners.remove(listener);
}
void BPMMonitor::update(AudioPlayHead* playHead)
{
	if (playHead == NULL) {
		return;
	}
	bool infoGot = playHead->getCurrentPosition (currentPositionInfo);
	if (infoGot)
		update(currentPositionInfo.bpm, currentPositionInfo.timeSigNumerator, currentPositionInfo.timeSigDenominator);
}
void BPMMonitor::updateBPMOnly(AudioPlayHead::CurrentPositionInfo& temp)
{
	update(temp.bpm, temp.timeSigNumerator, temp.timeSigDenominator);
}
void BPMMonitor::update(double newBpm, int timeNum, int timeDen) {
	if ( fabs(bpm - newBpm) > .01
		|| timeNum != timeSigNum
		|| timeDen != timeSigDen)
	{
		bpm = newBpm;
		timeSigNum = timeNum;
		timeSigDen = timeDen;
		updateBPM();
	}
}
void BPMMonitor::increment(int frames, float rate)
{
	currentPositionInfo.timeInSamples += frames;
	float t = frames / rate;
	currentPositionInfo.timeInSeconds += t;
	//60 / BPM = seconds
	//60 = seconds * BPM
	//60 / seconds = BPM
	float beatChange = t *  bpm / 60;
	currentPositionInfo.ppqPosition += beatChange;
	currentPositionInfo.ppqPositionOfLastBarStart = int(currentPositionInfo.ppqPosition/4) * timeSigDen;
}
void BPMMonitor::updateOrIncrementIfPaused(AudioPlayHead* playHead, int numFrames, float rate)
{
	AudioPlayHead::CurrentPositionInfo temp;
	bool infoGot = playHead->getCurrentPosition(temp);
	if (infoGot)
	{
		if (!temp.isPlaying) {
			updateBPMOnly(temp);
			increment(numFrames, rate);
		}
		else {
			update(playHead);
		}
	}
}
void BPMMonitor::updateOrIncrement(AudioPlayHead* playHead, int numFrames, float rate)
{
	if (playHead==NULL) {
		increment(numFrames, rate);
	}
	else {
		updateOrIncrementIfPaused(playHead, numFrames, rate);
	}
}

void BPMMonitor::updateBPM()
{
	if (!listeners.isEmpty())
	{
		MessageManager::callAsync([&](){inThread_updateBPM(); });
	}
}
void BPMMonitor::inThread_updateBPM()
{
	listeners.call([&](Listener& l) { l.bpmChanged(bpm, timeSigNum, timeSigDen); });
}
