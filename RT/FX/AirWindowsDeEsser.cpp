//
//  AirWindowsDeEsser.cpp
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/6/20.
//

// Created by Isaac Roberts


#include "AirWindowsDeEsser.h"


AirWindowsDeEsser::AirWindowsDeEsser()
: FX(),
input_intensity(1),
input_maxdess(-24),
frequency(.5)
{
	sa = NULL;
	ma = NULL;
	ratio = NULL;
	iirSample = NULL;
	
	channels = 0;
}
void AirWindowsDeEsser::prepare2(Spec& spec)
{
	int ch = spec.numChannels;
	
	if (channels == ch)
		//duplicate protection
		return;
	else
	{
		//if prepared twice, delete previous to prevent memory leaks
		if (sa != NULL)
		{
			for (int c=0; c < channels; ++c)
				delete[] sa[c];
			delete[] sa;
		}
		if (ma != NULL)
		{
			for (int c=0; c < channels; ++c)
				delete[] ma[c];
			delete[] ma;
		}
		if (ratio != NULL)
			delete ratio;
		if (iirSample != NULL)
			delete iirSample;
		
		channels = ch;
		
		sa = new double*[ch];
		for (int n=0;n<ch;++n)
		{
			sa[n] = new double[s_ct];
			for (int z=0;z<s_ct;++z)
				sa[n][z]=0;
		}


		ma = new double*[ch];
		for (int n=0;n<ch;++n)
		{
			ma[n] = new double[m_ct];
			for (int z=0;z<m_ct;++z)
				ma[n][z]=0;
		}


		ratio = new double[ch];

		for (int c=0;c<ch;c++)
			ratio[c]=1.0;

		iirSample = new double[ch];

		for (int c=0;c<ch;c++)
			iirSample[c] = 0;
	}
}

AirWindowsDeEsser::~AirWindowsDeEsser()
{
	delete[] sa;
	delete[] ma;
	
	delete ratio;
	delete iirSample;
}

void AirWindowsDeEsser::commonProc(InputBlock& input, OutputBlock& output, BlockStorage& b)
{
	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= rate;
	
	double intensity = pow(input_intensity,5)*(8192/overallscale);
	double maxdess = 1.0 / pow(10.0,input_maxdess /20);
	double iirAmount = pow(frequency,2)/overallscale;
	
	
//	double recoveryPow = (params[Frequency])*3;
//	double recoverySlope = pow(10, -4+recoveryPow );
	double recoverySlope = .01;
	
	double offset;
	double sense;
	double recovery;
	double attackspeed;

	for (int n=0;n < input.getNumSamples(); ++n)
	{
		for (int c=0; c< input.getNumChannels(); ++c)
		{
			float inputSample = input.getSample(c, n);
			 
			//stored previous samples
			for (int n=1;n<s_ct;n++)
				sa[c][n] = sa[c][n-1];
			//current sample
			sa[c][0] = inputSample;
			
			//slew^2
			for (int n=0;n<m_ct; n++)
				ma[c][n] = pow(sa[c][n]-sa[c][n+1], 2) / 1.3;
			//this ONLY works with m_ct=2
			// acceleration ^4 / 1.3^2
			sense = pow(ma[c][0]-ma[c][1], 2)/1.3;
			//this will be 0 for smooth, high for SSS
			attackspeed = 7.0+(sense*1024);
			//this does not vary with intensity, but it does react to onset transients
			
			//sense will be 1 for smooth, 'intensity' for SSS

			// 1 + intensity^2 * acceleration
			sense = 1.0+(intensity*intensity*sense);
			//cap at intensity
			if (sense > intensity)
			{
				sense = intensity;
			}

			//recovery will be 1.1 for smooth, 1.0000000...1 for SSS
			recovery = 1.0+(recoverySlope/sense);
			if (recovery > 1001)
				recovery = 1001;
			offset = 1.0-fabs(inputSample);
			
			//rolling sample
			//mixes previous iirSample vs current sample
			//input_frequency -> iirAmount**2 ->
			
			iirSample[c] = (iirSample[c] * (1 - (offset * iirAmount))) + (inputSample * (offset * iirAmount));
			
			if (ratio[c] < sense)
			{
				// = (ratioA * (7+sense*1024) + sense) / (7+sense*1024 + 1)
				if (attackspeed > 1000)
				{
					//ratio[c] = ((ratio[c] * ∞) + sense) / (∞ + 1)
					//ratio[c] = (ratio[c] * ∞ + ∞/1024 ) / (∞)
					ratio[c] = ratio[c] + 1./1024.;
				}
				else
					ratio[c] = ((ratio[c]*attackspeed)+sense)/(attackspeed+1.0);
			}
			else
			{
				// = 1 + (ratioA -1 ) / (1+ .01/sense)
				if (recovery >= 1000)
					ratio[c] = 1.0;
				else
					ratio[c] = 1.0+((ratio[c]-1.0)/recovery);
			}
			//returny to 1/1 code
			if (ratio[c] > maxdess)
				ratio[c] = maxdess;
			
			// iirSample + (input - iir)/ratioA
			if (fabs(ratio[c]) < .99)
				std::cout<<"ratio < 1 : "<<ratio[c]<<std::endl;
			
			//inputSample = iir + (inputSample - iir) / ratio
			//inputSample = iir + inputSample/ratio - iir/ratio
			//inputSample = iir * (1-1/ratio) + inputSample/ratio
			inputSample = iirSample[c] + ( (inputSample - iirSample[c]) / ratio[c]);
		
//			output.setSample(c, n, inputSample);
		}//c
	}//n
}
