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

class Wavetable_ADSR    : public Instrument
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
    Wavetable_ADSR (const String& name,
                  AudioFormatReader& source,
                  const BigInteger& midiNotes,
                  int midiNoteForNormalPitch,
                  double attackTimeSecs,
                  double releaseTimeSecs,
                  double maxSampleLengthSeconds);

    /** Destructor. */
    ~Wavetable_ADSR() override;

	// --- Instrument functions ---
	Note* getAssociatedVoice() override;

	
	
    //==============================================================================
    /** Returns the sample's name */
    const String& getName() const noexcept                  { return name; }

    /** Returns the audio sample data.
        This could return nullptr if there was a problem loading the data.
    */
    AudioBuffer<float>* getAudioData() const noexcept       { return data.get(); }

    //==============================================================================
    /** Changes the parameters of the ADSR envelope which will be applied to the sample. */
    void setEnvelopeParameters (ADSR::Parameters parametersToUse)    { params = parametersToUse; }

    //==============================================================================
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;

private:
    //==============================================================================
    friend class RTSamplerVoice;

    String name;
    std::unique_ptr<AudioBuffer<float>> data;
    double sourceSampleRate;
    BigInteger midiNotes;
//    int length = 0,
	int midiRootNote = 0;

    ADSR::Parameters params;

    JUCE_LEAK_DETECTOR (Wavetable_ADSR)



	//==============================================================================
	/**
		A subclass of SynthesiserVoice that can play a RTSampler.

		To use it, create a Synthesiser, add some RTSamplerVoice objects to it, then
		give it some SampledSound objects to play.

		@see RTSampler, Synthesiser, SynthesiserVoice

		@tags{Audio}
	*/
	class JUCE_API  Voice    : public Note
	{
	public:
		//==============================================================================
		/** Creates a RTSamplerVoice. */
		Voice();

		/** Destructor. */
		virtual ~Voice();

		// ========= Note Overrides ============================
		
		virtual bool canPlaySound (RTSynthSound*) override;

		
		void onStart() override;
		void onStop(float velocity, bool allowTailOff) override;
	//    void startNote (int midiNoteNumber, float velocity, RTSynthSound*, int pitchWheel) override;
	//    void stopNote (float velocity, bool allowTailOff) override;
		
		virtual void pitchWheelMoved(float value) override;

		void playNote (ContextR&, int startSample, int numSamples) override;
	//    using SynthesiserVoice::renderNextBlock;

	private:
		//==============================================================================
		double pitchRatio = 0;
		double sourceSamplePosition = 0;
		float lgain = 0, rgain = 0;

		ADSR adsr;

		JUCE_LEAK_DETECTOR (Voice);
	};
};
