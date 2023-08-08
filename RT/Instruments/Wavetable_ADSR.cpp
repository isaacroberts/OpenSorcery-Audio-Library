//
//  SamplerWrapper.cpp
//  
//
//  Created by Isaac Roberts on 7/14/20.
//

// Created by Isaac Roberts


#include "Wavetable_ADSR.h"
#include "SampleUtil.h"

Wavetable_ADSR::Wavetable_ADSR (const String& soundName,
                            AudioFormatReader& source,
                            const BigInteger& notes,
                            int midiNoteForNormalPitch,
                            double attackTimeSecs,
                            double releaseTimeSecs,
                            double maxSampleLengthSeconds)
    : Instrument(),
	  name (soundName),
      sourceSampleRate (source.sampleRate),
      midiNotes (notes),
      midiRootNote (midiNoteForNormalPitch)
{
    if (sourceSampleRate > 0 && source.lengthInSamples > 0)
    {
		data.reset(new AudioBuffer<float>( SampleUtil::readSample(source, true, maxSampleLengthSeconds)));
		
        params.attack  = static_cast<float> (attackTimeSecs);
        params.release = static_cast<float> (releaseTimeSecs);
    }
	
//	preset = new Preset("Sampler");
	
}
// ===== Instrument / Note Overrides ===========
Note* Wavetable_ADSR::getAssociatedVoice()
{
	return new Wavetable_ADSR::Voice();
}


Wavetable_ADSR::~Wavetable_ADSR()
{
}

bool Wavetable_ADSR::appliesToNote (int midiNoteNumber)
{
    return midiNotes[midiNoteNumber];
}

bool Wavetable_ADSR::appliesToChannel (int /*midiChannel*/)
{
    return true;
}

//==============================================================================
Wavetable_ADSR::Voice::Voice() {}
Wavetable_ADSR::Voice::~Voice() {}

bool Wavetable_ADSR::Voice::canPlaySound (RTSynthSound* sound)
{
    return dynamic_cast<const Wavetable_ADSR*> (sound) != nullptr;
}

void Wavetable_ADSR::Voice::onStart()
{
    if (auto* sound = dynamic_cast<const Wavetable_ADSR*> (instrument))
    {
        pitchRatio = std::pow (2.0, (noteNum - sound->midiRootNote) / 12.0)
                        * sound->sourceSampleRate / getSampleRate();

        sourceSamplePosition = 0.0;
        lgain = velo;
        rgain = velo;

        adsr.setSampleRate (sound->sourceSampleRate);
        adsr.setParameters (sound->params);

        adsr.noteOn();
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
}

void Wavetable_ADSR::Voice::onStop (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        adsr.reset();
    }
}

void Wavetable_ADSR::Voice::pitchWheelMoved (float value)
{
	//Todo: change the pitch ratio
}

//==============================================================================
void Wavetable_ADSR::Voice::playNote (ContextR& output, int startSample, int numSamples)
{
    if (auto* playingSound = static_cast<Wavetable_ADSR*> (getCurrentlyPlayingSound().get()))
    {
        auto& data = *playingSound->data;
		bool shouldStop = SampleUtil::playStereoNoteWithADSR(data, output,
					   sourceSamplePosition, startSample, numSamples, adsr, pitchRatio, lgain, rgain);
		
		if (shouldStop)
		{
			stopNote (0.0f, false);
		}
    }
}
