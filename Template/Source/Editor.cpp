/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "Processor.h"
#include "Editor.h"

//==============================================================================
Editor::Editor (Processor& p)
    : GuiEditor (&p), audioProcessor (p)
{
	
	
	
	const float w= 500;
	const float h = 250;
	
	setRatio(w/h, w);
	completeSetup();
}
void Editor::connectKnobs()
{
	auto* preset = audioProcessor.getPreset();
	
}

//==============================================================================
void Editor::paint (juce::Graphics& g)
{
    g.fillAll (Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (20.f);
    g.drawFittedText ("Algorithm", getLocalBounds(), juce::Justification::centred, 1);
}

void Editor::resized()
{
	
}

Editor::~Editor()
{
}
