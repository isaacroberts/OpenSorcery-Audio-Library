//
//  GenericBiquad.cpp
//  
//
//  Created by Isaac Roberts on 4/19/21.
//

#include <stdio.h>

#include "GenericBiquad.h"

/*
	 Obtained from https://github.com/dimtass/DSP-Cpp-filters
	 No license listed which means this is technically copyright infringement
*/

namespace Filter {

	
	
	void GenericBiquad::make_2nd_Order_LR_HiPass(float fc, float fs, Coeffs& m_coeffs)
	{
		//fc = center freq
		//fs = sample rate
		float th = M_PI * fc / fs;
		float Wc = M_PI * fc;
		float k = Wc / tan(th);
		
		float d = pow(k, 2.0) + pow(Wc, 2.0) + 2.0 * k * Wc;
		m_coeffs.a0 = pow(k, 2.0) / d;
		m_coeffs.a1 = -2.0 * pow(k, 2.0) / d;
		m_coeffs.a2 = m_coeffs.a0;
		m_coeffs.b1 = (-2.0 * pow(k, 2.0) + 2.0 * pow(Wc, 2.0)) / d;
		m_coeffs.b2 = (-2.0 * k * Wc + pow(k, 2.0) + pow(Wc, 2.0)) / d;
	}
	void GenericBiquad::make_2nd_Order_LR_LoPass(float fc, float fs, Coeffs& m_coeffs)
	{
		//fc = center freq
		//fs = sample rate
		float th = M_PI * fc / fs;
		float Wc = M_PI * fc;
		float k = Wc / tan(th);
		
		float d = pow(k, 2.0) + pow(Wc, 2.0) + 2.0 * k * Wc;
		m_coeffs.a0 = pow(Wc, 2.0) / d;
		m_coeffs.a1 = 2.0 * pow(Wc, 2.0) / d;
		m_coeffs.a2 = m_coeffs.a0;
		m_coeffs.b1 = (-2.0 * pow(k, 2.0) + 2.0 * pow(Wc, 2.0)) / d;
		m_coeffs.b2 = (-2.0 * k * Wc + pow(k, 2.0) + pow(Wc, 2.0)) / d;
		
	}


}
