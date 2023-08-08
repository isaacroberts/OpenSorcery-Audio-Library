//
//  Preset.h
//  Library - Shared Code
//
//  Created by Isaac Roberts on 7/2/20.
//

// Created by Isaac Roberts 

//#pragma once

#include <string>

#include <iostream>
#include <ctime>

#include <JuceHeader.h>

//#include "looper.h"
//
//#include "Parameter.h"
//#include "ParameterValue.h"

using namespace juce;



template <typename ... Types>
class PresetType {
public:
	typedef std::shared_ptr<PresetType> Ptr;
	
	PresetType(String name_="UnnammedPreset")
	:
	name(name_)
	{
	}
//	PresetType()
//	{
//	}
	
    template <int Index>       auto& get()       noexcept { return std::get<Index> (parameters); }

	
	
	void setRate(float set)
	{
		looper::forEachInTuple([&] (auto& param, size_t index) noexcept
		 {
			param.setRate(set);
		 }
		, parameters);
	}
	void clearCallbacks()
	{
		looper::forEachInTuple([&] (auto& param, size_t index) noexcept
		 {
		 	 param.clearCallback();
		 }
		, parameters);
	}
	void clearUiCallbacks()
	{
		looper::forEachInTuple([&] (auto& param, size_t index) noexcept
		 {
			 param.clearUiCallback();
		 }
		, parameters);
	}
	void clearCallback()
	{
		looper::forEachInTuple([&] (auto& param, size_t index) noexcept
		 {
			 param.clearCallback();
		 }
		, parameters);
	}
	void clearUiCallback()
	{
		looper::forEachInTuple([&] (auto& param, size_t index) noexcept
		 {
			 param.clearUiCallback();
		 }
		, parameters);
	}
	void setName(String set) {
		name = set.replace(" ", "_");		
	}
	
protected:
	
	friend class Preset;
	
	String name;
	std::tuple<Parameter<Types>...> parameters;
	
public:
	class Preset
	{
	public:
		Preset(AudioProcessor* processor, std::shared_ptr<PresetType> type_)
		: type(type_), name("")
		{
			constructAll<0, Types...>();
			looper::forEachInTuple([&] (auto* val, size_t index) noexcept
			{
				processor->addParameter(val);
			}, values);
		}
		
		Preset(AudioProcessor* processor, PresetType& type_)
		: type(type_), name("")
		{
			constructAll<0, Types...>();
			looper::forEachInTuple([&] (auto* val, size_t index) noexcept
			{
				processor->addParameter(val);
			}, values);
		}
		Preset(std::shared_ptr<PresetType> type_)
		: type(type_), name("")
		{
			constructAll<0, Types...>();
		}
		
		template<size_t Ix>
		void constructAll()
		{ }
		template <size_t Ix, typename Type1, typename ... RemainingTypes>
		void constructAll()
		{
			std::get<Ix>(values) = new ParameterValue<Type1>(&std::get<Ix>(type->parameters));
			constructAll<Ix+1, RemainingTypes...>();
		}
		
		void copyValue( Preset& other)
		{
			looper::zip([&] (auto*& myVal, auto*& hisVal, size_t index) noexcept
			{
				myVal->copyValue(*hisVal);
			}
			, values, other.values);
		}
		
		Preset()
		: type(NULL), name("")
		{
		}
		
		
		
		void construct(AudioProcessor* p, std::shared_ptr<PresetType>& type_)
		{
			type = type_;
			
			constructAll<0, Types...>();

			looper::forEachInTuple([&] (auto* val, size_t index) noexcept
			{
				
				p->addParameter(val);

			}, values);
		}
		void construct(std::shared_ptr<PresetType>& type_)
		{
			type = type_;
			constructAll<0, Types...>();
		}
		

		template <int Index>	auto& get()	 { return *std::get<Index> (values); }
		template <int Index>	auto getVal() const { return  std::get<Index> (values)->get(); }
		
		template <int Index> inline	void set(float value)
		{
			get<Index>().set(value);
		}
		template <int Index> inline	void set(double value)
		{
			get<Index>().set(value);
		}
		template <int Index>	inline void set(int value)
		{
			get<Index>().set(value);
		}
		template <int Index>	inline void set(bool value)
		{
			get<Index>().set(value);
		}
		
		void updateBackend()
		{
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				val->update();
			}, values);
		}
		
		inline void setRate(float set) {}
		
		inline bool isBypassed() const{
			return bypass;
		}
		inline void setBypass(bool set) {
			bypass = set;
		}
		bool toggleBypass() {
			bypass = !bypass;
			return bypass;
		}
		
		inline String getXMLName() const {
			return type->name;
		}
		
		
		void printPreset()
		{
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				std::cout<<val->get()<<", ";
			}, values);
			std::cout<<std::endl;
		}
		//save
		void save(XmlElement* parent, bool isPreset)
		{
			XmlElement* element =new XmlElement(getXMLName());
			parent->addChildElement(element);
			
			element->setAttribute("name", name);
			
//			std::cout<<"saving..."<<std::endl;
			
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				val->save(element, isPreset);
			}, values);
			
			element->setAttribute("bypass", (int)bypass);
		}
		void save(std::unique_ptr<XmlElement> parent, bool isPreset)
		{
			XmlElement* element =new XmlElement(getXMLName());
			parent->addChildElement(element);

			element->setAttribute("name", name);
			
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				val->save(element, isPreset);
			}, values);
			
			element->setAttribute("bypass", (int)bypass);
		}
		//load
		void load(XmlElement* parent, bool isPreset)
		{
//			std::cout<<"loading..."<<std::endl;
			
			XmlElement* element = parent->getChildByName(getXMLName());
			if (element != NULL)
			{
				name = element->getStringAttribute("name", name);
				
				looper::forEachInTuple([&] (auto& val, int index) noexcept
				{
					val->load(element, isPreset);
				}, values);
				bypass = (bool) element->getIntAttribute("bypass", 0);
			}
		}
		void load(std::unique_ptr<XmlElement> parent, bool isPreset)
		{
//			std::cout<<"loading unique..."<<std::endl;
			
			XmlElement* element = parent->getChildByName(getXMLName());
			if (element != NULL)
			{
				name = element->getStringAttribute("name", name);
				
				looper::forEachInTuple([&] (auto& val, int index) noexcept
				{
					val->load(element, isPreset);
				}, values);
				bypass = (bool) element->getIntAttribute("bypass", 0);
			}
		}
		//save
		void manualDelete()
		{
			//Call if preset is not added to AudioProcessor
			looper::forEachInTuple([&] (auto*& val, size_t index) noexcept
			{
				delete val;
			}
			, values);
		}
		virtual ~Preset() {
			looper::forEachInTuple([&] (auto*& val, size_t index) noexcept
			{
				//AudioProcessor deletes this i guess
				//TODO: better object ownership - add clear method to audioprocessor
				//delete val;
			}
			, values);
		}
		
		std::shared_ptr<PresetType> getType() {
			return type;
		}
		void setType(std::shared_ptr<PresetType> set) {
			type = set;
			
			looper::zip([&] (auto& param, auto*& val, size_t index) noexcept
			{
				val->updateParamType(&param);
			}
			, type->parameters, values);
		}
		void setTypeKeepUi(std::shared_ptr<PresetType> set) {
			type = set;
			
			looper::zip([&] (auto& param, auto*& val, size_t index) noexcept
						{
				val->updateParamTypeKeepUi(&param);
			}
			, type->parameters, values);
		}
		void setType(PresetType& set) {
			type = std::make_shared<PresetType>(set);
			
			looper::zip([&] (auto& param, auto*& val, size_t index) noexcept
			{
				val->updateParamType(&param);
			}
			, type->parameters, values);
		}
		void setTypeKeepUi(PresetType set) {
			type = std::make_shared<PresetType>(set);
			
			looper::zip([&] (auto& param, auto*& val, size_t index) noexcept
			{
				val->updateParamTypeKeepUi(&param);
			}
			, type->parameters, values);
		}
		
		void setName(String set) {
			name = set;
		}
		String getName()const {
			return name;
		}
		
		
	protected:
		std::shared_ptr<PresetType> type;
		String name;
		std::tuple<ParameterValue<Types>*...> values;
		bool bypass;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Preset);
	};
};

