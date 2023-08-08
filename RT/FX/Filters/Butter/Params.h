//
//  Params.h
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/23/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

#ifndef Params_h
#define Params_h

namespace Filter {
namespace Butter {
	
struct Params
{
	enum {
		maxParameters = 5
	};
	
	Params()
	{
		clear();
	}
	void clear ()
	{
		for (int i = 0; i < maxParameters; ++i)
			value[i] = 0;
	}
	Params& operator=(const Params& other)
	{
		for (int i = 0; i < maxParameters; ++i)
		{
			value[i] = other.value[i];
		}
		return *this;
	}
	Params& operator=(Params&& other)
	{
		for (int i = 0; i < maxParameters; ++i)
		{
			value[i] = other.value[i];
		}
		return *this;
	}
	
	double& operator[] (int index)
	{
		return value[index];
	}
	
	const double& operator[] (int index) const
	{
		return value[index];
	}
	
	inline int size() const { return maxParameters; }
	
	double value[maxParameters];
};

	
}}
#endif /* Params_h */
