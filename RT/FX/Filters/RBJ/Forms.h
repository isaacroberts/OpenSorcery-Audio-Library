
#pragma once


namespace RBJ
{
	
#if DEBUG_SAN
	inline void sanCrash_rbj(float e) {
		jassertfalse;
	}
#else
	inline void sanCrash_rbj(float e) {
		
	}
#endif
	
	
//		inline bool san(double& x) {
//			constexpr double SMAX=500;
//			if 		(isnan(x)) { sanCrash_rbj(x); x = 0;}
//			else if (x > SMAX) {sanCrash_rbj(x); x = SMAX;}
//			else if (x <-SMAX) {sanCrash_rbj(x); x =-SMAX;}
//			else return false;
//			return true;
//		}
	inline void san(double& x) {
		constexpr double SMAX=2;
		if 	(!std::isfinite(x)) {
			x = 0;
		}
		else if (x > SMAX) {
			
		}
		else if (x <-SMAX) {
			
		}
	}
	
	
	struct DirectForm1 {
		//Better for floating point but can blow up
		inline double proc (Coeff& coeff, double in)
		{
			jassert (std::isfinite(in));

			double out = coeff[B0] * in + coeff[B1] * x1 + coeff[B2] * x2
			- coeff[A1] * y1 - coeff[A2] * y2 + eps;

			x2 = x1;
			y2 = y1;
			x1 = in;
			y1 = out;
			
			san(y1);
			san(y2);
		
			eps = -eps;

			return out;
		}
		inline void procBypass(Coeff& coeff, int n) {
			if (n>1)
			{
				x2 = x1;
				y2 = y1;
			}
			else {
				x2 = y2 = 0;
			}
			y1 = x1 = 0;
		}
		inline void reset() {
			x1=x2=y2=y1=0;
		}
		void print() {
			printf("{y1=%.3e,\ty2=%.3e}\n", y1, y2);
		}
		void sanCheck() {
			san(x1);
			san(x2);
			san(y1);
			san(y2);
//			const double SMAX=500;
//			if 		(!std::isfinite(y1)) { sanCrash_rbj(y1); y1 = 0;}
//			else if (y1 > SMAX) y1 = SMAX;
//			else if (y1 <-SMAX) y1 =-SMAX;
//			if 		(!std::isfinite(y2)) { sanCrash_rbj(y2); y2 = 0;}
//			else if (y2 > SMAX) y2 = SMAX;
//			else if (y2 <-SMAX) y2 =-SMAX;
		}

		double x1=0, x2=0, y1=0, y2=0;
		double eps=1e-8;
	};
	
	
	struct DirectForm2 {
		//Better for floating point but can blow up
		inline double proc (Coeff& coeff, double in)
		{
			jassert (std::isfinite(in));

			double w   = in - coeff[A1] * y1 - coeff[A2] * y2 + eps;
			double out =      coeff[B0] * w  + coeff[B1] * y1 + coeff[B2] * y2;
			
			y2 = y1;
			y1 = w;
			
			san(y1);
			san(y2);
		
			eps = -eps;

			return out;
		}
		inline void procBypass(Coeff& coeff, int n) {
			y2 = n>1 ? 0 : y1;
			y1 = 0;
		}
		inline void reset() {
			y2=y1=0;
		}
		void print() {
			printf("{y1=%.3e,\ty2=%.3e}\n", y1, y2);
		}
		void sanCheck() {
			san(y1);
			san(y2);
//			const double SMAX=500;
//			if 		(!std::isfinite(y1)) { sanCrash_rbj(y1); y1 = 0;}
//			else if (y1 > SMAX) y1 = SMAX;
//			else if (y1 <-SMAX) y1 =-SMAX;
//			if 		(!std::isfinite(y2)) { sanCrash_rbj(y2); y2 = 0;}
//			else if (y2 > SMAX) y2 = SMAX;
//			else if (y2 <-SMAX) y2 =-SMAX;
		}

		double y1=0, y2=0;
		double eps=1e-8;
	};
	
	
	struct TransposedDirectForm2 {
		

		inline double proc (Coeff& coeff, double in)
		{
			jassert (std::isfinite(in));

			double out = p1 + coeff[B0] * in + eps;
			
			v1 = p2 + coeff[B1] * in - coeff[A1] * out;
			v2 = coeff[B2] * in - coeff[A2] * out;
			p1 = v1;
			p2 = v2;
			
			san(v1);
			san(v2);
			san(p1);
			san(p2);
			eps = -eps;
			
			return out;
		}
		inline void procBypass(Coeff& coeff, int n) {
			p1 = v1;
			p2 = v2;
			v1 = 0;
			v2 = 0;
		}
		inline void reset() {
			v1=v2=p1=p2 = 0;
		}
		
		void print() {
			//			printf("[%.3e,\t%.3e,\t%.3e]\n", coeff[B0]*a0, coeff[B1]*a0, coeff[B2]*a0);
			printf("{%.3e,\t%.3e\t%.3e\t%.3e}\n", v1, v2, p1, p2);
		}
		void sanCheck() {
			san(v1);
			san(v2);
			san(p1);
			san(p2);
//			print();
			
		}
		//Voltage
		double v1=0, v2=0;
		//Previous
		double p1=0, p2=0;
		//Epsilon
		double eps=1e-8;
	};


	

}
