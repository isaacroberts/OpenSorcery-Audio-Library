//
//  RTFastReverb.h
//  
//
//  Created by Isaac Roberts on 1/16/21.
//

// Created by Isaac Roberts 

#pragma once


#include <JuceHeader.h>


#include "RT/FX/FX.h"

#include "../TwoStageFFTConvolver.h"

#include "RT/Utilities/RTWaitEvent.h"
#include "RT/Utilities/RTJobQueue.h"

#include "ReverbCommon.h"

using namespace juce;


class ThreadedConvolver : public fftconvolver::TwoStageFFTConvolver
{
public:
	ThreadedConvolver() : fftconvolver::TwoStageFFTConvolver(),
	thread(NULL),
	bgWait(false)
	{}
	~ThreadedConvolver()
	{
		bgWait.signal();
	}
	void setThread(RTJobQueue<ThreadedConvolver>* t) {thread = t; }
	void prepare(int blockSize, int channelAmt, float sampleRate)
	{
		float graceTime = blockSize / sampleRate * 1.e6 / channelAmt;
		waitMicro = (int)graceTime;
		if (waitMicro <= 0)
			waitMicro = 1;
	}
	
	void cancelJobs() {
		cancelBackgroundProcessing();
		bgWait.signal();
	}
	void queue_uncancel() {
		thread->queueJob(this, uncancelThread);
	}
	
protected:
	/**
	* @brief Method called by the convolver if work for background processing is available
	*
	* The default implementation just calls doBackgroundProcessing() to perform the "bulk"
	* convolution. However, if you want to perform the majority of work in some background
	* thread (which is recommended), you can overload this method and trigger the execution
	* of doBackgroundProcessing() really in some background thread.
	*/
	static void bgRuns(ThreadedConvolver* c) {
		c->doBackgroundProcessing();
		c->completeJob();
	}
	static void uncancelThread(ThreadedConvolver* c) {
		c->uncancelBackground();
	}
	
	void startBackgroundProcessing() override {
		thread->queueJob(this, bgRuns);
//		doBackgroundProcessing();
//		completeJob();
	}
	
	void completeJob() {
		bgWait.signal();
	}
	/**
	* @brief Called by the convolver if it expects the result of its previous call to startBackgroundProcessing()
	*
	* After returning from this method, all background processing has to be completed.
	*/
	void waitForBackgroundProcessing() override {
		bgWait.wait(waitMicro);
	}
	RTJobQueue<ThreadedConvolver>* thread;
	RTWaitEvent bgWait;
	int waitMicro;

};




template <bool MULTICHANNEL>
class RTFastReverb : public FX
{
public:
	
	void setChannelLayerMode(ReverbCommon::ImpulseChannelLayerMode mode) {
		layerMode = mode;
	}
	
	RTFastReverb() : FX(),
		impulse(NULL),
		impulse_len(0),
		impulse_channels(0),
		layerMode(ReverbCommon::ImpulseChannelLayerMode::Repeat),
		convolver(NULL),
		backgroundThreads(NULL),
		num_threads(-1),
		channels(-1),
		blockSize(-1),
		hasCleared(true)
	{}
	
//	Getters & Setters
	int paramAmt() override {return 0;}
	void set(int n, float set) override {}
	float get(int n) override { return extraGet(); }
	
	void setImpulseFromBinary(const char* binaryData, size_t dataLen);
	
	void setImpulse(const float* data, size_t len);
	void setImpulse(const float** data, size_t len, int numChannels);
	
	void setImpulseChannel(const float* data, size_t len, int c, int totalC);
	
	void process(ContextR& context) ;
	void process(ContextNR& context) {
		jassertfalse;
	}
	
	void prepare2(const ProcessSpec& spec)override;
	
	void clear();
	
	//TODO: Allow tail 
	void bypassProcess(ContextR& c, bypassParams ) {
		if (!hasCleared)
			clear();
	}
	void bypassProcess(ContextNR& c, bypassParams) {
		if (!hasCleared)
			clear();
	}
	
	
	
	virtual ~RTFastReverb();
	
protected:
	
	void loadImpulse();
	void reloadImpulse();
	
	
	const float** impulse;
	size_t impulse_len;
	int impulse_channels;
	ReverbCommon::ImpulseChannelLayerMode layerMode;
	ThreadedConvolver* convolver;
	RTJobQueue<ThreadedConvolver>* backgroundThreads;
	int num_threads;
	int channels;
	int blockSize;
	bool threaded;
	bool hasCleared;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTFastReverb);

};

