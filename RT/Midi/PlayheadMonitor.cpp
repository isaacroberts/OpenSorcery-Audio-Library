//
//  PlayheadMonitor.cpp
//  
//
//  Created by Isaac Roberts on 6/10/22.
//

// Created by Isaac Roberts


#include "PlayheadMonitor.h"

#include "RTGraphics/DebugWindow.h"

//Getters

float PlayheadMonitor::getPositionInSeconds()
{
	return samplePosition/rate;
}
float PlayheadMonitor::getPosition()
{
	return barPosition;
}
float PlayheadMonitor::getPositionInBars()
{
	return barPosition;
}
float PlayheadMonitor::getPositionInBeats()
{
	return barPosition * timeSigDen;
}

float PlayheadMonitor::getBeatFraction()
{
	return getCurrentBeatWithinBar() / getBeatsInBar();
}
float PlayheadMonitor::getBeatOutOfN(int n)
{
	//	bar = ppq /4 * timeSigDen / timeSigNum;
	// bar * frac = ppq/4
//	float ppq = currentPositionInfo.ppqPosition - currentPositionInfo.ppqPositionOfLastBarStart;
//	ppq /= 4;
	
	float ppq = barPosition * getTimeSigFraction();
	
	//modulus
	ppq -= int(std::floor(ppq));
	ppq *= n;
	return ppq;
}
float PlayheadMonitor::getCurrentBeatWithinBar()
{
	return (barPosition-int(barPosition)) * timeSigDen;
}
float PlayheadMonitor::getCurrentBar()
{
	return barPosition;
}
float PlayheadMonitor::getCurrentBeat()
{
	return barPosition * getBeatsInBar();
}

bool PlayheadMonitor::isPlaying() {
	return playing;
}

PlayheadMonitor::PlayheadMonitor()
:
samplePosition(0),
nextSamples(0),
barPosition(0),
bpm(120),
timeSigNum(4), timeSigDen(4),
origSigNum(4), origSigDen(4),
playing(false),
dawPlaying(false),
virtualPlaying(false),
rate(696969)
{
//	currentPositionInfo.resetToDefault();
	updateTimePrecalcs();
}
PlayheadMonitor::~PlayheadMonitor()
{
	//Might be worth it to clear monitor value on each listener here
	jassert(guiListeners.size()==0);
	jassert(backendListeners.size()==0);
}


bool PlayheadMonitor::isInfoAvailable()
{
	//TODO: find out if info is available
	return true;
}
float PlayheadMonitor::getTimeSigFraction()
{
	return float(getTimeSigNum()) / getTimeSigDen();
}
int PlayheadMonitor::getMicrosecondsPerQuarterNote()
{
	return (int)(60000000./bpm);
}
//#include "RTGraphics/FileLoad.h"
float PlayheadMonitor::getMidiFactor(int ticksPerQn)
{
//	if (Format::getHostType().isLogic())
	return ticksPerQn * 4 * getTimeSigFraction();
//	if (Format::isAU())
//	{
//		else
//			return ticksPerQn * timeSigNum/timeSigDen;
//	}
//	else
//	{
//		return ticksPerQn * 4;
//	}
}

void PlayheadMonitor::updateTimePrecalcs()
{
	if (Format::getHostType().isFruityLoops())
	{
		barsPerPpq = 1./timeSigNum;
		barsPerSecond = double(bpm) / (60 * timeSigNum);
	}
	else
	{
		//bars/sec = double(bpm) * timeSigDen / (60 * 4 * timeSigNum);
		barsPerPpq = timeSigDen / (4 * timeSigNum);
		barsPerSecond = double(bpm) * timeSigDen / (60 * 4 * timeSigNum);
	}

}
float PlayheadMonitor::ppqToBars(float ppq)
{
	return ppq * barsPerPpq;
//	float b = ppq /4 * timeSigDen / timeSigNum;
//	return b;
}
float PlayheadMonitor::secToBars(float sec)
{
	return sec * barsPerSecond;
//	return sec * double(bpm) * timeSigDen / (60 * 4 * timeSigNum);
}

double PlayheadMonitor::getBarsPerSample()
{
	return barsPerSecond/rate;
//	return double(bpm) * timeSigDen / (60 * 4 * timeSigNum * rate);
}
int PlayheadMonitor::getSamplesPerBar()
{
	return rate/barsPerSecond;
//	float sp = (60 * 4 * timeSigNum * rate) / (bpm * timeSigDen);
//	return std::round(sp);
}
double PlayheadMonitor::getBeatsPerSample()
{
	return getBeatLen() / rate;
}
int PlayheadMonitor::getSamplesPerBeat()
{
	return std::round(rate / getBeatLen());
}
double PlayheadMonitor::getBarsPerSecond()
{
	return barsPerSecond;
//	return double(bpm) * timeSigDen / (60 * 4 * timeSigNum);
}
float PlayheadMonitor::getBeatLen()
{
	return 60.f / bpm;
}
float PlayheadMonitor::getBarLen()
{
	return getBeatLen() * getBeatsInBar();
}
float PlayheadMonitor::getBeatAtTime(float seconds)
{
	//Seconds / 60(seconds/minute) * Bpm(beats/minute) = beats
	return seconds / 60. * bpm;
}
float PlayheadMonitor::getBarAtTime(float seconds)
{
	return barsPerSecond * seconds;
//	return getBeatAtTime(seconds)/ getBeatsInBar();
}
int PlayheadMonitor::getBeatsInBar()
{
	return getTimeSigNum();
}

bool PlayheadMonitor::dawSigToTimeSig(AudioPlayHead::TimeSignature& sig)
{
	if (origSigNum != sig.numerator || origSigDen != sig.denominator)
	{
		origSigNum = sig.numerator;
		origSigDen = sig.denominator;
		
		if (Format::getHostType().isFruityLoops())
		{
			//Found this in FL
			//but it seems like Ableton also does this
			//Logic does not do this
			
			/*
			 3/4 : 12/4
			 4/4 : 16/4
			 3/2 : 24/8
			 4/2 : 32/8
			 3/8 : 6/2
			 6/16: 6/1
			 16/16: 16/1
			 */
			//		DBG (juce::String("Time sig") + juce::String(sig.numerator) + ", " + juce::String(sig.denominator));
			//		std::cout<< "Sig"<<sig.numerator <<  ", " << sig.denominator<<std::endl;
			
			timeSigDen = 16/sig.denominator;
			//Boy howdy
			if (Format::isAU())
				timeSigNum = sig.numerator/sig.denominator;
			else
				timeSigNum = sig.numerator;
			
		}
		else
		{
			timeSigNum = sig.numerator;
			timeSigDen = sig.denominator;
		}
		return true;
	}
	return false;
}
void PlayheadMonitor::updateBPMOnly(AudioPlayHead::PositionInfo& temp) {
	
	bool needsUpdate=false;
	
	auto sig = temp.getTimeSignature();
	if (sig)
	{
		if (dawSigToTimeSig(*sig))
		{
//		if ( num != timeSigNum
//			|| den != timeSigDen)
//		{
//			timeSigNum = num;
//			timeSigDen = den;
//			std::cout<<"Time Sig: "<<timeSigNum<<" / "<<timeSigDen<<";  Daw: "<<(*sig).numerator<<" . "<<(*sig).denominator<<std::endl;
			needsUpdate=true;
		}
	}
	auto newBpm = temp.getBpm();
	if (newBpm)
	{
		if ( fabs(bpm - *newBpm) > .01)
		{
			bpm = *newBpm;
			needsUpdate = true;
		}
	}
	if (needsUpdate)
		updateBPM();
}
void PlayheadMonitor::increment(int frames)
{
	if (virtualPlaying || dawPlaying)
	{
//		samplePosition += frames;
		samplePosition= nextSamples;
		nextSamples += frames;
//		float t = frames / rate;
//		float barChange = secToBars(t);
//		barPosition += barChange;
		barPosition = samplePosition * getBarsPerSample();
	}
}

//Different settings for different amounts of returned items
//Currently passing forward bars
//All 3
bool PlayheadMonitor::updatePosSBP(int numFrames, int64 smp, int64 bars, double ppqPos)
{
	return updatePosSP(numFrames, smp, ppqPos);
}
//Two of
bool PlayheadMonitor::updatePosBP (int numFrames, int64 bars, double ppqPos)
{
	return updatePosP(numFrames, ppqPos);
}
bool PlayheadMonitor::updatePosSP (int numFrames, int64 smp, double ppqPos)
{
	int64 diff = smp - (nextSamples);
	//Ableton will give and take a sample each buffer but only on VST
	bool jumped = std::abs(diff)>200;
	
	samplePosition = smp;
	nextSamples = smp + numFrames;

	barPosition = samplePosition * getBarsPerSample();
//	barPosition = ppqToBars(ppqPos);
//
//	double bps = barsPerSecond/rate;
//	float projBar = samplePosition * bps;
//	int barDif = int(std::round(barPosition*bps)) - samplePosition;
//	if (barDif!=0)
//	{
//		std::cout<<"Bar difference  "<<barDif<<" \tRatio="<<(barPosition/projBar)<<std::endl;
//	}
//

//	if (diff!=0)
//	{
//		std::cout<<"jump diff: "<<diff<<std::endl;
//	}
	
	return jumped;
}
bool PlayheadMonitor::updatePosSB (int numFrames, int64 smp, int64 bars)
{
	return updatePosS(numFrames, smp);
}
//Only 1
bool PlayheadMonitor::updatePosB  (int numFrames, int64 bars)
{
	return updatePosNa(numFrames);
}
bool PlayheadMonitor::updatePosP  (int numFrames, double ppqPos)
{
	float newBp = ppqToBars(ppqPos);
	float projBp = barPosition + getBarsPerSample() * numFrames;
	
	barPosition = newBp;
	samplePosition = nextSamples;
	nextSamples += numFrames;
	
	return (fabs(newBp - projBp) > .05);
}
bool PlayheadMonitor::updatePosS  (int numFrames, int64 smp)
{
	int64 diff = smp - (nextSamples);
	//Ableton will give and take a sample each buffer but only on VST
	bool jumped = std::abs(diff)>2;
	
	samplePosition = smp;
	nextSamples = smp + numFrames;
	barPosition = samplePosition * getBarsPerSample();
	
	return jumped;
}
//Zero
bool PlayheadMonitor::updatePosNa (int numFrames)
{
	//TODO: Pass in position struct to check additional sources, like time in seconds
	samplePosition = nextSamples;
	nextSamples += numFrames;
	barPosition = getBarsPerSample() * samplePosition;
	return false;
}

void PlayheadMonitor::update(AudioPlayHead::PositionInfo& pos, int numFrames)
{
	updateBPMOnly(pos);
	
	float orig = barPosition;
	
	auto s = pos.getTimeInSamples();
	auto b = pos.getBarCount();
	auto p = pos.getPpqPosition();
	
	bool jumped;
	
	if ( s &&  b &&  p)
		jumped = updatePosSBP(numFrames, *s, *b, *p);
	else if ( s &&  b && !p)
		jumped = updatePosSB (numFrames, *s, *b);
	else if ( s && !b &&  p)
		jumped = updatePosSP (numFrames, *s, *p);
	else if ( s && !b && !p)
		jumped = updatePosS  (numFrames, *s);
	else if (!s &&  b &&  p)
		jumped = updatePosBP (numFrames, *b, *p);
	else if (!s &&  b && !p)
		jumped = updatePosB  (numFrames, *b);
	else if (!s && !b &&  p)
		jumped = updatePosP  (numFrames, *p);
	else// if (!s && !b && !p)
		jumped = updatePosNa (numFrames);

	if (jumped)
	{
//		DebugWindow::print("Jumped");
		updatePosition(orig);
	}
}
void PlayheadMonitor::updateOrIncrementIfPaused(AudioPlayHead* playHead, int numFrames)
{
//	bool ctrl = playHead->canControlTransport();
//	if (int(ctrl) != lastTptCtrl)
//	{
//		lastTptCtrl = int(ctrl);
//		DebugWindow::print("Transport control?",  ctrl? "Yes" : "No");
//	}
		
	Optional<AudioPlayHead::PositionInfo> position = playHead->getPosition();
	
	if (position)
	{
		bool isPlaying = position->getIsPlaying();
		if (!isPlaying) {
			updateBPMOnly(*position);
			increment(numFrames);
		}
		else {
			update(*position, numFrames);
		}
		
		if (isPlaying != dawPlaying)
		{
//			DebugWindow::print(temp.isPlaying ? "Play" : "Pause");
			setDawPlaying(isPlaying);
		}
	}
	else
	{
		increment(numFrames);
	}
}

void PlayheadMonitor::update(AudioPlayHead* playHead, int numFrames)
{
	if (playHead==NULL) {
		increment(numFrames);
	}
	else {
		updateOrIncrementIfPaused(playHead, numFrames);
	}
}
void PlayheadMonitor::setPlaying(bool set, bool reset, bool changeTransportIfPossible, Listener* src)
{
//	if (changeTransport)
	if (!dawPlaying)
	{
		if (reset)
		{
			setPosition(0, src);
		}
		setVirtualPlaying(set, src);
	}
}
void PlayheadMonitor::resetPosition(Listener* src)
{
	if (!dawPlaying)
		setPosition(0, src);
}

void PlayheadMonitor::setPosition(float set, Listener* src)
{
	if (fabs(barPosition-set) < .001) return;
	float prev = barPosition;
	barPosition = set;
	nextSamples = barPosition * getSamplesPerBar();
	updatePosition(prev, src);
}

void PlayheadMonitor::setDawPlaying(bool set, Listener* src)
{
	if (set==dawPlaying) return;
	
	dawPlaying = set;
	//if (dawPlaying)
	virtualPlaying = false;
	
	bool newPlaying = dawPlaying || virtualPlaying;
	
	if (playing != newPlaying)
	{
		playing = newPlaying;
		updatePlayingListeners(src);
	}
}
void PlayheadMonitor::setVirtualPlaying(bool set, Listener* src)
{
	if (dawPlaying) return;
	if (set==virtualPlaying) return;
	
	virtualPlaying = set;

	bool newPlaying = dawPlaying || virtualPlaying;
	
	if (playing != newPlaying)
	{
		playing = newPlaying;
		updatePlayingListeners(src);
	}
}

void PlayheadMonitor::updatePlayingListeners(Listener* src)
{
	src=NULL;
	backendListeners.callExcluding(src, [&](Listener& l) { l.pauseChanged(playing); } );
	if (!guiListeners.isEmpty())
	{
		MessageManager::callAsync([&, src](){inThread_updatePlaying(src); });
	}
}
void PlayheadMonitor::inThread_updatePlaying(Listener* src)
{
	guiListeners.callExcluding(src, [&](Listener& l) { l.pauseChanged(playing); });
}
void PlayheadMonitor::updateBPM(Listener* src)
{
	updateTimePrecalcs();
	
	src=NULL;
	backendListeners.callExcluding(src, [&](Listener& l) { l.bpmChanged(bpm, timeSigNum, timeSigDen); } );
	if (!guiListeners.isEmpty())
	{
		MessageManager::callAsync([&, src](){inThread_updateBPM(src); });
	}
}
void PlayheadMonitor::inThread_updateBPM(Listener* src)
{
	guiListeners.callExcluding(src, [&](Listener& l) { l.bpmChanged(bpm, timeSigNum, timeSigDen); });
}
void PlayheadMonitor::updatePosition(float from, Listener* src)
{
	src=NULL;
	backendListeners.callExcluding(src, [&](Listener& l) { l.positionJumped(from, barPosition); } );
	if (!guiListeners.isEmpty())
	{
		MessageManager::callAsync([&, from, src](){inThread_updatePosition(from, src); });
	}
}
void PlayheadMonitor::inThread_updatePosition(float from, Listener* src)
{
	guiListeners.callExcluding(src, [&](Listener& l) { l.positionJumped(from, barPosition); });
}
