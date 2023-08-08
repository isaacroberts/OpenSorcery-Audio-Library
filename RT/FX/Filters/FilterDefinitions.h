
#pragma once



//#define COEFF_SIZE_FLOAT

#ifndef M_PI
#define M_PI  3.1415926535897932384626433832795
#endif

#ifndef sqrt2
#define sqrt2	1.414213562373095048802
#endif

#ifndef sqrt2over2
#define sqrt2over2  0.707106781186547524401
#endif


namespace Filter
{
	enum Mode {
		Active, Silence, Bypass, Unprepared
	};
	
	enum Type : int {
		LowPass=0, HighPass, BandPass, BandStop, LowShelf, HighShelf, BandShelf, AllPass
	};
	
	static inline bool isBand(Type type)
	{
		return type==BandPass || type==BandStop || type==BandShelf;
	}
	static inline bool isShelf(Type type)
	{
		return type == BandShelf || type == HighShelf || type == LowShelf;
	}
	
	static inline Filter::Mode modeAboveNyquist(Type type)
	{
		if (type == HighPass || type == BandPass )
			return Filter::Silence;
		else return Filter::Bypass;
	}
	static inline Filter::Mode modeBelowZero(Type type)
	{
		if (type == LowPass || type == BandPass)
			return Filter::Silence;
		else return Filter::Bypass;
	}
	
	
	static inline Filter::Mode modeAboveNyquist(bool isHigh)
	{
		return isHigh ? Filter::Silence : Filter::Bypass;
	}
	static inline Filter::Mode modeBelowZero(bool isHigh)
	{
		return isHigh ? Filter::Bypass : Filter::Silence;
	}
}
