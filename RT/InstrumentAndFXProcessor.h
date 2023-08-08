//
//  InstrumentAndFXProcessor.h
//  
//
//  Created by Isaac Roberts on 7/16/21.
//

// Created by Isaac Roberts 

#pragma once

#if 0

#include <JuceHeader.h>

#include "BasicFXProcessor.h"
#include "GuiEditor.h"

#include "Instruments/Instrument.h"
#include "FX/Chain.h"
#include "RTGraphics/Presets/Preset.h"


#define PRINT_OVERFLOWS (DEBUG && (TARGET_STANDALONE || DEBUG_WINDOW))

//Timing functions
#include <iostream>
#include <chrono>
//using namespace std::chrono;

#include "FX/RTGain.h"

using namespace juce;

/*
 // Paste this into your code and replace Processor with your class name.
 juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
 {
	 return new Processor();
 }
 */

struct DummyProcessChain : public Chain<RTGain>
{
	void prepare(const juce::dsp::ProcessSpec& spec) override {	Chain<RTGain>::prepare(spec); }
	std::shared_ptr<PresetType<>> getPresetType() {return std::shared_ptr<PresetType<>>();}
};
typedef PresetType<> DummyPresetType;


template <typename ProcessChain,
		  typename PresetType>
class InstrumentAndFXProcessor : public BasicFXProcessor
{
public:
	/*
	  GuiEditor* createEditor() override;
	 
	  int getInstrumentAmt() const override;
	  Instrument* getInstrument(int n) override;
	 
	  //Prepare extras
	  void connectPreset() override;
	  void postPrepare(bool reprepare) override;
	  void repreparePreDeletion() override;
	  
	  //Process extras
	  void preFXProcess(ContextR& context) override;
	  void preInstrumentProcess(AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;

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
  */
	 GuiEditor* createEditor() override = 0;
	 
	 virtual int getInstrumentAmt() const =0;
	 virtual Instrument* getInstrument(int n)=0;

	 //Prepare extras
	 virtual void connectPreset() {}
	 virtual void postPrepare(bool reprepare) {}
	 virtual void repreparePreDeletion() {}
	 
	 //Process extras
	 virtual void preFXProcess(ContextR& context) {}
	 virtual void preInstrumentProcess(AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {}
	 virtual void inLockPreProcess(ContextR& context) {}
	 virtual void inLockPostProcess(ContextR& context) {}
	 virtual void postProcess(ContextR& context) {}

	 //Process bypass extras
	 virtual void bypassPreProcess(ContextR& context) {}
	 virtual void bypassProcess(ContextR& context) {}
	 virtual void bypassProcess(AudioBuffer<double>& buffer) {}
	 virtual void handleLockedProcessor(ContextR& context, bool bypassed) { if (!bypassed) context.getOutput().clear(); }
	 
	 //Load/save extras
	 virtual void xmlSave(XmlElement*) {}
	 virtual void xmlLoad(XmlElement*) {}
	 
	
protected: // Callables
	void addInstrumentWithPolyphony(Instrument* instr, int poly) {
		synth.addSound(instr);
		for (int n=0; n < poly; ++n)
		{
			synth.addVoice(instr->getAssociatedVoice());
		}
	}
	void addInstrument(Instrument* instr) { addInstrumentWithPolyphony(instr, instr->polyphony()); }
	 
 public:
	 //==============================================================================

	 InstrumentAndFXProcessor() : BasicFXProcessor(),
		 midiCollector(),
		 keyboardState(),
		 synth(),
		 prepared(false),
		 processorChain(NULL),
		 processorProtector(),
		 preset(NULL),
		 blockStorage(),
		 sampleRate(0), samples_per_block(0), channels(0),
		 bypass(false)
	 {
		 processorChain = new ProcessChain();
		 auto type = processorChain->getPresetType();
		 jassert (type!=nullptr);
		 preset = new typename PresetType::Preset(this, type);
		 keyboardState.addListener(&midiCollector);
		 //You need to add voices and sounds to the synth in your own constructor
	 }
	 
	 InstrumentAndFXProcessor(int neededBlocks) : BasicFXProcessor(),
		 prepared(false),
		 processorChain(NULL),
		 processorProtector(),
		 preset(NULL),
		 blockStorage(neededBlocks),
		 sampleRate(0), samples_per_block(0), channels(0),
		 bypass(false)
	 {
		 processorChain = new ProcessChain();
		 preset = new typename PresetType::Preset(this, processorChain->getPresetType());
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
 public:
	 void prepareToPlay (double sample_rate, int samplesPerBlock) override
	 {
		 midiCollector.reset(sample_rate);
		 synth.prepare(sample_rate);
		 
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
			 
			 //Critical Section
			 processorProtector.lock();
			 
			 processorChain->prepare(spec);

			 blockStorage.prepare(getTotalNumOutputChannels(), samplesPerBlock);
			 
			 connectPreset();

			 preset->updateAll();
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
 //				Debug::print ("different prepare: rate,samples,channels = ", sampleRate, samplesPerBlock,channels);
				 
				 processorProtector.lock();
				 
				 repreparePreDeletion();
				 
				 processorChain->getPresetType()->clearCallbacks();
				 
				 //Recreate process chain
				 delete processorChain;
				 //NULL to avoid other threads attempting to use the deleted object
				 processorChain = NULL;
				 processorChain = new ProcessChain();
				 
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
				 
				 preset->setType(processorChain->getPresetType());
				 
				 connectPreset();
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
		 preset->save(xml);
		 
		 xml->setAttribute("Bypass", bypass);

		 GuiEditor* e = getGuiEditor();
		 if (e != NULL)
			 e->saveState(xml);
		 
		 AudioPluginInstance::copyXmlToBinary (*xml, destData);
		 delete xml;
	 }
	 
	 void setStateInformation (const void* data, int sizeInBytes) override
	 {
		 std::unique_ptr<XmlElement> xml (AudioPluginInstance::getXmlFromBinary (data, sizeInBytes));

		 if (xml.get() != nullptr)
		 {
			  if (xml->hasTagName (getXmlName()))
			  {
				  preset->load(&*xml);
			  }
		 }
		 
		 bypass = xml->getIntAttribute("Bypass", bypass);
		 xmlLoad(&*xml);
		 
		 GuiEditor* e = getGuiEditor();
		 if (e != NULL) {
			 e->loadState(&*xml);
			 //TODO: else, save xmlElement for loadState
		 }
		 
		 preset->updateAll();
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
					 Debug::print("Overrun", (rr*100), "% over ", nBlocks<<" blocks.", overflows<< "Overflows.");
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
	 
	 void process__(OutputBlock& levels, ContextR& pcontext)
	 {
//		 levels.
		 preFXProcess(pcontext);
		 
		 if (processorProtector.try_lock() && processorChain != NULL)
		 {
			 inLockPreProcess(pcontext);
			 
			 processorChain->process(pcontext);
			 
			 inLockPostProcess(pcontext);
			 processorProtector.unlock();
		 }
		 else
		 {
			 handleLockedProcessor(pcontext, false);
		 }
		 
		 postProcess(pcontext);
	 }
	 void process_(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
	 {
		 blockStorage.setCurSamples(buffer.getNumSamples());
		 buffer.clear();
		 		 
		 preInstrumentProcess(buffer, midi);
		 
		 synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

		 //Move to FX-readable block
		 auto block = AudioBlock<float>(buffer);
		 ContextR pcontext = ContextR(blockStorage, block);
		 process__(block, pcontext);
	 }
	 
 public:
	 void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override
	 {
		 #if PRINT_OVERFLOWS
		 auto start = std::chrono::high_resolution_clock::now();
		 #endif
		 
		 juce::ScopedNoDenormals noDenormals;
		 
		 
		 if (bypass || processorChain==NULL)
		 {
			 processBlockBypassed(buffer, midi);
		 }
		 else
		 {
			 buffer.clear();
			 process_(buffer, midi);
			 Utility::frame();
		 }
		 
		 #if PRINT_OVERFLOWS
		 print_overflows(start, buffer.getNumSamples());
		 #endif
	 }
	 void processBlockBypassed (AudioBuffer<float> & buffer, MidiBuffer &midi) override
	 {
		 buffer.clear();
		 
		 blockStorage.setCurSamples(buffer.getNumSamples());
		 AudioBlock<float> block(buffer);
		 auto context = ContextR(blockStorage, block);

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
	 
	bool canSidechain() override { return false; }
	
	 //	bool toggleBypass();//returns current state
	 void setBypass(bool set) {
		 bypass = set;
		 
	 }
	 bool isBypassed() {return bypass;}
	 
	 bool hasPreset() override {
		 return preset != NULL;
	 }

	 std::shared_ptr<PresetType> getPresetType() {
		 return processorChain->getPresetType();
	 }
	 
	 typename PresetType::Preset* getPreset() {
		 return preset;
	 }
	 void setPreset(typename PresetType::Preset* p) {
		 preset = p;
	 }
	 
	 void clearUiCallbacks() override {
		 if (processorChain != NULL)
		 {
			 if (processorChain->getPresetType() != NULL)
			 {
				 processorChain->getPresetType()->clearUiCallbacks();
			 }
		 }
	 }
	 ProcessChain* getProcessChain() { return processorChain; }
	 void clearSynth() {
		 synth.clearSounds();
		 synth.clearVoices();
	 }
	 virtual ~InstrumentAndFXProcessor()
	 {
		 delete preset;
		 delete processorChain;
	 }
	 
	
private:
	MidiMessageCollector midiCollector;
	MidiKeyboardState keyboardState;
	Synthesiser synth;
	
 private:
	 bool prepared=false;
 protected:
	 ProcessChain* processorChain=NULL;
 private:
	 std::mutex processorProtector;
	 
 protected:
	 typename PresetType::Preset* preset=NULL;
	 
	 BlockStorage blockStorage;
	 
	 float sampleRate;
	 int samples_per_block;
	 int channels;
 private:
	 bool bypass=false;

 protected:
	 float getSampleRate() { return sampleRate;}
	 int getMaxSamples() { return samples_per_block;}
	 int getNumChannels() { return channels; }
	 bool hasPrepared() { return prepared; }
	 
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentAndFXProcessor);

};

#endif
