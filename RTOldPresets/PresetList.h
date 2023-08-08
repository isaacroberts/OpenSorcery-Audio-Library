//
//  PresetList.h
//  
//
//  Created by Isaac Roberts on 2/1/21.
//

// Created by Isaac Roberts

//#pragma once

//#include <string>
//
//#include <iostream>
//#include <ctime>
//
//#include <JuceHeader.h>
//
//#include "Parameter.h"
//#include "ParameterValue.h"
//
//#include "looper.h"

//using namespace juce;




template <typename ... Presets>
class PresetTypeList {
public:
	typedef std::shared_ptr<PresetTypeList> Ptr;

	PresetTypeList()
	: name()
	{
		constructAll<0, Presets...>();
	}
	PresetTypeList(String name_)
	: name(name_)
	{
		constructAll<0, Presets...>();
	}
	
	void skipNames()
	{
		looper::forEachInTuple([&] (auto& val, size_t index) noexcept
		{
			val->setName(name+"_sub_"+String(index));
		}, presetTypes);
	}
private:
	template<size_t Ix>
	void constructAll()
	{ }
	template <size_t Ix, typename Type1, typename ... RemainingTypes>
	void constructAll()
	{
		std::get<Ix>(presetTypes) = std::make_shared<Type1>();
		constructAll<Ix+1, RemainingTypes...>();
	}
public:
	template <int Index>       auto& get()       noexcept { return std::get<Index> (presetTypes); }

	template <int Index, int Ix2>       auto& get()       noexcept { return std::get<Index> (presetTypes)->template get<Ix2>(); }
	
	void setRate(float set)
	{
		looper::forEachInTuple([&] (auto& param, size_t index) noexcept
		 {
			param->setRate(set);
		 }
		, presetTypes);
	}
	void clearCallbacks()
	{
		looper::forEachInTuple([&] (auto& param, size_t index) noexcept
		 {
			 param->clearCallback();
		 }
		, presetTypes);
	}
	void clearUiCallbacks()
	{
		looper::forEachInTuple([&] (auto& param, size_t index) noexcept
		 {
			 param->clearUiCallback();
		 }
		, presetTypes);
	}
	void clearCallback()
	{
		looper::forEachInTuple([&] (auto& param, size_t index) noexcept
		 {
			 param->clearCallback();
		 }
		, presetTypes);
	}
	void clearUiCallback()
	{
		looper::forEachInTuple([&] (auto& param, size_t index) noexcept
		 {
			 param->clearUiCallback();
		 }
		, presetTypes);
	}
	void setName(String set) {
		name = set.replace(" ", "_");
	}
	
protected:
	
	friend class PresetList;
	
	String name;
	std::tuple<std::shared_ptr<Presets>...> presetTypes;
	
public:
	class PresetList
	{
	public:
		PresetList(AudioProcessor* processor, std::shared_ptr<PresetTypeList> type_)
		: type(type_), bypass(false), name()
		{
			constructAll<0, Presets...>(processor);
		}
	private:
		template<size_t Ix>
		void constructAll(AudioProcessor* p)
		{ }
		template <size_t Ix, typename Type1, typename ... RemainingTypes>
		void constructAll(AudioProcessor* p)
		{
//			std::get<Ix>(presets) = new typename Type1::Preset(p, std::get<Ix>(type->presetTypes));
			std::get<Ix>(presets).construct(p, std::get<Ix>(type->presetTypes));
			
			constructAll<Ix+1, RemainingTypes...>(p);
		}
		
		template<size_t Ix>
		void constructAll()
		{ }
		template <size_t Ix, typename Type1, typename ... RemainingTypes>
		void constructAll()
		{
			std::get<Ix>(presets).construct(std::get<Ix>(type->presetTypes));
			
			constructAll<Ix+1, RemainingTypes...>();
		}
	public:
		PresetList()
		: type(NULL), bypass(false), name()
		{
		}
		void construct(AudioProcessor* p, std::shared_ptr<PresetTypeList>& pr)
		{
			type = pr;
			constructAll<0, Presets...>(p);
		}
		void construct(std::shared_ptr<PresetTypeList>& pr)
		{
			type = pr;
			constructAll<0, Presets...>();
		}
		
		
		void copyValue( PresetList& other)
		{
			looper::zip([&] (auto& myVal, auto& hisVal, size_t index) noexcept
			{
				myVal.copyValue(hisVal);
			}
			, presets, other.presets);
		}
		
		template <int Index> const  auto& get() const	{ return std::get<Index> (presets); }
		template <int Index>		auto& get()    		{ return std::get<Index> (presets); }
		
		template <int Index, int Ix2>
		auto getVal()   const  {
			return std::get<Index> (presets).template getVal<Ix2>();
		}
		
		template <int Index, int Ix2>
		auto& get()  	{
			return std::get<Index> (presets).template get<Ix2>();
		}
		
		template <int Index>	void set(float value)
		{
			getVal<Index>()->set(value);
		}
		template <int Index>	void set(double value)
		{
			getVal<Index>()->set(value);
		}
		template <int Index>	void set(int value)
		{
			getVal<Index>()->set(value);
		}
		template <int Index>	void set(bool value)
		{
			getVal<Index>()->set(value);
		}
		
		void updateBackend()
		{
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				val.updateBackend();
			}, presets);
		}
		
		void setRate(float set)
		{
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				val->setRate();
			}, presets);
		}
		
		inline bool isBypassed() const{
			return bypass;
		}
		void setBypass(bool set) {
			bypass = set;
		}
		bool toggleBypass() {
			bypass = !bypass;
			return bypass;
		}
		
		String getXMLName() const {
			return type->name;
		}
		
		//save
		void save(XmlElement* parent)
		{
			XmlElement* element =new XmlElement(getXMLName());
			parent->addChildElement(element);
			
			element->setAttribute("name", name);
			
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				val.save(element);
			}, presets);
			
			element->setAttribute("bypass", (int)bypass);
		}
		void save(std::unique_ptr<XmlElement> parent)
		{
			XmlElement* element =new XmlElement(getXMLName());
			parent->addChildElement(element);

			element->setAttribute("name", name);
			
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				val.save(element);
			}, presets);
			
			element->setAttribute("bypass", (int)bypass);
		}
		//load
		void load(XmlElement* parent)
		{
			XmlElement* element = parent->getChildByName(getXMLName());
			if (element != NULL)
			{
				name = element->getStringAttribute("name", name);
				
				looper::forEachInTuple([&] (auto& val, int index) noexcept
				{
					val.load(element);
				}, presets);
				bypass = (bool) element->getIntAttribute("bypass", 0);
			}
		}
		void load(std::unique_ptr<XmlElement> parent)
		{
			XmlElement* element = parent->getChildByName(getXMLName());
			if (element != NULL)
			{
				name = element->getStringAttribute("name", name);
				
				looper::forEachInTuple([&] (auto& val, int index) noexcept
				{
					val.load(element);
				}, presets);
				bypass = (bool) element->getIntAttribute("bypass", 0);
			}
		}
		//save
		void saveToPreset(XmlElement* parent)
		{
			XmlElement* element =new XmlElement(getXMLName());
			parent->addChildElement(element);
			
			element->setAttribute("name", name);
			
//			std::cout<<"saving..."<<std::endl;
			
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				val.saveToPreset(element);
			}, presets);
			
			element->setAttribute("bypass", (int)bypass);
		}
		void saveToPreset(std::unique_ptr<XmlElement> parent)
		{
			XmlElement* element =new XmlElement(getXMLName());
			parent->addChildElement(element);

			element->setAttribute("name", name);
			
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				val.saveToPreset(element);
			}, presets);
			
			element->setAttribute("bypass", (int)bypass);
		}
		//load
		void loadPreset(XmlElement* parent)
		{
//			std::cout<<"loading..."<<std::endl;
			
			XmlElement* element = parent->getChildByName(getXMLName());
			if (element != NULL)
			{
				name = element->getStringAttribute("name", name);
				
				looper::forEachInTuple([&] (auto& val, int index) noexcept
				{
					val.loadPreset(element);
				}, presets);
				bypass = (bool) element->getIntAttribute("bypass", 0);
			}
		}
		void loadPreset(std::unique_ptr<XmlElement> parent)
		{
//			std::cout<<"loading unique..."<<std::endl;
			
			XmlElement* element = parent->getChildByName(getXMLName());
			if (element != NULL)
			{
				name = element->getStringAttribute("name", name);
				
				looper::forEachInTuple([&] (auto& val, int index) noexcept
				{
					val->loadPreset(element);
				}, presets);
				bypass = (bool) element->getIntAttribute("bypass", 0);
			}
		}
		
		void manualDelete()
		{
			//Call if preset is not added to AudioProcessor
			looper::forEachInTuple([&] (auto& preset, size_t index) noexcept
			{
				preset.manualDelete();
			}
			, presets);
		}
		virtual ~PresetList() {
			looper::forEachInTuple([&] (auto& val, size_t index) noexcept
			{
				//AudioProcessor deletes this i guess
				//TODO: better object ownership - add clear method to audioprocessor
//				delete val;
			}
			, presets);
		}
		
		std::shared_ptr<PresetTypeList> getType() {
			return type;
		}
		void setType(std::shared_ptr<PresetTypeList> set) {
			type = set;
			
			looper::zip([&] (auto& param, auto& val, size_t index) noexcept
			{
				val.setType(param);
			}
			, type->presetTypes, presets);
		}
		void setTypeKeepUi(std::shared_ptr<PresetTypeList> set) {
			type = set;
			
			looper::zip([&] (auto& param, auto& val, size_t index) noexcept
			{
				val->setTypeKeepUi(&param);
			}
			, type->presetTypes, presets);
		}
		
		void setType(PresetTypeList set) {
			type = std::make_shared<PresetTypeList<Presets...>> (set);
			
			looper::zip([&] (auto& param, auto& val, size_t index) noexcept
			{
				val.setType(param);
			}
			, type->presetTypes, presets);
		}
		void setTypeKeepUi(PresetTypeList set) {
			type = std::make_shared<PresetTypeList<Presets...>> (set);
			
			looper::zip([&] (auto& param, auto& val, size_t index) noexcept
			{
				val->setTypeKeepUi(&param);
			}
			, type->presetTypes, presets);
		}
		
		void setName(String set) {
			name = set;
		}
		String getName()const {
			return name;
		}
		
		
	protected:
		std::shared_ptr<PresetTypeList> type;
		std::tuple< typename Presets::Preset...> presets;
		bool bypass;
		String name="";
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetList);
	};
	typedef PresetList Preset;
};

