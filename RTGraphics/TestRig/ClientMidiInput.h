//
//  ClientMidiInput.h
//  Dispersion Drum
//
//  Created by Isaac Roberts on 4/12/23.
//  Copyright © 2023 Algorithmic Design. All rights reserved.
//

// Created by Isaac Roberts 

#ifndef ClientMidiInput_h
#define ClientMidiInput_h

#include <stdio.h>

/*	Reads a server and requests
	each block of midi from that server
 
 */
class ClientMidiInput : public juce::Thread
{
	typedef unsigned char n_t;
public:
	// init
	ClientMidiInput(int port);//starts right away
	ClientMidiInput();
	
	ClientMidiInput(const ClientMidiInput& other);
	ClientMidiInput& operator=(const ClientMidiInput& other);

	void start(int port);

	// process
	
	void run() override;
	
	void readChunk();
	
	void processMidi(MidiBuffer& midi, int numSamples);
	
	void noteOn(int n);


	// post
	
	virtual ~ClientMidiInput();
	
	
protected:
	String url;
	
protected:
	static constexpr n_t EMPTY=255;
	static constexpr int maxNextNotes = 5;
	n_t nextNotes[maxNextNotes];
	
	
	std::vector<n_t> onNotes;
	

	
};

#endif /* ClientMidiInput_h */
