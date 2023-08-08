//
//  VData.h
//  Yuzik - App
//
//  Created by Isaac Roberts on 12/9/21.
//

// Created by Isaac Roberts 

#ifndef VData_h
#define VData_h

//#include <stdio.h>

#include <JuceHeader.h>


#if USING_NODE_JS

#include "env-inl.h"
#include "node.h"
#include "uv.h"

#else

#include "libplatform/libplatform.h"
#include "v8.h"
//#include "V8/include/libplatform/libplatform.h"
//#include "V8/include/v8-context.h"
//#include "V8/include/v8-exception.h"
//#include "V8/include/v8-initialization.h"
//#include "V8/include/v8-isolate.h"
//#include "V8/include/v8-local-handle.h"
//#include "V8/include/v8-script.h"
//#include "V8/include/v8-template.h"

//#include "v8-context.h"
//#include "v8-exception.h"
//#include "v8-initialization.h"
//#include "v8-isolate.h"
//#include "v8-local-handle.h"
//#include "v8-script.h"
//#include "v8-template.h"
#endif



/*
	Version: 9.6.180.20
 */


class VData {
public:
	
	
#if USING_NODE_JS
	node::CommonEnvironmentSetup* GetCES();
	node::Environment* GetEnv() { return ces->env();}
#else

#endif
	v8::Local<v8::Context> GetContext();
	v8::Isolate* GetIsolate();
	
protected:
	
#if USING_NODE_JS
	std::unique_ptr<node::MultiIsolatePlatform> platform;
	std::unique_ptr<node::CommonEnvironmentSetup> ces;
#else
	std::unique_ptr<v8::Platform> platform;
	v8::Isolate* isolate;
	v8::Global<v8::Context> context;
#endif
//	node::Environment* env;
	
protected:
	VData();
	virtual ~VData();
private:
	VData(const VData& other)=delete;
	VData& operator=(const VData& other)=delete;
};

#endif /* VData_h */
