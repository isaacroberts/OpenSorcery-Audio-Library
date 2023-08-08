//
//  LinkedParameter.h
//  
//
//  Created by Isaac Roberts on 2/15/21.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>

//#include "TypeHandling.h"

//#include "../Sliders/TwoValueSlider.h"

using namespace juce;

template <typename Type, int Amt>
class LinkedParameterType
{
	typedef std::array<Type, Amt> Arr;
public:
	
	enum LinkType {
		Push, Refuse//= Lowest to Highest
	};
	
	LinkedParameterType(String name_)
	:
	name(name_),
	savePreset(true)
	{
		for (int n=0; n < Amt; ++n)
			callbacks[n] = [&](Type s) {};
		uiCallback = [&](Arr s) {};
	}
	LinkedParameterType()
	: name(),
	savePreset(true)
	{
		for (int n=0; n < Amt; ++n)
			callbacks[n] = [&](Type s) {};
		uiCallback = [&](Arr s) {};
	}
	
	void setInfo(String id_, String name_, Type low_, Type high_)
	{
		id = id_;
		setName(name_);
		low = low_;
		high = high_;
//		def = default_;
		
		range.start=low;
		range.end=high;
		range.interval = getDefaultInterval();
		
	}
	void setDefaults(Arr set) {
		for (int n=0; n < Amt; ++n) {
			def[n] = set[n];
		}
	}
	void setDefault(Type set_) {
		for (int n=0; n < Amt; ++n) {
			def[n] = set_;
		}
	}
	
	void setCallback(int ix, std::function<void(Type)> set) {
		callbacks[ix] = set;
	}
	void setUiCallback(std::function<void(Arr)> set) {
		uiCallback = set;
	}
	
//	template <int Index>       auto& get()       noexcept { return std::get<Index> (parameters); }
	
	Type bound(Type value) const
	{
		
		if (value < low)
			return low;
		if (value > high)
			return high;
		return value;
	}
	void bound(Type* value) const
	{
		if (*value < low)
			*value = low;
		else if (*value > high)
			*value = high;
	}
	void bound(Arr& values) const
	{
		if (behavior == Push || behavior == Refuse)
		{			
			//Check minimum
			if (values[0] < low)
				values[0] = low;
			//Check max because loop avoids element 0
			if (values[0] > high)
				values[0] = high;
			
			//Iterate
			for (int n=1; n < values.size(); ++n)
			{
				if (values[n] < values[n-1])
				{
					values[n] = values[n-1];
				}
				else if (values[n] > high)
					values[n] = high;
			}
		}
	}
	void setRate(float set)
	{
		
	}
	void clearCallbacks()
	{
		clearCallback();
	}
	void clearUiCallbacks()
	{
		clearUiCallback();
	}
	
	void clearCallback()
	{
		for (int n=0; n < Amt; ++n)
			callbacks[n] = [&](Type s) {};
	}
	void clearUiCallback()
	{
		uiCallback = [&](Arr s) {};
	}
	float normalize(Type value) { return TypeHandling::normalize(value, low, high);}
	Type denormalize(float flat_value) { return TypeHandling::denormalize(flat_value, low, high);}
	
	inline Type getLow() const {return low;}
	inline Type getHigh() const {return high;}
	inline Type getDefault(int n) const {return def[n];}

	inline void setRange(Type from, Type to) {
		low = from;
		high = to;
	}


	virtual void callCallback(int ix, Type value) {
		callbacks[ix](value);
	}
	virtual void callUiCallback(Arr values)
	{
		auto* messageManager = juce::MessageManager::getInstanceWithoutCreating();
		if (messageManager)
		{
			if (messageManager->isThisTheMessageThread())
			{
//				std::cout<<"ui callback"<<std::endl;
				uiCallback(values);
			}
			else
			{
				messageManager->callAsync([&, values](){this->uiCallback(values);});
			}
		}
	}
	
	auto getUiCallback() { return uiCallback; }
	
	void setName(String set) {
		name = set.replace(" ", "_");
	}
	String getName() const {return name;}
	
	String getParamName(int n) const { return name + "_"+String(n); }
	
	void setLinkBehavior(LinkType mode) { behavior = mode;}
	
	Type getDefault(int n) { return def[n];}
	
	virtual ~LinkedParameterType() {}
	
	bool savesToPreset() { return savePreset; }
	
	void setSavesToPreset(bool set) { savePreset = set; }
	
	Type getDefaultInterval() { return TypeHandling::getDefaultInterval(low, high);}
	
private:
	friend class LinkedParameter;
	friend class LinkedParameterValue;
	
	String name;
	String id;
	
	Type high;
	Type low;
	Arr def;
	NormalisableRange<float> range;
	
	std::array<std::function<void(Type)>, Amt> callbacks;
	std::function<void(Arr)> uiCallback;
	LinkType behavior;
	bool savePreset;
	
public:
	
	class LinkedParameter {
		
		class LinkedParameterValue : public RangedAudioParameter {
		public:
			LinkedParameterValue(LinkedParameter* boss_, int ix_)
			: RangedAudioParameter(boss_->type->id + "_"+String(ix_+1), boss_->type->name),
			boss(boss_), ix(ix_)
			{
			}
			const NormalisableRange<float>& getNormalisableRange() const override {
				
				return boss->type->range;
			}

			float getValue() const override {
//				assert (boss != NULL);
//
//				if (!boss) return 0;
				return boss->getFlatValue(ix);
			}
			void setValue(float norm) override {
//				assert (boss != NULL);
//
//				if (!boss) return;
				boss->setFlatValue(ix, norm);
			}
			float getDefaultValue() const override {
				return boss->getFlatDefault(ix);
			}
			
			bool isAutomatable() { return !TypeHandling::isBoolean<Type>(); }
			
			//Meta parameter may affect other parameters
			//But causes AU to set it to max at beginning of run
//			bool isMetaParameter() const override { return false; }
		
			
			String getName(int maxLength) const override {
//				assert (boss != NULL);
				return boss->type->name.substring(0, maxLength-3)+"_"+String(ix+1);
			}
			
			virtual String getLabel() const override { return ""; }
			
			bool isDiscrete() const override {return TypeHandling::isDiscrete<Type>();}
			bool isBoolean() const override {return TypeHandling::isBoolean<Type>();}
			
			float getValueForText(const String& text) const override {return text.getFloatValue();}
			
			
			LinkedParameter* boss;
			int ix;
		};
		
	public:
		LinkedParameter(AudioProcessor* processor, std::shared_ptr<LinkedParameterType> type_)
		: type(type_), name("")
		{
			_construct(processor);
		}
		LinkedParameter(AudioProcessor* processor, LinkedParameterType& type_)
		: type(type_), name("")
		{
			_construct(processor);
		}
		LinkedParameter(std::shared_ptr<LinkedParameterType> type_)
		: type(type_), name("")
		{
			_construct();
		}
		LinkedParameter()
		: type(NULL), name("")
		{
		}
		void resetToDefaults()
		{
			for (int n=0; n < Amt; ++n)
			{
				values[n] = type->getDefault(n);
				flatValues[n] = type->normalize(values[n]);
				type->callCallback(n, values[n]);
			}
			type->callUiCallback(values);
		}
		void construct(AudioProcessor* p, std::shared_ptr<LinkedParameterType>& type_)
		{
			type = type_;
			_construct(p);
		}
		void construct(std::shared_ptr<LinkedParameterType>& type_)
		{
			type = type_;
			_construct();
		}
	private:
		void _construct(AudioProcessor* p)
		{
			for (int n=0; n < Amt; ++n)
			{
				paramValues[n] = new LinkedParameterValue(this, n);
				p->addParameter(paramValues[n]);
				values[n] = type->getDefault(n);
				flatValues[n] = type->normalize(values[n]);
			}
		}
		void _construct()
		{
			for (int n=0; n < Amt; ++n)
			{
				paramValues[n] = new LinkedParameterValue(this, n);
				values[n] = type->getDefault(n);
				flatValues[n] = type->normalize(values[n]);
			}
		}
	public:
		void copyValue(LinkedParameter& other)
		{
			jassert(Amt == other.getAmt());
			for (int n=0; n < Amt; n ++)
			{
				values[n] = other.values[n];
				flatValues[n] = other.flatValues[n];
				type->callCallback(n, values[n]);
			}
			type->callUiCallback(values);
		}

		template <int Index>	auto get()   const  { return values[Index]; }
		template <int Index>	auto* getVal()  { return paramValues[Index]; }
		
		template <int Index>	void set(float value)
		{
			setValue(Index, value);
		}
		template <int Index>	void set(double value)
		{
			setValue(Index, value);
		}
		template <int Index>	void set(int value)
		{
			setValue(Index, value);
		}
		template <int Index>	void set(bool value)
		{
			setValue(Index, value);
		}
		Type getValue(int ix) {
			return values[ix];
		}
		float getFlatDefault(int ix) {
			return type->normalize(type->getDefault(ix));
		}
		float getFlatValue(int ix) {
			return flatValues[ix];
		}
		void set(int ix, Type value) {
			setValue(ix, value);
		}
		void set(const Arr newVals)
		{
			for (int n=0; n < Amt; ++n)
			{
				float s = newVals[n];
				
				if (s > type->high)
					s = type->high;
				if (s < type->low)
					s = type->low;
				
				if (n>0)
				{
					if (s < values[n-1])
						s = values[n-1];
				}
				
				values[n] = s;
				flatValues[n] = type->normalize(s);
			}
			for (int n=0; n < Amt; ++n)
				type->callCallback(n, values[n]);
			type->callUiCallback(values);
		}
		void set(const Type* newVals)
		{
			for (int n=0; n < Amt; ++n)
			{
				float s = newVals[n];
				
				if (s > type->high)
					s = type->high;
				if (s < type->low)
					s = type->low;
				
				if (n>0)
				{
					if (s < values[n-1])
						s = values[n-1];
				}
				
				values[n] = s;
				flatValues[n] = type->normalize(s);
			}
			for (int n=0; n < Amt; ++n)
				type->callCallback(n, values[n]);
			type->callUiCallback(values);
		}


		void setValues (Arr newVals) { set(newVals); }
		
		void setFromGraphics(Arr newVals)
		{
			for (int n=0; n < Amt; ++n)
			{
				float s = newVals[n];
				
				if (s > type->high)
					s = type->high;
				if (s < type->low)
					s = type->low;
				
				if (n>0)
				{
					if (s < values[n-1])
						s = values[n-1];
				}
				
				if (values[n] != s)
				{
					values[n] = s;
					flatValues[n] = type->normalize(s);
					type->callCallback(n, values[n]);
				}
			}
//			for (int n=0; n < Amt; ++n)
		}
	private:
		void _subsetValue(int n, Type& val, bool towardsEnd)
		{
			if (getType()->behavior==Refuse)
			{
				val = values[n];
				return;
			}
			else
			{
				values[n] = val;
				flatValues[n] = type->normalize(values[n]);
				
				type->callCallback(n, values[n]);
				
				if (towardsEnd)
				{
					if (n+1 < Amt)
					{
						if (values[n] > values[n+1])
							_subsetValue(n+1, val, towardsEnd);
					}
				}
				else
				{
					if (n > 0)
					{
						if (values[n] < values[n-1])
						{
							_subsetValue(n-1, val, towardsEnd);
						}
					}
				}
			}
		}
		void _internalSet(int n, Type set)
		{
			if (set > type->high)
				set = type->high;
			if (set < type->low)
				set = type->low;

			//Check neighbors
			if (n+1 == Amt)
			{ }
			else if (set > values[n+1]) {
				_subsetValue(n+1, set, true);
			}
			if (n==0)
			{ }
			else if (set < values[n-1])
			{
				_subsetValue(n-1, set, false);
			}
			values[n] = set;
			flatValues[n] = type->normalize(set);
			type->callCallback(n, values[n]);
		}
	public:
		void setValue(int n, Type s) {
			if (s!=values[n])
			{
				_internalSet(n, s);
				type->callUiCallback(values);
			}
		}
		void setFlatValue(int n, float nrm) {
			float den = type->denormalize(nrm);
			_internalSet(n, den);
							
			type->callUiCallback(values);
		}
		void setValueFromGraphics(int n, Type s) {
			
			if (s!=values[n])
			{
				_internalSet(n, s);
			}
		}
		void setFlatValueFromGraphics(int n, float nrm) {
			float den = type->denormalize(nrm);
			_internalSet(n, den);
		}
		
		void recognizeTwoValueSlider(Slider* slider, double interval=0)
		{
			if (interval < 0)
			{
				interval = type->getDefaultInterval();
			}
			int numDec = slider->getNumDecimalPlacesToDisplay();
			slider->setRange(type->getLow(), type->getHigh(), interval);
			slider->setNumDecimalPlacesToDisplay(numDec);
			
			slider->onValueChange = [=]() {
				this->setFromGraphics(std::array<Type, 2>{(Type)slider->getMinValue(), (Type)slider->getMaxValue()});};
			
			type->setUiCallback( [=](Arr v) { slider->setMinAndMaxValues(v[0], v[1], sendNotificationSync);} );
			
			type->callUiCallback(values);
		}
		void recognizeTwoValueSlider(TwoValue::TwoValueSlider* slider, double interval=0)
		{
			if (interval < 0)
			{
				interval = type->getDefaultInterval();
			}
			int numDec = slider->getNumDecimalPlacesToDisplay();
			
			slider->setRange(type->getLow(), type->getHigh(), interval);
			slider->setNumDecimalPlacesToDisplay(numDec);

			type->setUiCallback( [=](Arr v) { slider->setMinAndMaxValues(v[0], v[1], sendNotificationSync);} );
			type->callUiCallback(values);
			
			slider->onLowValueChange = [=]() { this->setValueFromGraphics(0, (Type)slider->getMinValue());};
			slider->onHighValueChange= [=]() { this->setValueFromGraphics(1, (Type)slider->getMaxValue());};
			
		}

		
		
		void updateAll()
		{
			for (int n=0; n < Amt; ++n)
			{
				type->callCallback(n, values[n]);
			}
		}
		
		void setRate(float set) {}
		
		String getXMLName() const {
			return type->name;
		}
		
		//save
		void save(XmlElement* parent, bool isPreset)
		{
			if (isPreset && !saveToPreset())
				return;

			XmlElement* element =new XmlElement(getXMLName());
			parent->addChildElement(element);
			
			element->setAttribute("name", name);
						
			for (int n=0; n < Amt; ++n)
			{
				element->setAttribute(type->getParamName(n), values[n]);
			}
		}
		void save(std::unique_ptr<XmlElement> parent, bool isPreset)
		{
			save(&*parent, isPreset);
		}
		//load
		void load(XmlElement* parent, bool isPreset)
		{
			if (isPreset && !saveToPreset())
				return;

			XmlElement* element = parent->getChildByName(getXMLName());
			if (element != NULL)
			{
				name = element->getStringAttribute("name", name);
				
				for (int n=0; n < Amt; ++n)
				{
					Type v = loadVal(n, element, type->getParamName(n), type->getDefault(n));

					values[n] = type->bound(v);
					flatValues[n] = type->normalize(values[n]);
					type->callCallback(n, values[n]);
				}
				type->callUiCallback(values);
			}
		}
		void load(std::unique_ptr<XmlElement> parent, bool isPreset)
		{
			load(&*parent, isPreset);
		}
		
		//save
		void saveToPreset(XmlElement* parent)
		{
			if (type->savesToPreset())
			{
				save(parent);
			}
		}
		void saveToPreset(std::unique_ptr<XmlElement> parent)
		{
			if (type->savesToPreset())
			{
				save(parent);
			}
		}
		//load
		void loadPreset(XmlElement* parent)
		{
			if (type->savesToPreset())
			{
				load(parent);
			}
		}
		void loadPreset(std::unique_ptr<XmlElement> parent)
		{
			if (type->savesToPreset())
			{
				load(parent);
			}
		}
		
		void manualDelete() {
			//Call if preset is not added to AudioProcessor
			for (int n=0; n < Amt; ++n)
			{
				delete paramValues[n];
			}
		}
		virtual ~LinkedParameter() {
			//AudioProcessor deletes this i guess
			//TODO: better object ownership - add clear method to audioprocessor
			//delete paramValues;
		}
		
		std::shared_ptr<LinkedParameterType> getType() {
			return type;
		}
		void setType(std::shared_ptr<LinkedParameterType> set) {
			type = set;
		}
		void setTypeKeepUi(std::shared_ptr<LinkedParameterType> set) {
			type = set;
		}
		void setType(LinkedParameterType& set) {
			type = std::make_shared<LinkedParameterType>(set);
		}
		void setTypeKeepUi(LinkedParameterType set) {
			type = std::make_shared<LinkedParameterType>(set);
		}
		
		void setName(String set) {
			name = set;
		}
		String getName()const {
			return name;
		}
		inline int getAmt() const { return Amt; }
		
	private:
		friend class LinkedParameterValue;
		
		Type loadVal(int ix, XmlElement* element, String name, Type def) {
			return TypeHandling::loadVal(element, name, type->getDefault(ix));
		}
		
		std::shared_ptr<LinkedParameterType> type;
		String name;
		Arr values;
		std::array<float, Amt> flatValues;
		std::array<LinkedParameterValue*, Amt> paramValues;
		bool bypass;

		
	};
	typedef LinkedParameter Preset;
	
private:
//	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LinkedParameter)
	
	
	

};
