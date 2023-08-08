//
//  Smoother.h
//  
//
//  Created by Isaac Roberts on 3/21/21.
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

class Smoother {
public:
//	enum Mode {
//		Linear, Logarithmic
//	};
	Smoother()
	: goal(1), cur(1), maxSpeedPerFrame(0), isMoving(false)
	{}
	Smoother(float s)
	: goal(1), cur(1), maxSpeedPerFrame(s), isMoving(false)
	{}
	
	void setMaxSpeed(float speed, float rate)
	{
		maxSpeedPerFrame = speed / rate;
	}
	bool isTransitioning() { return isMoving; }
	bool inUse() { return maxSpeedPerFrame > 0; }
	
	void hardSet(float s)
	{
		goal = cur = s;
		isMoving = false;
	}
	void snapToTarget() {
		cur = goal;
		isMoving = false;
	}
	
	void set(float s)
	{
		if (s != cur)
		{
			goal = s;
			if (maxSpeedPerFrame > 0)
				isMoving = true;
		}
	}
	float frame()
	{
		if (isMoving)
		{
			if (maxSpeedPerFrame == 0)
			{
				isMoving=false;
				cur = goal;
			}
			else if (goal > cur)
			{
				cur += maxSpeedPerFrame;
				if (cur >= goal)
				{
					isMoving = false;
					cur = goal;
				}
			}
			else
			{
				cur -= maxSpeedPerFrame;
				if (cur <= goal)
				{
					isMoving = false;
					cur = goal;
				}
			}
		}
		return cur;
	}
	float get() { return cur; }
	float getTarget() { return goal; }
	
	virtual ~Smoother()
	{
	}

public:
//	std::function<void(float)> callback;
protected:
	float goal;
	float cur;
	
	float maxSpeedPerFrame;
	bool isMoving;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Smoother);

};
