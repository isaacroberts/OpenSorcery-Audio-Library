//
//  Utility.h
//  Library - Shared Code
//
//  Created by Isaac Roberts on 7/8/20.
//  Copyright © 2020 RobertsAudio. All rights reserved.
//

// Created by Isaac Roberts 

#ifndef Utility_h
#define Utility_h

#include <JuceHeader.h>

//#include "../RTTypedef.h"

//#include "BufferManager.h"

//#if DEBUG_WINDOW
#include "RTGraphics/DebugWindow.h"
//#endif

#include <math.h>

namespace Utility {
	
	
	static int fCt=0;
	
#if DEBUG
	static inline void frame() { fCt ++; }
#else 
	static inline void frame() {}
#endif 
	
	static inline bool printAnalysis(const juce::dsp::AudioBlock<const float>& buffer, juce::String name, int printLevel, bool useDebugWindow)
	{/*
	  PrintLevel:
			0 = no print
			1 = print if na
			2 = print if na or overflow
			3 = print always
	  */
		const float maxLevel = 1;
		float level = 0;
		int naCt=0;
		int aboveCt =0;
		int belowCt=0;
		for (int c=0; c < buffer.getNumChannels(); ++c)
		{
			auto* data = buffer[c];
			for (int n=0; n < buffer.getNumSamples(); ++n)
			{
				if (isnan(data[n]))
					naCt++;
				else if (data[n] > maxLevel)
					aboveCt++;
				else if (data[n] < -maxLevel)
					belowCt++;
				else
				{
					level += fabs(data[n]);
				}
			}
		}
		
		bool anyNa = naCt >0;
		bool anyBad = aboveCt > 0 || belowCt > 0 || anyNa;
		
		bool ret = printLevel > 2 ? anyBad : anyNa;
		
		if (anyNa)
		{
			float xxx=0;
			for (int c=0; c < buffer.getNumChannels(); ++c)
			{
				for (int n=0; n < buffer.getNumSamples(); ++n)
					xxx=buffer[c][n];
			}
		}
		
		level /= buffer.getNumChannels() * buffer.getNumSamples() - naCt - aboveCt - belowCt;
		
		bool print = (printLevel == 3) || ((printLevel == 2) && anyBad) || ((printLevel ==1) && anyNa);
		
		if (print)
		{
			if (fCt % 500 >0)
				return ret;
			if (useDebugWindow && !DEBUG_WINDOW)
				return ret;
		  
			if (useDebugWindow)
			{
				//Prevent from overloading the buffer
				if (DebugWindow::hasInstancesActive())
				{
					DebugWindow::print(name, " Level =", level, "; ",
						 naCt, "nas,", aboveCt, "above,", belowCt, "below");
				}
			}
			else
			{
				std::cout<<name<<" Level = "<<level<<"; "<<
					naCt<<" nas, "<<aboveCt<<" above, "<<belowCt<<" below"<<std::endl;
			}
		}
		return ret;
	}
	
//	static inline float q(float N)
//	{/**
//			N measures "Bandwidth in Octaves"
//			N is the standard measure of filter falloff for consumer-facing applications
//			http://sengpielaudio.com/calculator-bandwidth.htm
//	 */
//		return N;
//
//		float expN = pow(2.0f, 1.f/N);
//		///this is the original listed at sengpielaudio.com/calculator-bandwidth.htm
//		 float Q = sqrt(expN) / ( expN - 1);
//		///but working backwards from the other one gets the wrong number
//		/// N = 2/ln(2) * arcsinh(1/(2Q))
//		/// Especially for the default - putting the default 1/sqrt(2) into the N= equation gets 1 for Q
//		/// But putting 1 into the Q= equation gets sqrt(2)
//		/// So logically, the Q one must be flipped
//		/// I don't think it works if the N one is instead flipped
//		/// Ah, but then again, using flipped Q sounds wrong. Sengpiel wins again
////		float Q = ( expN - 1) / sqrt(expN);
//		return Q;
//	}
	
	
	
//	static inline float NfromQ(float q)
//	{
//		float N = 2.f/log(2.f) * asinh(1.f/(2.f * q));
//		return N;
//	}
	
	
	
};

namespace Util = Utility;

#endif /* Utility_h */

