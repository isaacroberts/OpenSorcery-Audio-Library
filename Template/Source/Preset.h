//
//  TemplatePreset.h
//  Template - Shared Code
//
//  Created by Isaac Roberts on 9/19/22.
//

// Created by Isaac Roberts 

#ifndef TemplatePreset_h
#define TemplatePreset_h

#include <JuceHeader.h>

namespace Params {
	enum FParams {
		FParamAmt
	};
	enum IParams {
		IParamAmt
	};
	enum BParams {
		BParamAmt
	};
}

class TemplatePreset : public PresetObject
{
public:
	TemplatePreset();
	
	virtual ~TemplatePreset();
	
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
	
	
	
private:
	TemplatePreset(const TemplatePreset& other)=delete;
	TemplatePreset& operator=(const TemplatePreset& other)=delete;
	
	
};

 
/* // Less involved version
 
class TemplatePreset : public PresetObject {
public:
	TemplatePreset()
	: PresetObject()
	{
	}
	
	
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
	
private:
	TemplatePreset(const TemplatePreset& other)=delete;
	TemplatePreset& operator=(const TemplatePreset& other)=delete;
	
};

 */



#endif /* TemplatePreset_h */
