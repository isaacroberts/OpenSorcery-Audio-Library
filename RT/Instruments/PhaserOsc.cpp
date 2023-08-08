//
//  PhaserOsc.cpp
//  
//
//  Created by Isaac Roberts on 7/16/21.
//

// Created by Isaac Roberts


#include "PhaserOsc.h"
#include <random>
#include "../FX/Compressors/CompressorCommon.h"
//#include "../Utilities/Utility."
int PhaserOscNote::instrsInUse=0;


PhaserOsc::PhaserOsc(int ch_)
: OscInstr(),
ch(ch_),
chorusMix(1.f),
unisonFactor(1.f),
spread(1.f),
gateThresh(-40),
adsrParams()
{
	for (int n=0; n < unisonAmt; ++n)
	{
		rPhase[n] = Util::rand1() *  1;//pow(1.f - r, 2.f);
	}
	
	adsrParams.attack = .01f;
	adsrParams.decay = 1.0f;
	adsrParams.sustain = .5f;//.5f;
	adsrParams.release = 0.25f;
}
void PhaserOsc::setUnisonNoteShift(int shift)
{
	unisonFactor = pow(2., shift/12.);
}
RTSynthVoice* PhaserOsc::getAssociatedVoice()
{
	return new PhaserOscNote();
}

PhaserOscNote::PhaserOscNote()
: Note(),
adsr()
{
	for (int n=0; n < unisonAmt; ++n)
	{
		pos[n] = Util::rand_c();// * Util::rand_c();
	}
}

void PhaserOscNote::onStart()
{
//	bool fromSilent=true;
	auto* instr = static_cast<PhaserOsc*>(instrument);
	
	instrsInUse++;
	
//	if (fromSilent)
	{
//		setLegatoTime(.025);
		adsr.setSampleRate (getSampleRate());
		adsr.setParameters (instr->adsrParams);
		adsr.noteOn();

	}
	for (int u=0; u < unisonAmt; ++u)
	{
		//		float r = Util::rand1() * M_PI * 2;
		fPhase[u] = 0 ;
	}
//	alpha = Compressors::alpha(std::min(3/freq, .01f), getSampleRate());
//	rollingDb = 0;
}
void PhaserOscNote::clearCurrentNote()
{
	if (getCurrentlyPlayingNote()>0)
	{
		instrsInUse--;
		jassert(instrsInUse >= 0);
	}
	RTSynthVoice::clearCurrentNote();
}

void PhaserOscNote::onStop(float velo, bool allowTailOff)
{
	if (allowTailOff)
	{
		adsr.noteOff();
	}
	else
	{
		clearCurrentNote();
		adsr.reset();
	}
}
void PhaserOscNote::playNote(ContextR& c, int startIx, int numSamples)
{
//	if (instrsInUse==0)
//		return;
	
//	const int C = buffer.getNumChannels();
	const float tf = 1 / getSampleRate();

//	const int unAmt = 16;
	
	auto* instr = static_cast<PhaserOsc*>(instrument);
//	auto rPhase = instr->rPhase;

	osc_func_ptr func = getFunc(getType());

	auto& buffer = c.getOutput();
	
	auto* dstL = buffer[0] + startIx;
	auto* dstR = buffer.getNumChannels() > 1 ? buffer[1] + startIx : dstL;
 
	for (int c=2; c < buffer.getNumChannels(); ++c)
		Util::clearChannel(buffer, c, startIx, numSamples);
	
//	float chorusMix = instr->chorusMix;
//	float freqFactor = instr->unisonFactor;
//	float spread = instr->spread;
	
//	float freqM = freq;
	
//	const float max_detune = instr->spread /12.f;// * (noteNum/128.);
//	const float max_detune = instr->spread * 100.f;// * (noteNum/128.);

	int myU = jlimit(1, unisonAmt, (int)(100 / (instrsInUse==0 ? 1 : instrsInUse)));
	
	const float pwFactor = pow(2, pitchWheel) ;
//	float pSpread = sqrt(freq * pwFactor);
//	const float pSpread = 1.0f;
	for (int n=0; n < numSamples; ++n)
	{
		if (!adsr.isActive()) {
			clearCurrentNote();
			return;
		}
//		updateLegatoSample();
		float env = adsr.getNextSample();

		
		float max_detune = (std::max(time, 1.f)) * instr->spread / Util::mapNorm(velo, .25, 1.0);
//		float std_dev = .333;
//		float gauss_coeff_c = 2 * pow(std_dev, 2);

		for (int u = 0; u < myU; ++u)
		{
			float pos = getPos(u, unisonAmt);
//			float lPan = getPan(u, unisonAmt);

			float lPan = (pos * .8 + 1) / 2;
			lPan = 0.5;
			
//			bool isMid = (u == unisonAmt/2);
			
//			float posAmp = exp(-pow(pos, 2)/gauss_coeff_c);
			float posAmp = 1.0f;
			
			float uf = (pos * max_detune) + freq * pwFactor;

			
			fPhase[u]+= uf;// * (1 + drift(gen)*1e-3);
			float pn = fPhase[u];
				
//			float x = func(uf * (tf * pn + (rPhase[u] * spread)));
			float x = func(tf * pn);

			float vol = 1;
//			vol *= (1 - chorusMix/2);
			vol *= posAmp;
			vol *= (1+velo)/2.f;
			x *= vol * env;
			
			dstL[n] += x * lPan;
			dstR[n] += x * (1-lPan);
		}
		
	}
//	fPhase += numSamples * pitchWheel;
}
