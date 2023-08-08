/*
  ==============================================================================

    This file contains the basic framework code for an Algorithm plugin.

  ==============================================================================
*/

#include "Processor.h"
#include "Editor.h"

//==============================================================================
Processor::Processor()
: ProcBase()
{
}


GuiEditor* Processor::createEditor()
{
    return new Editor (*this);
}

Processor::~Processor()
{
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Processor();
}
