//
//  AppMidiInput.cpp
//  
//
//  Created by Isaac Roberts on 7/6/22.
//

// Created by Isaac Roberts


#include "AppMidiInput.h"


AppMidiInput::AppMidiInput()
{
	octave = 4;
	onNotes.reserve(4);
	
	for (int n=0; n < maxNextNotes; ++n)
		nextNotes[n] = EMPTY;

}
int AppMidiInput::getNoteCode(n_t n)
{
	n = n % 12;
	auto binding = customBindingsReverse.find(n);
	if (binding != customBindingsReverse.end())
	{
		return binding->second;
	}
	if (useDefaults)
	{
		switch (n)
		{
			case 0: return 'A';
			case 2: return 'S';
			case 4: return 'D';
			case 7: return 'F';
			case 10: return 'G';
		}
	}
	return 0;
}
bool AppMidiInput::keyPressed(const KeyPress& key)
{
	if (!Format::isStandalone())
		return false;
	
	auto code = key.getKeyCode();
	auto binding = customBindings.find(code);
	if (binding != customBindings.end())
	{
		noteOn(binding->second);
		return true;
	}
	if (useDefaults)
	{
		switch (code)
		{
			case 'A':
				noteOn(0);
				return true;
			case 'S':
				noteOn(2);
				return true;
			case 'D':
				noteOn(4);
				return true;
			case 'F':
				noteOn(7);
				return true;
			case 'G':
				noteOn(10);
				return true;
		}
	}
	if (code== 'Z')
	{
		octave = std::max(0, octave-1);
		return true;
	}
	else if (code== 'X')
	{
		octave = std::min(10, octave+1);
		return true;
	}
	return false;
}
void AppMidiInput::processMidi(MidiBuffer& midi, int numSamples)
{
	for (int x = (int)onNotes.size()-1; x>=0; --x)
	{
		int n = onNotes[x];
		int c = getNoteCode(n);
		if (c==0 || !KeyPress::isKeyCurrentlyDown(c))
		{
			midi.addEvent(MidiMessage::noteOff(1, n, .7f), numSamples/2);
			onNotes.erase(onNotes.begin()+x);
		}
	}
	
	for (int s=0; s < maxNextNotes; ++s)
	{
		if (nextNotes[s]!=EMPTY)
		{
			int n = nextNotes[s];
//			uint8 vel = rand()%128;
			uint8 vel = 127;
			midi.addEvent(MidiMessage::noteOn(1, n, vel), 0);
#if DEBUG
			std::cout<<"#"<<n<<"\t"<<vel<<std::endl;
#endif 
			onNotes.push_back(n);
			nextNotes[s]=EMPTY;
		}
	}
	
	
}
void AppMidiInput::noteOn(int n)
{
	n += octave*12;
	for (int s=0; s < maxNextNotes; ++s)
	{
		if (nextNotes[s] == EMPTY)
		{
			nextNotes[s]=n;
			return;
		}
	}
}

void AppMidiInput::addCustomBinding(int press, int note)
{
	jassert (press != 0);
	if (press >= 97 && press <= 122)
	{
		//Capitalize
		press -= (97-65);
	}
	
	customBindings[press] = (n_t) note;
	customBindingsReverse[note] = press;
}
//void addCustomBinding(char press, int note)
//{
//	addCustomBinding((int)press, int note);
//}

AppMidiInput::~AppMidiInput()
{
	
}


