//
//  Chain.h
//  Library - VST3
//
//  Created by Isaac Roberts on 7/7/20.
//  Copyright © 2020 RobertsAudio. All rights reserved.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "../Utilities/looper.h"

#include "Templates/Shared.h"

//#include "../Instrument/InstrData.h"

#include "Mixer.h"

using namespace juce;

//JUCE UPDATE: Check dsp::ProcessorChain to make sure this matches 

class InstrData;


template <typename... Processors>
class Chain
{
public:
    /** Get a reference to the processor at index `Index`. */
    template <int Index>       auto& get()       noexcept { return std::get<Index> (processors); }

    /** Get a reference to the processor at index `Index`. */
    //template <int Index> const auto& get() const noexcept { return std::get<Index> (processors); }

    /** Set the processor at index `Index` to be bypassed or enabled. */
    template <int Index>
    void setBypassed (bool b) noexcept  {
		bypassed[(size_t) Index] = b;
//		if (b)
//			std::get<Index>(processors).startBypass();
//		else
//			std::get<Index>(processors).endBypass();
	}
	void setBypassed (int n, bool b) noexcept  {
		bypassed[(size_t) n] = b;
	}


    /** Query whether the processor at index `Index` is bypassed. */
    template <int Index>
    bool isBypassed() const noexcept    { return bypassed[(size_t) Index]; }

    /** Prepare all inner processors with the provided `ProcessSpec`. */
    virtual void prepare (const dsp::ProcessSpec& spec)
    {
		if (mixer)
			mixer->prepare(spec);
		
        juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t)
		{
			proc.prepare (spec);
		}, processors);
    }
	
	virtual void preProcess  (ContextR& c) {}
	virtual void preProcess  (ContextNR& c) {}

	virtual void postProcess (ContextR& c) {}
	virtual void postProcess (ContextNR& c) {}
	
	virtual void preBypassProcess  (ContextR& c, bypassParams) {}
	virtual void preBypassProcess  (ContextNR& c, bypassParams) {}

	virtual void postBypassProcess (ContextR& c, bypassParams) {}
	virtual void postBypassProcess (ContextNR& c, bypassParams) {}
	
	
	/** Reset all inner processors. */
	void reset()
	{
		juce::dsp::detail::forEachInTuple ([] (auto& proc, size_t)
		{
			proc.reset();
		}, processors);
	}
//	
//	//Alerts FX when chain sets it to bypassed
//	void startBypass() {
//		juce::dsp::detail::forEachInTuple ([] (auto& proc, size_t)
//		{
//			proc.startBypass();
//		}, processors);
//	}
//	//Alerts FX when chain turns off its bypass
//	void endBypass() {
//		juce::dsp::detail::forEachInTuple ([] (auto& proc, size_t)
//		{
//			proc.endBypass();
//		}, processors);
//	}
	template <typename ProcessContext>
	void bypassProcess (ProcessContext& context, bypassParams) noexcept
	{
		preBypassProcess(context, bypassCall);
		juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
		{
			proc.bypassProcess(context, bypassCall);
		}, processors);
		postBypassProcess(context, bypassCall);
	}
	
    void process (ContextR& context) noexcept
    {
		if (mixer)
			mixer->frame();

		
		if (context.isBypassed() || (mixer && mixer->bypassed()))
		{
			bypassProcess<ContextR>(context, false, false);
			return;
		}
		if (bypass)
		{
			bypassProcess<ContextR>(context, true, false);
			return;
		}
		if (mixer)
			mixer->replacingPreMix(context);

		preProcess(context);
		
//		Util::printAnalysis(context.getInputBlock(), String(long(this))+" R", 2, true);

		juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
		{
			if (!bypassed[index])
			{
				proc.process(context);
			}
			else
			{
				proc.bypassProcess(context, true, false);
			}
		}, processors);
		
		postProcess(context);
	
		if (mixer)
			mixer->replacingPostMix(context);
	}
	
    void process (ContextNR& context) noexcept
    {
		if (mixer)
			mixer->frame();
		
		
		if (context.isBypassed() || (mixer && mixer->bypassed()) )
		{
			bypassProcess<ContextNR>(context, false, false);
			return;
		}
		if (bypass)
		{
			bypassProcess<ContextNR>(context, true, false);
			return;
		}
//		Util::printAnalysis(context.getInputBlock(), "Chain NR "+String(long(this)), 2, true);
		
		preProcess(context);
		
		ContextR replacing(context.getStorage(), context.getOutput());
		
		bool hasNR=false;
		
		juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
		{
			if (!hasNR)
			{
				if (!bypassed[index])
				{

					proc.process (context);
					hasNR=true;
				}
				else
				{
					proc.bypassProcess(context, true, false);
				}
			}
			else
			{
				if (!bypassed[index])
				{
					proc.process (replacing);
				}
				else
				{
					proc.bypassProcess(replacing, true, false);
				}
			}
		}, processors);
		
		if (!hasNR)
		{
			context.bypassProcess();
		}
		
		postProcess(context);
		
		if (mixer)
			mixer->nrMix(context);
	}

	
	void sidechain(InputBlock& input, ContextR& context)
	{
		if (mixer)
			mixer->frame();
		
		
		if (context.isBypassed() || (mixer && mixer->bypassed()) )
		{
			bypassProcess<ContextR>(context, false, false);
			return;
		}
		if (bypass)
		{
			bypassProcess<ContextR>(context, true, false);
			return;
		}
		if (mixer)
			mixer->replacingPreMix(context);
		
		preProcess(context);
				
		juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
	    {
			if (!bypassed[index])
			{
				proc.sidechain(input, context);
			}
			else
			{
				proc.bypassProcess(context, true, false);
			}
		}, processors);
		
		postProcess(context);
		
		if (mixer)
			mixer->replacingPostMix(context);
	}
	
//	void sidechain(InputBlock& input, ContextNR& context)
//	{
//		if (mixer)
//			mixer->frame();
//		
//		if (bypass || (mixer && mixer->bypassed()) || context.isBypassed())
//		{
//			bypassProcess<ContextNR>(context);
//			return;
//		}
//		if (mixer)
//			mixer->replacingPreMix(context);
//		
//		preProcess(context);
//				
//		juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
//	    {
//			if (!bypassed[index])
//			{
//				proc.sidechain(input, context);
//			}
//			else
//			{
//				proc.skipProcess(context);
//			}
//		}, processors);
//		
//		postProcess(context);
//		
//		if (mixer)
//			mixer->replacingPostMix(context);
//	}
	
	
//	bool hasBeenPrepared()
//	{
//		if (std::tuple_size<decltype(processors)>::value >= 1)
//		{
//			return std::get<0>(processors).hasBeenPrepared();
//		}
//		else
//		{
//			//Chain of size 0 cannot answer hasBeenPrepared
//			jassert (false);
//			return true;
//		}
//	}
	
	virtual ~Chain() {
		//Added
		if (mixer)
			delete mixer;
	}

protected://Changed to protected
//	ProcessData pd;
	std::tuple<Processors...> processors;
	std::array<bool, sizeof...(Processors)> bypassed { {} };
	
	// ------------ After this point is RT Added -----------------
	
private:
	template<typename Dummy, typename Value>
	Value _fwd(Value&& value)
	{ return std::forward<Value>(value); }
public:
	
	Chain() :
//	pd(),
//	processors(_fwd<Processors>(pd)... ),
	processors(),
	bypass(false), mixer(NULL)
	{
	}
	
	void setInstrData(InstrData* instrData) {
		juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t)
		{
			proc.setInstrData (instrData);
		}, processors);
	}
	void attachMixer() {
		if (!mixer)
			mixer = new Mixer();
	}
	void setMix(float set) {
		if (mixer)
			mixer->setMix(set);
	}
	void setIntMix(int set) {
		if (mixer)
			mixer->setMix(set/100.f);
	}
	void setBypass(bool set) { bypass = set;}
	
protected:
	bool bypass;
	Mixer* mixer;
};

//template <typename... Processors>
//using Chain = Chain<false,  Processors...>;
//
//template <typename... Processors>
//using InstrChain = Chain<true,  Processors...>;
