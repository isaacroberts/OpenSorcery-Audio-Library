//
//  DrumSampler.h
//  
//
//  Created by Isaac Roberts on 5/14/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "Instrument.h"

using namespace juce;

struct DrumMapping {
	/*
	 //Must be constant
	 int getNumDrums() override { return 1; }
	 //Return -1 to indicate no drum
	 int getMapping(int midiNote) override;
	 */
	//Must be constant
	virtual int getNumDrums() = 0;
	//Return -1 to indicate no drum
	virtual int getMapping(int midiNote) = 0;
	virtual ~DrumMapping() {}
};

class DrumSampler : public Instrument
{
public:
	DrumSampler(std::shared_ptr<DrumMapping> mapping) : DrumSampler(mapping, false) {}
	DrumSampler(std::shared_ptr<DrumMapping> mapping, bool useStereo);
	
	virtual ~DrumSampler();

	bool appliesToNote(int num) override;

	
	void addSample(const void* data, size_t dataSize, int drum, float maxT=5);
	void addSample(juce::File,  int drum=-1, float maxT = 5);
	void addSample(AudioFormatReader& source, int drum, float maxT=5);
	void addBlankSample(int drum);
	void setSample(AudioFormatReader& source, int drum, float maxT=5);
	void setSample(AudioBuffer<float> set, int drum);

	
	AudioBuffer<float>& getData(int ix);
	int getIndex(int note) const;
	
	int getParamAmt() override { return 0; }
	void set(int n, float set) override {}
	float get(int n) override {return 0; }
//
//	//----------Synth Definitions-------------
	int polyphony() override {return 16;}
	Note* getAssociatedVoice() override {
	   return new Voice();
	}
	
protected:
	std::shared_ptr<DrumMapping> mapping;
	friend class Voice;
	std::vector<AudioBuffer<float>> data;
	bool useStereo;
//	std::vector<int> rootNotes;
	
	double sourceSampleRate;

	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrumSampler);

public:
	class Voice : public Note
	{
	public:
		Voice();
		virtual ~Voice();
		void onStart() override;
		void onStop(float velo, bool allowTail) override;
		void playNote(ContextR& c, int startIx, int numSamples) override;
		void playTaper (ContextR& c, int startSample, int numSamples, int taperIx, int taperLen) override;

		bool canPlaySound (RTSynthSound* instr) override {
		   return dynamic_cast<const DrumSampler*> (instr) != nullptr;
		}
//		//Instrument Definitions
		bool usePitchWheel() override { return false; }
//		float pitchWheelSemitones() override {return 2.f;}
//		//Controls
//		void pitchWheelMoved(float value) override;
//		void controllerMoved(int cNum, float value) override {}
//		//void afterTouchChanged(float value) override;
//		void channelPressureChanged(float value) override {}

//		double pitchRatio = 0;
//		double currentPitchRatio = 1;
		double sourceSamplePosition = 0;
		float lgain = 0, rgain = 0;

		int drumIx=-1;
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Voice);
	};
	
};
