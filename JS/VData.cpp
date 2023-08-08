//
//  VData.cpp
//  Yuzik - App
//
//  Created by Isaac Roberts on 12/9/21.
//

// Created by Isaac Roberts

#include "VData.h"



#if USING_NODE_JS

VData::VData()
{
	
}

v8::Isolate* VData::GetIsolate()
{
	return ces->isolate();
//	return env->isolate();
}
node::CommonEnvironmentSetup* VData::GetCES()
{
	return &*ces;
}
v8::Local<v8::Context> VData::GetContext()
{
	return ces->context();
}

//Destructor
VData::~VData()
{
	
}

#else
VData::VData()
: platform(nullptr),
isolate(NULL),
context()
{
	
}

v8::Local<v8::Context> VData::GetContext()
{
	return context.Get(isolate);
//	return GetIsolate()->GetCurrentContext();
}
v8::Isolate* VData::GetIsolate()
{
	return isolate;
}

//Destructor
VData::~VData()
{
	
}
#endif

