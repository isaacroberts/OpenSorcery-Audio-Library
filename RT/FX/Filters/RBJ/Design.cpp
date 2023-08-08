
#include "Design.h"



/*
 usesBW () { return type == BandPass || type == BandPass2 || type == Notch || type == PeakingEQ; }
 usesS () { return type == LowShelf || type == HighShelf; }
 usesQ () { return type == LowPass || type == HighPass || type == AllPass; }
 
 
 double RTRBJ::alpha(double omega)
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
 */

/*
 file:///Users/isaacroberts/Dropbox/Scratch/Audio%20Education/Audio-EQ-Cookbook-master/audio-eq-cookbook.html#mjx-eqn-direct-form-1
 */

namespace RBJ
{
	
	// Q Alpha
	template <bool Low>
	void LH_Pass<Low>::setAlpha()
	{
		alpha = sin(omega) / (2* q);
	}
	void AllPass::setAlpha()
	{
		alpha = sin(omega) / (2* q);
	}
	
	//BW Alpha
	
	const double ln2o2 = 0.34657359028;
	
	template <bool ConstantPeak>
	void BandPass<ConstantPeak>::setAlpha()
	{
		double sh = sinh(ln2o2 * bw * omega / sinom);
		alpha = sinom * sh;
	}
	void BandStop::setAlpha()
	{
//		double sinom = sin(omega);
		double sh = sinh(ln2o2 * bw * omega / sinom);
		alpha = sinom * sh;
	}
	void PeakingEQ::setAlpha()
	{
		double sino = sin(omega);
		double sh = sinh(ln2o2 * bw * omega / sino);
		alpha = sino * sh;
	}
	
	
	// S Alpha
	template <bool Low>
	void Shelf<Low>::setAlpha()
	{
		double sterm = (A + (1./A)) * (1./s - 1.) + 2.;
		alpha= sin(omega) / 2 * sqrt(sterm);
	}
	
	// Omega
	
	template <bool Low>
	void LH_Pass<Low>::setOmega() {
		omega= 2 * M_PI * freq / rate;
		cosom = cos(omega);
	}
	void AllPass::setOmega() {
		omega= 2 * M_PI * freq / rate;
		cosom = cos(omega);
	}
	template <bool ConstantPeak>
	void BandPass<ConstantPeak>::setOmega() {
		omega= 2 * M_PI * freq / rate;
		cosom = cos(omega);
		sinom = sin(omega);
	}
	void BandStop::setOmega() {
		omega= 2 * M_PI * freq / rate;
		cosom = cos(omega);
		sinom = sin(omega);
	}

	void PeakingEQ::setOmega() {
		omega= 2 * M_PI * freq / rate;
		cosom = cos(omega);
	}
	template <bool Low>
	void Shelf<Low>::setOmega() {
		omega= 2 * M_PI * freq / rate;
		cosom = cos(omega);
	}
	
	/*
	 if (freq > rate/2)
	 {
		 if (type == LowPass_T)
			 status = Bypass;
		 else if (type == HighPass_T)
			 status = Silence;
		 else {
			 jassert(false);
			 status = Bypass;
		 }
		 return false;
	 }
	 else if (freq <= 0)
	 {
		 if (type == LowPass_T)
			 status = Silence;
		 else if (type == HighPass_T)
			 status = Bypass;
		 else {
			 jassert(false);
			 status = Bypass;
		 }
		 return false;
	 }
	 else {
		 status = Active;
		 return true;
	 }
	 */
	
	bool handle(Status stat, Coeff& coeff)  {
		coeff.status = stat;
		if (stat==Error)
		{
			jassertfalse;
			coeff.silence();
			return true;
		}
		if (stat==Active)
			return false;
		if (stat==Bypass)
			coeff.bypass();
		if (stat==Silence)
			coeff.silence();
		return true;
	}
	
	//TODO: Some of these can be sped up by integrating a0
	
#define endStep coeff.a0 = a0; \
	shouldUpdate=false;
	
	template <>
	void LH_Pass<true>::make(Coeff& coeff)
	{
		if (handle(getStatus(), coeff))
			return;
		
		double a0 = 1+alpha;
		
		double cs1 = 1-cosom;
		coeff[B0] = cs1/2;
		coeff[B1] = cs1;
		coeff[B2] = coeff[B0];
		coeff[A1] = -2 * cosom;
		coeff[A2] = 1-alpha;
		
		for (int n=0; n < coeffAmt; ++n)
			coeff[n] /= a0;
		
		endStep;
	}
	template <>
	void LH_Pass<false>::make(Coeff& coeff)
	{
		if (handle(getStatus(), coeff))
			return;
		
		double a0 = 1+alpha;
		
		double cs1 = 1+cosom;
		coeff[B0] = cs1/2;
		coeff[B1] = -cs1;
		coeff[B2] = coeff[B0];
		coeff[A1] = -2 * cosom;
		coeff[A2] = 1-alpha;
		
		for (int n=0; n < coeffAmt; ++n)
			coeff[n] /= a0;
		
		endStep;
	}
	
	template <>
	void BandPass<true>::make(Coeff& coeff)
	{
		if (handle(getStatus(), coeff))
			return;

		double a0 = 1 + alpha;
		coeff[B0] = sinom/2;
		coeff[B1] = 0;
		coeff[B2] = -coeff[B0];
		coeff[A1] = -2 * cosom;
		coeff[A2] = 1 - alpha;
	
		for (int n=0; n < coeffAmt; ++n)
			coeff[n] /= a0;
		
		endStep;
	}
	template <>
	void BandPass<false>::make(Coeff& coeff)
	{
		if (handle(getStatus(), coeff))
			return;

		double a0 = 1 +alpha;
		coeff[B0] = alpha;
		coeff[B1] = 0;
		coeff[B2] = -alpha;
		coeff[A1] = -2 * cosom;
		coeff[A2] = 1 - alpha;
	
		for (int n=0; n < coeffAmt; ++n)
			coeff[n] /= a0;
		
		endStep;
	}
	void BandStop::make(Coeff& coeff)
	{
		if (handle(getStatus(), coeff))
			return;

		coeff[B0] = 1;
		coeff[B1] = -2 * cosom;
		coeff[B2] = 1;
		const double a0 = 1+alpha;
		coeff[A1] = coeff[B1];
		coeff[A2] = 1-alpha;
		
		for (int n=0; n < coeffAmt; ++n)
			coeff[n] /= a0;
		
		endStep;
	}
	void PeakingEQ::make(Coeff& coeff)
	{
		if (handle(getStatus(), coeff))
			return;
		
		const double alA = alpha*A;
		const double aloA = alpha/A;
		
		const double a0 = 1 + aloA;
		
		coeff[B0] = 1 + alA;
		coeff[B1] = -2 * cosom;
		coeff[B2] = 1 - alA;
		coeff[A1] = coeff[B1];
		coeff[A2] = 1 - aloA;
		
		for (int n=0; n < coeffAmt; ++n)
			coeff[n] /= a0;
		
		endStep;
	}
	template <>
	void Shelf<true>::make(Coeff& coeff)
	{
		if (handle(getStatus(), coeff))
			return;
		
		const double Ap1=A+1;
		const double Am1=A-1;
		const double nca1 = (Ap1) - (Am1) * cosom;
		const double pca1 = (Am1) - (Ap1) * cosom;
		const double a2al = 2 * sqrtA * alpha;
		
		coeff[B0] = A*(nca1 + a2al);
		coeff[B1] = 2*A * pca1;
		coeff[B2] = A*(nca1 - a2al);
		
		const double nca2 = (Ap1) + (Am1) * cosom;
		const double pca2 = (Am1) + (Ap1) * cosom;
		
		const double a0  = nca2 + a2al;
		coeff[A1] = -2 * pca2;
		coeff[A2] = nca2 - a2al;
		
		for (int n=0; n < coeffAmt; ++n)
			coeff[n] /= a0;
		
		endStep;
	}
	template <>
	void Shelf<false>::make(Coeff& coeff)
	{
		if (handle(getStatus(), coeff))
			return;
		
		const double a2al = 2 * sqrtA * alpha;
		
		const double Ap1=A+1;
		const double Am1=A-1;
		
		const double nca2 = (Ap1) + (Am1) * cosom;
		const double pca2 = (Am1) + (Ap1) * cosom;
		
		coeff[B0] = A*(nca2 + a2al);
		coeff[B1] = -2*A * pca2;
		coeff[B2] = A*(nca2 - a2al);
		
		const double nca1 = (Ap1) - (Am1) * cosom;
		const double pca1 = (Am1) - (Ap1) * cosom;
		
		const double a0  = nca1 + a2al;
		coeff[A1] = 2 * pca1;
		coeff[A2] = nca1 - a2al;
		
		for (int n=0; n < coeffAmt; ++n)
			coeff[n] /= a0;
		
		endStep;
	}
	
	void AllPass::make(Coeff& coeff)
	{
		if (handle(getStatus(), coeff))
			return;

		coeff[B0] = 1 - alpha;
		coeff[B1] = -2 * cosom;
		coeff[B2] = 1+alpha;
		const double a0 = coeff[B2];
		coeff[A1] = coeff[B1];
		coeff[A2] = coeff[B0];
		
		for (int n=0; n < coeffAmt; ++n)
			coeff[n] /= a0;
		
		endStep;
	}
	
	
}
