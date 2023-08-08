/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "InstrumentProcessor.h"

#if 0

//==============================================================================
InstrumentProcessor::InstrumentProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), false)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#else
	:
#endif
	midiCollector(),
	keyboardState()
{
	keyboardState.addListener(&midiCollector);
	//You need to add voices and sounds to the synth in your own constructor
}

InstrumentProcessor::~InstrumentProcessor()
{
}

void InstrumentProcessor::addInstrumentAndPolyphony(Instrument* instr, int poly)
{
	synth.addSound(instr);
	for (int n=0; n < poly; ++n)
	{
		synth.addVoice(instr->getAssociatedVoice());
	}
}

//==============================================================================
const juce::String InstrumentProcessor::getName() const
{
    return JucePlugin_Name;
}
const String InstrumentProcessor::getProgramName (int index)
{
	return JucePlugin_Name;
}
int InstrumentProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int InstrumentProcessor::getCurrentProgram()
{
    return 0;
}

void InstrumentProcessor::setCurrentProgram (int index)
{
}


void InstrumentProcessor::changeProgramName (int index, const juce::String& newName)
{
}


void InstrumentProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void InstrumentProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	midiCollector.reset(sampleRate);
	synth.setCurrentPlaybackSampleRate(sampleRate);
	prepare(sampleRate, samplesPerBlock);
}

void InstrumentProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

//	MidiBuffer incomingMidi;
//	midiCollector.removeNextBlockOfMessages (incomingMidi, buffer.getNumSamples());
	
//	std::cout<<midiMessages.getNumEvents()<<std::endl;
	
	synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
	
	postProcess(buffer);
}


//==============================================================================
void InstrumentProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	std::unique_ptr<XmlElement> xml (new XmlElement (getNameForPreset()));
	
//	int len = getInstrumentAmt();f
//	for (int n=0; n< len; ++n)
//	{
//		getInstrument(n) -> save(&(*xml));
//	}

	AudioPluginInstance::copyXmlToBinary (*xml, destData);
}

void InstrumentProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xml (AudioPluginInstance::getXmlFromBinary (data, sizeInBytes));

	if (xml->hasTagName (getNameForPreset()))
	{
//		int len = getInstrumentAmt();
//		for (int n=0; n< len; ++n)
//		{
//			getInstrument(n) -> load(&(*xml));
//		}
	}
	
}
#endif
