//
//  MidiUtil.cpp
//  
//
//  Created by Isaac Roberts on 5/29/22.
//

// Created by Isaac Roberts


#include "MidiUtil.h"

using namespace juce;


int MidiUtil::smpteToTickRate(short time)
{
	jassert(time < 0);
	//frames per second
	int fps = -(time>>8);
	//ticks per frame
	int tpf = time & 0xff;
	return fps * tpf;
}

juce::MidiMessageSequence* MidiUtil::mergeFile(juce::MidiFile file)
{
	auto* all = new juce::MidiMessageSequence();
	for (int n=0; n < file.getNumTracks(); ++n)
	{
		all->addSequence(*file.getTrack(n), 0);
	}
	all->updateMatchedPairs();
	return all;
}
MidiMessageSequence* MidiUtil::mergeAndConvertToBPM(MidiFile file, int& ticks)
{
	//Not tested yet - it's 1 AM
	
	auto* all = new juce::MidiMessageSequence();
	for (int n=0; n < file.getNumTracks(); ++n)
	{
		all->addSequence(*file.getTrack(n), 0);
	}
	all->updateMatchedPairs();


	ticks = file.getTimeFormat();
	if (ticks > 0)
		return all;
	else
	{
		int tickRate = smpteToTickRate(file.getTimeFormat());
		
		float bpm = 120;
		float bpmFactor = 60/120.f;
		float beatPosition=0;
		float secPosition=0;
//		float qnPerBar=4;
		for (auto event = all->begin(); event != all->end(); ++event)
		{
			auto& msg = (*event)->message;
			float eventSec = msg.getTimeStamp()/tickRate;
			beatPosition += (eventSec - secPosition) * bpmFactor;
			secPosition = eventSec;
			if (msg.isTempoMetaEvent())
			{
				float secPerQn = msg.getTempoSecondsPerQuarterNote();
				bpm = secPerQn * 60/4.f;
				bpmFactor = 60/bpm;
			}
			else if (msg.isTimeSignatureMetaEvent())
			{
				//Not implemented
//				jassertfalse;
			}
			else
			{
				msg.setTimeStamp(beatPosition);
			}
		}
		return all;
	}
}
MidiMessageSequence* MidiUtil::mergeAndConvertToSeconds(MidiFile file)
{
	//not tested yet
	file.convertTimestampTicksToSeconds();
	return mergeFile(file);
}

void MidiUtil::saveToXml(XmlElement* xml, MidiMessage& msg)
{
	const uint8* data = msg.getRawData();
	int size = msg.getRawDataSize();
//	String hex = String::toHexString (msg.getRawData(), msg.getRawDataSize());
//	std::cout<<"Hex string size = "<<msg.getRawDataSize()<<std::endl;
//	std::cout<<"\t"<<hex<<std::endl;
	//Max 15 digits
//	jassert(msg.getRawDataSize() < 16);
//	hex = hex.removeCharacters(" ") + " ";
	XmlElement* child = new XmlElement("Msg");
	//uint8 = 2**8  (1 byte)
	//int   = 2**32 (4 bytes)
	for (int b=0; b < size; b+=4)
	{
		int num = 0;
		for (int c=0; c < 4 && (b+c)<size; ++c)
		{
//			std::cout<<"frm "<<"                        " <<std::bitset<8>(data[b+c])<<std::endl;
			int n = (data[b+c]) << (8*c);
			num = num | n;
//			std::cout<<"add "<<std::bitset<32>(n)<<std::endl;
//			std::cout<<"num "<<std::bitset<32>(num)<<std::endl;
		}
		child->setAttribute("x"+String(b), num);
	}
	child->setAttribute("Size", size);
	child->setAttribute("T", msg.getTimeStamp());
//	std::cout<<child->toString()<<std::endl;
	
	xml->addChildElement(child);
}
bool MidiUtil::loadMessageFromXml(XmlElement* xml, MidiMessage& msg)
{
	const int size = xml->getIntAttribute("Size", 0);
	
	if (size==0)
		return false;

	double t = xml->getDoubleAttribute("T", -1);
	if (t < 0)
	{
		return false;
	}

	uint8* data = new uint8[size];

	for (int b=0; b < size; b+=4)
	{
		if (!xml->hasAttribute("x"+String(b))) return false;
		int num = xml->getIntAttribute("x"+String(b));
//		std::cout<<"num "<<std::bitset<32>(num)<<std::endl;
		for (int c=0; c < 4 && (b+c)<size; ++c)
		{
			int mask = 0xff << (8*c);
//			std::cout<<"msk "<<std::bitset<32>(mask)<<std::endl;

			int n = num & mask;
			n = n >> (8*c);
//			std::cout<<"    "<<"                        " <<std::bitset<8>(n)<<std::endl;
			data[b+c] = (uint8)n;
		}
	}
	msg = MidiMessage(data, size, t);
	delete[] data;
	return true;
}


void MidiUtil::saveToXml(XmlElement* xml, const MidiMessageSequence* seq)
{
	for (auto it = seq->begin(); it != seq->end(); ++it)
	{
		auto& msg = (*it)->message;
//		std::cout<<msg.getNoteNumber()<<std::endl;
		saveToXml(xml, msg);
	}
}
MidiMessageSequence* MidiUtil::loadSequenceFromXml(XmlElement* xml)
{
	MidiMessageSequence* seq = new MidiMessageSequence();
	for (int n=0; n < xml->getNumChildElements(); ++n)
	{
		MidiMessage msg;
		bool sxs = loadMessageFromXml(xml->getChildElement(n), msg);
		if (sxs)
		{
			seq->addEvent(msg);
		}
	}
	seq->updateMatchedPairs();
	return seq;
}

void MidiUtil::saveToXml(XmlElement* xml, const MidiMessageSequence* seq, int ticks)
{
	saveToXml(xml, seq);
	xml->setAttribute("Ticks", ticks);
}
MidiMessageSequence* MidiUtil::loadSequenceFromXml(XmlElement* xml, int& ticks)
{
	ticks = xml->getIntAttribute("Ticks", ticks);
	return loadSequenceFromXml(xml);
}
