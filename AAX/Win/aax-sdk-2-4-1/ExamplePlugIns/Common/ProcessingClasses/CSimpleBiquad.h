/*================================================================================================*/
/*
 *	Copyright 2008-2015, 2019 by Avid Technology, Inc.
 *	All rights reserved.
 *
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 */

/**  
 *	\file   CSimpleBiquad.h
 *
 *	\brief  This class implements a simple biquad filter with direct access to coefficients:
 *          y(n) = (a0) * ( b0*x(n) + b1*x(n-1) + b2*x(n-2) - a1*y(n-1) - a2*y(n-2) )
 *
 *  \author Rob Majors
 */ 
/*================================================================================================*/

#ifndef CSIMPLEBIQUAD_H
#define CSIMPLEBIQUAD_H

#include "AAX.h"

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_ALG
#include AAX_ALIGN_FILE_END

// NOTE: Classes and structures that inherit from base structures or contain
// virtual functions should not be passed between the host and the algorithm
// in AAX DSP plug-ins. For more information, see the AAX_ALIGN_FILE_ALG macro
// documentation.
class CSimpleBiquad
{
public:
	enum EBiquadCoefs
	{
		eA0 = 0,
		eA1 = 1,
		eA2 = 2,
		eB0 = 3,
		eB1 = 4,
		eB2 = 5,
		eNumBiquadCoefs
	};
	
	CSimpleBiquad(float fb1, float fb2, float fb3, float ff1, float ff2, float ff3)
	{
		SetCoeffs(fb1, fb2, fb3, ff1, ff2, ff3);
		ResetFilterData();
	}
	CSimpleBiquad(void)
	{
		CSimpleBiquad::ResetFilter();
	}

	// Do filtering:
	// y(n) = (a0) * ( b0*x(n) + b1*x(n-1) + b2*x(n-2) - a1*y(n-1) - a2*y(n-2) )
	float DoFilt(float inputSample)
	{
		PushSample(inputSample);
		return float(mY[0]);
	}
	void PushSample(float inputSample)
	{
		mX[2] = mX[1];
		mX[1] = mX[0];
		mX[0] = inputSample;

		mY[2] = mY[1];
		mY[1] = mY[0];
		mY[0] = (float) (
			   (mCf[eA0]) * ( mCf[eB0]*mX[0] + mCf[eB1]*mX[1] + mCf[eB2]*mX[2]
			                                 - mCf[eA1]*mY[1] - mCf[eA2]*mY[2] ));
	}
	void GetSamples(float inp[3], float outp[3]) const
	{
		inp[0] = mX[0]; 
		inp[1] = mX[1];
		inp[2] = mX[2];

		outp[0] = mY[0];
		outp[1] = mY[1];
		outp[2] = mY[2];
	}

	// Remember not to give fb1 = 0
	void SetCoeffs(float fb1, float fb2, float fb3, float ff1, float ff2, float ff3)
	{
		mCf[eA0] = fb1 == 0.0f ? 0.0f : fb1; 
		mCf[eA1] = fb2;
		mCf[eA2] = fb3;

		mCf[eB0] = ff1;
		mCf[eB1] = ff2;
		mCf[eB2] = ff3;
	}
	void GetCoeffs(float *fb1, float *fb2, float *fb3, float *ff1, float *ff2, float *ff3) const
	{
		*fb1 = mCf[eA0];
		*fb2 = mCf[eA1];
		*fb3 = mCf[eA2];

		*ff1 = mCf[eB0];
		*ff2 = mCf[eB1];
		*ff3 = mCf[eB2];
	}
	void SetCoeffsArray(const float* const inCoefs)
	{
		SetCoeffs(inCoefs[eA0], inCoefs[eA1], inCoefs[eA2], inCoefs[eB0], inCoefs[eB1], inCoefs[eB2]);
	}
	void GetCoeffsArray(float* outCoefs) const
	{
		GetCoeffs(&outCoefs[eA0], &outCoefs[eA1], &outCoefs[eA2], &outCoefs[eB0], &outCoefs[eB1], &outCoefs[eB2]);
	}

	void ResetFilterData()
	{
		for (int i = 0; i<3; i++)
		{
			mX[i] = 0.0f;
			mY[i] = 0.0f;
		}
	}
	void ResetFilter()
	{
		this->SetCoeffs(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		ResetFilterData();
	}



	// This function is used in DemoDist to calculate a set of 
	// simple lowpass filter coefficients. It's pretty arbitrary.
	// It doesn't even account for sample rate.
	void SetCoeffsForLPFUsingCutoff(float cutoff)
	{
		mCf[eA0] = 0.980392f;
		mCf[eA1] = -cutoff + 0.00001f;
		mCf[eA2] = 0.0f;

		mCf[eB0] = 0.0001f;
		mCf[eB1] = 0.55f - cutoff;
		mCf[eB2] = 0.0f;
	}

private:
	float mCf[eNumBiquadCoefs];

	float mY[3];
	float mX[3];
};

#include AAX_ALIGN_FILE_BEGIN
#include AAX_ALIGN_FILE_RESET
#include AAX_ALIGN_FILE_END

#endif
