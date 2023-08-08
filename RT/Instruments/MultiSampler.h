//
//  SamplerWrapper.h
//
//
//  Created by Isaac Roberts on 7/14/20.
//

// Created by Isaac Roberts

#pragma once

#include <JuceHeader.h>

#include "Instrument.h"

using namespace juce;


class MultiSampler    : public Instrument
{
public:
	//==============================================================================
	/** Creates a sampled sound from an audio reader.

		This will attempt to load the audio from the source into memory and store
		it in this object.

		@param name         a name for the sample
		@param source       the audio to load. This object can be safely deleted by the
							caller after this constructor returns
		@param midiNotes    the set of midi keys that this sound should be played on. This
							is used by the RTSynthSound::appliesToNote() method
		@param midiNoteForNormalPitch   the midi note at which the sample should be played
										with its natural rate. All other notes will be pitched
										up or down relative to this one
		@param attackTimeSecs   the attack (fade-in) time, in seconds
		@param releaseTimeSecs  the decay (fade-out) time, in seconds
		@param maxSampleLengthSeconds   a maximum length of audio to read from the audio
										source, in seconds
	*/
	MultiSampler();
	MultiSampler (int numSamples);
	MultiSampler (const String& name, int numSamples=0);

	void addSample(const void* data, size_t dataSize, int rootNote, float maxT=5, bool isOrdered=false);
	void addSample(AudioFormatReader& source, int rootNote, float maxT=5, bool isOrdered=false);
//	void setSourceRate(double rate);
	
	/** Destructor. */
	~MultiSampler() override;

	// --- Instrument functions ---
	Note* getAssociatedVoice() override;

	
	
	//==============================================================================
	/** Returns the sample's name */

	/** Returns the audio sample data.
		This could return nullptr if there was a problem loading the data.
	*/

	//==============================================================================
	/** Changes the parameters of the ADSR envelope which will be applied to the sample. */
	void setEnvelopeParameters (ADSR::Parameters parametersToUse)    { adsr = parametersToUse; }

	//==============================================================================
	bool appliesToNote (int midiNoteNumber) override {return true;}
	bool appliesToChannel (int midiChannel) override {return true;}

	
protected:
	void resetIndexes()
	{
		//TODO: remove constantly updating ix in process and replace it with this
//		std::cout<<"Voices need to be reset"<<std::endl;
//		int nV = getNumVoices();
//		for (int n=0; n < nV; n++)
//		{
//			if (auto* voice = static_cast<MultiSampler*> (getVoice(n)))
//			{
//				voice->resetIndex();
//			}
//		}
	}
	//==============================================================================
	friend class MultiSamplerVoice;

	AudioBuffer<float> getData(int ix) const {return data[ix];}
	int getRootNote(int ix) const {return rootNotes[ix];}

	AudioBuffer<float> getDataByNote(int note) const;
	int getIndexByNote(int note) const;
	
	//======== Member Variables ===============

//	String name;
	std::vector<AudioBuffer<float>> data;
	std::vector<int> rootNotes;
	
	double sourceSampleRate;
public:
	ADSR::Parameters adsr;
private:
	JUCE_LEAK_DETECTOR (MultiSampler)
};


//==============================================================================
/**
	A subclass of SynthesiserVoice that can play a MultiSampler.

	To use it, create a Synthesiser, add some MultiSamplerVoice objects to it, then
	give it some SampledSound objects to play.

	@see MultiSampler, Synthesiser, SynthesiserVoice

	@tags{Audio}
*/
class JUCE_API  MultiSamplerVoice    : public Note
{
public:
	//==============================================================================
	/** Creates a MultiSamplerVoice. */
	MultiSamplerVoice();

	/** Destructor. */
	virtual ~MultiSamplerVoice();

	// ========= Note Overrides ============================
	
	virtual bool canPlaySound (RTSynthSound*) override;

	
	void onStart() override;
	void onStop(float velocity, bool allowTailOff) override;
//    void startNote (int midiNoteNumber, float velocity, RTSynthSound*, int pitchWheel) override;
//    void stopNote (float velocity, bool allowTailOff) override;
	
	virtual void pitchWheelMoved(float value) override;

	void playNote (ContextR& c, int startSample, int numSamples) override;
//    using SynthesiserVoice::renderNextBlock;

	void resetIndex() { dataIx=-1; }
protected:
	//==============================================================================
	double pitchRatio = 0;
	double currentPitchRatio = 1;
	double sourceSamplePosition = 0;
	float lgain = 0, rgain = 0;

	int dataIx=-1;
	
	ADSR adsr;

	JUCE_LEAK_DETECTOR (MultiSamplerVoice);
};
