//
//  Splitter.h
//  
//
//  Created by Isaac Roberts on 2/12/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "Templates/Shared.h"

using namespace juce;

template <typename... Processors>
class Splitter
{
public:
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

	/** Prepare all inner processors with the provided `ProcessSpec`. */
	virtual void prepare (const ProcessSpec& spec)
	{
		//Added
		if (mixer)
			mixer->prepare(spec);
		//
		juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t) { proc.prepare (spec); }, processors);
	}
	void setInstrData(InstrData* instrData) {
		juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t)
		{
			proc.setInstrData (instrData);
		}, processors);
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
	
	void bypassProcess (ContextR& context, bypassParams) noexcept
	{
		if (!allowTail || inputIsSilent)
		{
			juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
											   {
				proc.bypassProcess(context, bypassCall);
			}, processors);
		}
		else
		{
			if (mixer)
				mixer->frame();
			
			if (mixer)
				mixer->replacingPreMix(context);
			
			if (!anyPlaying())
			{
//				context.getOutput().clear();
			}
			else if (std::tuple_size<std::tuple<Processors...>>::value == 1)
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

				juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
				{
					if (!silenced[index])
					{
						proc.bypassProcess (throughContext, bypassCall);
						context.getOutput().add(*workspace);
					}
					else
					{
						proc.bypassProcess(context, false, inputIsSilent);
					}
				}, processors);
			}
			
			
			if (mixer)
				mixer->replacingPostMix(context);

		}
	}
	void bypassProcess (ContextNR& context, bypassParams) noexcept
	{
		if (!allowTail || inputIsSilent)
		{
			juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
											   {
				proc.bypassProcess(context, bypassCall);
			}, processors);
		}
		else
		{
			if (mixer)
				mixer->frame();
			
			if (!anyPlaying())
			{
//				context.getOutput().clear();
			}
			else if (std::tuple_size<std::tuple<Processors...>>::value == 1)
			{
				std::get<0>(processors).bypassProcess(context, bypassCall);
			}
			else
			{
				BlockPtr output(context);
							
				//TODO: I changed this without having tried it
				ContextNR firstContext(context.getStorage(), context.getInput(), *output);
				context.getOutput().clear();
				
				juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
				{
					if (!silenced[index])
					{
						proc.bypassProcess(firstContext, bypassCall);
						context.getOutput().add(*output);
					}
					else
					{
						proc.bypassProcess(context, false, inputIsSilent);
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
		
		if ((mixer && mixer->bypassed()) || context.isBypassed() )
		{
			bypassProcess(context, false, false);
			return;
		}
		if (bypass)
		{
			bypassProcess(context, true, false );
			return;
		}
		
		if (mixer)
			mixer->replacingPreMix(context);
		
		if (!anyPlaying())
		{
			context.getOutput().clear();
		}
		else if (std::tuple_size<std::tuple<Processors...>>::value == 1)
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

			juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
			{
				if (!silenced[index])
				{
					proc.process (throughContext);
					context.getOutput().add(*workspace);
				}
				else
				{
					proc.bypassProcess(context, false, false);
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
		
		if ( (mixer && mixer->bypassed()) || context.isBypassed() )
		{
			bypassProcess(context, false, false);
			return;
		}
		if (bypass)
		{
			bypassProcess(context, true, false);
			return;
		}
		
		if (!anyPlaying())
		{
			context.getOutput().clear();
		}
		else if (std::tuple_size<std::tuple<Processors...>>::value == 1)
		{
			std::get<0>(processors).process(context);
		}
		else
		{
			BlockPtr output(context);
						
			//TODO: I changed this without having tried it
			ContextNR firstContext(context.getStorage(), context.getInput(), *output);
			context.getOutput().clear();
			
			juce::dsp::detail::forEachInTuple ([&] (auto& proc, size_t index) noexcept
			{
				if (!silenced[index])
				{
					proc.process(firstContext);
					context.getOutput().add(*output);
				}
				else
				{
					proc.bypassProcess(context, false, false);
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
////			static_assert(false, "Splitter of size 0 cannot answer hasBeenPrepared");
//		}
//	}
	
	virtual ~Splitter() {
		//Added
		if (mixer)
			delete mixer;
	}
	
	void setSilenced(int ix, bool set ) {
		silenced[ix] = set;
	}

protected://Changed to protected
	std::tuple<Processors...> processors;
	std::array<bool, sizeof...(Processors)> silenced { {} };
	
	// ------------ After this point is RT Added -----------------
public:
	Splitter()
	: bypass(false), mixer(NULL)
	{}
	
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

