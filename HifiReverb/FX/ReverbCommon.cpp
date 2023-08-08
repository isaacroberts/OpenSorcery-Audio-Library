//
//  ReverbCommon.cpp
//  
//
//  Created by Isaac Roberts on 8/13/22.
//

// Created by Isaac Roberts

#include "ReverbCommon.h"



void ReverbCommon::normalize(float* impulse, size_t impulse_len)
{
	float total=0;
	for (int n=0; n < impulse_len; ++n)
	{
		total += pow(impulse[n], 2);
	}
	total = sqrt(total);
	if (total < 1e-20)
	{
		return;
	}
	else
	{
		for (int n=0; n < impulse_len; ++n)
		{
			impulse[n] /= total;
		}
	}
}
