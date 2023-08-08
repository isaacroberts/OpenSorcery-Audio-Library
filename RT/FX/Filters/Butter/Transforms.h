//
//  Transforms.h
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/23/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

// Created by Isaac Roberts 

#ifndef Transforms_h
#define Transforms_h

#include <stdio.h>

#include "LayoutBase.h"

namespace Filter {
namespace Butter {

/*
 * s-plane to z-plane transforms
 *
 * For pole filters, an analog prototype is created via placement of
 * poles and zeros in the s-plane. The analog prototype is either
 * a halfband low pass or a halfband low shelf. The poles, zeros,
 * and normalization parameters are transformed into the z-plane
 * using variants of the bilinear transformation.
 *
 */

// low pass to low pass
class LowPassTransform
{
public:
  LowPassTransform (double fc,
					LayoutBase& digital,
					LayoutBase const& analog);

private:
  complex_t transform (complex_t c);

  double f;
};

//------------------------------------------------------------------------------

// low pass to high pass
class HighPassTransform
{
public:
  HighPassTransform (double fc,
					 LayoutBase& digital,
					 LayoutBase const& analog);

private:
  complex_t transform (complex_t c);

  double f;
};

//------------------------------------------------------------------------------

// low pass to band pass transform
class BandPassTransform
{

public:
  BandPassTransform (double fc,
					 double fw,
					 LayoutBase& digital,
					 LayoutBase const& analog);

private:
  ComplexPair transform (complex_t c);

  double wc;
  double wc2;
  double a;
  double b;
  double a2;
  double b2;
  double ab;
  double ab_2;
};

//------------------------------------------------------------------------------

// low pass to band stop transform
class BandStopTransform
{
public:
  BandStopTransform (double fc,
					 double fw,
					 LayoutBase& digital,
					 LayoutBase const& analog);

private:
  ComplexPair transform (complex_t c);

  double wc;
  double wc2;
  double a;
  double b;
  double a2;
  double b2;
};

}}

#endif /* Transforms_h */
