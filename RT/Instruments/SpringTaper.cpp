//
//  SpringTaper.cpp
//  
//
//  Created by Isaac Roberts on 11/14/22.
//

// Created by Isaac Roberts

#include "SpringTaper.h"

#include "RTSynth.h"

SpringTaper::SpringTaper()
{
	
}
void SpringTaper::prepare(Spec& spec)
{
	sampleRate = spec.sampleRate;
	
	if (spec.numChannels != numChannels)
	{
		if (hardStopTaper != NULL)
		{
			for (int c=0; c < numChannels; ++c)
			{
				delete[] hardStopTaper[c];
			}
			delete[] hardStopTaper;
		}
		numChannels = spec.numChannels;
		hardStopTaper = new float*[numChannels];
		for (int c=0; c < numChannels; ++c)
		{
			hardStopTaper[c] = new float[taperCount];
		}

	}
	for (int c=0; c < numChannels; ++c)
	{
		for (int n=0; n < taperCount; ++n)
			hardStopTaper[c][0] = 0;
	}
	hardStopTapering=false;
}


//bool doDecay(float& val, float rate, bool clamp)
//{
//	if (val==0)
//		return true;
//	else if (val > 0) {
//		val -= rate;
//		if(val <= 0)
//		{
//			if (clamp)
//				val=0;
//			return true;
//		}
//	}
//	else
//	{
//		val += rate;
//		if (val >= 0)
//		{
//			if (clamp)
//				val=0;
//			return true;
//		}
//	}
//	return false;
//}

void SpringTaper::renderTaper(OutputBlock& b)
{
//	return;
	if (hardStopTapering)
	{
		bool allDone=true;
		// ((amp/sec) / (smp/sec))
		//amp/ smp

//		float decay = 1/(hardStopTaperRate*sampleRate);
		for (int c=0; c< b.getNumChannels();++c)
		{
			auto o = b[c];
//			auto h = hardStopTaper[c];
			auto& v = hardStopTaper[c][V_];
			auto& x = hardStopTaper[c][X_];
			auto k = hardStopTaper[c][K_];

			bool done=false;
			for (int n=0; n < b.getNumSamples(); ++n)
			{
//				o[n]+=x;
//				if (x > 0)
//				{
//					x -= decay;
//					if (x <= 0) {
//						x=0;
//						done=true;
//						break;
//					}
//				}
//				else
//				{
//					x += decay;
//					if (x >= 0) {
//						x=0;
//						done=true;
//						break;
//					}
//				}
				
//				x += v;
				
//				if (x==0) {
//					v=0;
//					done=true;
//					break;
//				}
//				else if (x > 0) {
//					x += v;
//					if (x <0) {
//						x=v=0;
//						done=true;
//						break;
//					}
//					v -= decay;
//				}
//				else {
//					x+=v;
//					if (x > 0) {
//						x=v=0;
//						done=true;
//						break;
//					}
//					v += decay;
//				}
				
				o[n] += x;
				
				bool wasPositive = (x>0);
				x += v;
				v += -x * k;
//				v *= h[D_];
				
				if (wasPositive != (x>0))
				{
					x=0;
					v=0;
					
					done=true;
					break;
				}
//				x *= .99;
			
//				doDecay(v, decay/10, true);
//				doDecay(x, decay*10, true);
//				v *= .95;
				
//				x *= .99;
				
			}
			allDone &= done;
		}
		if (allDone)
			hardStopTapering=false;
	}
}

void SpringTaper::gatherVoiceTaper(RTSynthVoice* voice)
{
	if (voice == nullptr) return;
	

#if DEBUG
	std::cout<<"Stealing note "<<voice<<std::endl;
#endif
	
	const int blockSize = taperCount/2;
	AudioBlock<float> block(hardStopTaper, numChannels, blockSize);
	block.clear();
	//Fuck
	BlockStorage b;
	b.prepare(numChannels, blockSize);
	ContextR c(b, block);
	
	
	
	//Render one sample into taper block
	voice->renderNextBlock (c, 0, blockSize);
	hardStopTapering=true;
	
	for (int ch=0; ch < numChannels; ++ch)
	{
		auto h = hardStopTaper[ch];
		if (fabs(h[1]) > .0001 && h[0] !=0)
		{
			h[X_] = h[0];
			//
			h[V_] = (h[0]-h[2]);
			// F=a=-kx; k = a / -x
			h[K_] = abs((h[0] - 2*h[1] + h[2]) / (-h[1]));
			const float min  =5.667e-4;
			const float max = .7;
//			if (h[K_<0])
//			{
//				h[K_] = abs(h{)
//			}
			if (h[K_]<=min)
			{
				h[K_]=min;
			}
			else if (h[K_]>max)
			{
				h[K_]=max;
			}

//			if ((h[X_]>0 ) == (h[V_]>0))
//			{
//				h[D_] = Util::alpha(M_PI * 2 * sqrt(1.f/h[K_])/5);
//			}
//			else
			{
				h[D_] = Util::alpha(M_PI * 2 * sqrt(1.f/h[K_]));
				
			}
			
		}
		else
		{
			h[X_] = h[V_]=0;
			h[K_]=1e-3;
			h[D_] =  exp( -1.0f / (M_PI * 2 * sqrt(1.f/1e-3f)));

		}
#if DEBUG
		bool sameSign = (h[X_]>0 ) == (h[V_]>0);
		std::cout<<"k, v, x "<<sqrt(h[K_])/(2*M_PI)*sampleRate<<"\t"<<h[V_]<<"\t"<<h[X_]<<"\t"<<h[D_]<<" "<<sameSign<<std::endl;
#endif
	}
	return;
	//In process method, gradually taper off voice's current value
	
}


//Destructor
SpringTaper::~SpringTaper()
{
	
	if (hardStopTaper != NULL)
	{
		for (int c=0; c < numChannels; ++c)
		{
			delete[] hardStopTaper[c];
		}
		delete[] hardStopTaper;
	}
}

