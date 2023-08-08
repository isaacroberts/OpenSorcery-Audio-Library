//
//  SamplerWrapper.cpp
//
//
//  Created by Isaac Roberts on 7/14/20.
//

// Created by Isaac Roberts


#include "MultiSampler.h"
#include "SampleUtil.h"

#define use_stereo false

MultiSampler::MultiSampler ()
	: Instrument(),
//	  name ("MultiSampler"),
	  data(),
	  rootNotes(),
	  sourceSampleRate(0)
{
	adsr.attack = 0;
	adsr.decay = 1;
	adsr.sustain = 1;
	adsr.release= .5;
}
MultiSampler::MultiSampler (int numSamples)
	: Instrument(),
//	  name ("MultiSampler"),
	  data(),
	  rootNotes(),
	  sourceSampleRate(0)
{
	if (numSamples > 0)
	{
		data.reserve(numSamples);
		rootNotes.reserve(numSamples);
	}
	adsr.attack = 0;
	adsr.decay = 1;
	adsr.sustain = 1;
	adsr.release= .5;
}
MultiSampler::MultiSampler (const String& soundName, int numSamples)
	: Instrument(),
//	  name (soundName),
	  data(),
	  rootNotes(),
	  sourceSampleRate(0)
{
	if (numSamples > 0)
	{
		data.reserve(numSamples);
		rootNotes.reserve(numSamples);
	}
	adsr.attack = 0;
	adsr.decay = 1;
	adsr.sustain = 1;
	adsr.release= .5;
}
void MultiSampler::addSample(const void* data, size_t dataSize, int note, float maxLengthSeconds, bool isOrdered)
{
	MemoryInputStream *inputStream = new MemoryInputStream(data, dataSize, false);
	WavAudioFormat wavFormat;
	AudioFormatReader* reader = wavFormat.createReaderFor(inputStream, false);
	
	addSample(*reader, note, maxLengthSeconds, isOrdered);
	
	delete reader;
}
void MultiSampler::addSample(AudioFormatReader& source, int note, float maxLengthSeconds, bool isOrdered)
{
	if (sourceSampleRate <= 0)
	{
		sourceSampleRate = source.sampleRate;
	}
	else
	{
		//epsilon for double
		//assert sample rate is same for all samples
		jassert(fabs(sourceSampleRate - source.sampleRate) < 1);
	}
	
	auto sample = SampleUtil::readSample(source, use_stereo, maxLengthSeconds);

	if (isOrdered)
	{
		//Assume samples are being added in order
		data.push_back(sample);
		rootNotes.push_back(note);
	}
	else
	{
		int insertIx = 0;
		for (int n=0; n < rootNotes.size(); ++n)
		{
			if ( rootNotes[n] > note)
			{
				insertIx = n;
				break;
			}
		}
		data.insert(data.begin()+insertIx, sample);
		rootNotes.insert(rootNotes.begin()+insertIx, note);
	}
	//Ensure all voices know that indices have changed 
	resetIndexes();
}
//void MultiSampler::setSourceRate(double rate)
//{
//	sourceSampleRate = rate;
//}
AudioBuffer<float> MultiSampler::getDataByNote(int note) const
{
	int ix = getIndexByNote(note);
	return data[ix];
}
int MultiSampler::getIndexByNote(int note) const
{
	//Binary search for speed
	int min = 0;
	int max = (int)rootNotes.size()-1;
	
	while (min < max-1)
	{
		int cur = (min+max)/2;
		int r = rootNotes[cur];
		if (r == note)
			return (int)cur;
		else if (r < note)
		{
			min = cur;
		}
		else
		{
			max = cur;
		}
	}
	int minD = abs(rootNotes[min]-note);
	int maxD = abs(rootNotes[max]-note);
	if (minD < maxD)
		return min;
	else return max;
}
// ===== Instrument / Note Overrides ===========
Note* MultiSampler::getAssociatedVoice()
{
	return new MultiSamplerVoice();
}


MultiSampler::~MultiSampler()
{
}

//bool MultiSampler::appliesToNote (int midiNoteNumber)
//{
//	return midiNotes[midiNoteNumber];
//}
//
//bool MultiSampler::appliesToChannel (int /*midiChannel*/)
//{
//	return true;
//}

//==============================================================================
MultiSamplerVoice::MultiSamplerVoice()
: dataIx(-1)
{
	
}
MultiSamplerVoice::~MultiSamplerVoice() {}

bool MultiSamplerVoice::canPlaySound (RTSynthSound* sound)
{
	return dynamic_cast<const MultiSampler*> (sound) != nullptr;
}

void MultiSamplerVoice::onStart()
{
	if (auto* sound = dynamic_cast<const MultiSampler*> (instrument))
	{
		
		dataIx = sound->getIndexByNote(noteNum);
		int rootNote = sound->getRootNote(dataIx);
		
		float sampleRateRatio = sound->sourceSampleRate / getSampleRate();
		if (rootNote == noteNum)
			pitchRatio = sampleRateRatio;
		else
			pitchRatio = std::pow (2.0, (noteNum - rootNote) / 12.0)
						* sampleRateRatio;
		
		currentPitchRatio = pitchRatio;

		sourceSamplePosition = 0.0;
		lgain = velo;
		rgain = velo;

		adsr.setSampleRate (getSampleRate());
		adsr.setParameters (sound->adsr);

		adsr.noteOn();
	}
	else
	{
		jassertfalse; // this object can only play SamplerSounds!
	}
}

void MultiSamplerVoice::onStop (float velocity, bool allowTailOff)
{
	if (allowTailOff)
	{
		adsr.noteOff();
	}
	else
	{
//		clearCurrentNote();
		adsr.reset();
	}
}

void MultiSamplerVoice::pitchWheelMoved (float value)
{
	if (value==0)
		currentPitchRatio = pitchRatio;
	currentPitchRatio = pitchRatio * pow(2.f, value);
}

//==============================================================================
void MultiSamplerVoice::playNote (ContextR& c, int startSample, int numSamples)
{
	if (auto* playingSound = static_cast<MultiSampler*> (getCurrentlyPlayingSound().get()))
	{
		if (dataIx==-1)
			dataIx = playingSound->getIndexByNote(noteNum);
		
		auto data = playingSound->getData(dataIx);
		
		if (!use_stereo)
		{
			bool shouldStop = SampleUtil::playMonoNoteWithADSR(data, c,
						   sourceSamplePosition, startSample, numSamples, adsr, currentPitchRatio, lgain, rgain);
			
			if (shouldStop)
				stopNote (0.0f, false);
		}
		else
		{
			bool shouldStop = SampleUtil::playStereoNoteWithADSR(data, c,
						   sourceSamplePosition, startSample, numSamples, adsr, currentPitchRatio, lgain, rgain);
			
			if (shouldStop)
				stopNote (0.0f, false);
		}
		
	}
}
