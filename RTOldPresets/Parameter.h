//
//  Parameter.h
//  Library - Shared Code
//
//  Created by Isaac Roberts on 7/2/20.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>


//#include "TypeHandling.h"

//using namespace juce;

//#include <iostream>

template <class Type>
class Parameter 
{
public:
	Parameter()
	: id(""), name(""),
	low(0), high(1), def(1),
	range(0, 1),
	saveToPreset(true)
	{
		id = "";
		callback = [&](Type s) {};
		uiCallback = [&](Type s) {};
		range.interval = getDefaultInterval();
//		safeCaller = std::make_shared<SafeCallback>(SafeCallback(this));
	}
	
//	Parameter(String id_, String name_, Type low_, Type high_, Type default_)
//	: id(id_), name(name_),
//	low(low_), high(high_), def(default_),
//	range(low, high),
//	saveToPreset(true)
//	{
//		callback = [&](Type s) {};
//		uiCallback = [&](Type s) {};
//		range.interval = getDefaultInterval();
////		safeCaller = std::make_shared<SafeCallback>(SafeCallback(this));
//	}
	
//	RangedAudioParameter* createJuceParameter();
	
	void setInfo(String id_, String name_, Type low_, Type high_, Type default_);
	void setInfo(String id_, String name_, Type def);
	
	void clearCallback() {
		callback = [&](Type s) {};
	}
	void clearUiCallback() {
		
		uiCallback = [&](Type s) {};
	}
	Type bound(Type value) const
	{
		
		if (value < low)
			return low;
		if (value > high)
			return high;
		return value;
	}
	Type* bound(Type* value) const
	{
		if (*value < low)
			*value = low;
		else if (*value > high)
			*value = high;

		return value;
	}
	
	float normalize(Type value) { return TypeHandling::normalize<Type>(value, low, high);}
	Type denormalize(float flat_value) { return TypeHandling::denormalize<Type>(flat_value, low, high);}
	
	
	inline Type getLow() const {return low;}
	inline Type getHigh() const {return high;}
	inline Type getDefault() const {return def;}

	inline void setRange(Type from, Type to) {
		low = from;
		high = to;
	}
	inline void setDefault(Type set_) { def = set_; }

	virtual void callCallback(Type value) {
		callback(value);
	}
//private:
//	struct SafeCallback {
//		SafeCallback(Parameter<Type>* caller_)
//		: caller(caller_), cancelled(false)
//		{ }
//		void reset(Type value_) {
////			value = value_;
////			cancelled=false;
//		}
//		void call(Type value) {
//			if (!cancelled)
//				caller->uiCallback(value);
//		}
//		void cancel() {
//			cancelled=true;
//		}
////		Type value;
//		Parameter<Type>* caller;
//		bool cancelled;
//	};
//	std::shared_ptr<SafeCallback> safeCaller;
	
public:
	virtual void callUiCallback(Type value) {
		
		auto* messageManager = juce::MessageManager::getInstanceWithoutCreating();
		if (messageManager)
		{
			if (messageManager->isThisTheMessageThread())
			{
				uiCallback(value);
			}
			else
			{
//				safeCaller->reset(value);
//				auto dupeCallback = uiCallback;
				messageManager->callAsync([=](){uiCallback(value);});
			}
		}
	}
	virtual ~Parameter()
	{
//		if (safeCaller)
//			safeCaller->cancel();
	}

	
	void setCallback(std::function<void(Type)> set)
	{
		callback = set;
		jassert (callback);
	//		callCallback();
	}
	void setUiCallback(std::function<void(Type)> set)
	{
		uiCallback = set;
		jassert(uiCallback);
	}
	
//	void bindCallback(Type& bind)
//	{
//		callback = [&](Type set) { bind = set; };
////		callCallback();
//	}
	void bindCallback(Type* bind)
	{
		callback = [=](Type set){ *bind = set; };
//		callCallback();
	}
	
	auto getUiCallback() { return uiCallback; }
	
	String getName() const {return name;}
	
	void setName(String set) {
		name = set.replace(" ", "_");
	}
private:
//	static String getNextID() {
//		static int curID=0;
//		return "Parr"+String(++curID);
//	}
	
public:
	String getID() {
//		jassert (id != "");
//		if (id=="")
//		{
//			id = getNextID();
//		}
		return id;
		
	}
	
	const NormalisableRange<float>& getRange() const { return range; }
	
	virtual void setRate(float set) {}
	
	bool savesToPreset() { return saveToPreset;}
	void setSavesToPreset(bool set) {saveToPreset = set;}
	
	double getDefaultInterval() {return TypeHandling::getDefaultInterval<Type>(low, high);}
	
protected:
	String id;
	String name;
	std::function<void(Type)> callback;
	std::function<void(Type)> uiCallback;
	Type low, high;
	Type def;
//	int interval = 100;
	NormalisableRange<float> range;
	bool saveToPreset=false;
};

/*
 JUCE UPDATE
 
 goto juce_core > xml > juce_XmlElement.h
 add to line ~290:
 
	 void getAttribute(StringRef name, int& value) const {
		 value = getIntAttribute(name, value);
	 }
	 void getAttribute(StringRef name, size_t& value) const {
		 value = getIntAttribute(name, (int)value);
	 }
	 void getAttribute(StringRef name, unsigned int& value) const {
		 value = getIntAttribute(name, value);
	 }
	 void getAttribute(StringRef name, String& value) const {
		 value = getStringAttribute(name, value);
	 }
	 void getAttribute(StringRef name, double& value) const {
		 value = getDoubleAttribute(name, value);
	 }
	 void getAttribute(StringRef name, float& value) const {
		 value = getDoubleAttribute(name, value);
	 }
	 void getAttribute(StringRef name, bool& value) const {
		 value = getBoolAttribute(name, value);
	 }
 */
