//
//  RTTypedef.h
//  
//
//  Created by Isaac Roberts on 8/2/20.
//

#ifndef RTTypedef_h
#define RTTypedef_h

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



#define TARGET_AU1 JucePlugin_Build_AU
#define TARGET_AU3 JucePlugin_Build_AUv3
#define TARGET_VST1 JucePlugin_Build_VST
#define TARGET_VST3 JucePlugin_Build_VST3

#define TARGET_AU_ANY (TARGET_AU1 || TARGET_AU3)
#define TARGET_VST_ANY (TARGET_VST1 || TARGET_VST3)

#define TARGET_AAX JucePlugin_Build_AAX

#define TARGET_STANDALONE JucePlugin_Build_Standalone

#define TARGET_APP (TARGET_STANDALONE && JUCE_MAC)
#define TARGET_EXE (TARGET_STANDALONE && JUCE_WINDOWS)

#define TARGET_AU_ONLY (TARGET_AU_ANY && !TARGET_VST_ANY && !TARGET_STANDALONE)
#define TARGET_STANDALONE_ONLY (!TARGET_AU_ANY && !TARGET_VST_ANY && TARGET_STANDALONE)
#define TARGET_VST_ONLY (!TARGET_AU_ANY && TARGET_VST_ANY && !TARGET_STANDALONE)

#ifndef DEBUG
#define DEBUG JUCE_DEBUG
#endif


#define TARGET_OSX JUCE_MAC
#define TARGET_MAC TARGET_OSX
#define TARGET_WIN JUCE_WINDOWS
#define TARGET_WINDOWS TARGET_WIN
#define TARGET_LINUX 0



#define RT_TYPEDEF_INCLUDED 1

//Tells what format plugin is in, with allowances for compile-time optimizations when plugin is only being built in one format.
// Should only be used with C++ structure, because it may be at run-time

#if TARGET_AU_ONLY
#define RUNNING_AU true
#elif TARGET_AU_ANY
#define RUNNING_AU Format::isAU()
#else
#define RUNNING_AU false
#endif

#if TARGET_VST_ONLY
#define RUNNING_VST true
#elif TARGET_VST_ANY
#define RUNNING_VST Format::isVST()
#else
#define RUNNING_VST false
#endif

#if TARGET_STANDALONE_ONLY
#define RUNNING_STANDALONE true
#elif TARGET_STANDALONE
#define RUNNING_STANDALONE Format::isStandalone()
#else
#define RUNNING_STANDALONE false
#endif


#ifdef JUCE_DSP_H_INCLUDED

typedef const juce::dsp::ProcessSpec Spec;

typedef const juce::dsp::ProcessContextReplacing<float> Replacing;
typedef const juce::dsp::ProcessContextNonReplacing<float> StreamContext;
typedef const juce::dsp::AudioBlock<const float> InputBlock;
typedef juce::dsp::AudioBlock<float> OutputBlock;
typedef juce::dsp::AudioBlock<float> ABlock;

#endif
/*
 JUCE UPDATE
 go to juce::dsp::AudioBlock and add this
 juce_dsp > containers > juce_AudioBlock.h
 add it under the word public:
 
 
	 const SampleType* operator[](int c) const noexcept {
		return getChannelPointer(c);
	}
	SampleType* operator[](int c) noexcept {
		return getChannelPointer(c);
	}
	const SampleType* operator[](size_t c) const noexcept {
		return getChannelPointer(c);
	}
	SampleType* operator[](size_t c) noexcept {
		return getChannelPointer(c);
	}
 */

#endif /* RTTypedef_h */
