//
//  RTJobQueue.h
//  
//
//  Created by Isaac Roberts on 1/16/21.
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

#include <boost/lockfree/queue.hpp>
//#include <boost/tuple/tuple.hpp>


/**
	Must know the Type of the caller
 
	Uses boost::lockfree to prevent using locks on the audio thread
 
	Processes data off of the audio thread
 
	Must call thread.start(int priority) or send priority in the constructor (in which case thread will be started immediately)
 
 
 */

#define RTJOBQUEUE_MAXQUEUE 10

template <typename T>
class RTJobQueue : public juce::Thread
{
public:
	
//	typedef boost::tuple<void(*)(T*), T*> Functor;
	
	struct Functor
	{
		T* caller;
		void(*function)(T*);
		void call() {
			function(caller);
		}
	};
	
	RTJobQueue(String name)
	: Thread(name),
	jobQueue(RTJOBQUEUE_MAXQUEUE),
	terminated(false)
	{}
	RTJobQueue()
	: Thread(""),
	jobQueue(RTJOBQUEUE_MAXQUEUE),
	terminated(false)
	{}
	RTJobQueue(String name, int priority)
	: Thread(name),
	jobQueue(RTJOBQUEUE_MAXQUEUE),
	terminated(false)
	{
		startThread(priority);
	}
	RTJobQueue(int priority)
	: Thread(""),
	jobQueue(RTJOBQUEUE_MAXQUEUE),
	terminated(false)
	{
		startThread(priority);
	}
	
//	void start(int priority);
	
	~RTJobQueue()
	{
		terminated = true;
		stopThread(100);
	}
	void run() override
	{
		while (!terminated && !threadShouldExit())
		{
			if (jobQueue.empty())
			{
				wait(2);
			}
			else
			{
				Functor f;
				bool sxs = jobQueue.pop(f);
				if (sxs)
				{
					somethin_runnin=true;
					f.call();
					somethin_runnin=false;
				}
			}
		}
	}
	void queueJob(T* caller, void(*job)(T*)) {
		Functor f;
		f.caller = caller;
		f.function = job;
		jobQueue.push(f);
		
		notify();
	}
	void cancelAllJobs() {
		Functor blank;
		while (jobQueue.pop(blank)) {}
	}
	void terminate() {
		terminated=true;
	}
	
	static int availableCpus() {
		return juce::SystemStats::getNumPhysicalCpus();
	}
	
private:
	boost::lockfree::queue<Functor> jobQueue;
	bool terminated=false;
	bool somethin_runnin=false;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RTJobQueue);
};
