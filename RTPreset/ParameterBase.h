//
//  Parameter.h
//  
//
//  Created by Isaac Roberts on 7/29/22.
//

// Created by Isaac Roberts 


class ParameterBase : public juce::AudioProcessorParameter
{
public:
	
	/* Overrides
	 
	 float getValue() const override;
	 void setValue (float newValue) override;
	 float getDefaultValue() const override;
	 
	 String getName (int maximumStringLength) const override;
	 //Units. "Hz" or "%"
	 String getLabel() const override;
	 float getValueForText (const String& text) const override;

	 void connectSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod, double interval);
	 void connectSliderMin(Slider* slider, double interval=-1);
	 void connectSliderMax(Slider* slider);
	 void connectButton(Button* button);
	 void connectTextEditor(TextEditor* editor);
	 
	 String getIdForSave() const override;
	 void save(XmlElement* element) const override;
	 void load(XmlElement* element) override;

	 void updateBackend() override;
	 void updateFrontend(void* data) override;

	 void clearBackendCallbacks() override;
	 void clearFrontendCallbacks() override;
	 
	 void resetToDefault() override;
	 void randomize() override;
	 */
	
	
	ParameterBase()
	//I'll have to change this if I ever re-release a plugin with additional parameters
	: juce::AudioProcessorParameter(1),
	saveToPreset(true),
	randomizes(true),
	automated(true)
	{}
		
//	virtual void connectSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod, double interval) {jassertfalse; }
//	virtual void connectSliderMin(Slider* slider, double interval=-1) {jassertfalse; }
//	virtual void connectSliderMax(Slider* slider) {jassertfalse; }
//	virtual void connectButton(Button* button) {jassertfalse; }
//	virtual void connectTextEditor(TextEditor* editor) {jassertfalse; }

	virtual String getIdForSave() const = 0;
	virtual void save(XmlElement* element) const = 0;
	virtual void load(XmlElement* element) = 0;
	//Sets the value as default - for loading customer-provided factory presets 
	virtual void loadAsDefault(XmlElement* element) =0;

	
	virtual void updateBackend() = 0;
	virtual void updateFrontend(void* data) = 0;

	virtual void clearBackendCallbacks() = 0;
	virtual void clearFrontendCallbacks() = 0;
	
	virtual void resetToDefault() = 0;
	virtual void randomize() = 0;
	
	virtual ~ParameterBase() {}
	
	String getName (int maximumStringLength) const override {
		return getIdForSave().substring(maximumStringLength);
	}
	virtual String getFullName() const = 0;

	
	inline void updateFrontend() { updateFrontend(NULL); }
	inline void callCallback () { updateBackend(); }
	inline void callUiCallback(void* data=NULL) { updateFrontend(data); }
	
	bool savesToPreset() const { return saveToPreset;}
	void setSavesToPreset(bool set) {saveToPreset = set;}
	
	bool shouldRandomize() const { return randomizes;}
	void setRandomizes(bool set) {randomizes = set;}
	
	bool shouldAutomate() const { return automated; }
	void setAutomated(bool set) { automated = set; }
	void setAutomates(bool set) { automated = set; }

	void setBehavior(bool savesToPreset, bool randomizes, bool automates) {
		this->saveToPreset = savesToPreset;
		this->randomizes = randomizes;
		this->automated = automates;
	}
	inline void behavior(bool savesToPreset, bool randomizes, bool automates) {
		setBehavior(savesToPreset, randomizes, automates);
	}

	
	void save(XmlElement* element, bool isPreset) const {
		if (isPreset && !saveToPreset)
			return;
		save(element);
	}
	void load(XmlElement* element, bool isPreset) {
		if (isPreset && !saveToPreset)
			return;
		load(element);
	}
	
protected:
	bool saveToPreset, randomizes, automated;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterBase);
	
};


