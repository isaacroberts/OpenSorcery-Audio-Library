//
//  Stage.cpp
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/21/21.
//  Copyright . All rights reserved.
//

#include "Biquad.h"
#include "MathSupplement.h"

#include <vector>

namespace Filter {
namespace Butter {
	
BiquadPoleState::BiquadPoleState (const Biquad& s)
{
  const double a0 = s.getA0 ();
  const double a1 = s.getA1 ();
  const double a2 = s.getA2 ();
  const double b0 = s.getB0 ();
  const double b1 = s.getB1 ();
  const double b2 = s.getB2 ();

  if (a2 == 0 && b2 == 0)
  {
	// single pole
	poles.first = -a1;
	zeros.first = -b0 / b1;
	poles.second = 0;
	zeros.second = 0;
  }
  else
  {
	{
	  const complex_t c = sqrt (complex_t (a1 * a1 - 4 * a0 * a2, 0));
	  double d = 2. * a0;
	  poles.first = -(a1 + c) / d;
	  poles.second =  (c - a1) / d;
	  assert (!poles.is_nan());
	}

	{
	  const complex_t c = sqrt (complex_t (
		b1 * b1 - 4 * b0 * b2, 0));
	  double d = 2. * b0;
	  zeros.first = -(b1 + c) / d;
	  zeros.second =  (c - b1) / d;
	  assert (!zeros.is_nan());
	}
  }

  gain = b0 / a0;
}

complex_t Biquad::response (double normalizedFrequency) const
{
  const double a0 = getA0 ();
  const double a1 = getA1 ();
  const double a2 = getA2 ();
  const double b0 = getB0 ();
  const double b1 = getB1 ();
  const double b2 = getB2 ();

  const double w = 2 * doublePi * normalizedFrequency;
  const complex_t czn1 = std::polar (1., -w);
  const complex_t czn2 = std::polar (1., -2 * w);
  complex_t ch (1);
  complex_t cbot (1);

  complex_t ct (b0/a0);
  complex_t cb (1);
  ct = addmul (ct, b1/a0, czn1);
  ct = addmul (ct, b2/a0, czn2);
  cb = addmul (cb, a1/a0, czn1);
  cb = addmul (cb, a2/a0, czn2);
  ch   *= ct;
  cbot *= cb;

  return ch / cbot;
}

std::vector<BiquadPoleState> Biquad::getPoleZeros () const
{
  std::vector<BiquadPoleState> vpz;
  BiquadPoleState bps (*this);
  vpz.push_back (bps);
  return vpz;
}

void Biquad::setCoefficients (double a0, double a1, double a2,
							  double b0, double b1, double b2)
{
	if (is_naan (a0) || is_naan (a1) || is_naan (a2) ||
		is_naan (b0) || is_naan (b1) || is_naan (b2))
	{
		jassert(false);
		
		m_a0 = 1;
		m_a1 = 0;
		m_a2 = 0;
		m_b0 = 1;
		m_b1 = 0;
		m_b2 = 0;
		
		return;
	}
	
	m_a0 = a0;
	m_a1 = a1/a0;
	m_a2 = a2/a0;
	m_b0 = b0/a0;
	m_b1 = b1/a0;
	m_b2 = b2/a0;
	
}

void Biquad::setOnePole (complex_t pole, complex_t zero)
{
  jassert (pole.imag() == 0);
  jassert (zero.imag() == 0);
  
  const double a0 = 1;
  const double a1 = -pole.real();
  const double a2 = 0;
  const double b0 = -zero.real();
  const double b1 = 1;
  const double b2 = 0;

  setCoefficients (a0, a1, a2, b0, b1, b2);
}

void Biquad::setTwoPole (complex_t pole1, complex_t zero1,
							 complex_t pole2, complex_t zero2)
{
  const double a0 = 1;
  double a1;
  double a2;

  if (pole1.imag() != 0)
  {
	jassert (is_equal(pole2,std::conj (pole1)));

	a1 = -2 * pole1.real();
	a2 = std::norm (pole1);
  }
  else
  {
	jassert (is_equal(pole2.imag(), 0));

	a1 = -(pole1.real() + pole2.real());
	a2 =   pole1.real() * pole2.real();
  }

  const double b0 = 1;
  double b1;
  double b2;

  if (zero1.imag() != 0)
  {
	  if (! is_equal(zero2, std::conj (zero1)))
	  {
		  zero2 = std::conj(zero1);
	  }

	b1 = -2 * zero1.real();
	b2 = std::norm (zero1);
  }
  else
  {
	jassert (is_equal(zero2.imag(), 0));

	b1 = -(zero1.real() + zero2.real());
	b2 =   zero1.real() * zero2.real();
  }
  setCoefficients (a0, a1, a2, b0, b1, b2);
}

//void Biquad::setPoleZeroForm (const BiquadPoleState& bps)
//{
//  setPoleZeroPair (bps);
//  applyScale (bps.gain);
//}

void Biquad::setIdentity ()
{
  setCoefficients (1, 0, 0, 1, 0, 0);
}

void Biquad::applyScale (double scale)
{
  m_b0 *= scale;
  m_b1 *= scale;
  m_b2 *= scale;
}
	
}}
