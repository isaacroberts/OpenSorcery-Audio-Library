//
//  LastSelectedMonitor.h
//  
//
//  Created by Isaac Roberts on 2/18/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "OmniListener.h"

//#include "../Sliders/TwoValueSlider.h"

using namespace juce;

class LastSelectedMonitor {
public:
	LastSelectedMonitor(int amt);
	LastSelectedMonitor();
	
	void setAmt(int set);
	
	template <typename Comp>
	void connect(int n, Comp* c) {
		c->addListener(&listeners [n]);
	}
	template <typename Comp>
	void connect(int n, Comp& c) {
		c.addListener(&listeners [n]);
	}
	
	void selected(int x);
	
	int getSelected() {return cur;}
	void setSelected(int x, bool callCallback=false);
	
	void prev(bool callCallback=false);
	void next(bool callCallback=false);
	
	int get() { return cur; }
	void set(int x, bool callCallback=false) {setSelected(x, callCallback);}
	
	virtual ~LastSelectedMonitor();
	
public:
	std::function<void(int)> callback;
private:
	OmniListener* listeners;
	int amt;
	int cur;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LastSelectedMonitor);

};
