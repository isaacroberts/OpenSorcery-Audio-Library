/** BEGIN_JUCE_MODULE_DECLARATION

    ID:            HifiReverb
    vendor:        HiFi-LoFi
    version:        1.0.1
    name:           HifiReverb
    description:
    website:       https://github.com/HiFi-LoFi/FFTConvolver
    license:        MIT

    dependencies: RTCore

    END_JUCE_MODULE_DECLARATION
*/


#ifndef HIFILOFI_REVERB
#define HIFILOFI_REVERB


#include <RTCore/RTCore.h>


/** Config: HIFI_USE_CHIP_FFTS
	Uses Intel IPP if applicable, Apple Accelerate otherwise
 
	Link:
		//TODO: Write down instructions
 */
#ifndef HIFI_USE_CHIP_FFTS
 #define HIFI_USE_CHIP_FFTS 0
#endif

/** Config: HIFI_USE_FFTW
	Link:
		//TODO: Write down folders and libraries
 */
#ifndef HIFI_USE_FFTW
 #define HIFI_USE_FFTW 0
#endif

#if HIFI_USE_CHIP_FFTS

#define AUDIOFFT_APPLE_ACCELERATE 1


#elif HIFI_USE_FFTW

#define AUDIOFFT_FFTW3 1

#else

//HIFI will define defaults

#endif


//#define AUDIOFFT_FFTW3 1
//#define AUDIOFFT_APPLE_ACCELERATE 1


//#include "RTTypedef.h"

#endif
