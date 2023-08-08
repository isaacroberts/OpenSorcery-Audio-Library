//
//  BasicFXProcessor.cpp
//  
//
//  Created by Isaac Roberts on 3/22/21.
//

// Created by Isaac Roberts


#include "BasicFXProcessor.h"
#include "GuiEditor.h"

//==============================================================================
BasicFXProcessor::BasicFXProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	 : AudioProcessor (BusesProperties()
					 #if ! JucePlugin_IsMidiEffect
					  #if ! JucePlugin_IsSynth
					   .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
					  #endif
					   .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
					 #endif
					   )
#endif
{
	
}
BasicFXProcessor::BasicFXProcessor(BusesProperties customBusProperties)
#ifndef JucePlugin_PreferredChannelConfigurations
	 : AudioProcessor (customBusProperties)
#endif
{
	
}


//==============================================================================
bool BasicFXProcessor::hasEditor() const
{
// (override this if you choose to not supply an editor)
	return true;
}

GuiEditor* BasicFXProcessor::getGuiEditor()
{
	AudioProcessorEditor* ae = getActiveEditor();
	if (ae == NULL) return NULL;
	GuiEditor* ge = static_cast<GuiEditor*>(ae);
	return ge;
}


//==============================================================================
const juce::String BasicFXProcessor::getName() const
{
	return JucePlugin_Name;
}
//Copied from XmlElement.cpp because it's not globally accessible
static bool isValidXmlNameStartCharacter (juce_wchar character) noexcept
{
	return character == ':'
		|| character == '_'
		|| (character >= 'a'     && character <= 'z')
		|| (character >= 'A'     && character <= 'Z')
		|| (character >= 0xc0    && character <= 0xd6)
		|| (character >= 0xd8    && character <= 0xf6)
		|| (character >= 0xf8    && character <= 0x2ff)
		|| (character >= 0x370   && character <= 0x37d)
		|| (character >= 0x37f   && character <= 0x1fff)
		|| (character >= 0x200c  && character <= 0x200d)
		|| (character >= 0x2070  && character <= 0x218f)
		|| (character >= 0x2c00  && character <= 0x2fef)
		|| (character >= 0x3001  && character <= 0xd7ff)
		|| (character >= 0xf900  && character <= 0xfdcf)
		|| (character >= 0xfdf0  && character <= 0xfffd)
		|| (character >= 0x10000 && character <= 0xeffff);
}

static bool isValidXmlNameBodyCharacter (juce_wchar character) noexcept
{
	return isValidXmlNameStartCharacter (character)
		|| character == '-'
		|| character == '.'
		|| character == 0xb7
		|| (character >= '0'    && character <= '9')
		|| (character >= 0x300  && character <= 0x036f)
		|| (character >= 0x203f && character <= 0x2040);
}
const juce::String BasicFXProcessor::getXmlName() 
{
	if (xmlTag == "")
	{
		String tag = getName();
	
		assert (tag != "");
		
		if (!isValidXmlNameStartCharacter(tag[0]))
		{
			tag = '_' + tag;
		}
				
		//Filter invalid characters
		xmlTag = "";
		for (int c = 0; c < tag.length(); ++c)
		{
			if (isValidXmlNameBodyCharacter(tag[c]))
			{
				xmlTag += tag[c];
			}
		}
		
	}
	return xmlTag;
}
bool BasicFXProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
	return true;
   #else
	return false;
   #endif
}

bool BasicFXProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
	return true;
   #else
	return false;
   #endif
}

bool BasicFXProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
	return true;
   #else
	return false;
   #endif
}


#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicFXProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
	juce::ignoreUnused (layouts);
	return true;
  #else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
	 && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
	{
		return false;
	}
   #endif
	
	//TODO: If sidechain

	return true;
  #endif
}
#endif

double BasicFXProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int BasicFXProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int BasicFXProcessor::getCurrentProgram()
{
	return 0;
}

void BasicFXProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicFXProcessor::getProgramName (int index)
{
	return {};
}

void BasicFXProcessor::changeProgramName (int index, const juce::String& newName)
{
}

BasicFXProcessor::~BasicFXProcessor()
{
	
}
