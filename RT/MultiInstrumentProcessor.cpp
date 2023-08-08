/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "MultiInstrumentProcessor.h"

#if 0

//==============================================================================
MultiInstrumentProcessor::MultiInstrumentProcessor(int synthCt)
     : InstrumentProcessor(),
	   synthAmt(synthCt)
{
	lastSynth = -1;
	curSynth = 0;
	synths = new Synthesiser[synthAmt];
}

void MultiInstrumentProcessor::setSynth(int i)
{
	if (i == curSynth)
		return;
	else
	{
		lastSynth = curSynth;
		curSynth = i;
		synths[lastSynth].allNotesOff(0, true);
	}
}
void MultiInstrumentProcessor::setInstrument(int n, Instrument* i)
{
	synths[n].addSound(i);
	
	int voiceCt = i->polyphony();
	
	for (int v=0; v < voiceCt; ++v)
	{
		synths[n].addVoice(i->getAssociatedVoice());
	}
}


MultiInstrumentProcessor::~MultiInstrumentProcessor()
{
	for (int n=0; n< synthAmt; ++n)
	{
		synths[n].clearVoices();
		synths[n].clearSounds();
	}
	delete[] synths;
}

int MultiInstrumentProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MultiInstrumentProcessor::getCurrentProgram()
{
    return 0;
}

void MultiInstrumentProcessor::setCurrentProgram (int index)
{
}


void MultiInstrumentProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void MultiInstrumentProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
//	synth.setCurrentPlaybackSampleRate(sampleRate);
//	std::cout<<"prepare"<<std::endl;
	for (int n=0; n<synthAmt; ++n) {
//		std::cout<<"pick "<<n<<std::endl;
		synths[n].prepare(sampleRate);
	}
	
	InstrumentProcessor::prepareToPlay(sampleRate, samplesPerBlock);

}


void MultiInstrumentProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
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

	
	synths[curSynth].renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
	if (lastSynth!=-1)
	{
		MidiBuffer empty;
		
		synths[lastSynth].renderNextBlock(buffer, empty, 0, buffer.getNumSamples());
	}
	
	postProcess(buffer);
}

#endif
