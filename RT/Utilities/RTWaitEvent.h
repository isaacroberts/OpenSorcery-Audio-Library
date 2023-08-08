#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
	An exact clone of the juce::WaitableEvent
	except it waits in microseconds to make it permissible for low-latency audio threads
 
 
	Allows threads to wait for events triggered by other threads.

	A thread can call RTWaitEvent::wait() to suspend the calling thread until
	another thread wakes it up by calling the RTWaitEvent::signal() method.

	@tags{Core}
*/
class  RTWaitEvent
{
public:
	//==============================================================================
	/** Creates a RTWaitEvent object.

		The object is initially in an unsignalled state.

		@param manualReset  If this is false, the event will be reset automatically when the wait()
							method is called. If manualReset is true, then once the event is signalled,
							the only way to reset it will be by calling the reset() method.
	*/
	explicit RTWaitEvent (bool manualReset = false) noexcept;

	//==============================================================================
	/** Suspends the calling thread until the event has been signalled.

		This will wait until the object's signal() method is called by another thread,
		or until the timeout expires.

		After the event has been signalled, this method will return true and if manualReset
		was set to false in the RTWaitEvent's constructor, then the event will be reset.

		@param timeOutMicroseconds  the maximum time to wait, in microseconds. A negative
									value will cause it to wait forever.

		@returns    true if the object has been signalled, false if the timeout expires first.
		@see signal, reset
	 
		* This is the function that has been modified by RTWaitEvent *
	*/
	bool wait (int timeOutMicroseconds = -1) const;

	/** Wakes up any threads that are currently waiting on this object.

		If signal() is called when nothing is waiting, the next thread to call wait()
		will return immediately and reset the signal.

		If the RTWaitEvent is manual reset, all current and future threads that wait upon this
		object will be woken, until reset() is explicitly called.

		If the RTWaitEvent is automatic reset, and one or more threads is waiting upon the object,
		then one of them will be woken up. If no threads are currently waiting, then the next thread
		to call wait() will be woken up. As soon as a thread is woken, the signal is automatically
		reset.

		@see wait, reset
	*/
	void signal() const;

	/** Resets the event to an unsignalled state.
		If it's not already signalled, this does nothing.
	*/
	void reset() const;

private:
	//==============================================================================
	bool useManualReset;

	mutable std::mutex mutex;
	mutable std::condition_variable condition;
	mutable std::atomic<bool> triggered { false };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTWaitEvent)
};

