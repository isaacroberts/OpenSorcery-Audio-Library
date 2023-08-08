//
//  TypeHandling.h
//  
//
//  Created by Isaac Roberts on 2/16/21.
//

// Created by Isaac Roberts 

#ifndef TypeHandling_h
#define TypeHandling_h

//#include <JuceHeader.h>

using namespace juce;


//OSX compiler and Windows compiler handle static templates differently
#if JUCE_MAC
#define DUPE_STAT
#elif JUCE_WINDOWS
#define DUPE_STAT static
#endif



//template <typename Type>
class TypeHandling {
public:
	
	//	static float normalize(Type value, Type low, Type high);
	
	template <class Type>
	static float normalize(Type value, Type low, Type high)
	{
		
		if (high==low)
			return 0;
		return (value-low)/(high-low);
	}
	template <>
	DUPE_STAT float normalize(int value, int low, int high)
	{
		if (high==low)
			return 0;
		return (float(value)-low)/(high-low);
	}
	template <>
	DUPE_STAT float normalize(bool value, bool low, bool high)
	{
		return value;
	}
	
	
	template<class Type>
	static Type denormalize(float flat_value, Type low, Type high)
	{
		
		if (high==low)
			return low;
		return (flat_value)*(high-low) + low;
	}
	template<>
	DUPE_STAT int denormalize(float flat_value, int low, int high)
	{
		if (high==low)
			return low;
		int v = int(std::round((flat_value)*(high-low))) + low;
		if (v <= low) return low;
		if (v >= high) return high;
		return v;
	}

	template<>
	DUPE_STAT bool denormalize(float flat_value, bool low, bool high)
	{
		return round(flat_value);
	}
	
	
	template<class Type>
	static Type modulus(Type v, Type m)
	{
		if (std::is_integral<Type>::value)
		{
			return v >= 0 ? v % m : ((v % m) + m)%m;
		}
		else
		{
			return  v - m * (std::floor(v/m));
		}
	}
	template<>
	DUPE_STAT int modulus(int v, int m)
	{
		return v >= 0 ? v % m : ((v % m) + m)%m;
	}
	template<>
	DUPE_STAT float modulus(float v, float m)
	{
		return v - m * (std::floor(v/m));
	}
	template<>
	DUPE_STAT double modulus(double v, double m)
	{
		return v - m * (std::floor(v/m));
	}
	
	template <class Type>
	static Type modRange(Type v, Type low, Type high)
	{
		//nv-low, high-low)+low
		if (std::is_integral<Type>::value)
		{
			return modulus(v-low, high-low+1)+low;
		}
		else
		{
			return modulus(v-low, high-low)+low;
		}
	}
	
	template <typename Type>
	static double getDefaultInterval(Type low, Type high)
	{
		return 0;
//		return (high-low)/10000.;
	}
	template<>
	DUPE_STAT double getDefaultInterval(int low, int high)
	{
		if (high-low > 1000000)
		{
			int p = log10(high-low);
			return pow(10, p-5);
		}
		else
			return 1.0;
	}
	template<>
	DUPE_STAT double getDefaultInterval(bool l, bool h)
	{
		return 1.0;
	}
	template<>
	DUPE_STAT double getDefaultInterval(float low, float high)
	{
		return 0;
//		return (high-low)/(100000.);
	}
	template<>
	DUPE_STAT double getDefaultInterval(double low, double high)
	{
		return 0;
//		return (high-low)/(10000000.0);
	}
	
	
	
	template<class Type>
	static Type randomInRange(Type low, Type high)
	{
		if (high==low) return low;
		if (std::is_integral<Type>::value)
			return rand() % (high-low+1) + low;
		else
			return low + static_cast <Type> (rand()) /( static_cast <Type> (RAND_MAX/(high-low)));
	}
	template<>
	DUPE_STAT int randomInRange<int>(int low, int high)
	{
		if (high==low) return low;
		int r= rand();
		return r % (high-low+1) + low;
	}
	template<>
	DUPE_STAT double randomInRange<double>(double low, double high)
	{
		if (high==low) return low;
		return low + static_cast <double> (rand()) /( static_cast <double> (RAND_MAX/(high-low)));
	}
	template<>
	DUPE_STAT float randomInRange<float>(float low, float high)
	{
		if (high==low) return low;
		return low + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(high-low)));
	}
	template<>
	DUPE_STAT bool randomInRange<bool>(bool, bool)
	{
		return rand()%2 == 0;
	}
	
	template<class Type>
	static bool isDiscrete() {
		return false;
	}
	template <>
	DUPE_STAT bool isDiscrete<int>() {return true;}
	template <>
	DUPE_STAT bool isDiscrete<bool>() {
		//Tricksy
		//TODO: Don't remember the verdict on this one
		//	std::cout<<"bool disc"<<std::endl;
		return false;
	}
	
	
	
	template <class Type>
	static bool isBoolean() {return false;}
	template <>
	DUPE_STAT bool isBoolean<bool>() { return true; }
	
	
	
	template<class Type>
	static Type getValueForText(const String& text)
	{
		return text.getFloatValue();
//		float floatValue = text.retainCharacters ("-0123456789.").getFloatValue();
//		return (Type)floatValue;
	}
	template<>
	DUPE_STAT int getValueForText<int>(const String& text)
	{
		return text.getIntValue();
	}
	template<>
	DUPE_STAT double getValueForText<double>(const String& text)
	{
		return text.getDoubleValue();
	}
	template<>
	DUPE_STAT bool getValueForText<bool>(const String& text)
	{
		//	std::cout<<"get val for text"<<std::endl;
		//	if (text.contains("true"))
		//		return 1.0f;
		//	if (text.contains("yes"))
		//		return 1.0f;
		//	if (text.contains("on"))
		//		return 1.0f;
		//
		//	if (text.contains("false"))
		//		return 0.0f;
		//	if (text.contains("no"))
		//		return 0.0f;
		//	if (text.contains("off"))
		//		return 0.0f;
		
		//else
		float floatValue = text.retainCharacters ("-0123456789.").getFloatValue();
		return floatValue < 0.5f ? 0.0f : 1.0f;
	}
	template<class Type>
	static String valueToString(Type val)
	{
		return String(val);
	}
	template<>
	DUPE_STAT String valueToString<bool>(bool val)
	{
		return val ? "true" : "false";
	}
	
	
	template <class Type>
	static Type loadVal(XmlElement* element, String name, Type def) {
		return element->getDoubleAttribute(name, def);
	}
	template <>
	DUPE_STAT int loadVal<int>(XmlElement* element, String name, int def) {
		return element->getIntAttribute(name, def);
	}
	template <>
	DUPE_STAT bool loadVal<bool>(XmlElement* element, String name, bool def) {
		//	std::cout<<"Load val"<<std::endl;
		return element->getBoolAttribute(name, def);
	}
	
	
};

#undef DUPE_STAT

#endif /* TypeHandling_h */
