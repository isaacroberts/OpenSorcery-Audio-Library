//
//  Coeff.cpp
//  
//
//  Created by Isaac Roberts on 2/23/21.
//

// Created by Isaac Roberts


#include "VarType.h"

namespace RBJ {
	
void VarType::getCoeff(Coeff& coeff)
{
	if (type==LowPass_T)
		return rbjLowpassCoeff(coeff);
	else if (type==HighPass_T)
		return rbjHighpassCoeff(coeff);
	else if (type==BandPass1_T)
		return rbjBp1Coeff(coeff);
	else if (type==BandPass2_T)
		return rbjBp2Coeff(coeff);
	else if (type==Notch_T)
		return rbjNotchCoeff(coeff);
	else if (type==AllPass_T)
		return rbjApCoeff(coeff);
	else if (type==PeakingEQ_T)
		return rbjPeakCoeff(coeff);
	else if (type==LowShelf_T)
		return rbjLowShelfCoeff(coeff);
	else if (type==HighShelf_T)
		return rbjHighShelfCoeff(coeff);
}

double VarType::alpha(double omega)
{
	if (usesQ())
	{
		jassert(qbws > 0);
		return sin(omega) / (2* qbws);
	}
	else if (usesBW())
	{
		// ln(2) / 2
		const double ln2o2 = 0.34657359028;
		double sino = sin(omega);
		double sh = sinh(ln2o2 * qbws * omega / sino);
		//TODO: Bounds check sh
//		assert (sh < 1000);
//		assert (sh > -1);
		double val = sino * sh;
		assert (val > -1);
		return val;
	}
	else// if (usesS())
	{
		//TODO: Bounds check gain and qbws
		double A = pow(10, gain/40.);
		double sterm = (A + (1./A)) * (1./qbws - 1.) + 2.;
		return sin(omega) / 2 * sqrt(sterm);
	}
}

/**
 file:///Users/isaacroberts/Dropbox/Scratch/Audio%20Education/Audio-EQ-Cookbook-master/audio-eq-cookbook.html#mjx-eqn-direct-form-1
 */
void VarType::rbjLowpassCoeff(Coeff& coeff)
{
	float om = omega();
	float al = alpha(om);
	float cosom = cos(om);
	float a0 = 1+al;
	
	coeff[B0] = (1-cosom)/2;
	coeff[B1] = 1-cosom;
	coeff[B2] = coeff[B0];
	coeff[A1] = -2 * cosom;
	coeff[A2] = 1-al;
	
	for (int n=0; n < coeffAmt; ++n)
		coeff[n] /= a0;
}

void VarType::rbjHighpassCoeff(Coeff& coeff)
{
	float om = omega();
	float al = alpha(om);
	float cosom = cos(om);
	
	float a0 = 1+al;
	
	coeff[B0] = (1+cosom)/2;
	coeff[B1] = -(1+cosom);
	coeff[B2] = coeff[B0];
	coeff[A1] = -2 * cosom;
	coeff[A2] = 1-al;
	
	for (int n=0; n < coeffAmt; ++n)
		coeff[n] /= a0;
}

void VarType::rbjBp1Coeff(Coeff& coeff)
{
	float om = omega();
	float al = alpha(om);

	float sinOm = sin(om);
	float a0 = 1 + al;
	
	coeff[B0] = sinOm/2;
	coeff[B1] = 0;
	coeff[B2] = -sinOm/2;
	coeff[A1] = -2 * cos(om);
	coeff[A2] = 1 - al;
	
	for (int n=0; n < coeffAmt; ++n)
		coeff[n] /= a0;
}
void VarType::rbjBp2Coeff(Coeff& coeff)
{
	float om = omega();
	float al = alpha(om);
	float a0 = 1 +al;
	
	coeff[B0] = al;
	coeff[B1] = 0;
	coeff[B2] = -al;
	coeff[A1] = -2 * cos(om);
	coeff[A2] = 1 - al;
	
	for (int n=0; n < coeffAmt; ++n)
		coeff[n] /= a0;
}
void VarType::rbjNotchCoeff(Coeff& coeff)
{
	jassertfalse;
}
void VarType::rbjApCoeff(Coeff& coeff)
{
	jassertfalse;
}
void VarType::rbjPeakCoeff(Coeff& coeff)
{
	float om = omega();
	float al = alpha(om);
	float A = pow(10, gain/20);
	
	float alA = al*A;
	float aloA = al/A;
	float a0 = 1 + aloA;
	float cosom = cos(om);
	
	coeff[B0] = 1 + alA;
	coeff[B1] = -2 * cosom;
	coeff[B2] = 1 - alA;
	coeff[A1] = -2 * cosom;
	coeff[A2] = 1 - aloA;
	
	for (int n=0; n < coeffAmt; ++n)
		coeff[n] /= a0;
}
void VarType::rbjLowShelfCoeff(Coeff& coeff)
{
	double sqrtA = pow(10, gain/80.);
	double A = sqrtA*sqrtA;
	float om = omega();
	float al = alpha(om);
	float cosom = cos(om);
	float nca1 = (A+1) - (A-1) * cosom;
	float pca1 = (A-1) - (A+1) * cosom;
	float a2al = 2 * sqrtA * al;
//
	coeff[B0] = A*(nca1 + a2al);
	coeff[B1] = 2*A * pca1;
	coeff[B2] = A*(nca1 - a2al);
	
	float nca2 = (A+1) + (A-1) * cosom;
	float pca2 = (A-1) + (A+1) * cosom;
	
	float a0  = nca2 + a2al;
	coeff[A1] = -2 * pca2;
	coeff[A2] = nca2 - a2al;
	
	for (int n=0; n < coeffAmt; ++n)
		coeff[n] /= a0;
}
void VarType::rbjHighShelfCoeff(Coeff& coeff)
{
	double sqrtA = pow(10, gain/80.);
	double A = sqrtA*sqrtA;
	float om = omega();
	float al = alpha(om);
	float cosom = cos(om);
	float nca1 = (A+1) - (A-1) * cosom;
	float pca1 = (A-1) - (A+1) * cosom;
	
	float nca2 = (A+1) + (A-1) * cosom;
	float pca2 = (A-1) + (A+1) * cosom;
	
	float a2al = 2 * sqrtA * al;
	
	coeff[B0] = A*(nca2 + a2al);
	coeff[B1] = -2*A * pca2;
	coeff[B2] = A*(nca2 - a2al);
	float a0  = nca1 + a2al;
	coeff[A1] = 2 * pca1;
	coeff[A2] = nca1 - a2al;
	

	for (int n=0; n < coeffAmt; ++n)
		coeff[n] /= a0;
}


}
