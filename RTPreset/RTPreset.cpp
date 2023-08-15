


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


	juce_core > xml > juce_XmlELement.h

	 // Convinience functions
	// Sets the local variable if the attribute was found, leave it if not.
	void getAttribute(StringRef name, int& value) const {
		value = getIntAttribute(name, value);
	}
	void getAttribute(StringRef name, size_t& value) const {
		value = getIntAttribute(name, (int)value);
	}
	void getAttribute(StringRef name, unsigned int& value) const {
		value = getIntAttribute(name, value);
	}
	void getAttribute(StringRef name, String& value) const {
		value = getStringAttribute(name, value);
	}
	void getAttribute(StringRef name, double& value) const {
		value = getDoubleAttribute(name, value);
	}
	void getAttribute(StringRef name, float& value) const {
		value = getDoubleAttribute(name, value);
	}
	void getAttribute(StringRef name, bool& value) const {
		value = getBoolAttribute(name, value);
	}
	template <class Type>
	void getAttribute(StringRef name, Type& value) const {
		static_assert("Unrecognized type");
	}
	// For all types except integral types:
	template<typename Type>
	typename std::enable_if<std::is_integral<Type>::value>::type getAttribute(StringRef name, Type& value) const
	{
		value = getIntAttribute(name, (int)value);
	}

	// For integral types only:
	template<typename Type>
	typename std::enable_if<std::is_floating_point<Type>::value>::type getAttribute(StringRef name, Type& value) const
	{
		value = getDoubleAttribute(name, (double)value);
	}
 
 */


#include "PresetDefinitions.cpp"
#include "TypeHandling.cpp"

#include "Parameter.cpp"
#include "BoolParameter.cpp"

#include "PresetObject.cpp"

#include "SMParamGroup.cpp"

#include "PresetFileList.cpp"

