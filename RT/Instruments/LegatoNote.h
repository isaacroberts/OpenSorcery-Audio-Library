//
//  LegatoNote.h
//  
//
//  Created by Isaac Roberts on 5/24/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;


class LegatoNote : public RTSynthVoice
{
public:
	LegatoNote();
	
	virtual void setInstrument(Instrument* instr);
	
	
	/*
	 void onStart() override;
	 void onStop(float velo, bool allowTail) override;
	 void playNote(ContextR& c, int startIx, int numSamples) override;
	 bool canPlaySound (RTSynthSound* instr) override {
		return dynamic_cast<const InstrumentClassName*> (instr) != nullptr;
	 }
	 //Instrument Definitions
	 bool usePitchWheel() override { return true; }
	 float pitchWheelSemitones() override {return 2.f;}
	 //Controls
	 void pitchWheelMoved(float value) override;
	 float getLegatoTime() override { return .25f; }

	 void controllerMoved(int cNum, float value) override {}
	 //void afterTouchChanged(float value) override;
	 void channelPressureChanged(float value) override {}
	 
	 */
	
// --------- Overrides ---------
	virtual void onStart(bool fromSilent) {}
	virtual void onStop(float velo, bool allowTail) {}
	//Should return
	virtual bool canPlaySound (RTSynthSound*) override = 0;
	virtual void playNote(ContextR& c, int startIx, int numSamples) =0;
	//Can be overridden
	virtual void pitchWheelMoved(float value);
	virtual void controllerMoved(int cNum, float value) {}
	virtual void afterTouchChanged(float value);
	virtual void channelPressureChanged(float value) {}
	//Instrument Definitions
	virtual bool usePitchWheel() { return true; }
	virtual float pitchWheelSemitones() {return 2.f;}
//	virtual float getLegatoTime() { return .25f; }
	
	//Controls
	void pitchWheelMoved(int value) override;
	inline void controllerMoved(int c, int value) override { controllerMoved(c, (value/127.f));}
	inline void aftertouchChanged(int value) override { afterTouchChanged(value/127.f);}
	inline void channelPressureChanged(int value) override { channelPressureChanged(value/127.f);}
	
	
	//Should not be overridden
	void startNote(int n, float velo, RTSynthSound* sound, int pitchWheel) override;
	void stopNote(float velo, bool allowTail) override;
	void renderNextBlock(ContextR&, int startIx, int numSamples) override;

	void setLegatoTime(float t) {
		if (t > 0)
			alpha = exp( -1.0f / (t * getSampleRate()));
		else
			alpha = 0;
	}
	
private:
	int noteNum;
	float targetFreq;
	float targetVelo;
	float freqBeforePW;
	float pitchWheelFactor;
	float alpha;
	
	inline float timeStep(float y_t, float x_t, float alpha)
	{
		if (alpha <= 0)
			return x_t;
		else
			return alpha * y_t + (1- alpha) * x_t;
	}

	
protected:
	float velo;
	float freq;
	
//	int phase;
	float time;
	
	Instrument* instrument;
	
	void updateLegatoSample();
	
	inline float pitchWheelValueToFactor(int value) {
		return (value-8192)/16383.f * pitchWheelSemitones()/12.f;
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LegatoNote);
};



