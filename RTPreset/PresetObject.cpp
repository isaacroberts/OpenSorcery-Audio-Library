//
//  PresetObject.cpp
//  
//
//  Created by Isaac Roberts on 8/7/22.
//

// Created by Isaac Roberts

PresetObject::PresetObject()
{
	
}
//Destructor
PresetObject::~PresetObject()
{
	
}

void PresetObject::addToProcessor(AudioProcessor* proc)
{
	int pamt=getParamAmt();
	for (int n=0; n < pamt; ++n)
	{
		auto* p = getParam(n);
		if (p->shouldAutomate())
			proc->addParameter(p);
	}
	int subp = getSubPresetAmt();
	for (int n=0; n < subp; ++n)
	{
		getSubPreset(n)->addToProcessor(proc);
	}
}

#define po_func(prName, paName) \
void PresetObject::prName() \
{ \
	int pamt=getParamAmt(); \
	for (int n=0; n < pamt; ++n) {\
		getParam(n)->paName(); \
	} \
	int subp = getSubPresetAmt(); \
	for (int n=0; n < subp; ++n) { \
		getSubPreset(n)->prName(); \
	} \
}

po_func(updateBackend, updateBackend)
po_func(updateFrontend, updateFrontend)
po_func(clearBackendCallbacks, clearBackendCallbacks)
po_func(clearFrontendCallbacks, clearFrontendCallbacks)
//po_func(resetToDefaults, resetToDefault)

#undef po_func


void PresetObject::randomize()
{
	int pamt=getParamAmt();
	for (int n=0; n < pamt; ++n)
	{
		auto p = getParam(n);
		if (p->shouldRandomize())
			p->randomize();
	}
	int subp = getSubPresetAmt();
	for (int n=0; n < subp; ++n)
	{
		getSubPreset(n)->randomize();
	}
}
void PresetObject::resetToDefaults()
{
	int pamt=getParamAmt();
	for (int n=0; n < pamt; ++n)
	{
		auto p = getParam(n);
		if (p->shouldRandomize())
			p->resetToDefault();
	}
	int subp = getSubPresetAmt();
	for (int n=0; n < subp; ++n)
	{
		getSubPreset(n)->resetToDefaults();
	}
}



void PresetObject::printPreset()
{
	jassertfalse;
	std::cout<<getXmlName()<<std::endl;
	int pamt=getParamAmt();
	for (int n=0; n < pamt; ++n)
	{
		auto p = getParam(n);
		std::cout<<p->getName(10)<<"="<< p->getValue()<<", ";
	}
	int subp = getSubPresetAmt();
	for (int n=0; n < subp; ++n)
	{
		getSubPreset(n)->printPreset();
	}
	std::cout<<std::endl;
	
}

void PresetObject::save(XmlElement* parent, bool isPreset)
{
	XmlElement* element =new XmlElement(getXmlName());
	parent->addChildElement(element);
			
	int pamt=getParamAmt();
	for (int n=0; n < pamt; ++n)
	{
		auto p = getParam(n);
		if (!isPreset || p->savesToPreset())
			p->save(element, isPreset);
	}
	int subp = getSubPresetAmt();
	for (int n=0; n < subp; ++n)
	{
		getSubPreset(n)->save(element, isPreset);
	}
	
}

//load
void PresetObject::load(XmlElement* parent, bool isPreset)
{
	XmlElement* element = parent->getChildByName(getXmlName());
	if (element != NULL)
	{
		int pamt=getParamAmt();
		for (int n=0; n < pamt; ++n)
		{
			auto p = getParam(n);
			if (!isPreset || p->savesToPreset())
				p->load(element, isPreset);
		}
		int subp = getSubPresetAmt();
		for (int n=0; n < subp; ++n)
		{
			getSubPreset(n)->load(element, isPreset);
		}
	}
}
void PresetObject::loadAsDefault(XmlElement* parent)
{
	XmlElement* element = parent->getChildByName(getXmlName());
	if (element != NULL)
	{
		int pamt=getParamAmt();
		for (int n=0; n < pamt; ++n)
		{
			auto p = getParam(n);
			p->loadAsDefault(element);
		}
		int subp = getSubPresetAmt();
		for (int n=0; n < subp; ++n)
		{
			getSubPreset(n)->loadAsDefault(element);
		}
	}
}

void PresetObject::save(std::unique_ptr<XmlElement> parent, bool isPreset)
{
	save(&*parent, isPreset);
}

void PresetObject::load(std::unique_ptr<XmlElement> parent, bool isPreset)
{
	load(&*parent, isPreset);
}
void PresetObject::loadAsDefault(std::unique_ptr<XmlElement> parent)
{
	loadAsDefault(&*parent);
}

void PresetObject::loadAsDefault(const char* const xmlData)
{
	std::unique_ptr<XmlElement> xml = juce::XmlDocument::parse(xmlData);
	loadAsDefault(std::move(xml));
}


void PresetObject::assertAllNamed()
{
#if DEBUG
	int pamt=getParamAmt();
	for (int n=0; n < pamt; ++n)
	{
		ParameterBase* p = getParam(n);
		if (p->getIdForSave()=="")
		{
			//So you can debug;
			p = getParam(n);
			std::cout<<p->getIdForSave()<<std::endl;
			jassertfalse;
		}
		if (p->getName(5)=="")
		{
			//So you can debug;
			p = getParam(n);
			std::cout<<p->getName(5)<<std::endl;
			jassertfalse;
		}
	}
	int subp = getSubPresetAmt();
	for (int n=0; n < subp; ++n)
	{
		getSubPreset(n)->assertAllNamed();
	}
#endif
}

#if DEBUG
void PresetObject::saveAutomatableParamsToDesktop()
{
	StringArray params;
	_gatherAutomatableParams(params);
	
	File file = File::getSpecialLocation(File::userDesktopDirectory).getChildFile("automated_params.txt");
	
	file.replaceWithText (params.joinIntoString("\n"), false, false, "\n");

}
void PresetObject::_gatherAutomatableParams(StringArray& names)
{
	int pamt=getParamAmt();
	for (int n=0; n < pamt; ++n)
	{
		auto p = getParam(n);
		if (p->shouldAutomate())
		{
			names.add(p->getFullName()+";"+getXmlName());
		}
	}
	int subp = getSubPresetAmt();
	for (int n=0; n < subp; ++n)
	{
		getSubPreset(n)->_gatherAutomatableParams(names);
	}
}

#endif 

//void PresetObject::updateBackend()
//{
//	int pamt=getParamAmt();
//	for (int n=0; n < pamt; ++n)
//	{
//		getParam(n)->updateBackend();
//	}
//	int subp = getSubPresetAmt();
//	for (int n=0; n < subp; ++n)
//	{
//		getSubPreset(n)->updateBackend();
//	}
//}
//void PresetObject::updateFrontend()
//{
//	int pamt=getParamAmt();
//	for (int n=0; n < pamt; ++n)
//	{
//		getParam(n)->updateFrontend();
//	}
//	int subp = getSubPresetAmt();
//	for (int n=0; n < subp; ++n)
//	{
//		getSubPreset(n)->updateFrontend();
//	}
//}
//
//void PresetObject::clearBackendCallbacks()
//{
//	int pamt=getParamAmt();
//	for (int n=0; n < pamt; ++n)
//	{
//		getParam(n)->clearBackendCallbacks();
//	}
//	int subp = getSubPresetAmt();
//	for (int n=0; n < subp; ++n)
//	{
//		getSubPreset(n)->clearBackendCallbacks();
//	}
//}
//void PresetObject::clearFrontendCallbacks()
//{
//	int pamt=getParamAmt();
//	for (int n=0; n < pamt; ++n)
//	{
//		getParam(n)->clearFrontendCallbacks();
//	}
//	int subp = getSubPresetAmt();
//	for (int n=0; n < subp; ++n)
//	{
//		getSubPreset(n)->clearFrontendCallbacks();
//	}
//}
//void PresetObject::randomize()
//{
//	int pamt=getParamAmt();
//	for (int n=0; n < pamt; ++n)
//	{
//		getParam(n)->randomize();
//	}
//	int subp = getSubPresetAmt();
//	for (int n=0; n < subp; ++n)
//	{
//		getSubPreset(n)->randomize();
//	}
//}
//void PresetObject::resetToDefaults()
//{
//	int pamt=getParamAmt();
//	for (int n=0; n < pamt; ++n)
//	{
//		getParam(n)->resetToDefault();
//	}
//	int subp = getSubPresetAmt();
//	for (int n=0; n < subp; ++n)
//	{
//		getSubPreset(n)->resetToDefaults();
//	}
//}
