//
//  Presets.h
//  
//
//  Created by Isaac Roberts on 7/7/22.
//

#ifndef Presets_h
#define Presets_h

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



#endif /* Presets_h */
