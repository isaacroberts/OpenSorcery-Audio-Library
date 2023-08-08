/*******************************************************************************

"A Collection of Useful C++ Classes for Digital Signal Processing"
 By Vinne Falco

Official project location:
https://github.com/vinniefalco/DSPFilters

See Documentation.cpp for contact information, notes, and bibliography.

--------------------------------------------------------------------------------

License: MIT License (http://www.opensource.org/licenses/mit-license.php)
Copyright (c) 2009 by Vinne Falco

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*******************************************************************************/

#include "Common.h"
#include "Butterworth.h"

//#include <iostream>

namespace VF {

namespace Butterworth {

AnalogLowPass::AnalogLowPass ()
  : m_numPoles (-1)
{
  setNormal (0, 1);
}

void AnalogLowPass::design (int numPoles)
{
  if (m_numPoles != numPoles)
  {
    m_numPoles = numPoles;

    reset ();

    const double n2 = 2 * numPoles;
    const int pairs = numPoles / 2;
    for (int i = 0; i < pairs; ++i)
    {
      complex_t c = std::polar (1., doublePi_2 + (2 * i + 1) * doublePi / n2);
      addPoleZeroConjugatePairs (c, infinity());
    }

    if (numPoles & 1)
      add (-1, infinity());
  }
}

//------------------------------------------------------------------------------

AnalogLowShelf::AnalogLowShelf ()
  : m_numPoles (-1)
{
  setNormal (doublePi, 1);
}

void AnalogLowShelf::design (int numPoles, double gainDb)
{
//	std::cout<<"designing analog loww shelf"<<std::endl;
  if (m_numPoles != numPoles ||
      m_gainDb != gainDb)
  {
    m_numPoles = numPoles;
    m_gainDb = gainDb;

    reset ();

    const double n2 = numPoles * 2;
	  const double amp = pow (10., gainDb/20);
    const double g = pow (amp, 1. / n2);
//	  std::cout<<"gain, amp = "<<gainDb<<", "<<amp<<" -> "<<g<<std::endl;
    const double gp = -1. / g;
    const double gz = -g;

//	std::cout<<"I'm designing an analog low shelf "<<numPoles<<" db="<<gainDb<<std::endl;
//	  std::cout<<"n2, g, gp, gz = "<<n2<<" "<<g<<" "<<gp<<" "<<gz<<std::endl;
    const int pairs = numPoles / 2;
//	  std::cout<<"pi does equal "<<doublePi<<std::endl;
    for (int i = 1; i <= pairs; ++i)
    {
      const double theta = doublePi * (0.5 - (2 * i - 1) / n2);
		//Changed this from (gp/gz) to (fabs(gp/gz)) because having positive values for gp and gz was theoretically impossible
		auto pole =std::polar ((-gp), theta + doublePi);
		auto zero =std::polar ((-gz), theta + doublePi);
//		std::cout<<"gp, gz, theta "<<gp<<" "<<gz<<" "<<theta<<std::endl;
//		std::cout<<"pole="<<pole<<", zero = "<<zero<<std::endl;
		addPoleZeroConjugatePairs (pole, zero);
    }
    
    if (numPoles & 1)
      add (gp, gz);
	  
//	  std::cout<<"getNormalW = "<<getNormalW()<<std::endl;
  }
  else {
//		std::cout<<" analog low shelf skipped"<<std::endl;
//	  std::cout<<m_numPoles<<" "<<numPoles<<std::endl;
//	  std::cout<<m_gainDb<<" "<<gainDb<<std::endl;
  }
}

//------------------------------------------------------------------------------

void LowPassBase::setup (int order,
                         double sampleRate,
                         double cutoffFrequency)
{
  m_analogProto.design (order);

  LowPassTransform (cutoffFrequency / sampleRate,
                    m_digitalProto,
                    m_analogProto);

  Cascade::setLayout (m_digitalProto);
}

void HighPassBase::setup (int order,
                          double sampleRate,
                          double cutoffFrequency)
{
  m_analogProto.design (order);

  HighPassTransform (cutoffFrequency / sampleRate,
                     m_digitalProto,
                     m_analogProto);

  Cascade::setLayout (m_digitalProto);
}

void BandPassBase::setup (int order,
                          double sampleRate,
                          double centerFrequency,
                          double widthFrequency)
{
  m_analogProto.design (order);

  BandPassTransform (centerFrequency / sampleRate,
                     widthFrequency / sampleRate,
                     m_digitalProto,
                     m_analogProto);

  Cascade::setLayout (m_digitalProto);
}

void BandStopBase::setup (int order,
                          double sampleRate,
                          double centerFrequency,
                          double widthFrequency)
{
  m_analogProto.design (order);

  BandStopTransform (centerFrequency / sampleRate,
                     widthFrequency / sampleRate,
                     m_digitalProto,
                     m_analogProto);

  Cascade::setLayout (m_digitalProto);
}

void LowShelfBase::setup (int order,
                         double sampleRate,
                         double cutoffFrequency,
                         double gainDb)
{
  m_analogProto.design (order, gainDb);

  LowPassTransform (cutoffFrequency / sampleRate,
                    m_digitalProto,
                    m_analogProto);

  Cascade::setLayout (m_digitalProto);
}

void HighShelfBase::setup (int order,
                           double sampleRate,
                           double cutoffFrequency,
                           double gainDb)
{
//	std::cout<<"start setup highshelfbase"<<std::endl;
  m_analogProto.design (order, gainDb);
	
  HighPassTransform (cutoffFrequency / sampleRate,
                     m_digitalProto,
                     m_analogProto);
	
  Cascade::setLayout (m_digitalProto);
}

void BandShelfBase::setup (int order,
                           double sampleRate,
                           double centerFrequency,
                           double widthFrequency,
                           double gainDb)
{
  m_analogProto.design (order, gainDb);

  BandPassTransform (centerFrequency / sampleRate,
                     widthFrequency / sampleRate,
                     m_digitalProto,
                     m_analogProto);

  // HACK!
	
  m_digitalProto.setNormal (((centerFrequency/sampleRate) < 0.25) ? doublePi : 0, 1);

  Cascade::setLayout (m_digitalProto);
}

}

}
