//
//  Modules.h
//  
//
//  Created by Isaac Roberts on 3/30/22.
//

#pragma once

#include <RTCore/RTCore.h>

#include "Templates/_Templates.h"
#include "Templates/MixerT.h"

#include "Compressors/RTCompressor.h"
#include "Compressors/LookaheadCompressor.h"
#include "Compressors/UpDownCompressor.h"
#include "Compressors/UpDownLookahead.h"
#include "Compressors/UpwardCompressor.h"

#include "Filters/GenericBiquad.h"
#include "Filters/IIRBase.h"
#include "Filters/IIRFilters.h"
#include "Filters/LinkwitzRiley.h"
#include "Filters/LinkwitzRiley2nd.h"
#include "Filters/LinkwitzRiley4th.h"
#include "Filters/MoogLadderFilter.h"
#include "Filters/RTButterworth.h"
//#include "Filters/RTRBJ.h"
#include "Filters/RBJ/_Modules.h"
#include "Filters/SecondOrderIIRFilter.h"

//#include "Reverb/RTFastReverb.h"
//#include "Reverb/RTSingleReverb.h"

#include "BandSplitter.h"
#include "Chain.h"
#include "DummyFX.h"
//#include "DynamicEQ.h"
#include "MinorPitchShifter.h"
#include "MultibandCompressor.h"
#include "RTDeEsser.h"
#include "RTDelay.h"
#include "RTGain.h"
//#include "RTJuceReverb.h"
#include "RTLimiter.h"
#include "RTLufsMeter.h"
#include "RTMeterGauge.h"
#include "RTPan.h"
#include "Sanitizer.h"
#include "Splitter.h"

#include "SuperTapDelay.h"
