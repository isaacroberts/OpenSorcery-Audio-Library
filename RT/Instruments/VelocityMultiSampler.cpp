//
//  SamplerWrapper.cpp
//
//
//  Created by Isaac Roberts on 11/16/20.
//

// Created by Isaac Roberts


#include "VelocityMultiSampler.h"
#include "SampleUtil.h"

#include "RTGraphics/FileLoad.h"

VelocityMultiSampler::VelocityMultiSampler (int numRootNotes, int maxNumVelClasses, bool use_stereo_)
	: Instrument(),
//	  name ("VelocityMultiSampler"),
	  data(),
	  rootNotes(),
	  numRoots(numRootNotes),
	  velClasses(maxNumVelClasses),
	  sourceSampleRate(0),
	  useStereo(use_stereo_)
{
	data = new AudioBuffer<float>*[numRoots];
	rootNotes = new int[numRoots];
	numVelClasses = new int[numRoots];
	
	for (int n=0; n < numRoots; ++n)
	{
		rootNotes[n] = -1;
		data[n] = new AudioBuffer<float>[maxNumVelClasses];
		numVelClasses[n]=0;
	}
	
//	adsr.attack = 0;
//	adsr.decay = 1;
//	adsr.sustain = 1;
//	adsr.release= 0;
}

//void VelocityMultiSampler::setSourceRate(double set)
//{
//	sourceSampleRate = set;
//}

void VelocityMultiSampler::addSample(const void* data, size_t dataSize, int i, int note, int velClass, float maxLengthSeconds)
{
	MemoryInputStream *inputStream = new MemoryInputStream(data, dataSize, false);
	WavAudioFormat wavFormat;
	AudioFormatReader* reader = wavFormat.createReaderFor(inputStream, false);
	jassert(reader);
	if (!reader)
		return;
	addSample(*reader, i, note, velClass, maxLengthSeconds);
	
	delete reader;
}
bool VelocityMultiSampler::addSample(File file, bool isDefinitelyWav, int i, int note, int velClass, float maxLengthSeconds)
{
	AudioFormatReader* reader=NULL;
	if (isDefinitelyWav)
	{
		auto inputStream = file.createInputStream().release();
		if (!inputStream)
			return false;
		WavAudioFormat wavFormat;
		reader = wavFormat.createReaderFor(inputStream, false);
	}
	else {
		reader = FileLoad::getAudioFormatReader(file).release();
	}
	if (reader)
	{
		addSample(*reader, i, note, velClass, maxLengthSeconds);
		delete reader;
		return true;
	}
	else {
		addBlankSample(i, note, velClass);
		return false;
	}

}
void VelocityMultiSampler::addSample(AudioFormatReader& source, int i, int note, int velClass, float maxLengthSeconds)
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
	
	jassert (isPositiveAndBelow(i, numRoots));
	jassert (isPositiveAndBelow(velClass, velClasses));
	
	rootNotes[i] = note;
	
	if (i>0)
		jassert(rootNotes[i]> rootNotes[i-1]);
	
//	std::cout<<"Root notes"<<std::endl;
//	for (int n=0; n < i; ++n)
//	{
//		std::cout<<n<<" "<<rootNotes[n]<<std::endl;
//	}
	
	jassert(velClass < velClasses);
	jassert(velClass >= numVelClasses[i]);
	
	data[i][velClass] = SampleUtil::readSample(source, useStereo, maxLengthSeconds);

	//Fill in missing vel-classes
	//Fill with higher vel
	while (velClass > numVelClasses[i])
	{
		data[i][numVelClasses[i]] = data[i][velClass];
		numVelClasses[i] ++;
	}

	numVelClasses[i] ++;
	
}
void VelocityMultiSampler::addBlankSample(int i, int note, int velClass)
{
	jassert (isPositiveAndBelow(i, numRoots));
	jassert (isPositiveAndBelow(velClass, velClasses));
	
	rootNotes[i] = note;
	
	if (i>0)
		jassert(rootNotes[i]> rootNotes[i-1]);
	
	jassert(velClass < velClasses);
	jassert(velClass >= numVelClasses[i]);
	
	data[i][velClass] = AudioBuffer<float>();

	//Fill in missing vel-classes
	//Fill with higher vel
	while (velClass > numVelClasses[i])
	{
		data[i][numVelClasses[i]] = data[i][velClass];
		numVelClasses[i] ++;
	}

	numVelClasses[i] ++;

}
int VelocityMultiSampler::findI(int note)
{
	for (int n=0; n < numRoots; ++n)
	{
		if (rootNotes[n] == note)
		{
			return n;
		}
		if (rootNotes[n] > note)
			return -1;
	}
	
	for (int n=0; n < numRoots; ++n)
	{
		if (rootNotes[n]==-1 || numVelClasses[n] == 0)
			return n;
	}
	return -1;
	
}
void VelocityMultiSampler::addSampleWithoutChecks(AudioFormatReader& source, int note, float maxLengthSeconds)
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
	
	
	int i= findI(note);
	if (i==-1)
		return;

	jassert (isPositiveAndBelow(i, numRoots));
	
	rootNotes[i] = note;
	
	if (i>0)
		jassert(rootNotes[i]> rootNotes[i-1]);

	int velClass = numVelClasses[i];
	
	if (velClass >= velClasses)
		return;
		
	data[i][velClass] = SampleUtil::readSample(source, useStereo, maxLengthSeconds);

	numVelClasses[i] ++;
	
}

bool VelocityMultiSampler::isValid()
{
//	for (int n=0; n < numRoots; ++n)
//	{
//		if (rootNotes[n] == note)
//	}
	bool anyGoodRoots=false;
	for (int i=0; i < numRoots; ++i)
	{
		if (rootNotes[i]!=-1)
		{
			anyGoodRoots=true;
			
			if (i>0)
			{
				if (rootNotes[i] <= rootNotes[i-1])
					return false;
			}
			if (numVelClasses[i]==0)
				return false;
			if (numVelClasses[i] > velClasses)
				return false;
		}
	}
	return anyGoodRoots;
}
AudioBuffer<float>& VelocityMultiSampler::getDataByNote(int note, float vel) const
{
	int ix = getIndexByNote(note);
	int velClass = getVelIndex(vel, ix);
	return data[ix][velClass];
}
int VelocityMultiSampler::getIndexByNote(int note) const
{
	//Binary search
	int left = 0;
	int right = numRoots-1;
	int head = right/2;
	
	while (left < right-1)
	{
		int nh = rootNotes[head];
		if (nh==-1) {
			right = head;
			head = (left+right)/2;
		}
		else if (nh == note) {
			return head;
		}
		else if ( note > nh) {
			left = head;
			head = (left+right)/2;
		}
		else if (note < nh) {
			right = head;
			head = (left+right)/2;
		}
	}
	return right;
}
int VelocityMultiSampler::_getVelClass(float vel, int n) const
{
	// -.5 to give notes a little buffer
	// so the max vel class is reachable at slightly less than 100% velocity
	jassert(numVelClasses[n]>0);
	int i= vel * (numVelClasses[n]-.5);
	if ( i >= numVelClasses[n])
		return numVelClasses[n]-1;
	else return i;
}
// ===== Instrument / Note Overrides ===========
Note* VelocityMultiSampler::getAssociatedVoice()
{
	return new VelocityMultiSamplerVoice();
}


VelocityMultiSampler::~VelocityMultiSampler()
{
	for (unsigned long n=0; n < numRoots; ++n)
	{
		delete[] data[n];
	}
	delete[] data;
	delete[] rootNotes;
}

const AudioBuffer<float>* VelocityMultiSampler::getDataPtr(int ix, int vix) const
{
	jassert(data[ix][vix].getNumChannels() > 0);
	return &data[ix][vix];
	
}


//==============================================================================
VelocityMultiSamplerVoice::VelocityMultiSamplerVoice()
: Note(),
buffer(NULL),
bufferRootNote(-1)
{
	
}
VelocityMultiSamplerVoice::~VelocityMultiSamplerVoice() {}

bool VelocityMultiSamplerVoice::canPlaySound (RTSynthSound* sound)
{
	return dynamic_cast<const VelocityMultiSampler*> (sound) != nullptr;
}

void VelocityMultiSamplerVoice::onStart()
{
	if (auto* sound = dynamic_cast<const VelocityMultiSampler*> (instrument))
	{
		int dataIx = sound->getIndexByNote(noteNum);
		int velIx = sound->getVelIndex(velo, dataIx);
		
		buffer = sound->getDataPtr(dataIx, velIx);
		
		jassert (buffer->getNumChannels() > 0);
		
		bufferRootNote = sound->getRootNote(dataIx);
		
		float sampleRateRatio = sound->sourceSampleRate / getSampleRate();
		if (bufferRootNote == noteNum)
			pitchRatio = sampleRateRatio;
		else
			pitchRatio = std::pow (2.0, (noteNum - bufferRootNote) / 12.0)
						* sampleRateRatio;
		
		currentPitchRatio = pitchRatio;

		sourceSamplePosition = 0.0;

//		adsr.setSampleRate (sound->sourceSampleRate);
//		adsr.setParameters (sound->adsr);

//		delay = sound->pan;
		
		Util::getPan(lgain, rgain, sound->pan, sound->amp);
		
//		adsr.noteOn();
	}
	else
	{
		bufferRootNote = -1;
		buffer = NULL;
		jassertfalse; // this object can only play SamplerSounds!
	}
}

void VelocityMultiSamplerVoice::onStop (float /*velocity*/, bool allowTailOff)
{
	if (allowTailOff)
	{
//		adsr.noteOff();
	}
	else
	{
		buffer = NULL;
		clearCurrentNote();
//		adsr.reset();
	}
}

void VelocityMultiSamplerVoice::pitchWheelMoved (float value)
{
	if (value==0)
		currentPitchRatio = pitchRatio;
	currentPitchRatio = pitchRatio * pow(2.f, value);
}

//==============================================================================
void VelocityMultiSamplerVoice::playNote (ContextR& c, int startSample, int numSamples)
{
	if (buffer==NULL)
		return;
	
	if (auto* sound = dynamic_cast<const VelocityMultiSampler*> (instrument))
	{
		auto& data = *buffer;
		
		if (!sound->useStereo)
		{
			bool shouldStop = SampleUtil::playMonoNote (data, c,
						   sourceSamplePosition, startSample, numSamples, currentPitchRatio, lgain, rgain);
			
			if (shouldStop)
				stopNote (0.0f, false);
		}
		else
		{
			bool shouldStop = SampleUtil::playStereoNote (data, c,
						   sourceSamplePosition, startSample, numSamples, currentPitchRatio, lgain, rgain);
			
			if (shouldStop)
				stopNote (0.0f, false);
		}
	}
}
void VelocityMultiSamplerVoice::playTaper (ContextR& c, int startSample, int numSamples, int taperIx, int taperLen)
{
	if (buffer==NULL)
		return;
	
	if (auto* sound = dynamic_cast<const VelocityMultiSampler*> (instrument))
	{
		auto& data = *buffer;
		
		if (!sound->useStereo)
		{
			bool shouldStop = SampleUtil::playMonoNoteWithTaper (data, c.getOutput(),
						   sourceSamplePosition, startSample, numSamples, taperIx, taperLen, currentPitchRatio, lgain, rgain);
			
			if (shouldStop)
				stopNote (0.0f, false);
		}
		else
		{
			bool shouldStop = SampleUtil::playStereoNoteWithTaper (data, c.getOutput(),
						   sourceSamplePosition, startSample, numSamples, taperIx, taperLen, currentPitchRatio, lgain, rgain);
			
			if (shouldStop)
				stopNote (0.0f, false);
		}
	}
}
