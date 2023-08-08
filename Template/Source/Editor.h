/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Processor.h"

#include "RT/GuiEditor.h"

//==============================================================================
/**
*/
class Editor  : public GuiEditor
{
public:
    Editor (Processor&);
    ~Editor() override;
	
	void connectKnobs() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Processor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
