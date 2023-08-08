/** BEGIN_JUCE_MODULE_DECLARATION

    ID:            RTOldPresets
    vendor:       OpenSorcery LLC DBA Algorithmic Software
    version:        0.5.0
    name:          Old RT Preset
    description:
    website:       algorithmic-software.com
    license:        Proprietary

    dependencies: juce_audio_processors

    END_JUCE_MODULE_DECLARATION
*/

//#pragma once

#ifndef RT_OLD_PRESET_MODULE
#define RT_OLD_PRESET_MODULE

#ifndef OLD_PRESET
#define OLD_PRESET 1
#endif

#if !OLD_PRESET
#error "Wrong preset module!"
#endif

#include <string>

#include <iostream>
#include <ctime>


#include <juce_audio_processors/juce_audio_processors.h>

//If if I ever add parameters to a released plugin, this will need to be fixed to actually pass numbers
#ifndef LAZY_PARAM_VERSION_HACK
#define LAZY_PARAM_VERSION_HACK 1
#endif

using namespace juce;

#include "looper.h"

#include "PresetDefinitions.h"
#include "TypeHandling.h"

#include "Parameter.h"
#include "ParameterValue.h"
#include "Preset.h"

#include "LoneParam.h"
#include "LinkedParameter.h"

#include "PresetList.h"


#endif
