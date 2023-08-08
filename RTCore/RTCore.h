/** BEGIN_JUCE_MODULE_DECLARATION

    ID:            RTCore
    vendor:        Roberts.Tech
    version:        1.0.0
    name:           RT Core
    description:
    website:       isaacroberts.tech
    license:        Proprietary

    dependencies: juce_dsp

    END_JUCE_MODULE_DECLARATION
*/


#ifndef RT_CORE_MODULE
#define RT_CORE_MODULE

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

class InstrData;

#define TAU M_PI * 2

// ------------------ Plugin Format --------------------------------------------------
class Format {
public:
	//DAW name
	static const juce::PluginHostType& getHostType()
	{
	   static juce::PluginHostType hostType;
//		jassert(hostType != )
	   return hostType;
	}
	static inline const juce::PluginHostType& getHost() {
		return getHostType();
	}
	static inline bool isFL() {//Stupid FL
		return getHostType().isFruityLoops();
	}

	//Plugin Type
	static juce::AudioProcessor::WrapperType getPluginType() {
	   return juce::PluginHostType::getPluginLoadedAs();
	}
	static bool isAU() {
	   jassert(getPluginType() != juce::AudioPluginInstance::wrapperType_AudioUnitv3);
	   return getPluginType()==juce::AudioPluginInstance::wrapperType_AudioUnit;
	}
	static bool isVST() {
	   jassert(getPluginType() != juce::AudioPluginInstance::wrapperType_VST);
	   return getPluginType()==juce::AudioPluginInstance::wrapperType_VST3;
	}
	static bool isAAX() {
	   return getPluginType()==juce::AudioPluginInstance::wrapperType_AAX;
	}
	static bool isStandalone() {
	   return getPluginType()==juce::AudioPluginInstance::wrapperType_Standalone;
	}
};

class ProcessData {
	float sampleRate;
	int numChannels;
	int maxBlock;
};


#include "RTTypedef.h"

#include "BlockStorage.h"

#include "Context.h"

#include "Utilities/Utility.h"
#include "Utilities/BufferManager.h"

#include "RingBuffer/RingBuffer.h"
#include "RingBuffer/MonoRingBuffer.h"

#include "Osc.h"
#include "Oscillator.h"

#endif
