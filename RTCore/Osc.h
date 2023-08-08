//
//  GenTools.h
//  
//
//  Created by Isaac Roberts on 7/19/21.
//

//using namespace juce;

namespace Osc {
	
	enum Type : int {
		Sin=0, Saw=1, Sqr=2, Tri=3,
		
		Sine=0, Square=2 // Alt Spellings
	};

	
	inline float fastmod(float value, float modulus)
	{
		//	return fmod(value, modulus);
		return value - int(value/modulus)*modulus;
	}
	//I don't think this is faster
//	inline float fastmodf(float value, float modulus)
//	{
//		return value - int(value/modulus)*modulus;
//	}
	inline float fastmod1(float value)
	{
		return value - int(value);
	}
	/*
		These are expecting y = t (seconds) * freq (hz)
	 */
	inline float saw(float y)
	{
		return -fastmod1(y/2.f)*2.f + 1.f;
		//	return -fastmod(y, 2) + 1.f;
	}
	inline float sine(float y)
	{
		return std::sin(y * 2 * M_PI);
	}
//	inline float sin(float y)
//	{
//		return ::sin(y * 2 * M_PI);
//	}

	inline float sqr(float y)
	{
		if (fastmod1(y/2.f) > .5f)
			return -1;
		else return 1;
	}
	inline float tri(float y)
	{
		//np.abs((y/np.pi-.5)% 2 -1) *2 - 1
		float f= (y/(2*M_PI)-.25);
		f = fastmod1(f) - .5;
		f = abs(f);
		return f * 4 - 1;
	}
	inline float sqrPWM(float y, float pwm_pct)
	{
		if (fastmod1(y/2.f) > pwm_pct)
			return -1;
		else return 1;
	}
	inline float pwm(float y, float pwm_pct)
	{
		if (fastmod1(y/2.f) > pwm_pct)
			return -1;
		else return 1;
	}
	
	typedef float(*osc_func_ptr)(float);

	inline osc_func_ptr getFunc(Osc::Type type)
	 {
		 if (type == Sin)
			 return &sine;
		 else if (type == Saw)
			 return &saw;
		 else if (type == Sqr)
			 return &sqr;
		 else if (type == Tri)
			return &tri;
		 else
		 {
			 jassertfalse;
			 return &saw;
		 }
	 }
	
	inline int toInt(Osc::Type t) {
		return static_cast<int>(t);
	}
	inline Osc::Type toType(int i) {
		return static_cast<Osc::Type>(i);
	}
	
	inline float freq(int num) {
		return 440.f * pow(2.f, (num-69)/12.f);
	}
	inline float freq(int num, int oct) {
		num += oct*12;
		return 440.f * pow(2.f, (num-69)/12.f);
	}
	
};


	
