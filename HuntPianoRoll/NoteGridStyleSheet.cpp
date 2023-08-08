//
//  NoteGridStyleSheet.cpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 03/02/2020.
//

//#include "NoteGridStyleSheet.hpp"

StdNoteGridStyleSheet::StdNoteGridStyleSheet ()
{
//    drawMIDINum = false;
//    drawMIDINoteStr = false;
    drawVelocity = true;
    disableEditing = false;
}
juce::Colour StdNoteGridStyleSheet::getWhiteRowColor()
{
	return juce::Colours::lightgrey.darker().withAlpha(0.5f);
}
juce::Colour StdNoteGridStyleSheet::getBlackRowColor()
{
	return juce::Colours::darkgrey.withAlpha(0.5f);
}
juce::Colour StdNoteGridStyleSheet::getSidebarWhiteColor()
{
	return juce::Colours::lightgrey.darker();
}
juce::Colour StdNoteGridStyleSheet::getSidebarBlackColor()
{
	return juce::Colours::darkgrey;
}

juce::Colour StdNoteGridStyleSheet::getSidebarWhiteFontColor()
{
	return juce::Colours::white;
}
juce::Colour StdNoteGridStyleSheet::getSidebarBlackFontColor()
{
	return juce::Colours::white;
}
juce::Colour StdNoteGridStyleSheet::getNoteColor()
{
	return juce::Colour(252,97,92);
}
