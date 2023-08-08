//
//  Parameter.h
//  
//
//  Created by Isaac Roberts on 8/7/22.
//

// Created by Isaac Roberts 


#ifndef RT_TYPEDEF_INCLUDED 
#error "includes missing"
#endif 

template <class Type>
class Parameter : public ParameterBase, public PtrListenerList<Type>
{
public:
	/* Constructor
	 Parameter(String name, Type low, Type high, Type def);
	 Parameter(Type low, Type high, Type def);
	 Parameter();
	 
	 void setInfo(String name, Type low, Type high, Type def);

	 */
	
	
	//Accessors
	
	Type get() const {return value;}
	Type getVal() const {return value;}

	operator Type() const { return value; }
	
	inline Type getLow() const {return low;}
	inline Type getHigh() const {return high;}
	inline Type getDefault() const {return def;}

	inline void setRange(Type from, Type to) {
		low = from;
		high = to;
	}
	inline void setDefault(Type set_) {
		def = set_;
		
		value = def;
		flatValue = normalize(value);
	}
	
	void setInfo(String name_, Type low_, Type high_, Type def_) {
		name = name_;
		low = low_;
		high = high_;
		def = def_;
		
		value = def;
		flatValue = normalize(value);
	}
	
	void setCallback(std::function<void(Type)> clbk) {
		callback = clbk; 
	}
	
protected:
	//Member Variables
	Type low, high, def;
	Type value;
	float flatValue;
	
	String name;
	
public:
	std::function<void(Type)> callback;
	
public:
	Parameter() :
	low(0), high(100), def(0),
	value(0), flatValue(0),
	name(""),
	callback([&](Type v) { } )
	{
		
	}
	Parameter(Type low_, Type high_, Type def_) :
	low(low_), high(high_), def(def_),
	value(def_),
	name(""),
	callback([&](Type v) { } )
	{
		flatValue = normalize(def_);
	}
	
	Parameter(String name_, Type low_, Type high_, Type def_) :
	low(low_), high(high_), def(def_),
	value(def_), name(name_),
	callback([&](Type v) { } )
	{
		flatValue = normalize(def_);
	}

	virtual ~Parameter() {
		
	}

	//Overrides
	float getValue() const override {
		return flatValue;
	}
#if !TARGET_WIN

	void setValue(float nrm) override {

		if (flatValue != nrm)
		{
			flatValue = nrm;
			Type nv = denormalize(flatValue);
			if (value != nv)
			{
				value = nv;
				callCallback();
				callUiCallback(NULL);
			}
	//		notify();
		}
	}
#else 
	void setValue(float nrm) override {
		if constexpr (std::is_same<Type, int>::value)
		{
			flatValue = nrm;
			int nv = denormalize(flatValue);
			if (value != nv)
			{
				value = nv;
				callCallback();
				callUiCallback(NULL);
			}
		}
		else
		{
			if (flatValue != nrm)
			{
				flatValue = nrm;
				Type nv = denormalize(flatValue);
				if (value != nv)
				{
					value = nv;
					callCallback();
					callUiCallback(NULL);
				}
			}
		}
	}

#endif 
	float getDefaultValue() const override {
		return normalize(getDefault());
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
		return TypeHandling::getValueForText<Type>(text);
	}

	//Connectors
	void connectSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod, double interval=0)
	{
//		if (interal < 0)
//			interval = getDefaultInterval();
		int numDec = slider->getNumDecimalPlacesToDisplay();
		if (std::is_same<Type, int>::value)//if is int
		{
			if (interval < 1)
				interval = 1;
			numDec = 0;
		}
		slider->setRange(low, high, interval);
		RTPreset::setSliderResetMethod(slider, resetMethod, def);
		slider->setNumDecimalPlacesToDisplay(numDec);
		slider->onValueChange = [=]() {this->set(slider->getValue(), slider);};
		auto cbk = [=](Type v) {
			slider->setValue(v, dontSendNotification);
		};
		addParamListener(slider, cbk, true);
	}

	void connectSlider(Slider* slider) {
		connectSlider(slider, RTPreset::SliderResetMethod::None);
	}
	void connectSlider(Slider& slider) {
		connectSlider(&slider, RTPreset::SliderResetMethod::None);
	}
	void connectSlider(Slider& slider, RTPreset::SliderResetMethod resetMethod, double interval=0) {
		connectSlider(&slider, resetMethod, interval);
	}

	
	void connectSliderMin(Slider* slider, double interval=0)
	{
		int numDec = slider->getNumDecimalPlacesToDisplay();
		if (std::is_same<Type, int>::value)//if is int
		{
			if (interval < 1)
				interval = 1;
			numDec = 0;
		}
		if (interval < 0)
			interval = getDefaultInterval();
		slider->setRange(low, high, interval);
//		RTPreset::setSliderResetMethod(slider, resetMethod, def);
		if (std::is_same<Type, int>::value)//if is Int
			slider->setNumDecimalPlacesToDisplay(0);
		else
			slider->setNumDecimalPlacesToDisplay(numDec);

		slider->onValueChange = [=]() {this->set(slider->getMinValue(), slider);};
		addParamListener(slider, [=](Type v) {slider->setMinValue(v, dontSendNotification, true);}, true);
	}
	void connectSliderMax(Slider* slider)  {
		slider->onValueChange = [=]() {this->set(slider->getMaxValue(), slider);};
		addParamListener(slider, [=](Type v) {slider->setMaxValue(v, dontSendNotification, true);}, true);
	}
	void connectComboBox(ComboBox* combo) {
		combo->onChange = [=]() { this->set(combo->getSelectedId()-1, combo); };
		addParamListener(combo, [=](Type v) {
			combo->setSelectedId(value+1,  dontSendNotification);
		}, true);
	};
	void connectPopupSelector(PopupSelector* selector) {
		selector->onChange = [=]() { this->set(selector->getSelectedId()-1, selector); };
		addParamListener(selector, [=](Type v) {
			selector->setSelectedId(value+1,  dontSendNotification);
		}, true);
	};
	void connectTextEditor(TextEditor* editor)  {
		addParamListener(editor, [=](Type v) {
			editor->setText(TypeHandling::valueToString<Type>(value), false);
			}, true);
		
		editor->onReturnKey = [=]() {
			if (editor->isEmpty()) {
				snapToDefault(editor);
				editor->setText(TypeHandling::valueToString<Type>(value), false);
			}
			else
				set(TypeHandling::getValueForText<Type>(editor->getText()), editor);
			editor->moveKeyboardFocusToSibling(true);
		};
		editor->onFocusLost = [=]() {
			editor->setText(TypeHandling::valueToString<Type>(value), false);
		};
		editor->onEscapeKey = [=]() {
			//triggers onFocusLost command
			editor->moveKeyboardFocusToSibling(true);
		};
	}
	
	String getIdForSave() const override {
		return name.replace(" ", "");
	}
	
	void save(XmlElement* element) const override
	{
		element->setAttribute(getIdForSave(), value);
	}
	void load(XmlElement* element) override
	{
		value = TypeHandling::loadVal(element, getIdForSave(), def);
		bound(&value);
		flatValue = normalize(value);
		_update();
	}
	void loadAsDefault(XmlElement* element) override
	{
		def = TypeHandling::loadVal(element, getIdForSave(), def);
		bound(&def);
		value = def;
		flatValue = normalize(value);
		_update();
	}


	void updateBackend() override {
		callback(value);
	}
	void resetToDefault() override
	{
		snapToDefault(NULL);
	}
	void randomize() override
	{
		value = TypeHandling::randomInRange(low, high);
		bound(&value);
		flatValue = normalize(value);
		_update();
	}
	
	//Utility
	float normalize(Type value) const { return TypeHandling::normalize<Type>(value, low, high);}
	Type denormalize(float flat_value) const { return TypeHandling::denormalize<Type>(flat_value, low, high);}

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
	Type wrap(Type nv) const
	{
		return TypeHandling::modRange(nv, low, high);
//		return TypeHandling::modulus(nv-low, high-low)+low;
	}
	
	double getDefaultInterval() {return TypeHandling::getDefaultInterval<Type>(low, high);}
	
	
public:
	/* ---- Internal ---- */
	
	void set(Type set, void* source=NULL) {
		if (value != set)
		{
			value = bound(set);
			flatValue = normalize(value);
			_update(source);
		}
	}
	// --- Intended for Int but allowed for float -----
	//Stops at boundaries
	Type incrementWall(bool up, void* source=NULL) {
		value = bound(value + (up ? 1 : -1));
		flatValue = normalize(value);
		_update(source);
		return value;
	}
	//Wraps around at boundaries
	Type incrementWrap(bool up, void* source=NULL) {
		value = wrap(value + (up ? 1 : -1));
		flatValue = normalize(value);
		_update(source);
		return value;
	}
	Type addWall(Type add, void* source=NULL) {
		value = bound(value + add);
		flatValue = normalize(value);
		_update(source);
		return value;
	}
	//Wraps around at boundaries
	Type addWrap(Type add, void* source=NULL) {
		value = wrap(value + add);
		flatValue = normalize(value);
		_update(source);
		return value;
	}
	
	inline void setFromGraphics(Type set, void* source=NULL) {
		this->set(set, source);
	}
	void snapToDefault(void* source=NULL) {
		set(getDefault(), source);
	}
	
	float getFlatValue() { return flatValue; }
	float getFlatValue(float v) {
		return normalize(v);
	}

protected:
	void _update(void* source=NULL)
	{
//		std::cout<<getIdForSave()<<" "<<value<<std::endl;
		
		callCallback();
		callUiCallback(source);
		notify();
	}
	Type getValueForListenerList() override { return value;}

public:
	//UI Handling
	void updateFrontend(void* source) override {
		PtrListenerList<Type>::updateParamListeners(source);
	}
	void clearBackendCallbacks() override {
		callback = [&](Type v) { };
	}
	void clearFrontendCallbacks() override {
		PtrListenerList<Type>::clearParamListeners();
	}

	
	void addParamListener(void* id, std::function<void(Type)> callback, bool callNow=true) {
		PtrListenerList<Type>::addParamListener(id, callback, callNow);
	}
	void removeParamListener(void* id) {
		PtrListenerList<Type>::removeParamListener(id);
	}
	void clearParamListeners() {
		PtrListenerList<Type>::clearParamListeners();
	}
	void updateParamListeners(void* source) {
		PtrListenerList<Type>::updateParamListeners(source);
	}
	
	//uses type specific
	inline void notify() {
		sendValueChangedMessageToListeners(flatValue);
	}
	
private:
	Parameter(const Parameter& other) = delete;
	Parameter& operator=(const Parameter& other) = delete;
	
};


