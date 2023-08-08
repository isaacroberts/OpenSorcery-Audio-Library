//
//  MathSupplement.h
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/23/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

#ifndef MathSupplement_h
#define MathSupplement_h

//Common.h

#ifdef _MSC_VER
#  pragma warning (disable: 4100)
#endif

//#include <assert.h>
#include <stdlib.h>

#include <cassert>
#include <cfloat>
#include <cmath>
#include <complex>
#include <cstring>
#include <string>
#include <limits>
#include <vector>
#include <algorithm>

#ifdef _MSC_VER
//namespace tr1 = std::tr1;
#else
namespace tr1 = std;
#endif

//Turns off debug which often involves longer calculatios
#ifndef NDEBUG
#define NDEBUG 
#endif

//Jassert

//includes Jassert which only fails in debug mode
#include "juce_core/system/juce_PlatformDefs.h"

//MathSupplement.h

const double doublePi		=3.1415926535897932384626433832795028841971;
const double doublePi_2	=1.5707963267948966192313216916397514420986;
const double doubleLn2  =0.69314718055994530941723212145818;//?????
const double doubleLn10	=2.3025850929940456840179914546844;//??????

typedef std::complex<double> complex_t;
typedef std::pair<complex_t, complex_t> complex_pair_t;

template<typename Real>
inline std::complex<Real> solve_quadratic_1 (Real a, Real b, Real c)
{
	return (-b + sqrt (std::complex<Real> (b*b - 4*a*c, 0))) / (2. * a);
}

template<typename Real>
inline std::complex<Real> solve_quadratic_2 (Real a, Real b, Real c)
{
	return (-b - sqrt (std::complex<Real> (b*b - 4*a*c, 0))) / (2. * a);
}

inline const complex_t infinity()
{
	return complex_t (std::numeric_limits<double>::infinity());
}

inline const complex_t adjust_imag (const complex_t& c)
{
	if (fabs (c.imag()) < 1e-30)
		return complex_t (c.real(), 0);
	else
		return c;
}

template <typename Ty, typename To>
inline std::complex<Ty> addmul (const std::complex<Ty>& c,
								Ty v,
								const std::complex<To>& c1)
{
	return std::complex <Ty> (
							  c.real() + v * c1.real(), c.imag() + v * c1.imag());
}

template <typename Ty>
inline std::complex<Ty> recip (const std::complex<Ty>& c)
{
	Ty n = 1.0 / std::norm (c);
	
	return std::complex<Ty> (n * c.real(), n * c.imag());
}

template <typename Ty>
inline Ty asinh (Ty x)
{
	return log (x + std::sqrt (x * x + 1 ));
}

template <typename Ty>
inline Ty acosh (Ty x)
{
	return log (x + std::sqrt (x * x - 1));
}

template <typename Ty>
inline bool is_naan (Ty v)
{
	return !(v == v);
}

template <>
inline bool is_naan<complex_t> (complex_t v)
{
	return is_naan (v.real()) || is_naan (v.imag());
}

inline bool is_equal(complex_t a, complex_t b, float eps=.001)
{
	if (fabs(a.real()-b.real()) > eps )
		return false;
	else if (fabs(a.imag() - b.imag()) > eps)
		return false;
	else
		return true;
}
inline bool is_equal(float a, float b, float eps=.001)
{
	return fabs(a-b) < eps;
}
inline bool is_equal(double a, double b, double eps=.001)
{
	return fabs(a-b) < eps;
}
//------------------------------------------------------------------------------

/*
 * Hack to prevent denormals
 *
 */


#endif /* MathSupplement_h */
