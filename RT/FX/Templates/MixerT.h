
#pragma once


template <class Effect>
class MixerT
{
protected:
	Effect effect;
	float mix;
	float asyncMix;
public:
	MixerT()
	: effect(),
	mix(1.0f),
	asyncMix(1.0f)
	{
		
	}
	
	Effect& get() { return effect; }
	Effect& fx() { return effect; }
	Effect& operator*() { return effect; }
	Effect* operator->() { return &effect;}

			
	//Getters & Setters
	void setBypass(bool set)
	{
		effect.setBypass(set);
	}
	
	void setMix(float set)
	{
		asyncMix = set;
	}
	void setIntMix(int set) {
		asyncMix = set/100.f;
	}
	
	void prepare(Spec& spec)
	{
		effect.prepare(spec);
	}
 
	void process(ContextR& c)
	{
		mix = asyncMix;
		
		if (mix <= 0 )
		{
			effect.bypassProcess(c, false, false);
			return;
		}
		if (mix >= 1)
		{
			effect.process(c);
		}
		else {
			BlockPtr orig(c);
			orig->copyFrom(c.getInput());
			effect.process(c);

			auto& b = c.getOutput();
			b *= mix;
			b.addProductOf(*orig, 1-mix);
		}
	}
	void process(ContextNR& c)
	{
		mix = asyncMix;
		
		if (mix >= 1)
		{
			effect.process(c);
		}
		else if (mix <= 0)
		{
			c.bypassProcess();
			effect.bypassProcess(c, false, false);
		}
		else
		{
			effect.process(c);
			
			auto& b = c.getOutput();
			b *= mix;
			b.addProductOf(c.getInput(), 1-mix);
		}
	}
 
	void bypassProcess(ContextR& c, bool allowTail, bool inputIsSilent)
	{
		mix = asyncMix;

		if (!allowTail || inputIsSilent)
		{
			effect.bypassProcess(c, bypassCall);
		}
		else
		{
			if (mix <= 0 )
			{
				effect.bypassProcess(c, false, inputIsSilent);
				return;
			}
			if (mix >= 1)
			{
				effect.bypassProcess(c, true, inputIsSilent);
			}
			else {
				BlockPtr orig(c);
				orig->copyFrom(c.getInput());
				effect.bypassProcess(c, true, inputIsSilent);

				auto& b = c.getOutput();
				b *= mix;
				b.addProductOf(*orig, 1-mix);
			}
		}
	}
	void bypassProcess(ContextNR& c, bool allowTail, bool inputIsSilent)
	{
		mix = asyncMix;

		if (!allowTail || inputIsSilent)
			effect.bypassProcess(c, bypassCall);
		else
		{
			if (mix >= 1)
			{
				effect.bypassProcess(c, true, inputIsSilent);
			}
			else if (mix <= 0)
			{
				c.bypassProcess();
				effect.bypassProcess(c, false, inputIsSilent);
			}
			else
			{
				effect.bypassProcess(c, true, inputIsSilent);

				auto& b = c.getOutput();
				b *= mix;
				b.addProductOf(c.getInput(), 1-mix);
			}

		}
	}

	void sidechain(InputBlock& levels, ContextR& c)
	{
		mix = asyncMix;
		
		if (mix <= 0 )
		{
			effect.bypassProcess(c, false, false);
			return;
		}
		if (mix >= 1)
		{
			effect.process(levels, c);
		}
		else {
			BlockPtr orig(c);
			orig->copyFrom(c.getInput());
			effect.sidechain(levels, c);

			auto& b = c.getOutput();
			b *= mix;
			b.addProductOf(*orig, 1-mix);
		}
	}
	void sidechain(InputBlock& levels, ContextNR& c)
	{
		mix = asyncMix;
		
		if (mix >= 1)
		{
			effect.sidechain(levels, c);
		}
		else if (mix <= 0)
		{
			c.bypassProcess();
			effect.bypassProcess(c, false, false);
		}
		else
		{
			effect.sidechain(levels, c);
			
			auto& b = c.getOutput();
			b *= mix;
			b.addProductOf(c.getInput(), 1-mix);
		}
	}

 
	void reset()
	{
		effect.reset();
	}
	void setInstrData(InstrData* instrData)
	{
		effect.setInstrData(instrData);
	}
		
	~MixerT() {}
private:
	MixerT (const MixerT&) = delete;
	MixerT& operator= (const MixerT&) = delete;
};
