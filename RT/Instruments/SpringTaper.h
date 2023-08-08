//
//  SpringTaper.h
//  
//
//  Created by Isaac Roberts on 11/14/22.
//

// Created by Isaac Roberts 

#ifndef SpringTaper_h
#define SpringTaper_h

#include <stdio.h>

class RTSynthVoice;

class SpringTaper {
public:
	SpringTaper();
	
	
	virtual ~SpringTaper();
	
	void prepare(Spec& spec);
	void gatherVoiceTaper(RTSynthVoice* voice);
	void renderTaper(OutputBlock &b);

	
protected:
	SpringTaper(const SpringTaper& other)=delete;
	SpringTaper& operator=(const SpringTaper& other)=delete;
	
	static constexpr float hardStopTaperRate = .01;//sec / (amplitude=1)
	//Double array of floats so it can be used by AudioBlock
	enum Tapers {
		_a1,_a2,_a3,
		K_, V_, X_, D_,
		taperCount
	};
//	static constexpr int taperCount=4;
	int numChannels=0;
	float** hardStopTaper=NULL;
	
	bool hardStopTapering=false;
	
	float sampleRate=0;
	
};

#endif /* SpringTaper_h */
