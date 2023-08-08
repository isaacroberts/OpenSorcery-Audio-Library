//
//  Parameter.cpp
//  
//
//  Created by Isaac Roberts on 8/7/22.
//

// Created by Isaac Roberts

//#include "Parameter.h"

#include <RTCore/RTTypedef.h>

#ifndef RT_TYPEDEF_INCLUDED 
#error "includes missing"
#endif 


#if !TARGET_WIN
template <>
void Parameter<int>::setValue(float nrm) 
{
	if (flatValue != nrm)
	{
		flatValue = nrm;
		int nv = denormalize(flatValue);
		if (value != nv)
		{
			value = nv;
			callCallback();
			callUiCallback(NULL);
		}
//		notify();
	}
}


#endif 


#if TARGET_WIN

extern template class Parameter<int>;
//extern template class Parameter<bool>;
extern template class Parameter<float>;
extern template class Parameter<double>;

#else 
#error "no"

template class Parameter<int>;
//template class Parameter<bool>;
template class Parameter<float>;
template class Parameter<double>;
#endif 