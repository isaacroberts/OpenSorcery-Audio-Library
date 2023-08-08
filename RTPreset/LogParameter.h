/*
	LogParameter.h
 
	LogParameter: Created by Isaac Roberts on Sept 17 2022
 
 */



class LogParameter : public ParameterBase, public PtrListenerList<float>
{
public:
	/* Constructor
	 LogParameter(String name, float low, float high, float def);
	 LogParameter(float low, float high, float def);
	 LogParameter();
	 
	 void setInfo(String name, float low, float high, float def);

	 */
	
	
	//Accessors
	
	float get() const {return logval;}
	float getVal() const {return logval;}

	operator float() const { return logval; }
	
	inline float getLow() const {return low;}
	inline float getHigh() const {return high;}
	inline float getDefault() const {return def;}

	inline void setRange(float from, float to) {
		low = from;
		high = to;
	}
	inline void setDefault(float set_) {
		def = set_;
		
		logval = def;
		updatePow();
		flatValue = normalize(logval);
	}
	
	void setInfo(String name_, float low_, float high_, float def_) {
		name = name_;
		low = log2(low_);
		high = log2(high_);
		def = log2(def_);
		
		logval = def;
		updatePow();
		flatValue = normalize(logval);
	}
	

	
	void setCallback(std::function<void(float)> clbk) {
		callback = clbk;
	}
	
protected:
	//Member Variables
	float low, high, def;
	float logval;
	float powval;
	
	float flatValue;
	
	String name;
	
public:
	std::function<void(float)> callback;
	
public:
	LogParameter() :
	low(0), high(10), def(0),
	logval(0), powval(1), flatValue(0),
	name(""),
	callback([&](float v) { } )
	{
		
	}
	LogParameter(float low_, float high_, float def_) :
	low(log2(low_)), high(log2(high_)), def(log2(def_)),
	logval(def), powval(def_),
	name(""),
	callback([&](float v) { } )
	{
		flatValue = normalize(def_);
	}
	
	LogParameter(String name_, float low_, float high_, float def_) :
	low(log2(low_)), high(log2(high_)), def(log2(def_)),
	logval(def), powval(def_),
	name(name_),
	callback([&](float v) { } )
	{
		flatValue = normalize(def_);
	}

	virtual ~LogParameter() {
		
	}

	//Overrides
	float getValue() const override {
		return flatValue;
	}
	void setValue(float nrm) override {
		if (flatValue != nrm)
		{
			flatValue = nrm;
			logval = denormalize(flatValue);
			updatePow();
			callCallback();
			callUiCallback(NULL);
	//		notify();
		}
	}
	float getDefaultValue() const override {
		return normalize(getDefault());
	}

	String getFullName() const override  {return name; }

	String getName(int maxLength) const override {
		return name.substring(0, maxLength);
	}

	//Units. "Hz" or "%"
	String getLabel() const override {
		return "";
	}
	float getValueForText(const String& text) const override {
		return TypeHandling::getValueForText<float>(text);
	}

	//Connectors
	void connectSlider(Slider* slider, RTPreset::SliderResetMethod resetMethod)
	{
//		if (interal < 0)
//			interval = getDefaultInterval();
		int numDec = slider->getNumDecimalPlacesToDisplay();
		slider->setRange(low, high, 0);
		RTPreset::setSliderResetMethod(slider, resetMethod, def);
		slider->setNumDecimalPlacesToDisplay(numDec);
		slider->onValueChange = [=]() {this->set(slider->getValue(), slider);};
		auto cbk = [=](float v) {
			slider->setValue(v, dontSendNotification);
		};
		slider->textFromValueFunction = [&](double s) {
			if (s<=-1000)
				return String("0");
			return String(pow(2., s));
		};
		slider->valueFromTextFunction = [&](String s) {
			float v = s.getFloatValue();
			if (v<=0)
				return low;
			return log2(v);
		};
		addParamListener(slider, cbk, true);
	}

	void connectSlider(Slider* slider) {
		connectSlider(slider, RTPreset::SliderResetMethod::None);
	}
	void connectSlider(Slider& slider) {
		connectSlider(&slider, RTPreset::SliderResetMethod::None);
	}

	
	void connectSliderMin(Slider* slider, double interval=0)
	{
		int numDec = slider->getNumDecimalPlacesToDisplay();
		if (interval < 0)
			interval = getDefaultInterval();
		slider->setRange(low, high, interval);
//		RTPreset::setSliderResetMethod(slider, resetMethod, def);
		slider->setNumDecimalPlacesToDisplay(numDec);

		slider->onValueChange = [=]() {this->set(slider->getMinValue(), slider);};
		addParamListener(slider, [=](float v) {slider->setMinValue(v, dontSendNotification, true);}, true);
	}
	void connectSliderMax(Slider* slider)  {
		slider->onValueChange = [=]() {this->set(slider->getMaxValue(), slider);};
		addParamListener(slider, [=](float v) {slider->setMaxValue(v, dontSendNotification, true);}, true);
	}
	void connectComboBox(ComboBox* combo) {
		combo->onChange = [=]() { this->set(combo->getSelectedId()-1, combo); };
		addParamListener(combo, [=](float v) {
			combo->setSelectedId(logval+1,  dontSendNotification);
		}, true);
	};
	void connectPopupSelector(PopupSelector* selector) {
		selector->onChange = [=]() { this->set(selector->getSelectedId()-1, selector); };
		addParamListener(selector, [=](float v) {
			selector->setSelectedId(logval+1,  dontSendNotification);
		}, true);
	};
	void connectTextEditor(TextEditor* editor)  {
		addParamListener(editor, [=](float v) {
			editor->setText(TypeHandling::valueToString<float>(logval), false);
			}, true);
		
		editor->onReturnKey = [=]() {
			if (editor->isEmpty()) {
				snapToDefault(editor);
				editor->setText(TypeHandling::valueToString<float>(logval), false);
			}
			else
				set(TypeHandling::getValueForText<float>(editor->getText()), editor);
			editor->moveKeyboardFocusToSibling(true);
		};
		editor->onFocusLost = [=]() {
			editor->setText(TypeHandling::valueToString<float>(logval), false);
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
		element->setAttribute(getIdForSave(), logval);
	}
	void load(XmlElement* element) override
	{
		logval = TypeHandling::loadVal(element, getIdForSave(), def);
		bound(&logval);
		updatePow();
		flatValue = normalize(logval);
		_update();
	}
	void loadAsDefault(XmlElement* element) override
	{
		def = TypeHandling::loadVal(element, getIdForSave(), def);
		bound(&def);
		logval = def;
		updatePow();
		flatValue = normalize(logval);
		_update();
	}


	void updateBackend() override {
		callback(powval);
	}
	void resetToDefault() override
	{
		snapToDefault(NULL);
	}
	void randomize() override
	{
		logval = TypeHandling::randomInRange(low, high);
		bound(&logval);
		updatePow();
		flatValue = normalize(logval);
		_update();
	}
	
	//Utility
	float normalize(float logval) const { return TypeHandling::normalize<float>(logval, low, high);}
	float denormalize(float flat_value) const { return TypeHandling::denormalize<float>(flat_value, low, high);}

	void updatePow() {
		powval = std::pow(2.f, logval);
	}
	
	float bound(float logval) const
	{
		if (logval < low)
			return low;
		if (logval > high)
			return high;
		return logval;
	}
	float* bound(float* logval) const
	{
		if (*logval < low)
			*logval = low;
		else if (*logval > high)
			*logval = high;

		return logval;
	}
	float wrap(float nv) const
	{
		return TypeHandling::modRange(nv, low, high);
//		return TypeHandling::modulus(nv-low, high-low)+low;
	}
	
	double getDefaultInterval() {return TypeHandling::getDefaultInterval<float>(low, high);}
	
	
public:
	/* ---- Internal ---- */
	
	void set(float set, void* source=NULL) {
		if (logval != set)
		{
			logval = bound(set);
			updatePow();
			flatValue = normalize(logval);
			_update(source);
		}
	}
	// --- Intended for Int but allowed for float -----
	//Stops at boundaries
	float incrementWall(bool up, void* source=NULL) {
		logval = bound(logval + (up ? 1 : -1));
		updatePow();
		flatValue = normalize(logval);
		_update(source);
		return logval;
	}
	//Wraps around at boundaries
	float incrementWrap(bool up, void* source=NULL) {
		logval = wrap(logval + (up ? 1 : -1));
		updatePow();
		flatValue = normalize(logval);
		_update(source);
		return logval;
	}
	float addWall(float add, void* source=NULL) {
		logval = bound(logval + add);
		updatePow();
		flatValue = normalize(logval);
		_update(source);
		return logval;
	}
	//Wraps around at boundaries
	float addWrap(float add, void* source=NULL) {
		logval = wrap(logval + add);
		updatePow();
		flatValue = normalize(logval);
		_update(source);
		return logval;
	}
	
	inline void setFromGraphics(float set, void* source=NULL) {
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
		callCallback();
		callUiCallback(source);
		notify();
	}
	float getValueForListenerList() override { return logval;}

public:
	//UI Handling
	void updateFrontend(void* source) override {
		PtrListenerList<float>::updateParamListeners(source);
	}
	void clearBackendCallbacks() override {
		callback = [&](float v) { };
	}
	void clearFrontendCallbacks() override {
		PtrListenerList<float>::clearParamListeners();
	}

	
	void addParamListener(void* id, std::function<void(float)> callback, bool callNow=true) {
		PtrListenerList<float>::addParamListener(id, callback, callNow);
	}
	void removeParamListener(void* id) {
		PtrListenerList<float>::removeParamListener(id);
	}
	void clearParamListeners() {
		PtrListenerList<float>::clearParamListeners();
	}
	void updateParamListeners(void* source) {
		PtrListenerList<float>::updateParamListeners(source);
	}
	
	//uses type specific
	inline void notify() {
		sendValueChangedMessageToListeners(flatValue);
	}
	
private:
	LogParameter(const LogParameter& other) = delete;
	LogParameter& operator=(const LogParameter& other) = delete;
	
};
