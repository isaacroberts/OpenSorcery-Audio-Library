//
//  ReloadableMultiSampler.cpp
//  
//
//  Created by Isaac Roberts on 8/12/22.
//

// Created by Isaac Roberts

#include "ReloadableMultiSampler.h"





//Public Forms
void ReloadableMultiSampler::loadSampleFolder(File folder)
{
	Thread::launch([&, folder]() { _loadSampleFolder(folder, 0, 0); });
}
void ReloadableMultiSampler::loadSampleFolder(File folder, int nr, int nc)
{
	Thread::launch([&, folder, nr, nc]() { _loadSampleFolder(folder, nr, nc); });
}

//In-Thread
void ReloadableMultiSampler::_loadSampleFolder(File folder, int nr, int nc)
{
	if (!folder.exists())
		return;
	
	if (!reloadLock.try_lock())
		return;
	
	Array<SampleObj> samples = getObjArray(folder);
		
	SampleObjComparator comp;
	samples.sort(comp);

	if (nr==0 || nc==0)
		_determineRootVelCt(samples, nr, nc);
	
	VelocityMultiSampler* newi = newInstr(nr, nc);
	_loadSamples(newi, samples, nr, nc);
	
	if (!checkLoad(newi))
	{
		std::cout<<"Failed load "<<folder.getFullPathName()<<std::endl;
		delete newi;
		reloadLock.unlock();
		return;
	}
	
	silenced=true;
	replaceInstr(newi);
	silenced=false;
	
	loadedFolder = folder.getFullPathName();
	reloadLock.unlock();
}
Array<ReloadableMultiSampler::SampleObj> ReloadableMultiSampler::getObjArray(File folder)
{
	Array<File> files = folder.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav", File::FollowSymlinks::no);
	Array<SampleObj> samples;
	
	for (int n=0; n < files.size(); ++n)
	{
		SampleObj obj;
		obj.file = files[n];
		if (getRootAndVel(obj))
		{
			samples.add(obj);
		}
	}
	
	return samples;
}


void ReloadableMultiSampler::_determineRootVelCt(Array<SampleObj>& smp, int& nr, int& maxVelClass)
{
	maxVelClass = 0;
	nr = 0;
	int prevRoot = -1;
	for (int n=0; n < smp.size(); ++n)
	{
		if (smp[n].root != prevRoot)
			nr ++;
		prevRoot = smp[n].root;
		
		maxVelClass = std::max(maxVelClass, smp[n].velC);
	}
	maxVelClass+=1;
}

//Shared code
void ReloadableMultiSampler::_loadSamples(VelocityMultiSampler* instr, Array<SampleObj>& smp, int nr, int nc)
{
	const float maxT= 5;
		
	for (int n=0; n < smp.size(); ++n)
	{		
		FileInputStream* inputStream = smp[n].file.createInputStream().release();
		if (inputStream->openedOk())
		{
			WavAudioFormat wavFormat;
			AudioFormatReader* reader = wavFormat.createReaderFor(inputStream, false);
			if (reader)
			{
				instr->addSampleWithoutChecks(*reader, smp[n].root, maxT);
				delete reader;
			}
			else
				std::cout<<"Couldn't open reader "<<smp[n].file.getFullPathName()<<std::endl;
		}
		else
		{
			std::cout<<"Couldn't open stream "<<smp[n].file.getFileName()<<std::endl;
			delete inputStream;
		}
	}
}
bool ReloadableMultiSampler::checkLoad(VelocityMultiSampler* instr)
{
	if (!instr->isValid())
		return false;
	return true;
}

VelocityMultiSampler* ReloadableMultiSampler::newInstr(int numRoots, int maxVelC)
{
	return new VelocityMultiSampler(numRoots, maxVelC, false);
}

void ReloadableMultiSampler::replaceInstr(VelocityMultiSampler* newi)
{
	if (instr)
	{
		synth.clearSounds();
		synth.clearVoices();
		instr = NULL;
	}
	
	//Mono only for now
	instr = newi;
	synth.addSound(instr);
	for (int n=0; n < instr->polyphony(); ++n)
	{
		synth.addVoice(instr->getAssociatedVoice());
	}
}


bool ReloadableMultiSampler::getRootAndVel(SampleObj& obj)
{
	String fname = obj.file.getFileNameWithoutExtension();
	
	int underscore = fname.lastIndexOf("_");
	if (underscore==-1)
	{
		//Instead move to first number
		underscore = fname.indexOfAnyOf ("0123456789");

		if (underscore==-1)
			return false;
		
		else
			//Set to character before
			underscore --;
	}
	
	String nums = fname.substring(underscore+1);
	int vix = nums.indexOfChar('v');
	
	String root;
	if (vix==-1)
	{
		root = nums;
		obj.velC = 0;
	}
	else
	{
		root = nums.substring(0, vix);
		
		String vel = nums.substring(vix+1);
		obj.velC = vel.getIntValue();
	}
	
	obj.root= root.getIntValue();

	if (obj.root < 0 || obj.root >= 128)
		return false;
	if (obj.velC < 0 || obj.velC > 10)
		return false;
	
	return true;
}



