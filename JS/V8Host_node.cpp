//
//  V8Host.cpp
//  Yuzik - App
//
//  Created by Isaac Roberts on 12/5/21.
//

// Created by Isaac Roberts

#include "V8Host.h"

#include "Util.h"

using namespace node;
using namespace v8;

void V8Host::end_()
{
//	context.Get(env->isolate())->Exit();
//	V8::Dispose();
//	V8::ShutdownPlatform();
	
	node::Stop(ces->env());
}
V8Host::~V8Host()
{
	//If not ended, call end_
}

void V8Host::reset()
{
//	node::Stop(ces->env());

}


void fatal_callback(const char* location, const char* error)
{
	std::cout<<"Fatal @ "<<location<<":\n\t"<<error<<std::endl;
}
void log_message(Local<Message> message, Local<Value> data)
{
	auto str = cstr(message->Get());
	std::cout<<"V8: "<<str<<std::endl;
	auto ct = V8Host::getContext();
	std::cout<<cstr(message->GetSource(ct))<<std::endl;
	
	auto stack = message->GetStackTrace();
	if (!stack.IsEmpty())
	{
		for (int n=0; n < stack->GetFrameCount(); ++n)
		{
			auto f = stack->GetFrame(V8Host::getIsolate(), n);
			std::cout<<n<<":\t"<<cstr(f->GetScriptName())<<" @ "<<f->GetLineNumber()<<std::endl;
		}
		std::cout<<std::endl;
	}
}

void V8Host::start()
{
//	int argc = 1;
//	char* src = new char[cmdLineArgs.length()+1];
//	memcpy(src, cmdLineArgs.c_str(), cmdLineArgs.length());
//	src[cmdLineArgs.length()] = '\0';
//	char** argv = &src;
	
	int argc = 3;
	char** argv = new char*[argc]{"node", "-e", "index.js"};
	
	argv = uv_setup_args(argc, argv);
	
	std::vector<std::string> args(argv, argv + argc);
	std::vector<std::string> exec_args; //{ "npm --version" };
	std::vector<std::string> errors;
	// Parse Node.js CLI options, and print any errors that have occurred while
	// trying to parse them.
	int exit_code = node::InitializeNodeWithArgs(&args, &exec_args, &errors);
	for (const std::string& error : errors)
		fprintf(stderr, "Errors: %s: %s\n", args[0].c_str(), error.c_str());
	if (exit_code != 0) {
		jassertfalse;
		return;
	}
	
	// Create a v8::Platform instance. `MultiIsolatePlatform::Create()` is a way
	// to create a v8::Platform instance that Node.js can use when creating
	// Worker threads. When no `MultiIsolatePlatform` instance is present,
	// Worker threads are disabled.
	platform = MultiIsolatePlatform::Create(4);
	V8::InitializePlatform(platform.get());
	V8::Initialize();
	
	// See below for the contents of this function.
	//	RunNodeInstance(platform.get(), args, exec_args);
	std::vector<std::string> errors2;
//	EnvironmentFlags::Flags flags =  (EnvironmentFlags::Flags) (EnvironmentFlags::kHideConsoleWindows | EnvironmentFlags::kNoGlobalSearchPaths);
	ces = CommonEnvironmentSetup::Create(&*platform, &errors2, args, exec_args);
	
//	for (int n=0; n < argc; ++n) {
//		delete argv[n];
//	}
	delete[] argv;
	
	if (!ces) {
		for (const std::string& err : errors2)
			fprintf(stderr, "CES Err: %s\n", err.c_str());
		return;
	}
	
//	ces->isolate()->SetFatalErrorHandler(fatal_callback);
//	ces->isolate()->AddMessageListener(log_message);
//	ces->isolate()->Enter();
		
	OpenHandle;
	
//	GetIsolate()->SetPromiseRejectCallback(PromiseRejectCallback);
	
//	auto global = GetContext()->Global();
//	auto logfunc = Function::New(GetContext(), LogCallback).ToLocalChecked();
//	auto logkey = vstr("log");
//	(void)global->Set(GetContext(), logkey, logfunc);

	startExecution();
	
	ces->isolate()->SetFatalErrorHandler(fatal_callback);
	ces->isolate()->AddMessageListener(log_message);
	ces->isolate()->Enter();
	
	bindDefaults();
	
	return;
}

void V8Host::startExecution()
{
	//	CheckHandle;
	auto* isolate = ces->isolate();
	auto* env = ces->env();
	
	Locker locker(isolate);
	Isolate::Scope isolate_scope(isolate);
	HandleScope handle_scope(isolate);
	Context::Scope context_scope(ces->context());
	
	TryCatch try_catch(isolate);
	
//#define module_path process.cwd() + '/'
#define module_path "'/Users/isaacroberts/Dropbox/Lemonaid/Model/js/'"
	
	auto script =
				"const publicRequire ="
				"  require('module').createRequire(" module_path ");"
				"console.log(process.cwd());"
				"globalThis.require = publicRequire;"
				"globalThis.embedVars = { something_isaac: 'woofwoof' };"
				"require('vm').runInThisContext(process.argv[1]);"
//				"require('@magenta/music/node/music_rnn');"
//				"require('@magenta/music/node/core');"
	;
	
	MaybeLocal<Value> loadenv_ret = node::LoadEnvironment(env, script);
	
	if (try_catch.HasCaught())
	{
		ReportException(&try_catch);
		return;
	}
	
	if (loadenv_ret.IsEmpty())  // There has been a JS exception.
	{
		jassertfalse;
		return;
	}

	node::SpinEventLoop(env);

	// node::Stop() can be used to explicitly stop the event loop and keep
	// further JavaScript from running. It can be called from any thread,
	// and will act like worker.terminate() if called from another thread.
	node::Stop(env);

	
//	printValue(loadenv_ret);
}
void V8Host::spinLoop()
{
	node::SpinEventLoop(GetEnv());
}

Mval V8Host::callAsync(std::string funcName)
{
	CheckHandle;
	if (hasFailed()) return Mval();

	auto ct = GetContext();
//	GetIsolate()->Enter();
	Local<v8::Object> global = ct->Global();
	Mval funcm = global->Get(ct, vstr(funcName));
	Lval funcl;
	if (funcm.ToLocal(&funcl))
	{
		if (!funcl.IsEmpty() && !funcl->IsUndefined() && !funcl->IsNull())
		{
			Local<v8::Function> func = funcl.As<v8::Function>();

			TryCatch try_catch(GetIsolate());
//			auto ret = func->Call(ct, global, argC, argv);

			auto ret = node::MakeCallback(GetIsolate(), global, func, 0, NULL, {0,0});
			
			if (try_catch.HasCaught())
			{
				ReportException(&try_catch);
//				jassertfalse;
				return Mval();
			}
			else
				return ret;
		}
	}
	std::cout<<"function \""<<funcName<<"\" not found"<<std::endl;
//	jassertfalse;
	return Mval();

}
void V8Host::bindDefaults()
{
//	OpenHandle;
//
//	auto env = ces->env();
//	auto context = env->context();
//	auto ct = env->binding_data_ctor_template();
//	auto gf = ct->GetFunction(env->context());
//	Local<Function> ctor = gf.ToLocalChecked();
//	Local<Object> target = ctor->NewInstance(env->context()).ToLocalChecked();
	//??? crashing because it says isolate->context() is null 
//	Local<Object> module = Object::New(env->isolate());
//
////	Local<Object> target = Object::New(env->isolate());
////	module->Set(env->context(), vstr("juce"), target).Check();
//
//	Local<Object> undef_self = Object::New(env->isolate());
//	module->Set(env->context(), vstr("self"), undef_self).Check();
	
//	env->Set
	
//	env->SetMethod(target, "sayHi", SayHello);
	
}
