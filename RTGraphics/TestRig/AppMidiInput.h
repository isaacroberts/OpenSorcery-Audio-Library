//
//  AppMidiInput.h
//  
//
//  Created by Isaac Roberts on 7/6/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

//using namespace juce;

#if 1

class AppMidiInput {
	typedef unsigned char n_t;
public:
	AppMidiInput();
	
	// Call in
	// 		Editor::keyPressed(const juce::KeyPress& key) override {
	// 			appMidi.keyPressed(key);
	// 		}
	bool keyPressed(const juce::KeyPress& key);
	
	// Call in
	// 		Processor::preprocessMidi(MidiBuffer& midi, int numSamples) override {
	//	  		appMidi.processMidi(midi, numSamples);
	// 		}
	void processMidi(juce::MidiBuffer& midi, int numSamples);
	
	
	virtual ~AppMidiInput();
	
	void setOctave(int set) { octave = set; }
	
	void addCustomBinding(int press, int note);
	inline void addBinding(int press, int note) { addCustomBinding(press, note); }
//	void addCustomBinding(char press, int note);
	
	void setUseDefaults(bool set) { useDefaults = set; }
protected:
	void noteOn(int n);
	int getNoteCode(n_t n);
protected:
	static constexpr n_t EMPTY=255;
	static constexpr int maxNextNotes = 5;
	n_t nextNotes[maxNextNotes];
	
	int octave;
	
	std::vector<n_t> onNotes;
	
	std::map<int, n_t> customBindings;
	std::map<n_t, int> customBindingsReverse;
	bool useDefaults=true;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AppMidiInput);

};

#else

class AppMidiInput {
public:
	AppMidiInput() {}
	~AppMidiInput() {}
	
	bool keyPressed(const juce::KeyPress& key) { return false; }
	void processMidi(juce::MidiBuffer& midi, int numSamples) {}
};

#endif
