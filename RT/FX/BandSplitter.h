//
//  BandSplitter.h
//  
//
//  Created by Isaac Roberts on 7/18/21.
//

#pragma once

#include <JuceHeader.h>

#include "Filters/LinkwitzRiley4th.h"
#include "Filters/LinkwitzRiley2nd.h"

#include "Templates/Shared.h"

using namespace juce;

template <typename... Processors>
class BandSplitter
{
public:
//	typedef std::tuple_size<std::tuple<typename Processors...>>::value BandAmt;
#define bandAmt sizeof...(Processors)
#define filterAmt bandAmt-1
	
	static constexpr int BandAmt()   { return bandAmt; }
	static constexpr int FilterAmt() { return filterAmt; }
	
	/** Get a reference to the processor at index `Index`. */
	template <int Index>       auto& get()       noexcept { return std::get<Index> (processors); }

	/** Get a reference to the processor at index `Index`. */
	template <int Index> const auto& get() const noexcept { return std::get<Index> (processors); }

	/** Set the processor at index `Index` to be bypassed or enabled. */
	template <int Index>
	void setSilenced (bool b) noexcept  { silenced[(size_t) Index] = b; }

	/** Query whether the processor at index `Index` is bypassed. */
	template <int Index>
	bool isSilenced() const noexcept    { return silenced[(size_t) Index]; }

	void setInstrData(InstrData* instrData) {
		juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t)
		{
			proc.setInstrData (instrData);
		}, processors);
	}
	
	void setFreq(int n, float freq)
	{
		hiFilter[n]->setFreq(freq);
		loFilter[n]->setFreq(freq);
	}
	
	void initFilters(bool fourthOrder=true)
	{
		//TODO: 2nd order filters have an amplitude dip at the crossover
		// Meaning they are as good as busted
		// Found with the FilterTester
		jassert(fourthOrder);
		
		if (filterAmt <= 0)
			return;
		
		if (loFilter[0]!=NULL)
		{
			if (fourthOrder != (dynamic_cast<LinkwitzRiley4thLo*>(loFilter[0])!=NULL))
			{
				for (int n=0; n < filterAmt; ++n)
				{
					delete loFilter[n];
					delete hiFilter[n];
				}
			}
			else return;
		}
		
		if (fourthOrder)
		{
			for (int n=0; n < filterAmt; ++n)
			{
				loFilter[n] = new LinkwitzRiley4thLo();
				hiFilter[n] = new LinkwitzRiley4thHi();
			}
		}
		else
		{
			for (int n=0; n < filterAmt; ++n)
			{
				loFilter[n] = new LinkwitzRiley2ndLo();
				hiFilter[n] = new LinkwitzRiley2ndHi();
			}
		}
		
	}
	
	/** Prepare all inner processors with the provided `ProcessSpec`. */
	virtual void prepare (const ProcessSpec& spec)
	{
		if (filterAmt > 0 && loFilter[0]==NULL) {
			initFilters(true);
		}
		//Added
		if (mixer)
			mixer->prepare(spec);
		//
		
		for (int n=0; n < filterAmt; ++n)
		{
			loFilter[n]->prepare(spec);
			hiFilter[n]->prepare(spec);
		}
		
		juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t) { proc.prepare (spec); }, processors);
	}

	/** Reset all inner processors. */
	void reset()
	{
		juce::dsp::detail::forEachInTuple ([] (auto& proc, size_t) { proc.reset(); }, processors);
	}
	
	//Alerts FX when chain sets it to bypassed
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
	void filterBand(int n, ContextR& c)
	{
		if (n > 0)
		{
			hiFilter[n-1]->process(c);
		}
		if (n < filterAmt)
		{
			loFilter[n]->process(c);
		}
		//TODO: has to do with phase shift of LR
		//if (n ==0)
		//	output *= -1;
	}
	void filterBand(int n, ContextNR& c)
	{
		auto output= c.getOutput();
		
		if (n > 0)
		{
			hiFilter[n-1]->process(c);
			if (n < filterAmt)
			{
				ContextR c2(c.getStorage(), output);
				loFilter[n]->process(c2);
			}
		}
		else
		{
			loFilter[n]->process(c);
		}
		//TODO: has to do with phase shift of LR
		//if (n ==0)
		//	output *= -1;
	}
	void filterBandSilenced(int n, int nSamples)
	{
		if (n > 0)
		{
			loFilter[n-1]->reset();
	//		loFilter[n-1].skipProcess(nSamples);
		}
		if (n < filterAmt)
		{
			hiFilter[n]->reset();
	//		hiFilter[n].skipProcess(nSamples);
		}
	}
	void filterAllBandsSilenced(int nSamples)
	{
		for (int n=0; n < filterAmt; ++n)
		{
			loFilter[n]->reset();
			hiFilter[n]->reset();
//			loFilter[n].skipProcess(nSamples);
//			hiFilter[n].skipProcess(nSamples);
		}
	}
	
	void bypassProcess (ContextR& context, bypassParams) noexcept
	{
		if (!allowTail || inputIsSilent || (mixer && mixer->bypassed()))
		{
			for (int n=0; n < filterAmt; ++n)
			{
				loFilter[n]->bypassProcess(context, bypassCall);
				hiFilter[n]->bypassProcess(context, bypassCall);
			}
			juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t) { proc.bypassProcess (context, bypassCall); }, processors);
		}
		else// if (allowTail)
		{
			if (mixer)
				mixer->frame();
			
			if (mixer)
				mixer->replacingPreMix(context);
			
			else if (bandAmt == 1)
			{
				std::get<0>(processors).bypassProcess(context, bypassCall);
			}
			else
			{
				BlockPtr input(context);
				input->copyFrom(context.getInput());
				
				BlockPtr workspace(context);
				
//				context.getOutput().clear();
				
				ContextNR throughContext(context.getStorage(), *input, *workspace);
				ContextR  contextR(context.getStorage(), *workspace);
				
				juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
				{
					if (!silenced[index])
					{
						filterBand((int)index, throughContext);
						proc.bypassProcess (contextR, bypassCall);
//						context.getOutput().add(*workspace);
					}
					else
					{
						filterBandSilenced((int)index, context.getNumSamples());
						proc.bypassProcess(context, false, true);
					}
				}, processors);
			}
			
			
			if (mixer)
				mixer->replacingPostMix(context);

		}
	}
	
	void bypassProcess (ContextNR& context, bypassParams) noexcept
	{
		if (!allowTail || inputIsSilent || (mixer && mixer->bypassed()) || !anyPlaying())
		{
			for (int n=0; n < filterAmt; ++n)
			{
				loFilter[n]->bypassProcess(context, bypassCall);
				hiFilter[n]->bypassProcess(context, bypassCall);
			}
			juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t) { proc.bypassProcess (context, bypassCall); }, processors);
		}
		else// if (allowTail)
		{
			if (mixer)
				mixer->frame();
			
			if (bandAmt == 1)
			{
				std::get<0>(processors).bypassProcess(context, bypassCall);
			}
			else
			{
				BlockPtr output(context);
								
				//TODO: I changed this without having tried it
				ContextNR firstContext(context.getStorage(), context.getInput(), *output);
				ContextR contextR(context.getStorage(), *output);
//				context.getOutput().clear();
				
				juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
				{
					if (!silenced[index])
					{
						filterBand((int)index, firstContext);
						proc.bypassProcess(contextR, true, false);
//						context.getOutput().add(*output);
					}
					else
					{
						filterBandSilenced((int)index, context.getNumSamples());
						proc.bypassProcess(context, false, true);
					}
				}, processors);
			}
			
			if (mixer)
				mixer->nrMix(context);
		}
	}
	
	bool anyPlaying() {
		for (int n=0; n < silenced.size(); ++n)
		{
			if (!silenced[n])
			{
				return true;
			}
		}
		return false;
	}
	
//	void autoset(float& x1, float& x2, float s1, float s2) {
//		x1 = s1;
//		x2 = s2;
//	}
	
	void process (ContextR& context) noexcept
	{
		if (mixer)
			mixer->frame();
		
		if (context.isBypassed())
		{
			bypassProcess(context, false, false);
			return;
		}
		if (bypass || (mixer && mixer->bypassed()) )
		{
			bypassProcess(context, true, false);
			return;
		}
		if (mixer)
			mixer->replacingPreMix(context);
		
		if (!anyPlaying())
		{
			context.getOutput().clear();
			filterAllBandsSilenced(context.getNumSamples());
		}
		else if (bandAmt == 1)
		{
			std::get<0>(processors).process(context);
		}
		else
		{
			BlockPtr input(context);
			input->copyFrom(context.getInput());
			
			BlockPtr workspace(context);
			
			context.getOutput().clear();
			
			ContextNR throughContext(context.getStorage(), *input, *workspace);
			ContextR  contextR(context.getStorage(), *workspace);
			
			juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
			{
				if (!silenced[index])
				{
					filterBand((int)index, throughContext);
					proc.process (contextR);
					//TODO: Crash here on AU after opening, when Logic said last selected audio not available 
					context.getOutput().add(*workspace);
				}
				else
				{
					filterBandSilenced((int)index, context.getNumSamples());
					proc.bypassProcess(context, false, true);
				}
			}, processors);
		}
		
		
		if (mixer)
			mixer->replacingPostMix(context);
	}
	
	void process (ContextNR& context) noexcept
	{
		if (mixer)
			mixer->frame();
		if (mixer && mixer->bypassed())
		{
			bypassProcess(context, false, false);
			return;
		}
		if (bypass || context.isBypassed() )
		{
			bypassProcess(context, true, false);
			return;
		}

		if (!anyPlaying())
		{
			context.getOutput().clear();
			filterAllBandsSilenced(context.getNumSamples());
		}
		else if (bandAmt == 1)
		{
			std::get<0>(processors).process(context);
		}
		else
		{
			BlockPtr output(context);
							
			//TODO: I changed this without having tried it
			ContextNR firstContext(context.getStorage(), context.getInput(), *output);
			ContextR contextR(context.getStorage(), *output);
			context.getOutput().clear();
			
			juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
			{
				if (!silenced[index])
				{
					filterBand((int)index, firstContext);
					proc.process(contextR);
					context.getOutput().add(*output);
//					hasNR = true;
				}
				else
				{
//					output->clear();
					filterBandSilenced((int)index, context.getNumSamples());
					proc.bypassProcess(context, false, true);
				}
			}, processors);
		}
		
		if (mixer)
			mixer->nrMix(context);
	}

	
	void sidechain(InputBlock& input, ContextR& context)
	{
		//Not implemented yet
		jassertfalse;
	}
	
//	bool hasBeenPrepared()
//	{
//		if (std::tuple_size<decltype(processors)>::value >= 1)
//		{
//			return std::get<0>(processors).hasBeenPrepared();
//		}
//		else
//		{
//			jassert(false);
////			static_assert(false, "BandSplitter of size 0 cannot answer hasBeenPrepared");
//		}
//	}
	
	virtual ~BandSplitter() {
		//Added
		if (mixer)
			delete mixer;
		for (int n=0; n < filterAmt; ++n)
		{
			delete loFilter[n];
			delete hiFilter[n];
		}
	}
	
	void setSilenced(int ix, bool set ) {
		silenced[ix] = set;
	}

protected://Changed to protected
	std::array<LinkwitzRiley*, filterAmt> hiFilter { {} };
	std::array<LinkwitzRiley*, filterAmt> loFilter { {} };
	std::tuple<Processors...> processors;
	std::array<bool, sizeof...(Processors)> silenced { {} };
	
	// ------------ After this point is RT Added -----------------
public:
	BandSplitter()
	: bypass(false), mixer(NULL)
	{
		for (int n=0; n < bandAmt; ++n)
			silenced[n] = false;
		for (int n=0; n < filterAmt; ++n)
		{
			loFilter[n] = NULL;
			hiFilter[n] = NULL;
		}
	}
	
	void attachMixer() {
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

