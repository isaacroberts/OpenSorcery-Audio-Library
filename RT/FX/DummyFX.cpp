//
//  DummyFX.cpp
//  
//
//  Created by Isaac Roberts on 2/1/21.
//

// Created by Isaac Roberts


#include "DummyFX.h"

void DummyNoFX::process(ContextNR& c)
{
	c.bypassProcess();
}
void DummyNoFX::process(InputBlock& from, OutputBlock& to, BlockStorage& b)
{
	to.copyFrom(from);
}

