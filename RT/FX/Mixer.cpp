//
//  Mixer.cpp
//  
//
//  Created by Isaac Roberts on 8/15/20.
//

// Created by Isaac Roberts


#include "Mixer.h"

#include <assert.h>

Mixer::Mixer()
{
	mix = -1;
	asyncMix = 1;
}

void Mixer::prepare(Spec& s)
{
}
void Mixer::nrMix(ContextNR& c)
{
	#ifdef DEBUG
		assert (mix != -1);
	#endif
	if (mix >= 1)
	{
		//pass
	}
	else if (mix <= 0)
	{
		//pass, comparisons should have already bypassed this
		c.getOutput().copyFrom(c.getInput());
	}
	else
	{
		auto& b = c.getOutput();
		b *= mix;
		b.addProductOf(c.getInput(), 1-mix);
		
	}
}

void Mixer::replacingPreMix(ContextR& c)
{
	#if DEBUG
		assert (mix != -1);
	#endif
	if (mix <= 0 )
	{
		return;
	}
	if (mix >= 1)
	{
		//pass
	}
	else {
		ptr.alloc(c.getStorage());
		Utility::copy(*ptr, c.getOutput());
	}
}
void Mixer::replacingPostMix(ContextR& c)
{
	if (mix <= 0)
	{
		std::cout<<"Shouldn't be here - in replacing post Mix with mix = "<<mix<<std::endl;
		return;
	}

	if (mix >= 1)
	{
		//pass
	}
	else
	{
		if (!ptr.isAllocated())
		{
			#ifdef DEBUG
			std::cout<<"skipped mix"<<std::endl;
//			Debug::print("skipped mix");
			#endif
		}
		else
		{
			auto& b = c.getOutput();
			b *= mix;
			b.addProductOf(*ptr, 1-mix);
			ptr.free();
		}
	}
}


Mixer::~Mixer()
{
}

