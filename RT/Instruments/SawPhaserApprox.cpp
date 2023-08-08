//
//  SawPhaserApprox.cpp
//  
//
//  Created by Isaac Roberts on 5/25/22.
//

// Created by Isaac Roberts


#include "SawPhaserApprox.h"

//#include <random>
#include "../FX/Compressors/CompressorCommon.h"
//#include "../Utilities/Utility."

SawPhaser::SawPhaser()
: Instrument(),
type(Osc::Saw),
spread(.1f),
adsrParams()
{
	adsrParams.attack = .01f;
	adsrParams.decay = .5f;
	adsrParams.sustain = .5f;//.5f;
	adsrParams.release = 1.0f;
}
//void SawPhaser::setUnisonNoteShift(int shift)
//{
//	unisonFactor = pow(2., shift/12.);
//}
RTSynthVoice* SawPhaser::getAssociatedVoice()
{
	return new SawPhaserNote();
}

SawPhaserNote::SawPhaserNote()
: Note(),
adsr()
{
}

void SawPhaserNote::onStart()
{
//	bool fromSilent=true;
	auto* instr = static_cast<SawPhaser*>(instrument);
	
	adsr.setSampleRate (getSampleRate());
	adsr.setParameters (instr->adsrParams);
	adsr.noteOn();
	t=0;
	pureT = 0;
	lastX=0;
}
//void SawPhaserNote::clearCurrentNote()
//{
//	if (getCurrentlyPlayingNote()>0)
//	{
//		instrsInUse--;
//		jassert(instrsInUse >= 0);
//	}
//	RTSynthVoice::clearCurrentNote();
//}

void SawPhaserNote::onStop(float velo, bool allowTailOff)
{
	if (allowTailOff)
	{
		noteOffEnv=1.0f;
//		falloffRate =
		adsr.noteOff();
	}
	else
	{
		clearCurrentNote();
		adsr.reset();
	}
}
void SawPhaserNote::playNote(ContextR& c, int startIx, int numSamples)
{
	auto* instr = static_cast<SawPhaser*>(instrument);

//	osc_func_ptr func = getFunc(getType());

	auto& buffer = c.getOutput();
	
	auto* dstL = buffer[0] + startIx;
	auto* dstR = buffer.getNumChannels() > 1 ? buffer[1] + startIx : dstL;
 
	for (int c=2; c < buffer.getNumChannels(); ++c)
		Util::clearChannel(buffer, c, startIx, numSamples);
	
	
	auto osc = instr->getType();
	
	float f = freq ;
	if (osc != Osc::Sqr)
		f *= 2;
	if (osc==Osc::Sin)
		f *= 4;
	float pwkReduc = (.5-abs(pitchWheel))/.5;
	float k = pow( f, instr->spread/2.f);// * Util::mapNorm(velo, 1.0, .25);
	float k2 = k*k;
	
	const float pwFactor = pow(2, pitchWheel) ;

	
	int iters = 60;
	
	const float iter_to_nyquist = std::max(f*32, instr->topFreq)/f;
	float step;

	if (iters > iter_to_nyquist)
	{
		iters = std::ceil(iter_to_nyquist);
		step = 1.f;
	}
	else
	{
		step = std::max(iter_to_nyquist/iters, 1.f);
	}
//	float step = 1.f;
	
	//TODO: Try randomly sampling iters
	
	const float sr = getSampleRate();
	
//	std::complex<float> ifreq (0, f);
	for (int n=0; n < numSamples; ++n)
	{
//		if (!adsr.isActive()) {
//			clearCurrentNote();
//			return;
//		}
//		updateLegatoSample();
//		float env = adsr.getNextSample();

		std::complex<float> x(0, 0);
//		float kt2 = k * pureT * pureT;
		
		std::complex<float> eTerm (-k2 * pureT*pureT/4, -f*t);
		
		if (osc==Osc::Sin)
		{
			x = exp(eTerm);
			//1/sqrt(2)
			x *= 2.f;
		}
		else if (osc==Osc::Saw)
		{
			for (int j=1; j <= iters; ++j)
			{
				int i = std::round(j*step);
				std::complex<float> term = exp(float(i)*eTerm);
				if (i%2==0)
					term = -term;
				term /= sqrt(i*2);
				x += (term);
			}
		}
		else if (osc==Osc::Sqr)
		{
			for (int j=1; j <= iters; j++)
			{
				int i = 2 * std::round(j * step);
				std::complex<float> term = exp(float(i) * eTerm);
				term /= sqrt(i*2);
				x += (term);
			}
		}
		x /= k;
//		x = x / iters;
		
		x *= (1+2*instr->spread)/3;
		
		x *= (1+velo)/2.f;
		
//		lastX = std::abs(x);
		float rx = x.imag();
		
		//float x, float threshDb, float ratio, float knee=0, float pregain=0, float postgain=0
//		rx = Compressors::processSingleNoBallistics(rx, 0, .1, 0, 0);
		
//		if (isPlayingButReleased())
//		{
////			x *= noteOffEnv;
//			if (rx>noteOffEnv) rx = noteOffEnv;
//			else if (rx<-noteOffEnv) rx = -noteOffEnv;
//
//			noteOffEnv -= 1/sr;
//			if (noteOffEnv <= 0)
//			{
//				clearCurrentNote();
//			}
//		}
		


//		if (x>1) x = 1;
//		else if (x<-1) x = -1;
		
		//compress
//		{
//			const float thresh = 1;
//			const float ratio = .2;
//			if (x > thresh)
//			{
//				x = thresh + (x-thresh) * ratio;
//			}
//			else if (x < -thresh)
//			{
//				x = -thresh - (-x-thresh) * ratio;
//			}
//
//		}
		
//		float lPan = (pos * .8 + 1) / 2;
//		x *= env;
//		float mx = t/instr->adsrParams.attack;
//		if (x>0 && x > mx) x = mx;
//		else if (x<0 && x < -mx) x = -mx;
//		x*=5;
		
		
		float lPan = 0.5;
		dstL[n] += rx * lPan;
		dstR[n] += rx * (1-lPan);
		
		t += 1/sr * pwFactor;
		pureT += 1/sr * pwkReduc;
		
		if (pureT>2)
		{
			clearCurrentNote();
			return;
		}

	}
//	fPhase += numSamples * pitchWheel;
}
