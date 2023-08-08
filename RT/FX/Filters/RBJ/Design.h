
#pragma once

#include "Coeff.h"

namespace RBJ
{
	
	 
	 /*
		   Q/BW/S parameter:
			   LowPass, HighPass, AllPass:
				   Q is the classical EE definition
			   PeakingEQ:
				  A*Q is the classical EE definition of Q where A is square root of amplitude {pow(10, gain/40)}
			   BandPass1, BandPass2, Notch, PeakingEQ:
				   Bandwidth is in Octaves
			   LowShelf, HighShelf:
				   S is (0 and 1], where 0 is silence, .1 is gradual, and  1 is the maximum possible steepness.
		   
		   BandPass1 is constant skirt gain, peak gain = Q
		   BandPass2 is constant db peak gain
	  
	  
		Resonance is an optional way to set Q/BW/S. Acceptable values are from 0 to 1, where 0 is no slope and 1 is maximum slope.
	  
   */
	
	enum Type {
		None_T,
		LowPass_T, HighPass_T,
		BandPass1_T, // Constant Skirt Gain
		BandPass2_T, // Constant Peak Gain
		Notch_T,
		AllPass_T,
		PeakingEQ_T,
		LowShelf_T,
		HighShelf_T
	};
	/*
	class NDesign
	{
		void make(Coeff& c);
		void setRate(double set) {rate = set;setOmega(); }
		void setFreq(double set) {
			freq = set;
			setOmega();
		}
		void setQBWS(double set) {
			q = set;
			setAlpha();
		}
		void setGain(double set) {
			gain = set;
		}
		void setAll(double f, double qbws)  {
			freq = f;
			q = qbws;
			setAlpha();
		}
		void setAll(double f, double qbws, double g)  {
			freq = f;
			q = qbws;
			gain = g;
			setOmega();
			setAlpha();
		}

		double getFreq() { return freq; }
		double getQBWS() { return q; }
		double getGain() { return gain; }
		
		Type getType() { return LowShelf; }
		Status getStatus();
		
		inline bool usesBW () { return type == BandPass1 || type == BandPass2 || type == Notch || type == PeakingEQ; }
		inline bool usesS () { return type == LowShelf || type == HighShelf; }
		inline bool usesQ () { return type == LowPass || type == HighPass || type == AllPass; }
	protected:
		
		
		void setOmega() {
			omega= 2 * M_PI * freq / rate;
		}
		void setAlpha();
		//Variables
		double rate;
		double freq=100, q/bw/s=.707, gain=0;
		//Intermediates
		double omega;
		double alpha;
	};
	*/
	
	
	
	template <bool Low>
	struct LH_Pass
	{
		void make(Coeff& c);
		void setRate(double set) {rate = set; setOmega();setAlpha(); }
		void setFreq(double set);
		void setQBWS(double set);
		void setResonance(double set);
		void setGain(double set) {}
		
		double getFreq() { return freq; }
		double getQBWS() { return q; }
		double getGain() { return 0; }
		
		Status getStatus();
		Type getType() { return Low ? LowPass_T : HighPass_T; }
		inline bool needsUpdate() { return shouldUpdate; }

		inline bool usesBW () { return false; }
		inline bool usesS () { return false; }
		inline bool usesQ () { return true; }
	protected:
		void setOmega();
		void setAlpha();
		//Variables
		double rate;
		double freq=1000, q=.707;
		//Intermediates
		double omega, cosom;
		double alpha;
		bool shouldUpdate=true;
	};

	typedef LH_Pass<true> LowPass;
	typedef LH_Pass<false> HighPass;
	
	template <bool ConstantPeak>
	struct BandPass
	{//Constant Skirt Gain, Peak Gain=Q
		
		void make(Coeff& c);
		void setRate(double set) {rate = set; setOmega(); }
		void setFreq(double set);
		void setQBWS(double set);
		void setResonance(double set);
		void setGain(double set) {}
		
		double getFreq() { return freq; }
		double getQBWS() { return bw; }
		double getGain() { return 0; }
		
		Status getStatus();
		Type getType() { return ConstantPeak ? BandPass1_T : BandPass2_T; }
		inline bool needsUpdate() { return shouldUpdate; }

		inline bool usesBW() { return true; }
		inline bool usesS () { return false; }
		inline bool usesQ () { return false; }
	protected:
		void setOmega();
		void setAlpha();
		//Variables
		double rate;
		double freq=500, bw=1;
		//Intermediates
		double omega, cosom, sinom;
		double alpha;
		bool shouldUpdate=true;
	};
	typedef BandPass<true> BandPass1;
	typedef BandPass<false> BandPass2;
	
	struct PeakingEQ
	{
		void make(Coeff& c);
		void setRate(double set) {rate = set; setOmega(); }
		void setFreq(double set);
		void setQBWS(double set);
		void setGain(double set);
		void setResonance(double set);
		
		double getFreq() { return freq; }
		double getQBWS() { return bw; }
		double getGain() { return gain; }
		
		Status getStatus();
		Type getType() { return PeakingEQ_T; }
		inline bool needsUpdate() { return shouldUpdate; }

		inline bool usesBW() { return true; }
		inline bool usesS () { return false; }
		inline bool usesQ () { return false; }
	protected:
		void setOmega();
		void setAlpha();
		//Variables
		double rate;
		double freq=100, bw=1, gain=0;
		//Intermediates
		double omega, cosom;
		double alpha;
		float A;
		bool shouldUpdate=true;
	};
	
	template <bool Low>
	struct Shelf
	{
		void make(Coeff& c);
		void setRate(double set) {rate = set; setOmega(); }
		void setFreq(double set);
		void setQBWS(double set);
		void setGain(double set);
		void setResonance(double set);
		
		double getFreq() { return freq; }
		double getQBWS() { return s; }
		double getGain() { return gain; }
		
		Status getStatus();
		Type getType() { return Low ? LowShelf_T : HighShelf_T; }
		inline bool needsUpdate() { return shouldUpdate; }

		inline bool usesBW() { return false; }
		inline bool usesS () { return true; }
		inline bool usesQ () { return false; }
	protected:
		void setOmega();
		void setAlpha();
		//Variables
		double rate;
		double freq=100, s=1, gain=0;
		//Intermediates
		double omega, cosom;
		double alpha;
		float A, sqrtA;
		bool shouldUpdate=true;
	};
	
	typedef Shelf<true> LowShelf;
	typedef Shelf<false> HighShelf;
	
	
	struct BandStop
	{
		void make(Coeff& c);
		void setRate(double set) {rate = set; setOmega(); }
		void setFreq(double set);
		void setQBWS(double set);
		void setGain(double set) {}
		void setResonance(double set);
		
		double getFreq() { return freq; }
		double getQBWS() { return bw; }
		double getGain() { return 0; }
		
		Status getStatus();
		Type getType() { return Notch_T; }
		inline bool needsUpdate() { return shouldUpdate; }

		inline bool usesBW() { return false; }
		inline bool usesS () { return true; }
		inline bool usesQ () { return false; }
	protected:
		void setOmega();
		void setAlpha();
		//Variables
		double rate;
		double freq=100, bw=1;
		//Intermediates
		double omega, cosom, sinom;
		double alpha;
		bool shouldUpdate=true;
	};
	
	typedef BandStop Notch;


	//Can be used to match the phase transfer of Lo/Hi Pass filters
	struct AllPass
	{
		void make(Coeff& c);
		void setRate(double set) {rate = set; setOmega();setAlpha(); }
		void setFreq(double set);
		void setQBWS(double set);
		void setResonance(double set);
		void setGain(double set) {}
		
		double getFreq() { return freq; }
		double getQBWS() { return q; }
		double getGain() { return 0; }
		
		Status getStatus();
		Type getType() { return AllPass_T; }
		inline bool needsUpdate() { return shouldUpdate; }

		inline bool usesBW () { return false; }
		inline bool usesS () { return false; }
		inline bool usesQ () { return true; }
	protected:
		void setOmega();
		void setAlpha();
		//Variables
		double rate;
		double freq=1000, q=.707;
		//Intermediates
		double omega, cosom;
		double alpha;
		bool shouldUpdate=true;
	};
	
	
	
	//Template declarations
	template <> Status LH_Pass<true>::getStatus();
	template <> Status LH_Pass<false>::getStatus();
	template <> Status Shelf<true>::getStatus();
	template <> Status Shelf<false>::getStatus();

	
	
}
