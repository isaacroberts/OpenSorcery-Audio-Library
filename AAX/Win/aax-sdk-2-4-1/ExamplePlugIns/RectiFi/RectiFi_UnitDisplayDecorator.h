/*
 *  RectiFi_UnitDisplayDecorator.h
 *  AAX_ParameterModule
 *
 *  Created by Yaroslav Kryvyi on 12/04/11.
 *  Copyright 2011, 2017, 2019, 2021 AVID. All rights reserved.
 *
 */

#ifndef RECTIFI_UNITDISPLAYDELEGATEDECORATOR_H
#define RECTIFI_UNITDISPLAYDELEGATEDECORATOR_H

//AAX includes
#include "AAX_IDisplayDelegateDecorator.h"
#include "AAX_CEffectParameters.h"
//RectiFi includes
#include "RectiFi_Defs.h"


template <typename T>
class RectiFi_UnitDisplayDelegateDecorator : public AAX_IDisplayDelegateDecorator<T>
{
public:
	/** 
	*	Along with the standard decorator pattern argument, this class also takes a unit string.  This is the string
	*	that will be added to the end of valueString.
	*/
	RectiFi_UnitDisplayDelegateDecorator(const AAX_IDisplayDelegate<T>& displayDelegate, const AAX_CString& unitString);
	
	//Virtual Overrides
	RectiFi_UnitDisplayDelegateDecorator<T>*	Clone() const;
	bool										ValueToString(T value, AAX_CString* valueString) const;	
	bool										ValueToString(T value, int32_t maxNumChars, AAX_CString* valueString) const;
	bool										StringToValue(const AAX_CString& valueString, T* value) const;

protected:
	const AAX_CString	mUnitString;
};

template <typename T>
RectiFi_UnitDisplayDelegateDecorator<T>::RectiFi_UnitDisplayDelegateDecorator(const AAX_IDisplayDelegate<T>& displayDelegate, const AAX_CString& unitString)  :
	AAX_IDisplayDelegateDecorator<T>(displayDelegate),
	mUnitString(unitString)
{

}

template <typename T>
RectiFi_UnitDisplayDelegateDecorator<T>*	RectiFi_UnitDisplayDelegateDecorator<T>::Clone() const
{
	return new RectiFi_UnitDisplayDelegateDecorator(*this);
}

template <typename T>
bool		RectiFi_UnitDisplayDelegateDecorator<T>::ValueToString(T value, AAX_CString* valueString) const 
{
	return this->ValueToString(value, 63, valueString);
}

template <typename T>
bool		RectiFi_UnitDisplayDelegateDecorator<T>::ValueToString(T value, int32_t maxNumChars, AAX_CString* valueString) const
{
	int32_t maxLength = maxNumChars;
	value = value + 0.5;

	if (maxLength >= 8)
	{
 		valueString->AppendNumber(static_cast<int>(value));
		if (maxLength >= 11)
			valueString->Append(" Hertz");
		else
			valueString->Append(" Hz");
	}
	else if (value >= 1000)
	{
		valueString->AppendNumber(static_cast<int>(value/1000));
		valueString->Append(".");
		valueString->AppendNumber(((int)value % 1000) / 100);
		valueString->Append("k");
	}
	else
	{
		valueString->AppendNumber(static_cast<int>(value));
		if (maxLength >= 5)
			valueString->Append("Hz");
	}

	double normalizedValue = double(value) - double(kMinFilterFrequency);
	normalizedValue = normalizedValue / (double(kMaxFilterFrequency) - double(kMinFilterFrequency));
	
	int32_t longVal = NormalizedToInt32(normalizedValue);
	double maxCheck = LongControlToNewRange(longVal, 0, 1000);
	if ( maxCheck == 1000 )
	{
		valueString->Clear();
		valueString->Append("Thru");
		return true;
	}

	return	true;
}

template <typename T>
bool		RectiFi_UnitDisplayDelegateDecorator<T>::StringToValue(const AAX_CString& valueString, T* value) const
{
	//Just call through if there is obviously no unit string.
	if (valueString.Length() <= mUnitString.Length())
		return AAX_IDisplayDelegateDecorator<T>::StringToValue(valueString, value);
		
	//Just call through if the end of this string does not match the unit string.	
	AAX_CString unitSubString;
	valueString.SubString(valueString.Length() - mUnitString.Length(), mUnitString.Length(), &unitSubString);
	if (unitSubString != mUnitString)
		return AAX_IDisplayDelegateDecorator<T>::StringToValue(valueString, value);
	
	//Call through with the stripped down value string.  
	AAX_CString valueSubString;
	valueString.SubString(0, valueString.Length() - mUnitString.Length(), &valueSubString);
	return AAX_IDisplayDelegateDecorator<T>::StringToValue(valueSubString, value);
}

#endif //RECTIFI_UNITDISPLAYDELEGATEDECORATOR_H
