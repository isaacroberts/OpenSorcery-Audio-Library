//
//  StateBase.cpp
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/21/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//
#pragma once

#include "MathSupplement.h"

#include "DirectFormI.h"

namespace Filter {
namespace Butter {

typedef DirectFormI StateType;

class State
{
public:
	State()
	: maxStages(0),
	m_states(NULL)
	{
	}
	State(int maxStages_)
	: maxStages(maxStages_),
	m_states (new StateType[maxStages])
	{
		reset ();
	}
	void setMaxStages(int set)
	{
		maxStages = set;
		m_states = new StateType[maxStages];
	}
	
	void reset ()
	{
		for (int i = 0; i < maxStages; ++i)
		{
			m_states[i].reset();
		}
	}
	
	void boundsCheck() {
		for (int n=0; n < maxStages; ++n)
		{
			m_states[n].boundsCheck();
		}
	}
	
	template <typename Sample, typename CascadeType>
	inline Sample process (const Sample in, const CascadeType& c)
	{
		double out = in;
//		const double vsa = ac();
		
		out = m_states[0].process1 (out, c.m_stages[0], 0);
		
		for (int i=1; i < c.m_numStages; ++i)
		{
			out = m_states[i].process1 (out, c.m_stages[i], 0);
		}
		
		return static_cast<Sample> (out);
	}
	
private:
	int maxStages;
	StateType* m_states;
};


}}
