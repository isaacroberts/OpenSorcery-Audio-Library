//
//  Stage.cpp
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/21/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//
#pragma once

#include <complex>
#include "MathSupplement.h"

#include "ComplexPair.h"

namespace Filter {
namespace Butter {

class Biquad;
//PoleZeroPair = BiquadPoleState
struct BiquadPoleState
{
public:
	BiquadPoleState () { }
	// single pole/zero
	BiquadPoleState (const complex_t& p, const complex_t& z)
	: poles (p), zeros (z)
	{
	}
	// pole/zero pair
	BiquadPoleState (const complex_t& p1, const complex_t& z1,
				  const complex_t& p2, const complex_t& z2)
	: poles (p1, p2)
	, zeros (z1, z2)
	{
	}
	
	explicit BiquadPoleState (const Biquad& s);
	
	
	ComplexPair poles;
	ComplexPair zeros;
	
	bool isSinglePole () const
	{
		return poles.second == 0. && zeros.second == 0.;
	}
	
	bool is_nan () const
	{
		return poles.is_nan() || zeros.is_nan();
	}
	
	double gain;
};

//Cascade::Stage = BiquadBase = Biquad
class Biquad
{
public:
//	struct BiquadState : public DirectFormI, private DenormalPrevention
//	{
//		template <typename Sample>
//		inline Sample process (const Sample in, const Biquad& b)
//		{
//			return static_cast<Sample> (process1 (in, b, ac()));
//		}
//	};
	
	
public:
	Biquad ()
	{
		
	}
	
	explicit Biquad (const BiquadPoleState& bps);

	
	// Process a block of samples in the given form
	template <class StateType, typename Sample>
	void process (int numSamples, Sample* dest, StateType& state) const
	{
		while (--numSamples >= 0) {
			*dest = state.process (*dest, *this);
			dest++;
		}
	}
	
	// Process a block of samples, interpolating from the old section's coefficients
	// to this section's coefficients, over numSamples. This implements smooth
	// parameter changes.
	template <class StateType, typename Sample>
	void smoothProcess1 (int numSamples,
						 Sample* dest,
						 StateType& state,
						 Biquad sectionPrev) const
	{
		double t = 1. / numSamples;
		double da1 = (m_a1 - sectionPrev.m_a1) * t;
		double da2 = (m_a2 - sectionPrev.m_a2) * t;
		double db0 = (m_b0 - sectionPrev.m_b0) * t;
		double db1 = (m_b1 - sectionPrev.m_b1) * t;
		double db2 = (m_b2 - sectionPrev.m_b2) * t;
		
		while (--numSamples >= 0)
		{
			sectionPrev.m_a1 += da1;
			sectionPrev.m_a2 += da2;
			sectionPrev.m_b0 += db0;
			sectionPrev.m_b1 += db1;
			sectionPrev.m_b2 += db2;
			
			*dest = state.process (*dest, sectionPrev);
			dest++;
		}
	}
	
	// Process a block of samples, interpolating from the old section's pole/zeros
	// to this section's pole/zeros, over numSamples. The interpolation is done
	// in the z-plane using polar coordinates.
	template <class StateType, typename Sample>
	void smoothProcess2 (int numSamples,
						 Sample* dest,
						 StateType& state,
						 BiquadPoleState zPrev) const
	{
		BiquadPoleState z (*this);
		double t = 1. / numSamples;
		complex_t dp0 = (z.poles.first  - zPrev.poles.first) * t;
		complex_t dp1 = (z.poles.second - zPrev.poles.second) * t;
		complex_t dz0 = (z.zeros.first  - zPrev.zeros.first) * t;
		complex_t dz1 = (z.zeros.second - zPrev.zeros.second) * t;
		double dg = (z.gain - zPrev.gain) * t;
		
		while (--numSamples >= 0)
		{
			zPrev.poles.first += dp0;
			zPrev.poles.second += dp1;
			zPrev.zeros.first += dz0;
			zPrev.zeros.second += dz1;
			zPrev.gain += dg;
			
			*dest = state.process (*dest, Biquad (zPrev));
			dest++;
		}
	}
	
	
	
	
public:
	// Calculate filter response at the given normalized frequency.
	complex_t response (double normalizedFrequency) const;
	
	std::vector<BiquadPoleState> getPoleZeros () const;
	
	double getA0 () const { return m_a0; }
	double getA1 () const { return m_a1*m_a0; }
	double getA2 () const { return m_a2*m_a0; }
	double getB0 () const { return m_b0*m_a0; }
	double getB1 () const { return m_b1*m_a0; }
	double getB2 () const { return m_b2*m_a0; }
	
	
	
public:
	void setCoefficients (double a0, double a1, double a2,
						  double b0, double b1, double b2);
	
	void setOnePole (complex_t pole, complex_t zero);
	
	void setTwoPole (complex_t pole1, complex_t zero1,
					 complex_t pole2, complex_t zero2);
	
	void setPoleZeroPair (const BiquadPoleState& pair)
	{
		if (pair.isSinglePole ())
			setOnePole (pair.poles.first, pair.zeros.first);
		else
			setTwoPole (pair.poles.first, pair.zeros.first,
						pair.poles.second, pair.zeros.second);
	}
	
//	void setPoleZeroForm (const BiquadPoleState& bps);
	
	void setIdentity ();
	
	void applyScale (double scale);
	
public:
	double m_a0;
	double m_a1;
	double m_a2;
	double m_b1;
	double m_b2;
	double m_b0;
		

};

struct Storage
{
   Storage (int maxStages_, Biquad* stageArray_)
   : maxStages (maxStages_)
   , stageArray (stageArray_)
   {}
   
   int maxStages;
   Biquad* stageArray;
};

}}//namespace
