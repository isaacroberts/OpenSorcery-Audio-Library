//
//  BasicFXProcessor.h
//  
//
//  Created by Isaac Roberts on 3/22/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class GuiEditor;

class BasicFXProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
 , public juce::AudioProcessorARAExtension
#endif

{
public:
	BasicFXProcessor();
	BasicFXProcessor(BusesProperties customBusProperties);
	

	
#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
	
//	==============================================================================
//	GuiEditor* createEditor() = 0;
	
	bool hasEditor() const override;
	GuiEditor* getGuiEditor();
	
	virtual bool hasPreset()=0;
	
	virtual bool canSidechain() { return false; }
	
	//==============================================================================
	const juce::String getName() const override;
	const juce::String getXmlName();
	
	
	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;
	
	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int index) override;
	const juce::String getProgramName (int index) override;
	void changeProgramName (int index, const juce::String& newName) override;

//	bool isBusesLayoutSupported (const BusesLayout& layouts) const;

	
	virtual void clearUiCallbacks() {}
	
	virtual ~BasicFXProcessor();
	
protected:
	String xmlTag = "";
	
	
protected:
	std::unique_ptr<XmlElement> uiXml=nullptr;
	
public:
	bool hasUiXml() { return uiXml!=nullptr; }
	
	
	
	auto getUiXml() { return std::move(uiXml); }
	void setUiXml(std::unique_ptr<XmlElement> set) {
		uiXml = std::move(set);
	}
	void setUiXml(XmlElement* set) {
		uiXml = std::unique_ptr<XmlElement>(set);
	}
	//Ensures tag name is standardized
	XmlElement* createUiXml() {
		return new XmlElement("UI");
	}

	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicFXProcessor);
};
