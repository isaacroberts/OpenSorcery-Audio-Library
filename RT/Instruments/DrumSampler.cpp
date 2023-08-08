//
//  DrumSampler.cpp
//  
//
//  Created by Isaac Roberts on 5/14/22.
//

// Created by Isaac Roberts


#include "DrumSampler.h"

// ---------- Instrument ------------------

DrumSampler::DrumSampler(std::shared_ptr<DrumMapping> map, bool stereo)
: mapping(map),
useStereo(stereo),
sourceSampleRate(0)
{
	data.reserve(map->getNumDrums());
}

DrumSampler::~DrumSampler()
{
}

bool DrumSampler::appliesToNote(int note)
{
	return mapping->getMapping(note) != -1;
}

int DrumSampler::getIndex(int note) const
{
	return mapping->getMapping(note);
}
AudioBuffer<float>& DrumSampler::getData(int ix)
{
	return data[ix];
}
void DrumSampler::addSample(juce::File file, int drum, float maxLengthSeconds)
{
	if (drum==-1) {
		drum = (int)data.size();
	}
	//Samples must be added in order
	jassert(drum == data.size());
	//Amount must match mapping
	jassert(drum < mapping->getNumDrums());

	float sourceRate;
	AudioBuffer<float> sample = FileLoad::loadSample(file, sourceRate,  false, maxLengthSeconds);
	data.push_back(sample);
	
	if (sample.getNumSamples()==0) {
		std::cout<<"Warning: empty sample "<<file.getFullPathName()<<std::endl;
	}

}
void DrumSampler::addSample(const void* data, size_t dataSize, int drum, float maxLengthSeconds)
{
	MemoryInputStream *inputStream = new MemoryInputStream(data, dataSize, false);
	WavAudioFormat wavFormat;
	AudioFormatReader* reader = wavFormat.createReaderFor(inputStream, false);
	
	addSample(*reader, drum, maxLengthSeconds);
	
	delete reader;
}
void DrumSampler::addSample(AudioFormatReader& source, int drum, float maxLengthSeconds)
{
	//Samples must be added in order
	jassert(drum == data.size());
	//Amount must match mapping
	jassert(drum < mapping->getNumDrums());
	
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
	
	auto sample = SampleUtil::readSample(source, useStereo, maxLengthSeconds);
	//Assume samples are being added in order
	data.push_back(sample);
}
void DrumSampler::addBlankSample(int drum)
{
	//Samples must be added in order
	jassert(drum == data.size());
	//Amount must match mapping
	jassert(drum < mapping->getNumDrums());
	AudioBuffer<float> sample;
	data.push_back(sample);
}
void DrumSampler::setSample(AudioFormatReader& source, int drum, float maxLengthSeconds)
{
	//Amount must match mapping
	jassert(drum < mapping->getNumDrums());
		
	if (sourceSampleRate <= 0)
	{
		sourceSampleRate = source.sampleRate;
	}
	else
	{
		//TODO: Resample because they'll be user added
		//epsilon for double
		//assert sample rate is same for all samples
		jassert(fabs(sourceSampleRate - source.sampleRate) < 1);
	}
	
	//TODO: This might throw multithreading issues
	data[drum] = SampleUtil::readSample(source, useStereo, maxLengthSeconds);
}
void DrumSampler::setSample(AudioBuffer<float> sample, int drum)
{
	jassert(drum < mapping->getNumDrums());
	if (sample.getNumSamples() > 0)
	{
		data[drum] = sample;		
	}
}



// ---------- Voice ------------------

DrumSampler::Voice::Voice()
: Note(),
lgain(1.0f), rgain(1.0f)
{
}

DrumSampler::Voice::~Voice()
{
	
}


void DrumSampler::Voice::onStart()
{
	if (auto* sound = dynamic_cast<const DrumSampler*> (instrument))
	{
		drumIx = sound->getIndex(noteNum);
		if (drumIx==-1)
		{
			//Dunno if this is allowed here
			clearCurrentNote();
		}
		sourceSamplePosition=0;
		
	}
}
void DrumSampler::Voice::onStop(float velo, bool allowTail)
{
	
}

void DrumSampler::Voice::playNote(ContextR& c, int startSample, int numSamples)
{
	if (drumIx == -1)
	{
		clearCurrentNote();
		return;
	}
	
	if (auto* playingSound = static_cast<DrumSampler*> (getCurrentlyPlayingSound().get()))
	{
		auto& data = playingSound->getData(drumIx);
		
		float pitchRatio = playingSound->sourceSampleRate / getSampleRate();
		
		bool shouldStop;
		if (playingSound->useStereo)
		{
			shouldStop = SampleUtil::playMonoNote(data, c,
						   sourceSamplePosition, startSample, numSamples, pitchRatio, lgain, rgain);
		}
		else
		{
			shouldStop = SampleUtil::playStereoNote(data, c,
						   sourceSamplePosition, startSample, numSamples, pitchRatio, lgain, rgain);
		}
		if (shouldStop)
			stopNote (0.0f, false);
	}
}
void DrumSampler::Voice::playTaper (ContextR& c, int startSample, int numSamples, int taperIx, int taperLen)
{
	if (drumIx == -1)
	{
		clearCurrentNote();
		return;
	}
	
	if (auto* playingSound = static_cast<DrumSampler*> (getCurrentlyPlayingSound().get()))
	{
		auto& data = playingSound->getData(drumIx);
		
		float pitchRatio = playingSound->sourceSampleRate / getSampleRate();
		
		bool shouldStop;
		if (playingSound->useStereo)
		{
			shouldStop = SampleUtil::playMonoNoteWithTaper(data, c.getOutput(),
						   sourceSamplePosition, startSample, numSamples, taperIx, taperLen, pitchRatio, lgain, rgain);
		}
		else
		{
			shouldStop = SampleUtil::playStereoNoteWithTaper(data, c.getOutput(),
						   sourceSamplePosition, startSample, numSamples, taperIx, taperLen, pitchRatio, lgain, rgain);
		}
		if (shouldStop)
			stopNote (0.0f, false);
	}
}


