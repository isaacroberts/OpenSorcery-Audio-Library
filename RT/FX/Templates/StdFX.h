

template <class Effect>
class StdFX
{
	/*
		For StdFX FXs
	 
	 
	 
	class Effect {
		void prepare(Spec& spec);
		void process(ContextR& c);
		void process(ContextNR& c);

		void tailProcess(ContextR& c);
		void tailProcess(ContextNR& c);

	};
	 
	 */
	
	
	
	 
protected:
	Effect effect;
	
	bool bypass=false;
public:
	Effect& get() { return effect; }
	Effect& operator*() { return effect; }
	Effect* operator->() { return &effect;}

	StdFX() {}
	
	void attachMixer() {}
		
	//Getters & Setters
	void setBypass(bool set) {bypass = set;}
		
	void prepare(Spec& spec) {
		effect.prepare(spec);
	}
	
	
	void bypassProcess(ContextR& c, bypassParams) {
		
	}
	void bypassProcess(ContextNR& c, bypassParams) {
	}
	
	void sidechain(InputBlock& levels, ContextR& c) {
		
	}
	void sidechain(InputBlock& levels, ContextNR& c) {
		c.bypassProcess();
	}
	
	void process(ContextR& c) {
		if (!bypass)
		{
		for (int ch=0; ch < c.getNumChannels(); ++ch)
			effect.process(c.getOutput()[ch], c.getNumSamples());
		}
	}
	void process(ContextNR& c) {
		c.bypassProcess();
		if (!bypass)
		{
			effect.process(c);
		}
		else
			effect.processTail(c);
		
	}
	void process(InputBlock& from, OutputBlock& to, BlockStorage& b) {
		to.copyFrom(from);
		if (!bypass)
		{
		for (int ch=0; ch < to.getNumChannels(); ++ch)
			effect.process(to[ch], to.getNumSamples());
		}
	}
	void process(OutputBlock& b, BlockStorage& s) {
		if (!bypass)
		{
		for (int ch=0; ch < b.getNumChannels(); ++ch)
			effect.process(b[ch], b.getNumSamples());
		}
	}

	void reset() {}
	
	void setInstrData(InstrData* instrData) {}
	
//	bool hasBeenPrepared() {return hasPrepared;}
	
	~StdFX() {}
private:
	StdFX (const StdFX&) = delete;
	StdFX& operator= (const StdFX&) = delete;
};

