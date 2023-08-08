//
//  LastSelectedMonitor.cpp
//  
//
//  Created by Isaac Roberts on 2/18/21.
//

// Created by Isaac Roberts


#include "LastSelectedMonitor.h"


LastSelectedMonitor::LastSelectedMonitor(int amt_)
: callback([&](int i){}),
listeners(new OmniListener[amt_]),
amt(amt_),
cur(0)
{
	for (int n=0; n < amt; ++n)
	{
		listeners[n].callback = [&, n]() {this->selected(n);};
	}
}
LastSelectedMonitor::LastSelectedMonitor()
:
callback([&](int i){}),
listeners(NULL),
amt(0),
cur(0)
{
}
   
void LastSelectedMonitor::setAmt(int set)
{
	amt = set;
	listeners = new OmniListener[amt];
	for (int n=0; n < amt; ++n)
	{
		listeners[n].callback = [&, n]() {this->selected(n);};
	}
}
void LastSelectedMonitor::selected(int x)
{
	cur = x;
	callback(cur);
}
void LastSelectedMonitor::setSelected(int x, bool callCallback)
{
	cur = x;
	if (callCallback)
		callback(cur);
}

void LastSelectedMonitor::prev(bool callCallback)
{
	cur --;
	if (cur < 0)
		cur = amt-1;
	if (callCallback)
		callback(cur);
}
void LastSelectedMonitor::next(bool callCallback)
{
	cur = (cur+1) % amt;
	if (callCallback)
		callback(cur);
}

LastSelectedMonitor::~LastSelectedMonitor()
{
	delete[] listeners;
}

