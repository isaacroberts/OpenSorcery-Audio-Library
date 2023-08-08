//
//  Parameter<bool>.h
//  
//
//  Created by Isaac Roberts on 8/7/22.
//

// Created by Isaac Roberts 


//class Parameter<bool> : public ParameterBase, public PtrListenerList<bool>
template<>
class Parameter<bool> : public ParameterBase, public PtrListenerList<bool>
{
public:
	Parameter<bool>(String name, bool default_);
	Parameter<bool>(bool default_);
	Parameter<bool>();
	
	void setInfo(String id, bool def) {
		name = id;
		this->def = def;
	}
	inline void setDefault(bool set) { def = set; }

	
protected:
	bool value;
	bool def;
	String name;
public:
	std::function<void(bool)> callback;
	void setCallback(std::function<void(bool)> clbk) {
		callback = clbk;
	}

public:
	virtual ~Parameter<bool>() {}
	
	operator bool() const { return value; }

	float getValue() const override {
		return value;
	}
	void setValue (float newValue) override {
		value = newValue > .5f;
	}
	float getDefaultValue() const override {
		return def;
	}
	
	String getName(int maxLength) const override {
		return name.substring(0, maxLength);
	}
	String getFullName() const override {
		return name;
	}

	//Units. "Hz" or "%"
	String getLabel() const override {
		return "";
	}
	float getValueForText(const String& text) const override {
		return TypeHandling::getValueForText<bool>(text);
	}
	String getIdForSave() const override {
		return name.replace(" ", "");
	}

	void set(bool value, void* src=NULL);
	
	void connectButton(Button* button, bool flip=false);
	void connectButton(Button& button, bool flip=false) { connectButton(&button, flip); }

	void save(XmlElement* element) const override;
	void load(XmlElement* element) override;
	void loadAsDefault(XmlElement* element) override;

	void updateBackend() override;
	void updateFrontend(void* source) override {
		updateParamListeners(source);
	}
	void clearBackendCallbacks() override {
		callback = [&](bool) {};
	}
	void clearFrontendCallbacks() override {
		clearParamListeners();
	}
	
	void resetToDefault() override {
		set(def);
	}
	void randomize() override {
		value = (rand() % 2 == 0);
		_update();
	}

protected:
	
	void _update(void* source=NULL);
	
	//uses type specific
	inline void notify() {
		sendValueChangedMessageToListeners(value);
	}
	
	bool getValueForListenerList() override { return value; }
private:
	Parameter<bool>(const Parameter<bool>& other) = delete;
	Parameter<bool>& operator=(const Parameter<bool>& other) = delete;
	
	
};


typedef Parameter<bool> BoolParameter;
