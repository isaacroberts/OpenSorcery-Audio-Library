//
//  SmoothCascade.cpp
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/24/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

// Created by Isaac Roberts

#include "SmoothCascade.h"

namespace Filter {
namespace Butter {
	
void SmoothCascade::incrementFreq(int frames, bool& anyStillSmoothing)
{
	int n = Freq_;
	
	if (m_params[n] == target_params[n])
		return;
	
	if (m_params[n] < .0001)
		m_params[n] = .0001;
	
	float log2m = log2(m_params[n]);
	float log2t = log2(target_params[n]);

	if (target_params[n] < .001)
	{
		log2t = -10;//fairly close to log2(.001)
	}
	
	if (m_params[n] < target_params[n])
	{
		log2m += smoothSpeed[n] / m_params[Rate_] * frames;
		
		if (log2m >= log2t)
		{
			m_params[n] = target_params[n];
			return;
		}
		else
			anyStillSmoothing=true;
	}
	else if (m_params[n] > target_params[n])
	{
		log2m -= smoothSpeed[n] / m_params[Rate_] * frames;
		
		if (log2m <= log2t)
		{
			m_params[n] = target_params[n];
			return;
		}
		else
			anyStillSmoothing=true;
	}
	m_params[n] = pow(2, log2m);
}
void SmoothCascade::incrementParams(int frames)
{
	if (isSmoothing)
	{
		bool anyStillSmoothing=false;
		
		incrementFreq(frames, anyStillSmoothing);
		
		if (!(type == LowPass || type == HighPass))
		{
			for (int n=Bandwidth_; n < Params::maxParameters; ++n)
			{
				if (usesParam(n))
				{
					//Equal
					if (smoothSpeed[n] == 0)
					{
						m_params[n] = target_params[n];
					}
					//Less than
					else if (m_params[n] < target_params[n])
					{
						m_params[n] += smoothSpeed[n] / m_params[Rate_] * frames;
						
						if (m_params[n] >= target_params[n])
							m_params[n] = target_params[n];
						else
							anyStillSmoothing=true;
					}
					//Greater
					else if (m_params[n] > target_params[n])
					{
						m_params[n] -= smoothSpeed[n] / m_params[Rate_] * frames;
						
						if (m_params[n] <= target_params[n])
							m_params[n] = target_params[n];
						else
							anyStillSmoothing=true;
					}
				}
			}
		}
		isSmoothing = anyStillSmoothing;
		updateParams();
	}
}




complex_t SmoothCascade::response (double normalizedFrequency) const
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

void SmoothCascade::applyScale (double scale)
{
  // TODO: From original: For higher order filters it might be helpful
  // to spread this factor between all the stages.
  jassert (m_numStages > 0);
	
  m_stages[0].applyScale (scale);
}


void SmoothCascade::setLayout (const LayoutBase& proto)
{
  const int numPoles = proto.getNumPoles();
  m_numStages = (numPoles + 1)/ 2;
  jassert (m_numStages <= digitalStages);

//  Biquad* stage = m_stages;
	for (int i = 0; i < m_numStages; ++i) {
		m_stages[i].setPoleZeroPair (proto[i]);
	}

  applyScale (proto.getNormalGain() /
			  std::abs (response (proto.getNormalW() / (2 * doublePi))));
}

void SmoothCascade::setCascadeStorage (const Storage& storage)
{
	m_numStages = 0;
	digitalStages = storage.maxStages;
	m_stages = storage.stageArray;
}

void SmoothCascade::calc_stages (int order,
						   double sampleRate,
						   double cutoffFrequency,
						   double widthFrequency,
						   double gainDb)
{
	
	using namespace Filter;
	
	//Routes based on isShelf in analog proto
	m_analogProto.design (order, gainDb);
	m_digitalProto.reset();
	
	
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
