/** BEGIN_JUCE_MODULE_DECLARATION

    ID:            RTLottie
    vendor:       OpenSorcery LLC DBA Algorithmic Software
    version:        0.5.0
    name:          RT Lottie
 
	description: My juce-adaptation of RLottie
    website:       algorithmic-software.com
    license:        Proprietary

    dependencies: juce_gui_basics

    END_JUCE_MODULE_DECLARATION
*/

/**
 Add /usr/local/lib to Library paths
 and /usr/local/include to Include paths
 and rlottie to libraries 
 */


//#pragma once

#ifndef RT_LOTTIE_MODULE
#define RT_LOTTIE_MODULE

#include <juce_gui_basics/juce_gui_basics.h>

#include <rlottie/rlottie_capi.h>
#include <rlottie/rlottiecommon.h>

#if _WIN32 || _WIN64

__declspec(dllexport) Lottie_Animation* lottie_animation_from_data(const char* data, const char* key, const char* resource_path);

#endif 

#include "Lottie.h"
#include "RLottieComponent.h"
#include "LottieButton.h"
#include "HoldLottie.h"
//#include "Basics.h"

#endif
