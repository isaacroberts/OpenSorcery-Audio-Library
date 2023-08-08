//
//  LayoutBase.h
//  MagicVocals
//
//  Created by Isaac Roberts on 3/23/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

// Created by Isaac Roberts 

#ifndef LayoutBase_h
#define LayoutBase_h

#include <stdio.h>

#include "MathSupplement.h"
#include "Biquad.h"

#include "Types.h"

namespace Filter {
namespace Butter {
	
// Base uses pointers to reduce template instantiations
class LayoutBase
{
public:
  LayoutBase (int maxPoles_)
	: m_numPoles (0)
	, m_maxPoles (maxPoles_)
	, m_pair(new BiquadPoleState[m_maxPoles])
  {
  }

  LayoutBase (int maxPoles_, BiquadPoleState* pairs)
	: m_numPoles (0)
	, m_maxPoles (maxPoles_)
	, m_pair (pairs)
  {
  }

  void setStorage (const LayoutBase& other)
  {
	m_numPoles = 0;
	m_maxPoles = other.m_maxPoles;
	m_pair = other.m_pair;
  }

  void reset ()
  {
	m_numPoles = 0;
  }

  int getNumPoles () const
  {
	return m_numPoles;
  }

  int getMaxPoles () const
  {
	return m_maxPoles;
  }

  void add (const complex_t& pole, const complex_t& zero)
  {
	jassert (!(m_numPoles&1)); // single comes last
	jassert (!is_naan (pole));
	m_pair[m_numPoles/2] = BiquadPoleState (pole, zero);
	++m_numPoles;
  }

  void addPoleZeroConjugatePairs (const complex_t pole,
								  const complex_t zero)
  {
	jassert (!(m_numPoles&1)); // single comes last
	jassert (!is_naan (pole));
	m_pair[m_numPoles/2] = BiquadPoleState (
	  pole, zero, std::conj (pole), std::conj (zero));
	m_numPoles += 2;
  }

  void add (const ComplexPair& poles, const ComplexPair& zeros)
  {
	jassert (!(m_numPoles&1)); // single comes last
	jassert (poles.isMatchedPair ());
	jassert (zeros.isMatchedPair ());
	m_pair[m_numPoles/2] = BiquadPoleState (poles.first, zeros.first,
										 poles.second, zeros.second);
	m_numPoles += 2;
  }

  const BiquadPoleState& getPair (int pairIndex) const
  {
	jassert (pairIndex >= 0 && pairIndex < (m_numPoles+1)/2);
	return m_pair[pairIndex];
  }

  const BiquadPoleState& operator[] (int pairIndex) const
  {
	return getPair (pairIndex);
  }

  double getNormalW () const
  {
	return m_normalW;
  }

  double getNormalGain () const
  {
	return m_normalGain;
  }

  void setNormal (double w, double g)
  {
	m_normalW = w;
	m_normalGain = g;
  }

protected:
  int m_numPoles;
private:
  int m_maxPoles;
  BiquadPoleState* m_pair;
  double m_normalW;
  double m_normalGain;
};


class AnalogPrototype : public LayoutBase
{
public:
	AnalogPrototype (Filter::Type type_, int order);
	
	void design (int numPoles, double gainDb);
	
	void design (int numPoles) {
		design(numPoles, 0);
	}
	
private:
	Filter::Type type;
//	int m_numPoles;
	//This is not used for non-shelf types
	double m_gainDb;
};

}}
#endif /* LayoutBase_h */
