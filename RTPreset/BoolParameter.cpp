//
//  Parameter<bool>.cpp
//  
//
//  Created by Isaac Roberts on 8/7/22.
//

// Created by Isaac Roberts




Parameter<bool>::Parameter(String name_, bool def_)
: ParameterBase(),
value(def_), def(def_),
name(name_),
callback([&](bool){})
{
	
}

Parameter<bool>::Parameter(bool def_)
: ParameterBase(),
value(def_), def(def_),
name(""),
callback([&](bool){})
{
	
}

Parameter<bool>::Parameter()
: ParameterBase(),
value(false), def(false),
name(""),
callback([&](bool){})
{
	
}

void Parameter<bool>::set(bool set, void* src)
{
	if (set != value)
	{
		value = set;
		_update();
	}
}

void Parameter<bool>::connectButton(Button* button, bool flip)
{
	button->setClickingTogglesState(true);
	if (flip)
	{
		button->onClick = [=]() {this->set(!button->getToggleState(), button);};
		addParamListener(button, [=](bool v) {
			button->setToggleState(!v, dontSendNotification);
		});
	}
	else
	{
		button->onClick = [=]() {this->set(button->getToggleState(), button);};
		addParamListener(button, [=](bool v) {
			button->setToggleState(v, dontSendNotification);
		});
	}
}
void Parameter<bool>::save(XmlElement* element) const
{
	element->setAttribute(getIdForSave(), value);
}
void Parameter<bool>::load(XmlElement* element)
{
	value = element->getBoolAttribute(name, def);
	_update();
}
void Parameter<bool>::loadAsDefault(XmlElement* element)
{
	def = element->getBoolAttribute(name, def);
	value = def;
	_update();
}

void Parameter<bool>::updateBackend()
{
	callback(value);
}

void Parameter<bool>::_update(void* source)
{
//	std::cout<<getIdForSave()<<" "<<value<<std::endl;
	updateBackend();
	updateFrontend(source);
	notify();
}
