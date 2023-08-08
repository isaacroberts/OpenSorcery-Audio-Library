
#pragma once


class LookaheadGaugeCompressor : public MeterGaugeBase
{
public:
	LookaheadCompressor comp;
	
	LookaheadGaugeCompressor() :
	MeterGaugeBase(-40, 0, 60.f, 0.f, MeterGaugeBase::MeterTrackMode::NoSmooth),
	comp()
	{
		
	}
			   
   //Getters & Setters
   void setBypass(bool set) {comp.setBypass(set);}
   bool isBypassed() {return comp.isBypassed();}

   void prepare(Spec& spec) {
	   comp.prepare(spec);
	   prepare_meter(spec.sampleRate);

   }

   //Not necessary but convenient
	bool hasBeenPrepared() {return comp.hasBeenPrepared(); }

   void reset() {
	   comp.reset();
   }
   void setInstrData(InstrData* instrData) {}
	   
   ~LookaheadGaugeCompressor() {}

	float getBypassLevel()
	{
		return 0;
	}
	inline float getCompressLevel() {
		return comp.getCompressLevelCertain();
	}

	
	template <class ContextType>
	void bypassProcess(ContextType& c, bypassParams) {
		comp.bypassProcess(c, bypassCall);
		updateDb(getCompressLevel());
	}
	template <class ContextType>
	void process(ContextType& c) {
		comp.process(c);
		updateDb(getCompressLevel());
	}
	template <class ContextType>
	void sidechain(InputBlock& levels, ContextType& c) {
		comp.sidechain(levels, c);
		updateDb(getCompressLevel());
	}

	
private:
	LookaheadGaugeCompressor (const LookaheadGaugeCompressor&) = delete;
	LookaheadGaugeCompressor& operator= (const LookaheadGaugeCompressor&) = delete;


};
