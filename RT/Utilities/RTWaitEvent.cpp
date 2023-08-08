//
//  RTWaitEvent.cpp
//  
//
//  Created by Isaac Roberts on 1/16/21.
//

// Created by Isaac Roberts


#include "RTWaitEvent.h"


RTWaitEvent::RTWaitEvent (bool manualReset) noexcept
	: useManualReset (manualReset)
{
}

bool RTWaitEvent::wait (int timeOutMicroseconds) const
{
	std::unique_lock<std::mutex> lock (mutex);

	if (! triggered)
	{
		if (timeOutMicroseconds < 0)
		{
			condition.wait (lock, [this] { return triggered == true; });
		}
		else
		{
			if (! condition.wait_for (lock, std::chrono::microseconds (timeOutMicroseconds),
									  [this] { return triggered == true; }))
			{
				return false;
			}
		}
	}

	if (! useManualReset)
		reset();

	return true;
}

void RTWaitEvent::signal() const
{
	std::unique_lock<std::mutex> lock (mutex);

	triggered = true;
	condition.notify_all();
}

void RTWaitEvent::reset() const
{
	triggered = false;
}
