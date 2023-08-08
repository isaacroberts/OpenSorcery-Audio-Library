//
//  Mixer.h
//  
//
//  Created by Isaac Roberts on 8/15/20.
//

// Created by Isaac Roberts 

#pragma once

//#include <JuceHeader.h>



using namespace juce;

class Mixer {
public:
	Mixer();
	
	void prepare(Spec&);
	
	void nrMix(ContextNR&);
	
	inline void frame() {
	/**
	asyncMix prevents mix from being changed during process loop,
	which may result in past buffers being added to current buffer
	*/
		mix = asyncMix;
	}
	bool bypassed() { return mix <= 0;}
	
	void replacingPreMix(ContextR&);
	void replacingPostMix(ContextR&);
	
	void setMix(float set) {
		asyncMix = set;
	}
	inline float getMix() {return mix;}
	
	virtual ~Mixer();
	
protected:
	float mix;
	float asyncMix;
	PersistentBlockPtr ptr;
//	bool replacing;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Mixer);
};
