//
//  MidiUtil.h
//  
//
//  Created by Isaac Roberts on 5/29/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "FileLoad.h"

using namespace juce;

class MidiUtil {
public:
	

	static std::pair<bool, bool> midiExtPermitted()
	{//bool midi, bool mid
		auto host = Format::getHostType();
		typedef juce::PluginHostType::HostType Host;
		switch (host.type)
		{
			default:
				return {true, true};
		}
	}
	
	static constexpr const char* const supportedMidiPattern =
							"*.mid"
#if TARGET_WIN
							";*.midi"
#endif
								;
	
	static juce::String getMidiExtension()
	{
		return ".mid";
	}
	static juce::String midiExt() {
#if TARGET_WIN
		return "*.mid*";
#else
		return "*.mid";
#endif
	}
	static juce::MidiFile readMidiFile(juce::File file)
	{
		juce::FileInputStream myStream (file);
		juce::MidiFile midi;
		bool msxs = midi.readFrom (myStream);
		if (msxs)
		{
			return midi;
		}
		else return midi;
	}
	
	
	static int smpteToTickRate(short time);
	
	static juce::MidiMessageSequence* mergeFile(juce::MidiFile file);
	
	static juce::MidiMessageSequence* mergeAndConvertToBPM(juce::MidiFile file, int& ticks);
	static juce::MidiMessageSequence* mergeAndConvertToSeconds(juce::MidiFile file);


	//Save & Load XML
	
private:
	static void saveToXml(juce::XmlElement* xml, juce::MidiMessage& msg);
	static bool loadMessageFromXml(juce::XmlElement* xml, juce::MidiMessage&);
public:
	

	static void saveToXml(juce::XmlElement* xml, const juce::MidiMessageSequence* seq);
	static juce::MidiMessageSequence* loadSequenceFromXml(juce::XmlElement* xml);
	
	static void saveToXml(juce::XmlElement* xml, const juce::MidiMessageSequence* seq, int ticks);
	static juce::MidiMessageSequence* loadSequenceFromXml(juce::XmlElement* xml, int& ticks);

	//TODO: Desired ticks rate can multiply the timestamps 
};
