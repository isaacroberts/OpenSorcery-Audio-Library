/** BEGIN_JUCE_MODULE_DECLARATION

	ID:            HuntPianoRoll
	vendor:        SjHunt93
	version:        1.0.0
	name:           SJ Piano Roll Editor
	description:
	website:       github.com/Sjhunt93/Piano-Roll-Editor
	license:        MIT

	dependencies: juce_gui_basics, RT, RTScale

	END_JUCE_MODULE_DECLARATION
*/


#ifndef PIANO_ROLL_MODULE
#define PIANO_ROLL_MODULE

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <RT/RT.h>

#include <RTScale/RTScale.h>

#include "Util.h"

#include "RT/Midi/LiveMidi.h"

#include "PlayheadComponent.h"

#include "PConstants.h"
#include "NoteGridStyleSheet.hpp"
#include "NoteModel.hpp"
#include "PNoteComponent.hpp"
#include "KeyboardComponent.hpp"
#include "NoteGridComponent.hpp"
#include "NoteGridControlPanel.hpp"
#include "TimelineComponent.hpp"
#include "PianoRollComponent.hpp"


#endif
