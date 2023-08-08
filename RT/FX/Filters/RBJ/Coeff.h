

//Coeff Type:

#pragma once

namespace RBJ
{
	
	enum Status {
		Active, Silence, Bypass, Error
	};
	
	
	constexpr int coeffAmt = 5;
	enum CoeffPos {
		B0, B1, B2, A1, A2
	};

	struct Coeff {
		double& operator[] (int n) { return coeff[n]; }
		double coeff[coeffAmt];
		double a0;
		Status status;
		
		Coeff() : coeff() {
			bypass();//sets coeffs to {1, 0, 0, ...}
		}
		Coeff(const Coeff& other) {
			for (int n=0; n < coeffAmt; ++n)
			{
				coeff[n] = other.coeff[n];
			}
		}
		Coeff& operator= (Coeff&& other) {
			for (int n=0; n < coeffAmt; ++n)
			{
				coeff[n] = other.coeff[n];
			}
			return *this;
		}
		Coeff& operator= (const Coeff& other) {
			for (int n=0; n < coeffAmt; ++n)
			{
				coeff[n] = other.coeff[n];
			}
			return *this;
		}
		void silence() {
			for (int n=0; n < coeffAmt; ++n) {
				coeff[n] = 0;
			}
		}
		void bypass() {
			coeff[B0] = 1;
			for (int n=1; n < coeffAmt; ++n) {
				coeff[n] = 0;
			}
		}
		
		//Utility functions
		
		void approach(Coeff& other, double alpha)
		{
			float pa0 = a0;
			a0 = alpha * a0 + (1-alpha)*other.a0;
			for (int n=0; n < coeffAmt; ++n)
			{
				coeff[n] = (alpha * coeff[n]*pa0 + (1-alpha) * other.coeff[n]*other.a0 )/a0 ;
			}
		}
		
		void setAsSlope(Coeff& from, Coeff& to, int numSamples)
		{
			for (int n=0; n < coeffAmt; ++n)
			{
				coeff[n] = (to.coeff[n] - from.coeff[n])/numSamples ;
			}
		}
		
		void add(Coeff& slope, int num)
		{
			for (int n=0; n < coeffAmt; ++n)
			{
				coeff[n] += slope.coeff[n] * num;
			}
		}
		
		
		void print() {
			printf("[%.3e,\t%.3e,\t%.3e]\n", coeff[B0]*a0, coeff[B1]*a0, coeff[B2]*a0);
			printf("[%.3e,\t%.3e,\t%.3e]\n", a0,		   coeff[A1]*a0, coeff[A2]*a0);
		}
		
		
		~Coeff() {}
	};

}
