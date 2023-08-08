//
//  LookaheadCompressor.cpp
//  
//
//  Created by Isaac Roberts on 9/14/21.
//

// Created by Isaac Roberts


#include "LookaheadCompressor.h"


LookaheadCompressor::LookaheadCompressor()
: FX(), Compressors::Common(0, .2, .003, .1, .2, 0, 0),
audioRing(256),
dbRing(256),
type(Compressors::RMS), mono(true)
{
}

LookaheadCompressor::LookaheadCompressor(float t, float rat, float a, float r, float kw, float pg, float g)
: FX(), Compressors::Common(t, rat, a, r, kw, pg, g),
audioRing(256),
dbRing(256),
type(Compressors::RMS), mono(true)
{
}
void LookaheadCompressor::setMono(bool set)
{
	if (hasBeenPrepared())
	{
		jassertfalse;
		return;
	}
	mono = set;
}
void LookaheadCompressor::setBufferSize(int set)
{
	audioRing	.setLength(set);
	dbRing		.setLength(set);
	
	audioRing	.clear();
	dbRing		.clear();
	
	audioRing	.setWritePtrs(0);
	dbRing		.setWritePtrs(0);
	
	audioRing	.setReadPtrs(0);
	dbRing		.setReadPtrs(0);
}
void LookaheadCompressor::prepare2(Spec& spec)
{
	audioRing	.setNumChannels(spec.numChannels);
	
	audioRing	.clear();
	dbRing		.clear();
	
	audioRing	.setWritePtrs(0);
	dbRing		.setWritePtrs(0);
	
//	audioRing	.setReadPtrs(audioRing.size() - 1);
//	dbRing		.setReadPtrs(dbRing.size() - 1);
	
	audioRing	.setReadPtrs(0);
	dbRing		.setReadPtrs(0);

}

void LookaheadCompressor::bypassProcess(ContextNR& c, bypassParams)
{
	//TODO: This won't work if block is longer than ring
	audioRing.debugAllPointersMatch();

	if (inputIsSilent)
	{
		auto& output = c.getOutput();
		
		//TODO: The original had pregain and postgain separated somehow. This will need to do that
		
		audioRing.readReplaceFirst(output);
		dbRing.clear(c.getNumSamples());
//		dbRing.writeReplace(newDb[0], (int)newDb->getNumSamples());
		audioRing.writeReplace(c.getInput());
		
	}
	else
	{
		BlockPtr newDb(c);
		//newDb = input gain reduction
		
		ffToDb(c.getInput(), *newDb, rate, mono);

		//Backprop
		//TODO: This might look repeatedly at the same unchanging buffer
		Compressors::smoothGainReduction(newDb, dbRing, pregain);
		
		auto& output = c.getOutput();

		audioRing.readReplaceFirst(output);
		
		dbRing.writeReplace(newDb[0], (int)newDb->getNumSamples());
		audioRing.writeReplace(c.getInput());
	}
}
void LookaheadCompressor::bypassProcess(ContextR& c, bypassParams)
{
	//TODO: This won't work if block is longer than ring
	audioRing.debugAllPointersMatch();

	if (inputIsSilent)
	{
		BlockPtr newAudio(c);
		newAudio->copyFrom(c.getInput());
		auto& output = c.getOutput();
		
		//TODO: The original had pregain and postgain separated somehow. This will need to do that
		
		audioRing.readReplaceFirst(output);
		dbRing.clear(c.getNumSamples());
//		dbRing.writeReplace(newDb[0], (int)newDb->getNumSamples());
		audioRing.writeReplace(*newAudio);
		
	}
	else
	{
		BlockPtr newDb(c);
		//newDb = input gain reduction
		
		ffToDb(c.getInput(), *newDb, rate, mono);

		//Backprop
		//TODO: This might look repeatedly at the same unchanging buffer
		Compressors::smoothGainReduction(newDb, dbRing, pregain);

		BlockPtr newAudio(c);
		newAudio->copyFrom(c.getInput());
		auto& output = c.getOutput();

		
		audioRing.readReplaceFirst(output);

		dbRing.writeReplace(newDb[0], (int)newDb->getNumSamples());
		audioRing.writeReplace(*newAudio);
	}
}

void LookaheadCompressor::advance(int n) {
	audioRing.advanceWritePtrs(n);
	dbRing.advanceWritePtr(n);
}
void LookaheadCompressor::advance() {
	audioRing.incWritePtrs();
	dbRing.incWritePtr();
}


void LookaheadCompressor::processBand(InputBlock& levelBlock, OutputBlock& delayed,  BlockStorage& s)
{
//	bool mono = false;
//
////	OutputBlock delayedDb;
////	ffToDb(audioRing, writePos, delayedDb, rate, type, mono);
//
//	OutputBlock incomingDb;
//	ffToDb(levelBlock, incomingDb, rate, type, mono);
//
//	Compressors::smoothGainReductionVarying(delayedDb, bufferSize, incomingDb);
//
//	Compressors::applyGain(delayed, delayed, delayedDb, mono);

}

void LookaheadCompressor::process(ContextR& c)
{
	fx_proc_start(c);
	
	audioRing.debugAllPointersMatch();
	
	//TODO: This won't work if block is longer than ring
	
	BlockPtr newDb(c);
	//newDb = input gain reduction
	ffToDb(c.getInput(), *newDb, rate, mono);

	//Backprop
	//TODO: This might look repeatedly at the same unchanging buffer
	Compressors::smoothGainReduction(newDb, dbRing, pregain);
	
	BlockPtr newAudio(c);
	newAudio->copyFrom(c.getInput());
	auto& output = c.getOutput();

	
	
	
	//TODO: The original had pregain and postgain separated somehow. This will need to do that
	audioRing.readToBlockWithDb(dbRing, output, postgain);
	
	dbRing.writeReplace(newDb[0], (int)newDb->getNumSamples());
	audioRing.writeReplace(*newAudio);
}
void LookaheadCompressor::process(ContextNR& c)
{
	fx_proc_start(c);
	
	audioRing.debugAllPointersMatch();

	//TODO: This won't work if block is longer than ring
	
	BlockPtr newDb(c);
	//newDb = input gain reduction
	ffToDb(c.getInput(), *newDb, rate, mono);

	//Backprop
	//TODO: This might look repeatedly at the same unchanging buffer
	Compressors::smoothGainReduction(newDb, dbRing, pregain);
	
	auto& output = c.getOutput();
	
	//TODO: The original had pregain and postgain separated somehow. This will need to do that
	audioRing.readToBlockWithDb(dbRing, output, postgain);
	
	dbRing.writeReplace(newDb[0], (int)newDb->getNumSamples());
	audioRing.writeReplace(c.getInput());
	
}

void LookaheadCompressor::sidechain(InputBlock& l, ContextR& c)
{
	//Will be easy to implement, just use ffToDb(level) instead of ffToDb(input)
	jassertfalse;
}
void LookaheadCompressor::sidechain(InputBlock& l, ContextNR& c)
{
	//Will be easy to implement, just use ffToDb(level) instead of ffToDb(input)
	jassertfalse;
}

LookaheadCompressor::~LookaheadCompressor()
{
	
}

