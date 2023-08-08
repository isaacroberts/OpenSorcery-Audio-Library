
#include "Shared.h"

/*
struct Example {

	void process(float* data, int numSamples, float rate)
	{
	}
};
*/


template <class Liner>
class OnelinerFX
{
	/*
		For OnelinerFX FXs
	 
	 
	 
	struct Example {
		void process(float* data, int numSamples, float rate)
		{
		}
	};
	 */
	 
protected:
	Liner liner;
	
	float rate=69;
	bool bypass=false;
public:
	Liner& get() { return liner; }
	Liner& operator*() { return liner; }
	Liner* operator->() { return &liner;}

	OnelinerFX() {}
	
	void attachMixer() {}
		
	//Getters & Setters
	void setBypass(bool set) {bypass = set;}
	
	void setMix(float set) {}
	
	void bypassProcess(ContextR& c, bypassParams) {}
	void bypassProcess(ContextNR& c, bypassParams) {}
	
	void sidechain(InputBlock& levels, ContextR& c) {}
	void sidechain(InputBlock& levels, ContextNR& c) {c.bypassProcess(); }
	
	void prepare(Spec& spec) {
//		hasPrepared=true;
		rate = spec.sampleRate;
	}
	void process(ContextR& c) {
		if (!bypass)
		{
		for (int ch=0; ch < c.getNumChannels(); ++ch)
			liner.process(c.getOutput()[ch], c.getNumSamples(), rate);
		}
	}
	void process(ContextNR& c) {
		c.bypassProcess();
		if (!bypass)
		{
		for (int ch=0; ch < c.getNumChannels(); ++ch)
			liner.process(c.getOutput()[ch], c.getNumSamples(), rate);
		}
	}
	void process(InputBlock& from, OutputBlock& to, BlockStorage& b) {
		to.copyFrom(from);
		if (!bypass)
		{
		for (int ch=0; ch < to.getNumChannels(); ++ch)
			liner.process(to[ch], to.getNumSamples(), rate);
		}
	}
	void process(OutputBlock& b, BlockStorage& s) {
		if (!bypass)
		{
		for (int ch=0; ch < b.getNumChannels(); ++ch)
			liner.process(b[ch], b.getNumSamples(), rate);
		}
	}

	void reset() {}
	
	void setInstrData(InstrData* instrData) {}
	
//	bool hasBeenPrepared() {return hasPrepared;}
	
	~OnelinerFX() {}
private:
	OnelinerFX (const OnelinerFX&) = delete;
	OnelinerFX& operator= (const OnelinerFX&) = delete;
};




template <class Liner>
class MultiChOneLiner
{
	/**
		For OnelinerFX FXs, where the channels need to be kept separate 
	 */
	 
protected:
	Liner* liner;
	int channels=0;
		
	float rate=69;
	bool bypass=false;
public:
	Liner& get(int c) { return liner[c]; }
	Liner& operator[](int c) { return liner[c]; }

	MultiChOneLiner() : liner(NULL), channels(0) {}
	
	void attachMixer() {}
		
	//Getters & Setters
	void setBypass(bool set) {bypass = set;}
	
	void setMix(float set) {}
	
	void bypassProcess(ContextR& c, bypassParams) {}
	void bypassProcess(ContextNR& c, bypassParams) {}
	
	void sidechain(InputBlock& levels, ContextR& c) {}
	void sidechain(InputBlock& levels, ContextNR& c) {c.bypassProcess(); }
	
	void prepare(Spec& spec) {
//		hasPrepared=true;
		rate = spec.sampleRate;
		
		if (channels!=spec.numChannels)
		{
			if (liner)
			{
				delete[] liner;
			}
			channels = spec.numChannels;
			liner = new Liner[channels];
		}
	}
	void process(ContextR& c) {
		if (!bypass)
		{
			for (int ch=0; ch < c.getNumChannels(); ++ch)
				liner[ch].process(c.getOutput()[ch], c.getNumSamples(), rate);
		}
	}
	void process(ContextNR& c) {
		c.bypassProcess();
		if (!bypass)
		{
			for (int ch=0; ch < c.getNumChannels(); ++ch)
				liner[ch].process(c.getOutput()[ch], c.getNumSamples(), rate);
		}
	}
	void process(InputBlock& from, OutputBlock& to, BlockStorage& b) {
		to.copyFrom(from);
		if (!bypass)
		{
			for (int ch=0; ch < to.getNumChannels(); ++ch)
				liner[ch].process(to[ch], to.getNumSamples(), rate);
		}

	}
	void process(OutputBlock& b, BlockStorage& s) {
		if (!bypass)
		{
			for (int ch=0; ch < b.getNumChannels(); ++ch)
				liner[ch].process(b[ch], b.getNumSamples(), rate);
		}
	}

	void reset() {}
	
	void setInstrData(InstrData* instrData) {}
	
//	bool hasBeenPrepared() {return hasPrepared;}
	
	~MultiChOneLiner() {}
private:
	MultiChOneLiner (const MultiChOneLiner&) = delete;
	MultiChOneLiner& operator= (const MultiChOneLiner&) = delete;
};
