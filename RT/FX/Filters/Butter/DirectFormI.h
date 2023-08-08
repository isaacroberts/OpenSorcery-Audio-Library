//
//  DirectFormI.h
//  MagicVocals - All
//
//  Created by Isaac Roberts on 3/23/21.
//  Copyright © 2021 Chrys Gringo. All rights reserved.
//

#ifndef DirectFormI_h
#define DirectFormI_h

#include "MathSupplement.h"
//#include "Stage.h"
#include "Biquad.h"
//class Biquad;

namespace Filter {
namespace Butter {
	
class DirectFormI
{
public:
	DirectFormI ()
	{
		reset();
	}
	
	void reset ()
	{
		m_x1 = 0;
		m_x2 = 0;
		m_y1 = 0;
		m_y2 = 0;
	}
	
#define maxBds1 4
	
	void cry(float x)
	{
//		int x2 = x;
	}
	
	inline void bound(double& v)
	{
		if (isnan(v))
			v = 0;
		else if (v > maxBds1)
			v = maxBds1;
		else if (v < -maxBds1)
			v = -maxBds1;
	}
	
	void boundsCheck()
	{
		bound(m_x2);
		bound(m_y2);
		bound(m_x1);
		bound(m_y1);
	}
	
	inline float process1 (const float in,
					const Biquad& s,
					const double vsa) // very small amount
	
	{
		double out = s.m_b0*in + s.m_b1*m_x1 + s.m_b2*m_x2
		- s.m_a1*m_y1 - s.m_a2*m_y2
		+ vsa;
		m_x2 = m_x1;
		m_y2 = m_y1;
		m_x1 = in;
		m_y1 = out;
		
//		boundsCheck();
		
		return float(out);
	}
	
	inline double process1 (const double in,
					 const Biquad& s,
					 const double vsa)
	{
		double out = s.m_b0*in + s.m_b1*m_x1 + s.m_b2*m_x2
		- s.m_a1*m_y1 - s.m_a2*m_y2
		+ vsa;
		m_x2 = m_x1;
		m_y2 = m_y1;
		m_x1 = in;
		m_y1 = out;
		
//		boundsCheck();
		
		return (out);
	}

	
protected:
	double m_x2; // x[n-2]
	double m_y2; // y[n-2]
	double m_x1; // x[n-1]
	double m_y1; // y[n-1]
};

}}
#endif /* DirectFormI_h */
