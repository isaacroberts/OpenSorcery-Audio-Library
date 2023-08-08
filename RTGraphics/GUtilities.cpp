//
//  GUtilities.cpp
//  
//
//  Created by Isaac Roberts on 2/18/21.
//

// Created by Isaac Roberts


#include "GUtilities.h"


String Utility::ratioTextFromValueFunction(double v)
{
	if (v>=1)
		return "1:inf";
	else {
		double ratio = 1/(1-v);
		String ss = String(ratio, 2, false);
		if (ss.containsChar('.')) {
			ss = ss.trimCharactersAtEnd("0");
			ss = ss.trimCharactersAtEnd(".");
		}
		return "1:"+ss;
	}
}
double Utility::ratioValueFromTextFunction(String t)
{
	String ratio = t.fromLastOccurrenceOf(":", false, true);
	if (ratio.containsIgnoreCase("inf")) {
		return 1.;
	}
	else
	{
		float rVal = ratio.getFloatValue();
		if (rVal <= 0)
			return -1.f;
		float slope = 1-(1/rVal);
		return slope;
	}
}
//String Utility::reduceFraction(int num, int denom)
//{
//}
String Utility::reduceFraction2sAnd3s(int num, int den)
{
	if (num==0)
		return "0";
	if (num==den)
		return "1 / 1";
	while (num > 1 && den > 1 && ((num & 1) == 0) && ((den & 1) ==0))
	{
		num = num>>1;
		den = den>>1;
	}
	while (num > 1 && den > 1 && (num % 3 == 0) && (den % 3 ==0))
	{
		num /= 3;
		den /= 3;
	}
	return String(num) + " / " + String(den);
}
