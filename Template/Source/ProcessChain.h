//
//  ProcessChain.h
//  Template - Shared Code
//
//  Created by Isaac Roberts on 9/19/22.
//

// Created by Isaac Roberts 

#ifndef ProcessChain_h
#define ProcessChain_h

#include <JuceHeader.h>

#include "RT/Modules.h"

#include "Preset.h"

typedef Chain <
	RTGain
> BaseChain;


class ProcessChain : public BaseChain {
public:
	enum Pos {
		Gain=0
	};
	
	ProcessChain();
	
	void connectPreset(TemplatePreset* p);
	void prepare(Spec& spec) override;

	virtual ~ProcessChain();
	
	
	
private:
	ProcessChain(const ProcessChain& other)=delete;
	ProcessChain& operator=(const ProcessChain& other)=delete;
	
};

#endif /* ProcessChain_h */
