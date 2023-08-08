//
//  HardStopTaper.h
//  
//
//  Created by Isaac Roberts on 11/14/22.
//

// Created by Isaac Roberts 

#ifndef HardStopTaper_h
#define HardStopTaper_h

#include <JuceHeader.h>

class HardStopTaper {
public:
	HardStopTaper(int bufferSize=512);
	
	void prepare(Spec& spec);
	inline int taperLen() { return ringBufferSize; }
	void gatherVoiceTaper(RTSynthVoice* voice, int alreadyCaptured);
	void renderTaper(OutputBlock &b, int startSample, int numSamples);
	inline void renderTaper(OutputBlock &b) {
		renderTaper(b, 0, (int)b.getNumSamples());
	}

	void advance(int numSamples);
	
	virtual ~HardStopTaper();
	
	
protected:
	const int ringBufferSize=512;
	RingBuffer ring;
	int samplesSinceLastWrite=0;
	
private:
	HardStopTaper(const HardStopTaper& other)=delete;
	HardStopTaper& operator=(const HardStopTaper& other)=delete;
	
};

#endif /* HardStopTaper_h */
