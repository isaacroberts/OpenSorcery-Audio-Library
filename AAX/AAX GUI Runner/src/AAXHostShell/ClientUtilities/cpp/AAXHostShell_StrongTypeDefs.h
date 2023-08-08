//
//  AAXHostShell_StrongTypeDefs.h
//  AAXHostShell
//
//  Created by y.shandra on 6/3/15.
//  Copyright (c) 2015 Avid Technology Inc. All rights reserved.
//

#ifndef AAXHostShell_AAXHostShell_StrongTypeDefs_h
#define AAXHostShell_AAXHostShell_StrongTypeDefs_h

// Boost Includes
#include "boost/serialization/strong_typedef.hpp"
namespace AAXHostShell {
	BOOST_STRONG_TYPEDEF(int32_t, TEffectIndex);
	BOOST_STRONG_TYPEDEF(int32_t, TTypeIndex);
}

#endif
