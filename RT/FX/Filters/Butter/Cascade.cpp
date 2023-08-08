//
//  Cascade.cpp
//  MagicVocals
//
//  Created by Isaac Roberts on 3/21/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

// Created by Isaac Roberts

#include "Cascade.h"
#include "State.h"

namespace Filter {
namespace Butter {

AnalogPrototype::AnalogPrototype (Filter::Type type_, int order)
: LayoutBase(order),
type(type_),
m_gainDb(0)
{
	using namespace Filter;
	if (isShelf(type))
	{
		setNormal (doublePi, 1);
	}
	else
	{
		setNormal (0, 1);
	}
}


void AnalogPrototype::design (int numPoles, double gainDb)
{
	//Non-shelf types
	using namespace Filter;
	if (!isShelf(type))
	{
		if (m_numPoles != numPoles)
		{
//			m_numPoles = numPoles;
			
//			reset ();
			m_numPoles = 0;
			
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
	else
	{
		if (m_numPoles != numPoles ||
			m_gainDb != gainDb)
		{
			m_gainDb = gainDb;
			
			m_numPoles = 0;
			
			const double n2 = numPoles * 2;
			const double amp = pow (10., gainDb/20);
			const double g = pow (amp, 1. / n2);

			const double gp = -1. / g;
			const double gz = -g;
			
			const int pairs = numPoles / 2;
			for (int i = 1; i <= pairs; ++i)
			{
				const double theta = doublePi * (0.5 - (2 * i - 1) / n2);
				//Changed this from (gp/gz) to (fabs(gp/gz)) because having positive values for gp and gz was theoretically impossible
				auto pole =std::polar ((-gp), theta + doublePi);
				auto zero =std::polar ((-gz), theta + doublePi);
				addPoleZeroConjugatePairs (pole, zero);
			}
			
			if (numPoles & 1)
				add (gp, gz);
		}
	}
}




complex_t Cascade::response (double normalizedFrequency) const
{
  double w = 2 * doublePi * normalizedFrequency;
  const complex_t czn1 = std::polar (1., -w);
  const complex_t czn2 = std::polar (1., -2 * w);
  complex_t ch (1);
  complex_t cbot (1);

  const Biquad* stage = m_stages;
  for (int i = m_numStages; --i >=0; ++stage)
  {
	complex_t cb (1);
	complex_t ct    (stage->getB0()/stage->getA0());
	ct = addmul (ct, stage->getB1()/stage->getA0(), czn1);
	ct = addmul (ct, stage->getB2()/stage->getA0(), czn2);
	cb = addmul (cb, stage->getA1()/stage->getA0(), czn1);
	cb = addmul (cb, stage->getA2()/stage->getA0(), czn2);
	ch   *= ct;
	cbot *= cb;
  }

  return ch / cbot;
}

void Cascade::applyScale (double scale)
{
	// TODO: From original: For higher order filters it might be helpful
	// to spread this factor between all the stages.
	jassert (m_numStages > 0);
//	for (int n=0; n < m_numStages; ++n)
	
	m_stages[0].applyScale (scale);
}


void Cascade::setLayout (const LayoutBase& proto)
{
  const int numPoles = proto.getNumPoles();
  m_numStages = (numPoles + 1)/ 2;
  jassert (m_numStages <= digitalStages);

//  Biquad* stage = m_stages;
//	auto* m_stages =
	for (int i = 0; i < m_numStages; ++i) {
		m_stages[i].setPoleZeroPair (proto[i]);
	}

  applyScale (proto.getNormalGain() /
			  std::abs (response (proto.getNormalW() / (2 * doublePi))));
}

void Cascade::setCascadeStorage (const Storage& storage)
{
	m_numStages = 0;
	digitalStages = storage.maxStages;
	m_stages = storage.stageArray;
}

void Cascade::calc_stages (int order,
						   double sampleRate,
						   double cutoffFrequency,
						   double widthFrequency,
						   double gainDb)
{
	LayoutBase m_digitalProto(order);
	AnalogPrototype m_analogProto(type, order);
	
	using namespace Filter;
	
	//Routes based on isShelf in analog proto
	m_analogProto.design (order, gainDb);
	
	if (type == LowPass)
	{
		LowPassTransform (cutoffFrequency / sampleRate,
						  m_digitalProto,
						  m_analogProto);
	}
	else if (type == HighPass)
	{
		HighPassTransform (cutoffFrequency / sampleRate,
						   m_digitalProto,
						   m_analogProto);
	}
	else if (type == BandPass)
	{
		BandPassTransform (cutoffFrequency / sampleRate,
						   widthFrequency / sampleRate,
						   m_digitalProto,
						   m_analogProto);
	}
	else if (type == BandStop)
	{
		BandStopTransform (cutoffFrequency / sampleRate,
						   widthFrequency / sampleRate,
						   m_digitalProto,
						   m_analogProto);
	}
	else if (type == LowShelf)
	{
		LowPassTransform (cutoffFrequency / sampleRate,
						  m_digitalProto,
						  m_analogProto);
	}
	else if (type == HighShelf)
	{
		HighPassTransform (cutoffFrequency / sampleRate,
						   m_digitalProto,
						   m_analogProto);
	}
	else if (type == BandShelf)
	{
		BandPassTransform (cutoffFrequency / sampleRate,
						   widthFrequency / sampleRate,
						   m_digitalProto,
						   m_analogProto);
		// HACK!
		m_digitalProto.setNormal (((cutoffFrequency/sampleRate) < 0.25) ? doublePi : 0, 1);
	}
	
	setLayout (m_digitalProto);
}

}}
