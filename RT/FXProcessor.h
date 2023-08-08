//
//  FXProcessor.h
//  
//
//  Created by Isaac Roberts on 3/22/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "BasicFXProcessor.h"
#include "GuiEditor.h"

#include "FX/Chain.h"
#include "Instruments/InstrData.h"

#if OLD_PRESET
#include "RTOldPresets/RTOldPresets.h"
#else
#include "RTPreset/RTPreset.h"
#endif

#if DEBUG
//For start ding
#include "FX/Filters/RBJ/Fixed.h"
#endif

#include "Utilities/DateKiller.h"

using namespace juce;

/*
 // Paste this into your code and replace Processor with your class name.
 juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
 {
	 return new Processor();
 }
 */
#include "FX/RTGain.h"

#if OLD_PRESET
struct DummyProcessChain : public Chain<RTGain>
{
	void prepare(const juce::dsp::ProcessSpec& spec) override {	Chain<RTGain>::prepare(spec); }
	std::shared_ptr<PresetType<>> getPresetType() {return std::shared_ptr<PresetType<>>();}
};
typedef PresetType<> DummyPresetType;
#else

struct DummyProcessChain : public Chain<RTGain>
{
	void prepare(const juce::dsp::ProcessSpec& spec) override {	Chain<RTGain>::prepare(spec); }
	void connectPreset(PresetObject* p);
};
struct DummyPresetType : public PresetObject {
	int getParamAmt() override { return 0; }
	int getSubPresetAmt() override { return 0; }
	ParameterBase* getParam(int n) override { return NULL; }
	PresetObject* getSubPreset(int n) override { return NULL; }
	String getXmlName() override { return "Dummy"; }
};

#endif

#define PRINT_OVERFLOWS (DEBUG && (TARGET_STANDALONE || DEBUG_WINDOW))

//Timing functions
#include <iostream>
#include <chrono>
//using namespace std::chrono;


template <typename ProcessChain,
		  typename PresetType,
		  bool HasInstr=false>
class FXProcessor : public BasicFXProcessor
{
public:
	//========= Overrides ========================================================


/*
	 GuiEditor* createEditor() override;
	 
	 //Prepare extras
	 void connectPreset() override;
	 void postPrepare(bool reprepare) override;
	 void repreparePreDeletion() override;
	 
	 //Process extras
	 void preprocessMidi(MidiBuffer& midi, int numSamples) override;
	 void preProcess(ContextR& context) override;
	 void inLockPreProcess(ContextR& context) override;
	 void inLockPostProcess(ContextR& context) override;
	 void postProcess(ContextR& context) override;
	 
	 //Process bypass extras
	 void bypassPreProcess(ContextR& context) override;
	 void bypassProcess(ContextR& context) override;
     void bypassProcess(AudioBuffer<double>& buffer) override;
	 void handleLockedProcessor(ContextR& context, bool bypassed) override;
 
	 //Load/save extras
	 void xmlSave(XmlElement*) override;
	 void xmlLoad(XmlElement*) override;
 
	 juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
	 {
		 return new Processor();
	 }

	 AudioProcessorParameter* getBypassParameter() override { return nullptr; }
 */
	/* Accessors:
	 protected:
		 void setBypass(bool set);
		 bool isBypassed();
		 
		 bool hasPreset();

		 PresetTypePtr getPresetType();
		 Preset* getPreset();
		 
		 ProcessChain* getProcessChain();
		 
		 float getSampleRate();
		 int getMaxSamples();
		 int getNumChannels();
		 bool hasPrepared();

	 protected:
		 ProcessChain* processorChain;
		 Preset* preset;
		 BlockStorage blockStorage;
		 float sampleRate;
		 int samples_per_block;
		 int channels;
	 
	 */
	
	
	GuiEditor* createEditor() override = 0;
	
	//Prepare extras
	virtual void connectPreset() {}
	virtual void postPrepare(bool reprepare) {}
	virtual void repreparePreDeletion() {}
	
	//Process extras
	virtual void preprocessMidi(MidiBuffer& midi, int numSamples) {}
	virtual void preProcess(ContextR& context) {}
	virtual void inLockPreProcess(ContextR& context) {}
	virtual void inLockPostProcess(ContextR& context) {}
	virtual void postProcess(ContextR& context) {}

	//Process bypass extras
	virtual void bypassPreProcess(ContextR& context) {}
	virtual void bypassProcess(ContextR& context) {}
	virtual void bypassProcess(AudioBuffer<double>& buffer) {}
	virtual void handleLockedProcessor(ContextR& context, bool bypassed) {}
	
	//Load/save extras
	virtual void xmlSave(XmlElement*) {}
	virtual void xmlLoad(XmlElement*) {}
	
	virtual bool needsDeletionOnReprepare() { return true; }
	
	virtual AudioProcessorParameter* getBypassParameter() override = 0;

	//==============================================================================
private:
	inline void _constructor() {
		processorChain = new ProcessChain();
		
#if OLD_PRESET
		auto type = processorChain->getPresetType();
		jassert (type!=nullptr);
		preset = new typename PresetType::Preset(this, type);
#else
		preset = new PresetType();
		preset->addToProcessor(this);
#endif
		
		if (HasInstr)
		{
			instrData = new InstrData();
			processorChain->setInstrData(instrData);
		}

	}
	
public:
	FXProcessor() : BasicFXProcessor(),
		prepared(false),
		processorChain(NULL),
		processorProtector(),
		preset(NULL),
		blockStorage(),
		sampleRate(0), samples_per_block(0), channels(0),
		bypass(false)
	{
		_constructor();
	}
	FXProcessor(int neededBlocks) : BasicFXProcessor(),
		prepared(false),
		processorChain(NULL),
		processorProtector(),
		preset(NULL),
		blockStorage(neededBlocks),
		sampleRate(0), samples_per_block(0), channels(0),
		bypass(false)
	{
		_constructor();
	}
	FXProcessor(BusesProperties customBusProperties) : BasicFXProcessor(customBusProperties),
		prepared(false),
		processorChain(NULL),
		processorProtector(),
		preset(NULL),
		blockStorage(),
		sampleRate(0), samples_per_block(0), channels(0),
		bypass(false)
	{
		_constructor();
	}
	FXProcessor(BusesProperties customBusProperties, int neededBlocks) : BasicFXProcessor(customBusProperties),
		prepared(false),
		processorChain(NULL),
		processorProtector(),
		preset(NULL),
		blockStorage(neededBlocks),
		sampleRate(0), samples_per_block(0), channels(0),
		bypass(false)
	{
		_constructor();
	}
	
	
	//==============================================================================
private:
	void internal_connectKnobs()
	{
		GuiEditor* editor = getGuiEditor();
		if (editor)
		{
			editor->connectFromProcessor();
		}
	}
protected:
	void connectKnobs()
	{
		auto* messageManager = juce::MessageManager::getInstanceWithoutCreating();
		if (messageManager->isThisTheMessageThread())
		{
			internal_connectKnobs();
		}
		else
		{
			messageManager->callAsync([&](){internal_connectKnobs();});
		}
	}
	void disconnectKnobs() {
		GuiEditor* editor = getGuiEditor();
		if (editor)
		{
			editor->markKnobsDisconnected();
		}
	}
public:
	void prepareToPlay (double sample_rate, int samplesPerBlock) override
	{
		crash_if_unpaid;

		if (sample_rate <= 1) {
			#if DEBUG_WINDOW
				Debug::print("Unusually low sample rate", sample_rate);
			#else
				std::cout<<"Unusually low sample rate "<< sample_rate <<std::endl;
			#endif
			//FX uses sample rate to tell whether prepare has been called, a zero rate might cause pointer issues
			sample_rate = 1;
		}
		
		if (instrData)
			instrData->prepare(sample_rate);
		
		if (!prepared)
		{
			//Save details
			sampleRate = sample_rate;
			samples_per_block = samplesPerBlock;
			channels = getTotalNumOutputChannels();

			//Copy to ProcessSpec
			ProcessSpec spec;
			spec.sampleRate = sampleRate;
			spec.numChannels = getTotalNumOutputChannels();
			spec.maximumBlockSize = samplesPerBlock;
			
#if DEBUG
			prepareDing(spec);
#endif
			
			//Critical Section
			processorProtector.lock();
			
			processorChain->prepare(spec);

			blockStorage.prepare(getTotalNumOutputChannels(), samplesPerBlock);
			
#if !OLD_PRESET
			processorChain->connectPreset(preset);
#endif
			connectPreset();
			preset->updateBackend();

			postPrepare(false);
			
			processorProtector.unlock();
			
			prepared=true;
		}
		else
		{
			bool sDiff = samples_per_block != samplesPerBlock;
			bool cDiff = channels != getTotalNumOutputChannels();
			bool rDiff = fabs(sampleRate - sample_rate) > 1;
			
			//Copy new info
			samples_per_block = samplesPerBlock;
			channels = getTotalNumOutputChannels();
			sampleRate = sample_rate;
			
			if (sDiff || cDiff || rDiff)
			{
				samples_per_block = samplesPerBlock;
				
				processorProtector.lock();

				repreparePreDeletion();

				bool wasDeleted = needsDeletionOnReprepare();
				if (wasDeleted)
				{
#if OLD_PRESET
					processorChain->getPresetType()->clearCallbacks();
#else
					preset->clearBackendCallbacks();
#endif
					
					//Recreate process chain
					delete processorChain;
					//NULL to avoid other threads attempting to use the deleted object
					processorChain = NULL;
					processorChain = new ProcessChain();

					processorChain->setInstrData(instrData);
				}
				//Block storage
				if (cDiff || sDiff) {
					blockStorage.refresh(channels, samplesPerBlock);
				}
				
				//Spec
				ProcessSpec spec;
				spec.sampleRate = sampleRate;
				spec.numChannels = channels;
				spec.maximumBlockSize = samplesPerBlock;

				//Prepare
				processorChain->prepare(spec);
			
#if OLD_PRESET
				preset->setType(processorChain->getPresetType());
				connectPreset();
#else
				if (wasDeleted)
				{
					processorChain->connectPreset(preset);
					connectPreset();
					preset->updateBackend();
				}
#endif
				postPrepare(true);
				
				connectKnobs();
				
				prepared=true;
				
				processorProtector.unlock();
			}
		}
	}
	
	void releaseResources() override
	{//Used to lighten the load while processor is idle
		
	}
	
	//==============================================================================
	void getStateInformation (juce::MemoryBlock& destData) override
	{
		
		XmlElement* xml = new XmlElement (getXmlName());
		
		preset->save(xml, false);
		xml->setAttribute("Bypass", bypass);
		xmlSave(xml);
		
		GuiEditor* e = getGuiEditor();
		if (e != NULL && e->hasFinishedConstructing())
		{
			e->saveState(xml->createNewChildElement("UI"));
		}
		else
		{
//			uiXml->setTagName("UI");
			xml->addChildElement(uiXml.release());
		}
		
		AudioPluginInstance::copyXmlToBinary (*xml, destData);
		delete xml;
	}
	
	void setStateInformation (const void* data, int sizeInBytes) override
	{
#if DEBUG
		std::cout<<"Loading xml..."<<std::endl;
#endif
		std::unique_ptr<XmlElement> xml (AudioPluginInstance::getXmlFromBinary (data, sizeInBytes));
		
		if (xml.get() != nullptr)
		{
			 if (xml->hasTagName (getXmlName()))
			 {
				 preset->load(&*xml, false);
			 }
		}
		
		bypass = xml->getIntAttribute("Bypass", bypass);
		xmlLoad(&*xml);
		
		GuiEditor* e = getGuiEditor();
		if (e != NULL) {
			e->loadState(&*xml);
		}
		else
		{
			auto child = xml->getChildByName("UI");
			if (child)
			{
				//Deep copies
				uiXml = std::make_unique<XmlElement>(*child);
//				uiXml.reset(child);
			}
		}
#if DEBUG
		std::cout<<"XML Loaded"<<std::endl;
#endif
		
//		preset->updateBackend();
	}
	
private:
	void print_overflows(std::chrono::time_point<std::chrono::high_resolution_clock>& start, int nSamples)
	{
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		
		static int nBlocks=0;
		static float ratioSum=0;
		static int overflows=0;
		
		auto ratio = duration.count() * sampleRate / nSamples;
		
		ratio /= 1e6;
		
		ratioSum+=ratio;
		nBlocks++;
		
		if (ratio > 1)
		{
			++overflows;
		}
		
		const int printFreq=100;
		if (nBlocks >= printFreq)
		{
			float rr = ratioSum / nBlocks;
			if (rr > 1)
			{
				#if DEBUG_WINDOW
					Debug::print("Overrun", (rr*100), "% over ", nBlocks, " blocks.", overflows, "Overflows.");
				#else
					std::cout<<"Overrun "<<(rr*100)<<"% over "<<nBlocks<<" blocks. "<<overflows<<" Overflows."<<std::endl;
				#endif
			}
			else if (overflows > 0)
			{
				#if DEBUG_WINDOW
					Debug::print(overflows, "Overflows out of", nBlocks, ".", (100*overflows)/nBlocks, "%");
				#else
					std::cout<<overflows<<" Overflows out of "<<nBlocks<<".\n ";//<<((100*overflows)/nBlocks)<<"%"<<std::endl;
				#endif
			}
			nBlocks = 0;
			ratioSum = 0;
			overflows=0;
		}
	}
	
	void process__(OutputBlock& levels, ContextR& pcontext, bool useSidechain)
	{
		if (HasInstr)
		{
			levels.clear();
		}
		doDing(pcontext);
		
		preProcess(pcontext);
		
		if (processorProtector.try_lock() && processorChain != NULL)
		{
			inLockPreProcess(pcontext);
			
			if (useSidechain)
			{
				processorChain->sidechain(levels, pcontext);
			}
			else
			{
				processorChain->process(pcontext);
			}
			
			inLockPostProcess(pcontext);
			processorProtector.unlock();
		}
		else
		{
			handleLockedProcessor(pcontext, false);
		}
		
		postProcess(pcontext);
	}
	void process_(juce::AudioBuffer<float>& buffer)
	{
		blockStorage.setCurSamples(buffer.getNumSamples());
		
		bool moreIn = getTotalNumInputChannels() > getTotalNumOutputChannels();
		
		if (moreIn)
		{
			if (canSidechain())
			{
				auto buffIn 	= getBusBuffer (buffer, true, 0);
				auto buffLevels = getBusBuffer (buffer, true, 1);
				
				auto block = AudioBlock<float>(buffIn);
				auto levels = AudioBlock<float>(buffLevels);
				ContextR pcontext = ContextR(blockStorage, block);
				process__(levels, pcontext, true);
			}
			else
			{
				auto buff2 = getBusBuffer (buffer, true, 0);
				auto block = juce::dsp::AudioBlock<float>(buff2);
				ContextR pcontext = ContextR(blockStorage, block);
				process__(block, pcontext, false);
			}
		}
		else
		{
			auto block = AudioBlock<float>(buffer);
			ContextR pcontext = ContextR(blockStorage, block);
			process__(block, pcontext, false);
		}
	}
	
public:
	void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override
	{
		#if PRINT_OVERFLOWS
		auto start = std::chrono::high_resolution_clock::now();
		#endif
		
		if (!prepared)
		{
			buffer.clear();
			return;
		}
		juce::ScopedNoDenormals noDenormals;
		
		if (HasInstr)
		{
			instrData->bufferPtr = &midi;
			preprocessMidi(midi, buffer.getNumSamples());
		}
		
		for (auto i = getTotalNumInputChannels(); i < buffer.getNumChannels(); ++i)
		{
			buffer.clear (i, 0, buffer.getNumSamples());
		}
		if (bypass || processorChain==NULL)
		{
			processBlockBypassed(buffer, midi);
		}
		else
		{
			process_(buffer);
			Utility::frame();
		}
		
#if DEBUG
		protectYourEars(buffer);
#endif
		
		
#if !JucePlugin_ProducesMidiOutput
		if (HasInstr)
		{
			//Juce asserts-false if you don't do this 
			midi.clear();
		}
#endif
		
		#if PRINT_OVERFLOWS
		print_overflows(start, buffer.getNumSamples());
		#endif
	}
	void processBlockBypassed (AudioBuffer<float> & buffer, MidiBuffer &midi) override
	{
		if (!prepared)
		{
			buffer.clear();
			return;
		}
		
		blockStorage.setCurSamples(buffer.getNumSamples());
		AudioBlock<float> block(buffer);
		auto context = ContextR(blockStorage, block);
		
		if (HasInstr)
		{
			instrData->bufferPtr = &midi;
			block.clear();
		}

		if (processorChain != NULL && processorProtector.try_lock())
		{
			bypassPreProcess(context);
			processorChain->bypassProcess(context, false, false);
			bypassProcess(context);
			processorProtector.unlock();
		}
		else
		{
			handleLockedProcessor(context, true);
		}
				
		protectYourEars(buffer);
	}
	
	AudioBuffer<float> getFloatCast(AudioBuffer<double>& buffer) {
		AudioBuffer<float> fBuffer;
		fBuffer.makeCopyOf(buffer);
		return fBuffer;
	}
	void copyToDouble(AudioBuffer<float>& fBuffer, AudioBuffer<double>& dBuffer)
	{
		//Copy back to double buffer
		int numSamples = dBuffer.getNumSamples();
		for (int c=0; c < dBuffer.getNumChannels(); ++c)
		{
			double* data = dBuffer.getWritePointer(c);
			const float* read = fBuffer.getReadPointer(c);
			for (int n=0; n < numSamples; ++n)
			{
				data[n] = static_cast<double>(read[n]);
			}
		}
	}
	void processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midi) override
	{
		//Copy to float buffer
		AudioBuffer<float> fBuffer = getFloatCast(buffer);
		processBlock(fBuffer, midi);
		copyToDouble(fBuffer, buffer);
	}
	
	void processBlockBypassed (AudioBuffer<double> & buffer, MidiBuffer &midi) override
	{
		bypassProcess(buffer);
	}
	
	//	bool toggleBypass();//returns current state
	void setBypass(bool set) {
		bypass = set;
		
	}
	bool isBypassed() {return bypass;}
	
	bool hasPreset() override {
		return preset != NULL;
	}
#if OLD_PRESET
	std::shared_ptr<PresetType> getPresetType() {
		return processorChain->getPresetType();
	}
	
	typename PresetType::Preset* getPreset() {
		return preset;
	}
	void setPreset(typename PresetType::Preset* p) {
		//This would leak memory
		jassertfalse;
		preset = p;
	}
#else
	PresetType* getPreset() { return preset; }
	const PresetType* getPreset() const { return preset; }

#endif
	
	void clearUiCallbacks() override
	{
#if OLD_PRESET
		if (processorChain != NULL)
		{
			if (processorChain->getPresetType() != NULL)
			{
				processorChain->getPresetType()->clearUiCallbacks();
			}
		}
#else
		preset->clearUiCallbacks();
#endif
	}
	ProcessChain* getProcessChain() { return processorChain; }
	ProcessChain* getProcessorChain() { return processorChain; }

	virtual ~FXProcessor()
	{
		delete preset;
		delete processorChain;
		delete instrData;
	}
	
private:
#if DEBUG
	void prepareDing(Spec& spec)
	{
//		filter.setType(RTRBJ::LowPass);
//		filter.setFreq(1200);
//		filter.setQ(.7);
//		filter.prepare(spec);
	}
	void doDing(ContextR& c)
	{
		if (HasInstr)
		{
			using namespace Osc;
			if (startDing < 10)
			{
				auto& block = c.getOutputBlock();
				
				for (int n=0; n < block.getNumSamples(); ++n)
				{
					float p= startDing * 6.28;
					
					//F4 + A5 + C6
	//				int ns= 0;
	//				float x = 0;
	//				x += saw(p * freq(5+ns, 2));
	//				x += saw(p * freq(10+ns, 2));
	//				x += saw(p * freq(0+ns, 3));
	//				x *= pow(.1f, startDing / ( .3f ));
	//				x *= std::min(1.0f, (startDing)*50.f);
	//				x = filter.processSingle(x);
					
					float x2 = sin(p * freq(2, 5));// + sin(p*880) + sin(p*1046.5);
					x2 *= pow(.1f, startDing/.4f);
					x2 *= std::min(1.0f, (startDing)*8);
	//				x = x2;
					
					startDing += 1/sampleRate;
					for (int c=0; c < block.getNumChannels(); ++c)
					{
						block.setSample(c, n, x2);
					}
	//				if (startDing <= 0)
	//					return;
				}
			}
		}
	}
#else
	void doDing(ContextR& ) {}
#endif
	
#if DEBUG
//	void protectYourEars(AudioBuffer<float>& buff)
//	{
//		int N = buff.getNumSamples();
//		bool silence = false;
//		for (int c=0; c < buff.getNumChannels(); ++c)
//		{
//			bool firstWarning = true;
//			auto buffer = buff.getWritePointer(c);
//			for (int i = 0; i < N; ++i)
//			{
//				float x = buffer[i];
//				if (std::isnan(x)) {
//					DBG("!!! WARNING: nan detected in output, silencing !!!");
//					silence = true;
//				} else if (std::isinf(x)) {
//					DBG("!!! WARNING: inf detected in output, silencing !!!");
//					silence = true;
//				} else if (x < -2.0f || x > 2.0f) {  // screaming feedback
//					DBG("!!! WARNING: audio louder than 6db, silencing !!!");
//					silence = true;
//				} else if (x < -1.0f) {
//					if (firstWarning) {
//						DBG("!!! WARNING: audio louder than 0db, clamping !!!");
//						firstWarning = false;
//					}
//					buffer[i] = -1.0f;
//				} else if (x > 1.0f) {
//					if (firstWarning) {
//						DBG("!!! WARNING: audio louder than 0db, clamping !!!");
//						firstWarning = false;
//					}
//					buffer[i] = 1.0f;
//				}
//				if (silence) {
//					buff.clear();
//					return;
//				}
//			}
//
//		}
//	}

	void protectYourEars(AudioBuffer<float>& buff)
	{
//		if(!Format::isStandalone())
//			return;
//		
//		int N = buff.getNumSamples();
//		for (int c=0; c < buff.getNumChannels(); ++c)
//		{
////			bool firstWarning = true;
//			auto buffer = buff.getWritePointer(c);
//			for (int i = 0; i < N; ++i)
//			{
//				float x = buffer[i];
//				if (std::isnan(x)) {
////					if (firstWarning) {
////						firstWarning = false;
////						DBG("!!! WARNING: nan detected in output, zeroing !!!");
////					}
//					buffer[i] = 0.0f;
//				} else if (x < -1.0f) {
////					if (firstWarning) {
////						DBG("!!! WARNING: audio louder than 0db, clamping !!!");
////						firstWarning = false;
////					}
//					buffer[i] = -1.0f;
//				} else if (x > 1.0f) {
////					if (firstWarning) {
////						DBG("!!! WARNING: audio louder than 0db, clamping !!!");
////						firstWarning = false;
////					}
//					buffer[i] = 1.0f;
//				}
//			}
//		}
	}

#else
	void protectYourEars(AudioBuffer<float>& buff)
	{}
#endif
	
private:
	bool prepared=false;
protected:
	ProcessChain* processorChain=NULL;
private:
	std::mutex processorProtector;
	
protected:
#if OLD_PRESET
	typename PresetType::Preset* preset=NULL;
#else
	PresetType* preset;
#endif
	
	BlockStorage blockStorage;
	
	float sampleRate;
	int samples_per_block;
	int channels;
private:
	bool bypass=false;
	InstrData* instrData = NULL;

protected:
	float getSampleRate() { return sampleRate;}
	int getMaxSamples() { return samples_per_block;}
	int getNumChannels() { return channels; }
	bool hasPrepared() { return prepared; }
	
#if DEBUG
	float startDing = 0.f;
//	RTRBJ filter;
public:
	void disableDing() { startDing = 10.f; }
	void reDing() { startDing = 0.f; }
#endif
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FXProcessor);

};
