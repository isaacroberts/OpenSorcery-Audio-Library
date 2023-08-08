//
//  Parameter.cpp
//  Library - Shared Code
//
//  Created by Isaac Roberts on 7/2/20.
//

// Created by Isaac Roberts


//#include "Parameter.h"





template <class Type>
void Parameter<Type>::setInfo(String id_, String name_, Type low_, Type high_, Type default_)
{
	id = id_;
	setName(name_);
	low = low_;
	high = high_;
	def = default_;
	
	range.start=low;
	range.end=high;
	range.interval = getDefaultInterval();
}
template <>
void Parameter<int>::setInfo(String id_, String name_, int low_, int high_, int default_)
{
	id = id_;
	setName(name_);
	low = low_;
	high = high_;
	def = default_;
	
	range.start=low;
	range.end=high+1;//Inclusive
	range.interval = getDefaultInterval();
}

template<>
void Parameter<bool>::setInfo(String id_, String name_, bool def_)
{
	setInfo(id_, name_, false, true, def_);
}
template<class Type>
void Parameter<Type>::setInfo(String id_, String name_, Type def_)
{
	//Runtime error
	jassert(false);
}




template class Parameter<int>;
template class Parameter<bool>;
template class Parameter<float>;
template class Parameter<double>;
