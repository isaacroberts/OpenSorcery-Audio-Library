//
//  UnisonOsc.cpp
//  
//
//  Created by Isaac Roberts on 7/16/21.
//

// Created by Isaac Roberts


#include "UnisonOsc.h"

UnisonOsc::UnisonOsc()
: OscInstr(),
chorusMix(1.f),
unisonFactor(1.f),
spread(1.f),
adsrParams()
{
	for (int n=0; n < unisonAmt; ++n)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * M_PI * 2;
		rPhase[n] = r *  1;//pow(1.f - r, 2.f);
	}
	
	adsrParams.attack = .01f;
	adsrParams.release = 1.f;
	adsrParams.decay = .1f;
	adsrParams.sustain = .5f;
}
void UnisonOsc::setUnisonNoteShift(int shift)
{
	unisonFactor = pow(2., shift/12.);
}
Note* UnisonOsc::getAssociatedVoice()
{
	return new UnisonOscNote();
}

UnisonOscNote::UnisonOscNote()
: OscNote(),
adsr()
{
	fPhase=0;
}

void UnisonOscNote::onStart()
{
	auto* instr = static_cast<UnisonOsc*>(instrument);
//	savedChorusMix = std::max(instr->chorusMix, .01f);
//	savedChorusMix = 1;
	
	adsr.setSampleRate (getSampleRate());
	adsr.setParameters (instr->adsrParams);

	adsr.noteOn();
	
	fPhase=0;
}
void UnisonOscNote::onStop(float velo, bool allowTailOff)
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
void UnisonOscNote::playNote(ContextR& c, int startIx, int numSamples)
{
//	const int C = buffer.getNumChannels();
	const float tf = 1 / getSampleRate();

//	const int unAmt = 16;
	
	const float max_detune = .2f /12.f;
	
	auto* instr = static_cast<UnisonOsc*>(instrument);
	auto rPhase = instr->rPhase;

	osc_func_ptr func = getFunc(getType());

	auto& buffer = c.getOutput();
	
	auto* dstL = buffer[0] + startIx;
	auto* dstR = buffer.getNumChannels() > 1 ? buffer[1] + startIx : dstL;
 
	for (int c=2; c < buffer.getNumChannels(); ++c)
		Util::clearChannel(buffer, c, startIx, numSamples);
	
	float chorusMix = instr->chorusMix;
	float freqFactor = instr->unisonFactor;
	float spread = instr->spread;
	
	float freqM = freq * pitchWheel;
	
	for (int n=0; n < numSamples; ++n)
	{
		float pn = fPhase+n;
		
		float env = adsr.getNextSample();

		for (int u = 0; u < unisonAmt; ++u)
		{
			float pos = getPos(u, unisonAmt);
//			float lPan = getPan(u, unisonAmt);

			float lPan = (pos * .8 + 1) / 2;
			
			bool isMid = (u == unisonAmt/2);
			
			float uf = freqM * pow(2, pos * max_detune);
			if (!isMid)
				uf *= freqFactor;
				
			float x = func(uf * (tf * pn + (rPhase[u] * spread)));
			
			float vol = isMid ? 1.f : chorusMix;
			vol *= (1 - chorusMix/2);
			x *= vol * env;
			
			dstL[n] += x * lPan;
			dstR[n] += x * (1-lPan);
		}
	}
	fPhase += numSamples * pitchWheel;
}
