//
//  SMParamGroup.cpp
//  
//
//  Created by Isaac Roberts on 8/11/22.
//

// Created by Isaac Roberts


SMParamGroup::SoloParam::SoloParam(SMParamGroup* g)
: group(g),
ct(g->getCt()),
value(-1)
{
	setBehavior(true, false, false);
}
SMParamGroup::MuteParam::MuteParam()
: group(NULL),
n(69),
muted(false),
uiCallback([&](bool){})
{
	setBehavior(true, false, false);
}
void SMParamGroup::MuteParam::setBasics(SMParamGroup* g, int n_) {
	group = g;
	n = n_;
}


SMParamGroup::SMParamGroup(int ct_)
: PresetObject(),
ct(ct_),
solo(this)
{
	mute = new MuteParam[ct];
	for (int n=0; n < ct; ++n)
	{
//		mute[n] = new MuteParam();
		mute[n].setBasics(this, n);
	}
	
	soloCallbacks = new std::function<void(bool)>[ct];
	for (int n=0; n < ct; ++n)
	{
		soloCallbacks[n] = [&](bool){};
	}
}

bool SMParamGroup::isMuted(int n)
{
	return (solo!=-1 && solo!=n) || mute[n];
}

//void SMParamGroup::addToProcessor(AudioProcessor* p)
//{
//	//Intentionally do nothing 
//}

void SMParamGroup::connectSoloButton(int n, Button* button, int radioId, bool flip)
{
//	button->setRadioGroupId(radioId);
	button->setClickingTogglesState(true);
	if (flip)
	{
		button->onClick = [=]() {this->setSolo(n, !button->getToggleState());};
		soloCallbacks[n] = [=](bool v) {
			button->setToggleState(!v, dontSendNotification);
		};
	}
	else
	{
		button->onClick = [=]() {this->setSolo(n, button->getToggleState());};
		soloCallbacks[n] = [=](bool v) {
			button->setToggleState(v, dontSendNotification);
		};
	}
	soloCallbacks[n](solo==n);
}
void SMParamGroup::connectMuteButton(int n, Button* button, bool flip)
{
	button->setClickingTogglesState(true);
	if (flip)
	{
		button->onClick = [=]() {this->setMute(n, !button->getToggleState());};
		mute[n].uiCallback =  [=](bool v) {
			button->setToggleState(!v, dontSendNotification);
		};
	}
	else
	{
		button->onClick = [=]() {this->setMute(n, button->getToggleState());};
		mute[n].uiCallback =  [=](bool v) {
			button->setToggleState(v, dontSendNotification);
		};
	}
	mute[n].uiCallback(mute[n]);
}

void SMParamGroup::setSoloFlat(int n)
{
	//Turn everything off just to be safe
	for (int s=0; s < ct; ++s)
	{
		if (s != n)
		{
			soloCallbacks[s](false);
		}
	}
	if (n != -1)
	{
		soloCallbacks[n](true);
	}
}
void SMParamGroup::setSoloFlat(int n, int prev)
{
	if (n != -1)
	{
		if (mute[n])
		{
			mute[n].setFromGroup(false);
		}
	}
	if (prev != n)
	{
		if (prev >= 0)
		{
			soloCallbacks[prev](false);
		}
		if (n != -1)
		{
			soloCallbacks[n](true);
		}
	}
}
void SMParamGroup::setSolo(int n, bool set)
{
	if (set)
	{
		int prev = solo;
		solo.setFromGroup(n);
		
		mute[n].setFromGroup(false);
		
		if (n != prev)
		{
			if (prev != -1)
				soloCallbacks[prev](false);
		}
	}
	else
	{
		if (solo == n)
		{
			solo.setFromGroup(-1);
			soloCallbacks[n](false);
		}
	}
}
void SMParamGroup::setMute(int n, bool set)
{
	if (set)
	{
		if (solo==n)
		{
			setSoloFlat(-1, n);
			solo.setFromGroup(-1);
		}
	}
	mute[n].setFromGroup(set);
}


void SMParamGroup::clearSoloCallbacks()
{
	for (int n=0; n < ct; ++n)
	{
		soloCallbacks[n] = [&](bool){};
	}
}

//Destructor
SMParamGroup::~SMParamGroup()
{
	delete[] mute;
	delete[] soloCallbacks;
}


void SMParamGroup::SoloParam::save(XmlElement* element) const
{
	element->setAttribute("S", value);
}
void SMParamGroup::SoloParam::load(XmlElement* element)
{
	int v = element->getIntAttribute("S", -1);
	set(v);
}
void SMParamGroup::SoloParam::loadAsDefault(XmlElement* element)
{
	//Don't set default because this doesn't have a default - only set value
	int v = element->getIntAttribute("S", -1);
	set(v);
}

void SMParamGroup::MuteParam::save(XmlElement* element) const
{
	element->setAttribute("M"+String(n), muted);
}
void SMParamGroup::MuteParam::load(XmlElement* element)
{
	bool m = element->getBoolAttribute("M"+String(n), false);
	if (m != muted)
	{
		muted = m;
		update_();
	}
}
void SMParamGroup::MuteParam::loadAsDefault(XmlElement* element)
{
	//Don't set default because this doesn't have a default - only set value
	bool m = element->getBoolAttribute("M"+String(n), false);
	if (m != muted)
	{
		muted = m;
		update_();
	}
}
