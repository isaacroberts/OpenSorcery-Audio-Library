


#include "FX.h"

class WhiteNoise
{
	float level = .1;
	bool playing=true;
	
public:
	WhiteNoise()
	{

	}

	
	void setBypass(bool set) {playing = !set;}
	bool isBypassed() {return !playing;}
	
	void setLevel(float set) { level = set; }
	void setDb(float set) {
		level = Util::dbToAmp(set);
	}
	
	//FX Stuff
	void prepare(Spec& spec) 
	{
		
	}
	void commonProcess(OutputBlock& block)
	{
		if (playing && level > 0)
		{
			size_t N = block.getNumSamples();
			for (int c=0; c < block.getNumChannels(); ++c)
			{
				auto out = block[c];
				for (int n=0; n < N; ++n)
				{
					out[n] += Util::rand_c() * level;
				}
			}
		}
	}
	void process (ContextR& c)
	{
		fx_proc_start(c);
		commonProcess(c.getOutput());
	}
	void process(ContextNR& c)
	{
		fx_proc_start(c);
		c.bypassProcess();
		commonProcess(c.getOutput());
	}
	
	template <class Context>
	void bypassProcess(Context& c, bypassParams)
	{}
	
	void sidechain(InputBlock& levels, ContextR& c) {process(c); }
	void sidechain(InputBlock& levels, ContextNR& c) {process(c); }

	void setInstrData(InstrData* instrData) {}

	
	void reset()
	{
		
	}

};
