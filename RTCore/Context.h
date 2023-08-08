//
//  Context.h
//  
//
//  Created by Isaac Roberts on 1/14/21.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>

//#include ".BlockStorage.h"


using namespace juce;


struct ContextNR {
	ContextNR(BlockStorage& b, dsp::ProcessContextNonReplacing<float>& cnt)
	:
		context(cnt),
		blockStorage(b)
	{
	}
	ContextNR(BlockStorage&b, InputBlock& i, OutputBlock& block)
	: context(dsp::ProcessContextNonReplacing<float>(i, block)),
	blockStorage(b)
	{
		
	}

	int getNumSamples() {return (int)context.getInputBlock().getNumSamples();}
	int getNumChannels() { return (int)context.getInputBlock().getNumChannels(); }
	
	void bypassProcess() {
		context.getOutputBlock().copyFrom(context.getInputBlock());
	}
//	ContextNR(BlockStorage& b, AudioBuffer<float>& input);
	
	inline InputBlock& getInput() { return context.getInputBlock();}
	inline OutputBlock& getOutput() { return context.getOutputBlock(); }
	
	inline InputBlock& getInputBlock() { return context.getInputBlock();}
	inline OutputBlock& getOutputBlock() { return context.getOutputBlock(); }
	
	inline BlockStorage& getStorage() { return blockStorage; }
	
	inline bool isReplacing() {return false; }
	inline bool nonReplacing() { return true; }
	
	inline bool isBypassed() {
		return  context.isBypassed ||
				context.getOutputBlock().getNumSamples() < 0;
	}
	inline void setBypass(bool set) { context.isBypassed = set; }
	inline void setBypassed(bool set) { context.isBypassed = set; }
	
	inline juce::dsp::ProcessContextNonReplacing<float>& getContext() {return context; }
	
	BlockPtr blockPtr() { return BlockPtr(blockStorage);}
	
//	dsp::AudioBlock<float> input;
//	dsp::AudioBlock<float> output;
	
	juce::dsp::ProcessContextNonReplacing<float> context;
	BlockStorage& blockStorage;
	
	~ContextNR() {}

};


struct ContextR {
	ContextR(BlockStorage& b, dsp::ProcessContextReplacing<float>& cnt)
	:   context(cnt),
		blockStorage(b)
	{
	}
	ContextR(BlockStorage&b, OutputBlock& inBlock)
	:   context(dsp::ProcessContextReplacing<float>(inBlock)),
		blockStorage(b)
	{
		
	}
	ContextR(ContextNR& c)
	:   context(dsp::ProcessContextReplacing<float>(c.getOutput())),
		blockStorage(c.getStorage())
	{
		context.isBypassed = c.isBypassed();
	}
	
	int getNumSamples() {return (int)context.getInputBlock().getNumSamples();}
	int getNumChannels() { return (int)context.getInputBlock().getNumChannels(); }
	
	inline void bypassProcess() {}
	
	inline InputBlock& getInput() { return context.getInputBlock();}
	inline OutputBlock& getOutput() { return context.getOutputBlock(); }
	
	
	inline InputBlock& getInputBlock() { return context.getInputBlock();}
	inline OutputBlock& getOutputBlock() { return context.getOutputBlock(); }
	
	inline BlockStorage& getStorage() { return blockStorage; }
	
	inline bool isReplacing() {return true; }
	inline bool nonReplacing() { return false; }
	
	inline bool isBypassed() {
		return  context.isBypassed ||
			context.getOutputBlock().getNumSamples() <= 0;
		
	}
	inline void setBypass(bool set) { context.isBypassed = set; }
	inline void setBypassed(bool set) { context.isBypassed = set; }
	
	BlockPtr blockPtr() { return BlockPtr(blockStorage);}
	
	inline juce::dsp::ProcessContextReplacing<float>& getContext() {return context; }
	
//	juce::dsp::AudioBlock<float> block;
	juce::dsp::ProcessContextReplacing<float> context;
	BlockStorage& blockStorage;
	
	
	~ContextR() {}

};

//typedef ContextR Context;
