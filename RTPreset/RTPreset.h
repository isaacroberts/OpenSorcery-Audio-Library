/** BEGIN_JUCE_MODULE_DECLARATION

    ID:            RTPreset
    vendor:       OpenSorcery LLC DBA Algorithmic Software
    version:        0.5.0
    name:          RT Preset
    description:
    website:       algorithmic-software.com
    license:        Proprietary

    dependencies: juce_audio_processors

    END_JUCE_MODULE_DECLARATION
*/

//#pragma once

#ifndef RT_PRESET_MODULE
#define RT_PRESET_MODULE

#ifndef OLD_PRESET
#define OLD_PRESET 0
#endif

#if OLD_PRESET
#error "Wrong preset module!"
#endif

#include <juce_audio_processors/juce_audio_processors.h>

#include <RTCore/RTCore.h>

using namespace juce;

#include "PresetDefinitions.h"
#include "TypeHandling.h"

#include "ParameterBase.h"
#include "PtrListenerList.h"
#include "Parameter.h"
#include "BoolParameter.h"

#include "LogParameter.h"

#include "PresetObject.h"

#include "SMParamGroup.h"

//#include "PresetFileList.h"

#endif
