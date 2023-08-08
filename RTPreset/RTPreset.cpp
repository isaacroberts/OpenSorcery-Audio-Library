


#include "RTPreset.h"

/*
 JUCE UPDATE:
 
 
 goto:
	juce_audio_processors > processors > juce_AudioProcessorParameterGroup
	line ~30
 
 replace the AudioProcessorParameterNode destructor with:
	 AudioProcessorParameterGroup::AudioProcessorParameterNode::~AudioProcessorParameterNode()
	 {
	 #if !OLD_PRESET
		 parameter.release();
	 #endif
	 }

 
 
 */


#include "PresetDefinitions.cpp"
#include "TypeHandling.cpp"

#include "Parameter.cpp"
#include "BoolParameter.cpp"

#include "PresetObject.cpp"

#include "SMParamGroup.cpp"

#include "PresetFileList.cpp"

