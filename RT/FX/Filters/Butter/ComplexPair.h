//
//  ComplexPair.h
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/23/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

// Created by Isaac Roberts 

#ifndef ComplexPair_h
#define ComplexPair_h

#include <stdio.h>

#include "MathSupplement.h"

namespace Filter {
namespace Butter {

// A conjugate or real pair
struct ComplexPair : complex_pair_t
{
	ComplexPair ()
	{
	}
	
	explicit ComplexPair (const complex_t& c1)
	: complex_pair_t (c1, 0.)
	{
		assert (isReal());
	}
	
	ComplexPair (const complex_t& c1,
				 const complex_t& c2)
	: complex_pair_t (c1, c2)
	{
	}
	
	bool isConjugate () const
	{
		return second == std::conj (first);
	}
	
	bool isReal () const
	{
		return first.imag() == 0 && second.imag() == 0;
	}
	
	// Returns true if this is either a conjugate pair,
	// or a pair of reals where neither is zero.
	bool isMatchedPair () const
	{
		if (first.imag() != 0) {
			// USE AN EPSILON DAMMIT!
			
			return is_equal(second, std::conj(first));
			// return second == std::conj (first);
		}
		else
			//	  return second.imag () == 0 &&
			//			 second.real () != 0 &&
			//			 first.real () != 0;
			return is_equal(second.imag(), 0) &&
			!is_equal(second.real(), 0) &&
			!is_equal(first.real(), 0);
		
	}
	
	bool is_nan () const
	{
		return is_naan (first) || is_naan (second);
	}
};

}}//namespace

#endif /* ComplexPair_h */
