//
//  LinkwitzRileyAllPass.h
//  
//
//  Created by Isaac Roberts on 10/19/22.
//

// Created by Isaac Roberts 

#ifndef LinkwitzRileyAllPass_h
#define LinkwitzRileyAllPass_h

#include <vector>

class LinkwitzRileyAllPass
{
public:
	//==============================================================================

	//==============================================================================
	/** Constructor. */
	LinkwitzRileyAllPass();
	~LinkwitzRileyAllPass();

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
	
	void process(InputBlock& input, OutputBlock& allpass);
	void process(OutputBlock& allpass);

	void bypassProcess(int N);
	template <class InClass>
	void bypassProcess(InClass& input) {
		bypassProcess(input.getNumSamples());
	}

	void sanCheckState() {
		const float SMAX=1000;
		for (auto s : { &s1, &s2 })
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
	void processSample (size_t channel, float inputValue, float &ap);

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
	std::vector<float> s1, s2;//, s3, s4;

	double sampleRate = 44100.0;
	float cutoffFrequency = 2000.0;
	
	bool filterBypass=true;
};

#endif /* LinkwitzRileyAllPass_h */
