//
//  Types.h
//  MagicVocals
//
//  Created by Isaac Roberts on 3/23/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

#ifndef Types_h
#define Types_h

#include "RT/FX/Filters/FilterDefinitions.h"

namespace Filter
{
	
	
	namespace Butter {
		
		static inline int numParams(Type type)
		{
			if (type == LowPass || type == HighPass) //Type I
				return 3;
			else if (type == BandPass || type == BandStop) //Type II
				return 4;
			else if (type == LowShelf || type == HighShelf) //Type III
				return 4;
			else if (type == BandShelf) //Type IV
				return 5;
			else {
				jassert(false);
				return 0;
			}
		}
		
		static inline bool thirdParamGain(Type type) {
			return type == LowShelf || type == HighShelf;
		}
	}
};

#endif /* Types_h */
