
#pragma once

#include "Coeff.h"
#include "Forms.h"

namespace RBJ
{
	template <class Form=TransposedDirectForm2>
	struct State
	{
		State()
		: channels(0),
		form(NULL)
		{}
		~State() {
			if (form)
			{
				delete[] form;
			}
		}
		
		void setNumChannels(int set)
		{
			if (channels==set)
				return;
			
			if (form)
			{
				delete[] form;
			}
			channels = set;
			form = new Form[channels];
		}
		void sanCheck()
		{
			for (int c=0; c < channels; ++c)
				form[c].sanCheck();
		}
		void reset(){
			for (int c=0; c < channels; ++c)
			{
				form[c].reset();
			}
		}
		
		void process(InputBlock& inputBlock, OutputBlock& outputBlock, Coeff& coeff)
		{
			size_t numSamples = inputBlock.getNumSamples();
			
			for (int c = 0; c < channels && c < inputBlock.getNumChannels(); ++c)
			{
				auto* x = inputBlock .getChannelPointer (c);
				auto* y = outputBlock.getChannelPointer (c);
				
				for (size_t i=0 ; i < numSamples; ++i)
				{
					y[i] = form[c].proc(coeff, x[i]);
				}
			}
			sanCheck();
		}
		void processAcrossChannels(InputBlock& inputBlock, OutputBlock& outputBlock, int n, Coeff& coeff)
		{
			double out;
			for (int c = 0; c < channels && c < inputBlock.getNumChannels(); ++c)
			{
				out = form[c].proc(coeff, inputBlock.getSample(c, n));
				
				outputBlock.setSample(c, n, out);
			}
			//Don't forget to san check
			//		sanCheck();
		}
		
		void process(double* input, double* output, int numSamples, int c, Coeff& coeff)
		{
			for (size_t i=0 ; i < numSamples; ++i)
			{
				output[i] = form[c].proc(coeff, input[i]);
			}
			
			sanCheck();
		}
		double processSingle(double in, int c, Coeff& coeff)
		{
			return form[c].proc(coeff, in);
			
			//Don't forget to sanCheck
			//	sanCheck();
		}
		
		void processSilence(int n)
		{
			reset();
		}
		void processBypass(InputBlock& in, int n)
		{
			for (int c=0; c < channels; ++c)
			{
				form[c].procBypass(n);
			}
		}
		
		int channels;
		Form* form;
		
		
		Form& operator[](int n) {
			return form[n];
		}
	};
}
