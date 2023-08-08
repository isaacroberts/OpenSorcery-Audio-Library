//
//  Utility.h
//  Library - Shared Code
//
//  Created by Isaac Roberts on 7/8/20.
//  Copyright © 2020 RobertsAudio. All rights reserved.
//

// Created by Isaac Roberts 


namespace Utility {
	
	//Audio Math
	
	static inline float dbToAmp(float d) {
		return pow(10.f, d/20.f);
	}
	static inline float db(float d) {
		return pow(10.f, d/20.f);
	}
	
	static inline float ampToDb(float amp_) {
		return 20.f * log10(amp_);
	}
	static inline float addDb(float db1, float db2) {
		return ampToDb(dbToAmp(db1) + dbToAmp(db2));
	}
	
	static inline float alpha(float samplesToApproach) {
		return exp( -1.0f / (samplesToApproach));
	}

	static inline void getPan(float& lgain, float& rgain, float pan, float amp=1.f)
	{
		lgain = amp * std::min(1.f-pan, 1.f);
		rgain = amp * std::min(1.f+pan, 1.f);
	}
	static inline void stereoSpread(float& l, float& r, float spread)
	{
		/* 	@1.f, channels are equal. @0.f channels are separate 
		 */
		if (spread<=0) return;
		if (spread>=1) {
			l = (l+r)/2;
			r = l;
			return;
		}
		float lo = l;
		float ro = r;
		l += ro * spread;
		r += lo * spread;
		l /= 1+spread;
		r /= 1+spread;
	}
	
	static inline float midiToFreq(int num)
	{
//		std::cout<<"midi "<<num<<" = "<<exp2f((num-69)/12.f)<<std::endl;
		return exp2f((num-69)/12.f) * 440.f;
	}
	static inline float wheelToFreqFactor(float pitchWheel) {
//		std::cout<<"wheel "<<pitchWheel<<"="<<exp2f(pitchWheel)<<std::endl;
		return exp2f(pitchWheel);
	}
	static inline float midiAndWheelToFreq(int num, float pitchWheel)
	{
		return midiToFreq(num) * wheelToFreqFactor(pitchWheel);
	}
	
	//Basic Math
	
	static inline int mod(int x, int modulo) {
		return x >= 0 ? x % modulo : ((x % modulo) + modulo)%modulo;
	}

	
	static inline int dist(int x1, int y1, int x2, int y2)
	{
		return (int)sqrt(pow(x2-x1, 2.f) + pow(y2-y1,2.f));
	}
	
	static inline float mapNorm(float nrm, float min, float max)
	{
		return (nrm * (max-min)) + min;
	}
	
	static inline int randInt(int max) {
		if (max==0) return 0;
		return rand() % max;
	}
	static inline int randInt(int min, int max) {
		if (min==max)
			return min;
		return min + rand() % (max-min);
	}

	//Random float from 0 to 1
	static inline float rand1() {
		return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}
	static inline double rand1_dub() {
		return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	}

	//Random float between -1 and 1
	static inline float rand_c(float amt=0) {
		constexpr float rm = static_cast <float> (RAND_MAX)/2;
		return static_cast <float> (rand() - RAND_MAX/2) / rm;

		return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}
	static inline double rand_dub_c(double amt=0) {
		constexpr double rm = static_cast <double> (RAND_MAX)/2;
		return static_cast <double> (rand() - RAND_MAX/2) / rm;
	}
	
	
	
	static inline double randDouble(double min, double max) {
		if (min==max)
			return min;
		return min + rand1_dub() * (max-min);
	}
	static inline float randFloat(float min, float max) {
		if (min==max)
			return min;
		return min + rand1() * (max-min);
	}
	//Use __unused instead of inline if function is too long


	
	//Overload versions
	static inline float alpha(float secondsToApproach, float rate) {
		return exp( -1.0f / (secondsToApproach * rate));
	}
	static inline double alpha(double samplesToApproach) {
		return exp( -1.0 / (samplesToApproach));
	}
	static inline double alpha(double secondsToApproach, float rate) {
		return exp( -1.0 / (secondsToApproach * rate));
	}

	static inline void san(float& x) {
		if (!std::isfinite(x))
			x=0;
	}
	
	
};

namespace Util = Utility;

