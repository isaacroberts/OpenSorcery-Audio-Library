//
//  DelayedTask.h
//  
//
//  Created by Isaac Roberts on 3/15/21.
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

class DelayedTask : public juce::Thread
{
public:
	DelayedTask(String name, int millis, std::function<void()> callback_)
	: Thread(name),
	callback(callback_),
	milliseconds(millis)
	{}
	DelayedTask(String name) : DelayedTask(name, 100, [&](){} ) {}
	DelayedTask(int millis, std::function<void()> callback_)
				: DelayedTask("WaitThread_"+String(millis)+"ms", millis, callback_) {}
	DelayedTask() : DelayedTask("WaitThread", 100, [&](){} ) {}
	
	
	void start()
	{
		startThread();
	}

	void cancel()
	{
		signalThreadShouldExit();
	}
	
	void run() override {
		wait(milliseconds);
		if (! threadShouldExit())
		{
			callback();
		}
		// free any objects that the lambda might contain while the thread is still active
		callback = nullptr;
		
	}
	
	virtual ~DelayedTask()
	{
		// free any objects that the lambda might contain while the thread is still active
		callback = nullptr;
	}
	
	
	void setDelay(int millis) { milliseconds = millis; }
	
	std::function<void()> callback = nullptr;
	int milliseconds;
private:
	
	
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayedTask);

};
