//
//  VelocityMultiSampler.h
//  
//
//  Created by Isaac Roberts on 11/16/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class VelocityMultiSamplerVoice;

class VelocityMultiSampler    : public Instrument
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
	VelocityMultiSampler(int numRootNotes, int maxNumVelClasses, bool useStereo=false);
	
//	void setSourceRate(double set);
	void addSample(const void* data, size_t dataSize, int i, int rootNote, int velClass, float maxT=5);
	void addSample(AudioFormatReader& source, int i, int rootNote, int velClass, float maxT=5);
	bool addSample(File f, bool isDefinitelyWav, int i, int rootNote, int velClass, float maxT=5);
	void addBlankSample(int i, int rootNote, int velClass);
	
	void addSampleWithoutChecks(AudioFormatReader& source, int rootNote, float maxT=5);

	
	/** Destructor. */
	~VelocityMultiSampler() override;

	// --- Instrument functions ---
	Note* getAssociatedVoice() override;

	//==============================================================================
	/** Changes the parameters of the ADSR envelope which will be applied to the sample. */
//	void setEnvelopeParameters (ADSR::Parameters parametersToUse)    { adsr = parametersToUse; }

	//==============================================================================
	bool appliesToNote (int midiNoteNumber) override {return true;}
	bool appliesToChannel (int midiChannel) override {return true;}
	
	bool isValid();
	
protected:
	int findI(int root);
	//==============================================================================
	friend class ::VelocityMultiSamplerVoice;

	AudioBuffer<float>& getData(int ix, int vix) const {return data[ix][vix];}
	const AudioBuffer<float>* getDataPtr(int ix, int vix) const;

	int getRootNote(int ix) const {return rootNotes[ix];}

	AudioBuffer<float>& getDataByNote(int note, float vel) const;
	int getIndexByNote(int note) const;
	inline int getVelIndex(float vel, int dataIx) const { return _getVelClass(vel, dataIx); }
	
	//======== Member Variables ===============
	
	int _getVelClass(float vel, int dataIx) const;
	
//	String name;
	AudioBuffer<float>** data;
	int* rootNotes;
	int* numVelClasses;
	int numRoots;
//	std::vector<AudioBuffer<float>*> data;
//	std::vector<int> rootNotes;
	
	
	int velClasses;
	double sourceSampleRate;
	bool useStereo;
	
	float amp=1;
	float pan=0;
	
public:
//	ADSR::Parameters adsr;
	
	void setGain(float db) {
		amp = pow(10.f, db/20.f);
	}
	void setPan(float set) {
		//Between -1 and 1
		pan = set;
	}
	
private:
	JUCE_LEAK_DETECTOR (VelocityMultiSampler)
};


//==============================================================================
/**
	A subclass of SynthesiserVoice that can play a VelocityMultiSampler.

	To use it, create a Synthesiser, add some MultiSamplerVoice objects to it, then
	give it some SampledSound objects to play.

	@see VelocityMultiSampler, Synthesiser, SynthesiserVoice

	@tags{Audio}
*/
class JUCE_API  VelocityMultiSamplerVoice    : public Note
{
public:
	//==============================================================================
	/** Creates a VelocityMultiSamplerVoice. */
	VelocityMultiSamplerVoice();

	/** Destructor. */
	virtual ~VelocityMultiSamplerVoice();

	// ========= Note Overrides ============================
	
	virtual bool canPlaySound (RTSynthSound*) override;

	bool usePitchWheel() override { return true; }
	float pitchWheelSemitones() override {return 2.f;}
	
	void onStart() override;
	void onStop(float velocity, bool allowTailOff) override;
//    void startNote (int midiNoteNumber, float velocity, RTSynthSound*, int pitchWheel) override;
//    void stopNote (float velocity, bool allowTailOff) override;
	
	virtual void pitchWheelMoved(float value) override;

	void playNote (ContextR& c, int startSample, int numSamples) override;
	void playTaper (ContextR& outputBuffer, int startSample, int numSamples, int taperIx, int totalTaperLength) override;

	
//	void resetIndex() { dataIx=-1; }
protected:
	//==============================================================================
	double pitchRatio = 0;
	double currentPitchRatio = 1;
	double sourceSamplePosition = 0;

	const AudioBuffer<float>* buffer;
	int bufferRootNote;

	//int dataIx=-1;
	//int velIx=-1;
	
//	ADSR adsr;
	
	float lgain, rgain;
//	float delay;
	
	JUCE_LEAK_DETECTOR (VelocityMultiSamplerVoice);
};
