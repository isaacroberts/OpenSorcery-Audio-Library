

#include "Design.h"

namespace RBJ
{
	//Low Pass
	template <bool Low>
	void LH_Pass<Low>::setFreq(double set) {
		freq = set;
		setOmega();
		setAlpha();
		shouldUpdate=true;
	}
	template <bool Low>
	void LH_Pass<Low>::setQBWS(double set) {
		q = set;
		setAlpha();
		shouldUpdate=true;
	}
	//AllPass
	void AllPass::setFreq(double set) {
		freq = set;
		setOmega();
		setAlpha();
		shouldUpdate=true;
	}
	void AllPass::setQBWS(double set) {
		q = set;
		setAlpha();
		shouldUpdate=true;
	}
	
	//BandPass
	template <bool ConstantPeak>
	void BandPass<ConstantPeak>::setFreq(double set) {
		freq = set;
		setOmega();
		setAlpha();
		shouldUpdate=true;
	}
	template <bool ConstantPeak>
	void BandPass<ConstantPeak>::setQBWS(double set) {
		bw = set;
		setAlpha();
		shouldUpdate=true;
	}
	
	//PeakingEQ
	void PeakingEQ::setFreq(double set) {
		freq = set;
		setOmega();
		setAlpha();
		shouldUpdate=true;
	}
	void PeakingEQ::setQBWS(double set) {
		bw = set;
		setAlpha();
		shouldUpdate=true;
	}
	void PeakingEQ::setGain(double set) {
		gain = set;
		A = pow(10, gain/40);
		shouldUpdate=true;
	}
	
	//Shelf
	template <bool Low>
	void Shelf<Low>::setFreq(double set) {
		freq = set;
		setOmega();
		setAlpha();
		shouldUpdate=true;
	}
	template <bool Low>
	void Shelf<Low>::setQBWS(double set) {
		s = set;
		setAlpha();
		shouldUpdate=true;
	}
	template <bool Low>
	void Shelf<Low>::setGain(double set) {
		gain = set;
		sqrtA = pow(10, gain/80);
		A = sqrtA * sqrtA;
		setAlpha();
		shouldUpdate=true;
	}
	
	//BandStop
	void BandStop::setFreq(double set) {
		freq = set;
		setOmega();
		setAlpha();
		shouldUpdate=true;
	}
	void BandStop::setQBWS(double set) {
		bw = set;
		setAlpha();
		shouldUpdate=true;
	}

	
	
	double qResonance(double res) {
		//Tested expirementally for LH_Pass
		return pow(10., res*res+1)-9;
	}
	double bwResonance(double res) {
		jassertfalse;
		return 1;
	}
	double slopeResonance(double res) {
		jassertfalse;
		return .7;
	}
	
	template <bool Low>
	void LH_Pass<Low>::setResonance(double res)  { setQBWS(qResonance(res)); }
	
	void AllPass::setResonance(double res)  { setQBWS(qResonance(res)); }

	template <>
	void BandPass<true>::setResonance(double res) { setQBWS(1.-res); }

	template <>
	void BandPass<false>::setResonance(double res) { jassertfalse; setQBWS(res); }

	
	void PeakingEQ::setResonance(double res) { jassertfalse; setQBWS(res); }
	
	template <bool Low>
	void Shelf<Low>::setResonance(double res)  { setQBWS(slopeResonance(res)); }
	
	void BandStop::setResonance(double res)  { setQBWS(slopeResonance(res)); }
	

	
	
#if DEBUG
#define AssertErr {jassertfalse; return Status::Error;}
#else
#define AssertErr return Status::Error;
#endif

	template <>
	Status LH_Pass<true>::getStatus() {
		if (freq > rate/2) 	return Bypass;
		if (freq <= 0) 		return Silence;
		if (rate <= 0) 		return Silence;
		if (q <= 0)			return Silence;
		return Active;
	}
	template <>
	Status LH_Pass<false>::getStatus() {
		if (freq > rate/2) 	return Silence;
		if (freq <= 0) 		return Bypass;
		if (rate <= 0) 		return Silence;
		if (q <= 0)			return Silence;
		return Active;
	}
	Status AllPass::getStatus() {
		if (freq > rate/2) 	return Bypass;
		if (freq <= 0) 		return Bypass;
		if (rate <= 0) 		return Silence;
		if (q <= 0)			return Bypass;
		return Active;
	}
	
	template <bool ConstantPeak>
	Status BandPass<ConstantPeak>::getStatus() {
		if (freq > rate/2) 	return Silence;
		if (freq <= 0) 		return Silence;
		if (rate <= 0) 		return Silence;
		if (bw <= 0)		return Silence;
		return Active;
	}
	Status BandStop::getStatus() {
		if (freq > rate/2) 	return Silence;
		if (freq <= 0) 		return Bypass;
		if (rate <= 0) 		return Silence;
		if (bw <= 0)		return Bypass;
		return Active;
	}
	
	Status PeakingEQ::getStatus() {
		if (freq > rate/2) 	return Bypass;
		if (freq <= 0) 		return Bypass;
		if (rate <= 0) 		return Silence;
		if (bw <= 0)		return Bypass;
		if (gain==0) 		return Bypass;
		if (gain<-1000)		AssertErr;
		if (gain> 1000)		AssertErr;
		return Active;
	}
	
	template <>
	Status Shelf<true>::getStatus() {
		if (freq > rate/2) 	return Bypass;
		if (freq <= 0) 		return Silence;
		if (rate <= 0) 		return Silence;
		if (s <= 0)			return Bypass;
		if (s > 1)			return Silence;
		if (gain==0) 		return Bypass;
		if (gain<-1000)		AssertErr;
		if (gain> 1000)		AssertErr;
		return Active;
	}
	template <>
	Status Shelf<false>::getStatus() {
		if (freq > rate/2) 	return Silence;
		if (freq <= 0) 		return Bypass;
		if (rate <= 0) 		return Silence;
		if (s <= 0)			return Bypass;
		if (s > 1)			return Silence;
		if (gain==0) 		return Bypass;
		if (gain<-1000)		AssertErr;
		if (gain> 1000)		AssertErr;
		return Active;
	}
	
	
	
#undef AssertErr
	
	template class BandPass<true>;
	template class BandPass<false>;
	template class LH_Pass<true>;
	template class LH_Pass<false>;
	template class Shelf<true>;
	template class Shelf<false>;
}
