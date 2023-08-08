//
//  ClearBlock.h
//  
//
//  Created by Isaac Roberts on 3/30/22.
//

#ifndef ClearBlock_h
#define ClearBlock_h

#include "InstrData.h"

class ClearBlock
{
	/**
		This dummy class can be used to clear audio before an instrument.
		Unlike other FX, it does not derive from FX so it won't waste any time with virtual functions or FX member variables
	 */
public:
	ClearBlock() {}
	
	void attachMixer() {}
	
	//Abstract Gettors & Setters
	void set(int n, float set) {}
	float get(int n) {return 0;}
	
	int paramAmt() {return 0;}
	
	//Getters & Setters
	void setBypass(bool set) {}
	
	void setMix(float set) {}
	
	void bypassProcess(ContextR& c, bypassParams) { if(!inputIsSilent) process(c); }
	void bypassProcess(ContextNR& c, bypassParams) { if(!inputIsSilent) process(c);}
	
	void sidechain(InputBlock& levels, ContextR& c) { process(c); }
	void sidechain(InputBlock& levels, ContextNR& c) { process(c); }
	
	void prepare(Spec& spec) { }
	void process(ContextR& c) {
		c.getOutput().clear();
	}
	void process(ContextNR& c) {
		c.getOutput().clear();
	}
	void process(InputBlock& from, OutputBlock& to, BlockStorage& b) {
		to.clear();
	}
	void process(OutputBlock& o, BlockStorage& b) {
		o.clear();
	}
	void setInstrData(InstrData*) {}
	void reset() {}
		
	~ClearBlock() {}
	
	void setParameters() {}
private:
	
	ClearBlock (const ClearBlock&) = delete;
	ClearBlock& operator= (const ClearBlock&) = delete;
};


#endif /* ClearBlock_h */
