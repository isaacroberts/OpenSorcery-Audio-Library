//
//  PresetObject.h
//  
//
//  Created by Isaac Roberts on 8/7/22.
//

// Created by Isaac Roberts 


class PresetObject {
public:
	PresetObject();
	
	virtual ~PresetObject();
	
	
	/* Overrides

 public:
	 Parameter<float>& getFParam(Params::FParams p) { return fParams[p]; }
	 Parameter<int>&   getIParam(Params::IParams p) { return iParams[p]; }
	 Parameter<bool>&  getBParam(Params::BParams p) { return bParams[p]; }
 protected:
	 
	 Parameter<float> fParams[Params::FParamAmt];
	 Parameter<int>   iParams[Params::IParamAmt];
	 Parameter<bool>  bParams[Params::BParamAmt];
	 
	 int getParamAmt() override {
		 return Params::FParamAmt + Params::IParamAmt + Params::BParamAmt;
	 }
	 int getSubPresetAmt() override {
		 return 0;
	 }
	 ParameterBase* getParam(int n) override {
		 if (n<Params::FParamAmt)
			 return &fParams[n];
		 n-=Params::FParamAmt;
		 if (n < Params::IParamAmt)
			 return &iParams[n];
		 n -= Params::IParamAmt;
		 if (n < Params::BParamAmt)
			 return &bParams[n];
		 jassertfalse;
		 return NULL;
	 }
	 PresetObject* getSubPreset(int n) override {
		 return NULL;
	 }
	 
	 void saveExtra(XmlElement* parent, bool isPreset) override {}
	 void loadExtra(XmlElement* parent, bool isPreset) override {}
	 
	 String getXmlName() override {
		 return "Preset";
	 }
	 */
	
	/*
 protected:
	 int getParamAmt() override {
		 return 0;
	 }
	 int getSubPresetAmt() override {
		 return 0;
	 }
	 ParameterBase* getParam(int n) override {
		 return NULL;
	 }
	 PresetObject* getSubPreset(int n) override {
		 return NULL;
	 }
	 
	 void saveExtra(XmlElement* parent, bool isPreset) override {}
	 void loadExtra(XmlElement* parent, bool isPreset) override {}
	 
	 String getXmlName() override {
		 return "Preset";
	 }

	 
	 */
	
	
	inline void clearCallbacks() { clearBackendCallbacks(); }
	inline void clearUiCallbacks() { clearFrontendCallbacks(); }
	
	void clearBackendCallbacks();
	void clearFrontendCallbacks();

	void addToProcessor(AudioProcessor* p);
	void updateBackend();
	void updateFrontend();

	void randomize();
	void resetToDefaults();
	void printPreset();

	void save(std::unique_ptr<XmlElement> parent, bool isPreset);
	void load(std::unique_ptr<XmlElement> parent, bool isPreset);
	void loadAsDefault(std::unique_ptr<XmlElement> parent);

	void save(XmlElement* parent, bool isPreset);
	void load(XmlElement* parent, bool isPreset);
	//There's no isPreset value - if you don't want the value loaded here, exclude it from the xml 
	void loadAsDefault(XmlElement* parent);

	
	void loadAsDefault(const char* const xmlData);

	
#if DEBUG
	//Useful for creating AAX PageTables
	void saveAutomatableParamsToDesktop();
protected:
	void _gatherAutomatableParams(StringArray&);

#endif
	
protected:
	void assertAllNamed();
	
	virtual String getXmlName() = 0;
	
	virtual int getParamAmt() = 0;
	virtual int getSubPresetAmt() = 0;
	virtual ParameterBase* getParam(int n) = 0;
	virtual PresetObject* getSubPreset(int n) = 0;
	
	virtual void saveExtra(XmlElement* parent, bool isPreset) {}
	virtual void loadExtra(XmlElement* parent, bool isPreset) {}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetObject);
};

