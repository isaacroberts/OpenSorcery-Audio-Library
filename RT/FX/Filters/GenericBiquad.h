//
//  GenericBiquad.h
//  
//
//  Created by Isaac Roberts on 4/19/21.
//

// Created by Isaac Roberts 

#pragma once


#include <math.h>

using namespace juce;

/*
	Obtained from https://github.com/dimtass/DSP-Cpp-filters
	No license listed which means this is technically copyright infringement
*/
	
//typedef coef_size_t float;

//#include ""

#include "FilterDefinitions.h"
//#include "../../RTTypedef.h"

namespace Filter {
	
	
class GenericBiquad {
public:
	struct Coeffs
	{
		float a0, a1, a2;
		float b1, b2;
		Coeffs() : a0(1), a1(0), a2(0), b1(0), b2(0)//, c0(0), d0(0)
		{}
		static Coeffs silence() {
			Coeffs c;
			c.a0=0;
			return c;
		}
		static Coeffs bypass() {
			Coeffs c;
			return c;
		}
	};

	struct State
	{
		float m_xnz1, m_xnz2, m_ynz1, m_ynz2;
		State() : m_xnz1(0), m_xnz2(0), m_ynz1(0), m_ynz2(0)
		{}
	};
	
	
	GenericBiquad() : m_coeffs(), s(NULL), maxC(0) // ,m_offset(0)
	{ }
	
	virtual ~GenericBiquad()
	{
		delete[] s;
	}
	
	
	void prepare(int numC) {
		jassert(s==NULL);
		maxC = numC;
		s = new State[maxC];
	}
	
	void setCoeffs(const Coeffs& set) {
		m_coeffs = set;
	}
	inline float process(int c, float xn)
	{
		float yn = m_coeffs.a0 *     xn
				 + m_coeffs.a1 * s[c].m_xnz1
				 + m_coeffs.a2 * s[c].m_xnz2
				 - m_coeffs.b1 * s[c].m_ynz1
				 - m_coeffs.b2 * s[c].m_ynz2;
		
		s[c].m_xnz2 = s[c].m_xnz1;
		s[c].m_xnz1 = xn;
		s[c].m_ynz2 = s[c].m_ynz1;
		s[c].m_ynz1 = yn;
		return yn;
	}

	void process(int c, float* smp, int numSmp)
	{
		float yn, xn;
		
		for (int n=0; n < numSmp; ++n)
		{
			xn = smp[n];
			yn = m_coeffs.a0 *     xn
					 + m_coeffs.a1 * s[c].m_xnz1
					 + m_coeffs.a2 * s[c].m_xnz2
					 - m_coeffs.b1 * s[c].m_ynz1
					 - m_coeffs.b2 * s[c].m_ynz2;
			
			s[c].m_xnz2 = s[c].m_xnz1;
			s[c].m_xnz1 = xn;
			s[c].m_ynz2 = s[c].m_ynz1;
			s[c].m_ynz1 = yn;
			smp[n] = yn;
		}
	}
	void process(int c, const float* src, float* dst, int numSmp)
	{
		float yn, xn;
		
		for (int n=0; n < numSmp; ++n)
		{
			xn = src[n];
			yn = m_coeffs.a0 *     xn
					 + m_coeffs.a1 * s[c].m_xnz1
					 + m_coeffs.a2 * s[c].m_xnz2
					 - m_coeffs.b1 * s[c].m_ynz1
					 - m_coeffs.b2 * s[c].m_ynz2;
			
			s[c].m_xnz2 = s[c].m_xnz1;
			s[c].m_xnz1 = xn;
			s[c].m_ynz2 = s[c].m_ynz1;
			s[c].m_ynz1 = yn;
			dst[n] = yn;
		}
	}
	
	void process(InputBlock& input, OutputBlock& output)
	{
		jassert(input.getNumChannels() == maxC);
		float yn, xn;
		const size_t N = input.getNumSamples();
		for (int c=0; c < maxC; ++c)
		{
			const float* src = input[c];
			float* dst = output[c];
			for (int n=0; n < N; ++n)
			{
				xn = src[n];
				yn = m_coeffs.a0 *     xn
						 + m_coeffs.a1 * s[c].m_xnz1
						 + m_coeffs.a2 * s[c].m_xnz2
						 - m_coeffs.b1 * s[c].m_ynz1
						 - m_coeffs.b2 * s[c].m_ynz2;
				
				s[c].m_xnz2 = s[c].m_xnz1;
				s[c].m_xnz1 = xn;
				s[c].m_ynz2 = s[c].m_ynz1;
				s[c].m_ynz1 = yn;
				dst[n] = yn;
			}
		}
	}
	void process(OutputBlock& samples)
	{
		jassert(samples.getNumChannels() == maxC);
		float yn, xn;
		const size_t N = samples.getNumSamples();
		for (int c=0; c < maxC; ++c)
		{
			float* smp = samples[c];
			for (int n=0; n < N; ++n)
			{
				xn = smp[n];
				yn = m_coeffs.a0 *     xn
						 + m_coeffs.a1 * s[c].m_xnz1
						 + m_coeffs.a2 * s[c].m_xnz2
						 - m_coeffs.b1 * s[c].m_ynz1
						 - m_coeffs.b2 * s[c].m_ynz2;
				
				s[c].m_xnz2 = s[c].m_xnz1;
				s[c].m_xnz1 = xn;
				s[c].m_ynz2 = s[c].m_ynz1;
				s[c].m_ynz1 = yn;
				smp[n] = yn;
			}
		}
	}
	
	inline void skipSample(int c)
	{//xn = 0
		float yn = m_coeffs.a1 * s[c].m_xnz1
				 + m_coeffs.a2 * s[c].m_xnz2
				 - m_coeffs.b1 * s[c].m_ynz1
				 - m_coeffs.b2 * s[c].m_ynz2;
		
		s[c].m_xnz2 = s[c].m_xnz1;
		s[c].m_xnz1 = 0;
		s[c].m_ynz2 = s[c].m_ynz1;
		s[c].m_ynz1 = yn;
	}
	void skipProcess(int numSamples)
	{//xn = 0
		float yn;
		for (int c=0; c < maxC; ++c)
		{
			//TODO: Just set these to 0 if more than n sampels
			for (int n=0; n < numSamples; ++n)
			{
				yn = m_coeffs.a1 * s[c].m_xnz1
						 + m_coeffs.a2 * s[c].m_xnz2
						 - m_coeffs.b1 * s[c].m_ynz1
						 - m_coeffs.b2 * s[c].m_ynz2;
				
				s[c].m_xnz2 = s[c].m_xnz1;
				s[c].m_xnz1 = 0;
				s[c].m_ynz2 = s[c].m_ynz1;
				s[c].m_ynz1 = yn;
			}
		}
	}
	void reset() {
		for (int c=0; c < maxC; ++c)
		{
			s[c].m_xnz1=0;
			s[c].m_xnz2=0;
			s[c].m_ynz1=0;
			s[c].m_ynz2=0;
		}
	}
	void sanCheck() {
		for (int c=0; c < maxC; ++c)
		{
			san(s[c].m_xnz1);
			san(s[c].m_xnz2);
			san(s[c].m_ynz1);
			san(s[c].m_ynz2);
		}
	}
private:
	inline void san(float& x)
	{
		if (isnan(x)) x = 0;
		else if (x > 1000) x = 1000;
		else if ( x < -1000) x = -1000;
	}
	
protected:
	Coeffs m_coeffs;
	State *s;
	int maxC;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenericBiquad);

public:
	static void make_2nd_Order_LR_HiPass(float freq, float rate, Coeffs& c);
	static void make_2nd_Order_LR_LoPass(float freq, float rate, Coeffs& c);
	
	
private:
	//modified process method
	//this is probably the other DirectForm
	/*
	float process(float sample)
	{
		float xn = sample;
	 
		//Last term is different
		float ynn = m_coeffs.a0*xn + m_coeffs.a1*m_xnz1 + m_coeffs.a2*m_xnz2
		- m_coeffs.b1*m_ynz1 - m_coeffs.b2*m_xnz2;
		//This line is added from other
		float yn = m_coeffs.d0*xn + m_coeffs.c0*ynn;
		
		m_xnz2 = m_xnz1;
		m_xnz1 = xn;
		m_ynz2 = m_ynz1;
		m_ynz1 = yn;
		return(yn);
	}
	*/
	
};
	
}//Namespace
