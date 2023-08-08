/** BEGIN_JUCE_MODULE_DECLARATION

    ID:            RT
    vendor:        Roberts.Tech
    version:        1.0.0
    name:           RT Audio
    description:
    website:       isaacroberts.tech
    license:        Proprietary

    dependencies: RTCore

    END_JUCE_MODULE_DECLARATION
*/


#ifndef RT_MODULE
#define RT_MODULE

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <RTCore/RTCore.h>


/** Config: OLD_PRESET
	Switches to the old presets class
 */
#ifndef OLD_PRESET
 #define OLD_PRESET 0
#endif


#include "Utilities/Utility.h"

#include "FX/Templates/_Templates.h"

#endif
