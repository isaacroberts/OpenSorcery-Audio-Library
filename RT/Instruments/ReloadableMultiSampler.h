//
//  ReloadableMultiSampler.h
//  
//
//  Created by Isaac Roberts on 8/12/22.
//

// Created by Isaac Roberts 

#ifndef ReloadableMultiSampler_h
#define ReloadableMultiSampler_h

#include "InstrFXBase.h"
#include "VelocityMultiSampler.h"
#include "RTSynth.h"

class ReloadableMultiSampler : public InstrFXBase
{
public:
	ReloadableMultiSampler()
	: InstrFXBase(), instr(NULL), reloadLock(),
	silenced(true), loadedFolder("")
	{
	}
	
	void loadSampleFolder(File folder);
	void loadSampleFolder(File folder, int numRoots, int numVelClasses);

	String getLoadedFolder() { return loadedFolder; }
	
//	Override
	//FX Stuff
	void prepare2(Spec& spec) override
	{
//		if (synthCritical) return;
		synth.prepare(spec);
	}
	void process (ContextR& c)
	{
		fx_proc_start(c);
		if (silenced) return;
		
		synth.process(c, getMidiBuffer());
	}
	void process(ContextNR& c)
	{
		fx_proc_start(c);
		c.bypassProcess();
		if (silenced) return;
		
		synth.process(c, getMidiBuffer());
	}
	void bypassProcess(ContextR& c, bypassParams)
	{
		if (silenced) return;
		
		if (allowTail)
			synth.processTail(c, getMidiBuffer());
		else
			synth.processSilence(c, getMidiBuffer());
	}
	void bypassProcess(ContextNR& c, bypassParams) 
	{
		if (silenced) return;
		
		if (allowTail)
			synth.processTail(c, getMidiBuffer());
		else
			synth.processSilence(c, getMidiBuffer());
	}
	
	void allNotesOff() override
	{
		synth.allNotesOff(-1, true);
	}
	void allSoundsOff() override
	{
		synth.allNotesOff(-1, false);
	}
	
	//Getters & Setters
//	int paramAmt() override { if (instr) return instr->getParamAmt(); else return 0; }
//	void set(int n, float set) override { if (instr) instr->set(n, set); }
//	float get(int n) override { if (instr) return instr->get(n); else return extraGet(); }
	
	VelocityMultiSampler* getInstr() { return instr; }
	VelocityMultiSampler* operator->() { return instr; }
	
	virtual ~ReloadableMultiSampler()
	{
		synth.clearVoices();
		synth.clearSounds();
//		delete instr;
	}
protected:
	struct SampleObj {
		SampleObj() : file(), root(0), velC(0), inited(false)
		{}
		File file;
		int root, velC;
		bool inited;
		int cmp() { return root*100 + velC; }
	};
	struct SampleObjComparator {
		static int compareElements(SampleObj first, SampleObj second)
		{
			return first.cmp() - second.cmp();
		}
	};
	
	
	void _loadSampleFolder(File folder, int numRoots, int numVelClasses);
	Array<SampleObj> getObjArray(File folder);
	
	void _determineRootVelCt(Array<SampleObj>& f, int& nr, int& nc);
	void _loadSamples(VelocityMultiSampler*, Array<SampleObj>& f, int nr, int nc);
	
	VelocityMultiSampler* newInstr(int numRoots, int maxVelC);
	
	bool checkLoad(VelocityMultiSampler*);
	
	void replaceInstr(VelocityMultiSampler*);

//	void _loadSamples(Folder f);
	
	
	bool getRootAndVel(SampleObj& obj);
	
protected:
	RTSynth synth;
	VelocityMultiSampler* instr=NULL;
	std::mutex reloadLock;
	bool silenced=true;
	String loadedFolder;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReloadableMultiSampler);

};

#endif /* ReloadableMultiSampler_h */
