//
//  DualLinkwitzRiley.h
//  
//
//  Created by Isaac Roberts on 10/19/22.
//

// Created by Isaac Roberts 

#ifndef DualLinkwitzRiley_h
#define DualLinkwitzRiley_h

#include <JuceHeader.h>

#include "FilterDefinitions.h"

class DualLinkwitzRiley
{
public:
	//==============================================================================

	//==============================================================================
	/** Constructor. */
	DualLinkwitzRiley();
	~DualLinkwitzRiley();

	//==============================================================================

	/** Sets the cutoff frequency of the filter in Hz. */
	void setCutoffFrequency (float newCutoffFrequencyHz);
	void setFreq(float freq) { setCutoffFrequency(freq); }

	//==============================================================================

	/** Returns the cutoff frequency of the filter. */
	float getCutoffFrequency() const noexcept     { return cutoffFrequency; }
	float getFreq() const noexcept { return cutoffFrequency; }
	
	//==============================================================================
	/** Initialises the filter. */
	void prepare (const ProcessSpec& spec);

	/** Resets the internal state variables of the filter. */
	void reset();

	//==============================================================================
	/** Processes the input and output samples supplied in the processing context. */
	
	void process(InputBlock& input, OutputBlock& low, OutputBlock& high);
	
	void process(InputBlock& input, OutputBlock& low, OutputBlock& high, OutputBlock& allpass);

	
	void process_allpass(InputBlock& input, OutputBlock& allpass);
	
	void process_skipLow(InputBlock& input, OutputBlock& high);
	void process_skipHigh(InputBlock& input, OutputBlock& low);

	
	void process_addLow(InputBlock& input, OutputBlock& low, OutputBlock& high);
	void process_addHigh(InputBlock& input, OutputBlock& low, OutputBlock& high);
	void process_addBoth(InputBlock& input, OutputBlock& low, OutputBlock& high);

	
	void bypassProcess(int N);
	template <class InClass>
	void bypassProcess(InClass& input) {
		bypassProcess(input.getNumSamples());
	}

	
	void sanCheckState() {
		const float SMAX=1000;
		for (auto s : { &s1, &s2, &s3, &s4 })
		  for (auto& e : *s)
		  {
			  if (isnan(e)) { e=0;}
			  else if (e > SMAX) e = SMAX;
			  else if (e <-SMAX) e =-SMAX;
		  }
	}

protected:
	void endProc();
	void bypassSample(size_t channel);
	/** Performs the filter operation on a single sample at a time, and returns both
		the low-pass and the high-pass outputs of the TPT structure.
	*/
	void processSample (size_t channel, float inputValue, float &outputLow, float &outputHigh);
	void processSample (size_t channel, float inputValue, float &outputLow, float &outputHigh, float& allpassOut);

	/** Ensure that the state variables are rounded to zero if the state
		variables are denormals. This is only needed if you are doing
		sample by sample processing.
	*/
	void snapToZero() noexcept;

private:
	//==============================================================================
	void update();

	//==============================================================================
	float g, h;
	std::vector<float> s1, s2, s3, s4;

	double sampleRate = 44100.0;
	float cutoffFrequency = 2000.0;
	
	
	enum LRStatus {
		Active, DirectHigh, DirectLow, Silence,
		Unprepared
	};
	LRStatus status=Unprepared;
};







#if TESTING
class DualLinkwitzRileyTester : public FX
{
public:
	DualLinkwitzRileyTester() : FX()
	{
		
	}
	void setFreq(float set) { lr.setFreq(set);
		std::cout<<"Set freq "<<set<<std::endl;
	}
	//FX Stuff
	void prepare2(Spec& spec) override {
		lr.prepare(spec);
	}
	template <class Context>
	void process (Context& c) {
		BlockPtr high(c);
		
		lr.process(c.getInput(), c.getOutput(), high);
//		*high *= -1;
		c.getOutput().add(*high);
	}
	template <class Context>
	void bypassProcess(Context& c, bypassParams) {
		lr.bypassProcess(c);
	}
//  void bypassProcess(ContextR& c, bypassParams);
//  void bypassProcess(ContextNR& c, bypassParams);
	void sidechain(InputBlock& level, ContextR& c) { process(c); }
	void sidechain(InputBlock& level, ContextNR& c) { process(c); }
	
	void reset2() override {
		lr.reset();
	}
	
	DualLinkwitzRiley lr;
	
};
#endif


#endif /* DualLinkwitzRiley_h */
