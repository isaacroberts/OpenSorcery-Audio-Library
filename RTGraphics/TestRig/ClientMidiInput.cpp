//
//  ClientMidiInput.cpp
//  Dispersion Drum
//
//  Created by Isaac Roberts on 4/12/23.
//  Copyright © 2023 Algorithmic Design. All rights reserved.
//

// Created by Isaac Roberts

#include "ClientMidiInput.h"

#include "RT/Utilities/RestUtil.h"


ClientMidiInput::ClientMidiInput()
: Thread("CppMidiClientThread")
{
	onNotes.reserve(4);
	
	for (int n=0; n < maxNextNotes; ++n)
		nextNotes[n] = EMPTY;
}

ClientMidiInput::ClientMidiInput(int port)
: ClientMidiInput()
{
	start(port);
}

ClientMidiInput::ClientMidiInput(const ClientMidiInput& other)
: Thread("CppMidiClientThread")
{
	
}
ClientMidiInput& ClientMidiInput::operator=(const ClientMidiInput& other)
{
	
	return *this;
}


void ClientMidiInput::processMidi(MidiBuffer& midi, int numSamples)
{
	for (int x = (int)onNotes.size()-1; x>=0; --x)
	{
		int n = onNotes[x];
		if (rand()%300==0)
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
void ClientMidiInput::noteOn(int n)
{
	for (int s=0; s < maxNextNotes; ++s)
	{
		if (nextNotes[s] == EMPTY)
		{
			nextNotes[s]=n;
			return;
		}
	}
#if DEBUG
	std::cout<<"Couldn't place note #"<<n<<" ("<<maxNextNotes<<" slots) "<<std::endl;
#endif

}


void ClientMidiInput::start(int port)
{
	url = String("http://localhost:"+String(port)+"/get-block?num_samples=100");
	startThread(7);
}


void ClientMidiInput::readChunk()
{
	RestUtil::Response response = RestUtil::runRequest(url, 300, true, false);
	
	if (response.success())
	{
		juce::var data = response.json();
		//std::cout<<"data:"<<std::endl;
		//std::cout<<data.toString()<<std::endl;
		
		for (int n=0; n < data.size(); ++n)
		{
			auto& json_msg = data[n];
			//std::cout<<json_msg.toString()<<std::endl;
						
			auto nv = json_msg["note"];
			int note = nv;
			
			if (note >= 0)
			{
				int vel = json_msg["vel"];
				int ch = json_msg["ch"];
				noteOn(note);
			}
		}
	}
	else {
		std::cout<<"ClientMidi error"<<std::endl;
	}
}

void ClientMidiInput::run()
{
	while (!threadShouldExit())
	{
		//grab chunk
		readChunk();
		
		wait(350);
	}
}

//Destructor
ClientMidiInput::~ClientMidiInput()
{
	
}

