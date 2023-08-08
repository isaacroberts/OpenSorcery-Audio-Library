//
//  AAXHostShell_Utilities.h
//  AAXHostShell Utilities
//
//  Created by Rob Majors on 1/7/15.
//  Copyright (c) 2015 Avid Technology Inc. All rights reserved.
//

#ifndef AAXHostShell_AAXHostShell_Utilities_h
#define AAXHostShell_AAXHostShell_Utilities_h

// AAXHostShell includes
#include "AAXHostShell_Types.h"

// AAX includes
#include "AAX_Enums.h"

// Standard includes
#include <cstring>


namespace AAXHostShell
{
	
	inline bool HostContextPtrIsValid(const AAXHShell_SHostContext* inHostContextPtr)
	{
		bool result = false;
		
		if (NULL != inHostContextPtr)
		{
			result = true;
			const AAXHShell_SHostContext& context = *inHostContextPtr;
			
			if (true == result)
			{
				switch (context.mPlatform)
				{
					case kAAXHShell_Platform_Native:
					case kAAXHShell_Platform_TI:
					case kAAXHShell_Platform_None:
					case kAAXHShell_Platform_Any:
						break;
					default:
						result = false;
						break;
				}
			}
			
			if (true == result)
			{
				switch (context.mInputStemFormat) {
					case AAX_eStemFormat_Mono:
					case AAX_eStemFormat_Stereo:
					case AAX_eStemFormat_LCR:
					case AAX_eStemFormat_LCRS:
					case AAX_eStemFormat_Quad:
					case AAX_eStemFormat_5_0:
					case AAX_eStemFormat_5_1:
					case AAX_eStemFormat_6_0:
					case AAX_eStemFormat_6_1:
					case AAX_eStemFormat_7_0_SDDS:
					case AAX_eStemFormat_7_1_SDDS:
					case AAX_eStemFormat_7_0_DTS:
					case AAX_eStemFormat_7_1_DTS:
					case AAX_eStemFormat_7_0_2:
					case AAX_eStemFormat_7_1_2:
					case AAX_eStemFormat_Ambi_1_ACN:
					case AAX_eStemFormat_Ambi_2_ACN:
					case AAX_eStemFormat_Ambi_3_ACN:
					case AAX_eStemFormat_None:
					case AAX_eStemFormat_Any:
						break;
					default:
						result = false;
						break;
				}
			}
			
			if (true == result)
			{
				switch (context.mOutputStemFormat) {
					case AAX_eStemFormat_Mono:
					case AAX_eStemFormat_Stereo:
					case AAX_eStemFormat_LCR:
					case AAX_eStemFormat_LCRS:
					case AAX_eStemFormat_Quad:
					case AAX_eStemFormat_5_0:
					case AAX_eStemFormat_5_1:
					case AAX_eStemFormat_6_0:
					case AAX_eStemFormat_6_1:
					case AAX_eStemFormat_7_0_SDDS:
					case AAX_eStemFormat_7_1_SDDS:
					case AAX_eStemFormat_7_0_DTS:
					case AAX_eStemFormat_7_1_DTS:
					case AAX_eStemFormat_7_0_2:
					case AAX_eStemFormat_7_1_2:
					case AAX_eStemFormat_Ambi_1_ACN:
					case AAX_eStemFormat_Ambi_2_ACN:
					case AAX_eStemFormat_Ambi_3_ACN:
					case AAX_eStemFormat_None:
					case AAX_eStemFormat_Any:
						break;
					default:
						result = false;
						break;
				}
			}
			
			result = result && ((kAAXHShell_SampleRate_Any == context.mSampleRate) ||
								(AAX_eSampleRateMask_No != getMaskForSampleRate(context.mSampleRate)));
		}
		
		return result;
	}
	
	inline bool HostContextPtrIsAny(const AAXHShell_SHostContext* inHostContextPtr)
	{
		return (0 == std::memcmp(inHostContextPtr, &AAXHShell_kHostContext_Any, sizeof(AAXHShell_SHostContext)));
	}

}

#endif
