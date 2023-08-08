/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "RT/Modules.h"

#include "ProcessChain.h"
#include "Preset.h"

//==============================================================================
/**
*/
typedef FXProcessor<ProcessChain, TemplatePreset>
ProcBase;

class Processor : public ProcBase
{
public:
    //==============================================================================
    Processor();
    ~Processor() override;

	
	GuiEditor* createEditor() override;
	
	//Prepare extras
	//void connectPreset() override;
	//void postPrepare(bool reprepare) override;
	//void repreparePreDeletion() override;
	
	//Process extras
	//void preprocessMidi(MidiBuffer& midi, int numSamples) override;
	//void preProcess(ContextR& context) override;
	//void inLockPreProcess(ContextR& context) override;
	//void inLockPostProcess(ContextR& context) override;
	//void postProcess(ContextR& context) override;
	
	//Process bypass extras
	//void bypassPreProcess(ContextR& context) override;
	//void bypassProcess(ContextR& context) override;
	//void bypassProcess(AudioBuffer<double>& buffer) override;
	//void handleLockedProcessor(ContextR& context, bool bypassed) override;

	//Load/save extras
	//void xmlSave(XmlElement*) override;
	//void xmlLoad(XmlElement*) override;

	AudioProcessorParameter* getBypassParameter() override { return nullptr; }
	
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Processor)
};
