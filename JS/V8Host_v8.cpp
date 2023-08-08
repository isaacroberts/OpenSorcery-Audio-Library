//
//  V8Host_v8.cpp
//  
//
//  Created by Isaac Roberts on 1/12/22.
//

#include "V8Host.h"

#include "Util.h"


using namespace v8;


using namespace js;


void V8Host::end_()
{
	context.Get(getIsolate())->Exit();
//	V8::Dispose();
//	V8::ShutdownPlatform();
}
V8Host::~V8Host()
{
	
}


//void log_message(Local<Message> message, Local<Value> data)
//{
//	auto str = cstr(message->Get());
//	std::cout<<"V8: "<<str<<std::endl;
//	auto ct = context
//	std::cout<<cstr(message->GetSource(ct))<<std::endl;
//
//	auto stack = message->GetStackTrace();
//	if (!stack.IsEmpty())
//	{
//		for (int n=0; n < stack->GetFrameCount(); ++n)
//		{
//			auto f = stack->GetFrame(V8Host::getIsolate(), n);
//			std::cout<<n<<":\t"<<cstr(f->GetScriptName())<<" @ "<<f->GetLineNumber()<<std::endl;
//		}
//		std::cout<<std::endl;
//	}
//}



class Console {
	public:
		Console() {}
		~Console() {}
	private:
	public:

};
Local<Object> WrapObject(Console *c, v8::Local<v8::Context> context) {

	EscapableHandleScope handle_scope(V8Host::getIsolate());

	Local<ObjectTemplate> class_t;
	Local<ObjectTemplate> raw_t = ObjectTemplate::New(V8Host::getIsolate());
	raw_t->SetInternalFieldCount(1);
	raw_t->Set(
		v8::String::NewFromUtf8(V8Host::getIsolate(), "log", v8::NewStringType::kNormal).ToLocalChecked(),
		v8::FunctionTemplate::New(V8Host::getIsolate(), V8Host::LogCallback));
//	raw_t->Set(
//		v8::String::NewFromUtf8(GetIsolate(), "error", v8::NewStringType::kNormal).ToLocalChecked(),
//		v8::FunctionTemplate::New(GetIsolate(), error));
	class_t = Local<ObjectTemplate>::New(V8Host::getIsolate(),raw_t);
	//create instance
	Local<Object> result = class_t->NewInstance(context).ToLocalChecked();
	//create wrapper
	Local<External> ptr = External::New(V8Host::getIsolate(),c);
	result->SetInternalField(0,ptr);
	return handle_scope.Escape(result);
}


void V8Host::start()
{
	auto src = cmdLineArgs.c_str();
	
	v8::V8::InitializeICUDefaultLocation(src);
	v8::V8::InitializeExternalStartupData(src);
	platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();
	
	//Snapshot blob
	
//	const intptr_t* example_source = nullptr;
//	v8::SnapshotCreator snapshotCreator (example_source, nullptr);
//	v8::StartupData startupData = snapshotCreator.CreateBlob(v8::SnapshotCreator::FunctionCodeHandling::kKeep);
	
	
	//Create a new Isolate and make it the current one.
	//Isolate is like a window of google chrome
	v8::Isolate::CreateParams create_params;
	//	create_params.snapshot_blob = &startupData;
	create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	isolate = v8::Isolate::New(create_params);
	
	// Create a stack-allocated handle scope.
	v8::HandleScope handle_scope(isolate);
	
	Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
	
	Isolate::Scope isolate_scope(isolate);
	
	bindDefaults(global);
	
	v8::Local<v8::Context> context_ = Context::New(isolate, NULL, global);
	Context::Scope context_scope(context_);
	
	context.Reset(isolate, context_);
	
	isolate->SetPromiseRejectCallback(PromiseRejectCallback);
	
	isolate->Enter();
	context.Get(isolate)->Enter();
	
	printDict();

}

//void strConstructor_(const FunctionCallbackInfo<Value>& info)
//{
//	info.GetReturnValue().Set(v8::String::NewFromUtf8Literal(V8Host::getIsolate(), "hello"));
//}

#include <ctime>
#include <chrono>

void V8Host::hrtime(const FunctionCallbackInfo<v8::Value>& info)
{
//	std::cout<<" ==== HRTIME ===== "<<std::endl;
//	LogCallback(info);
	auto arr = v8::Array::New(getIsolate(), 2);
	
	using namespace std::chrono;

	uint64_t sec_since_epoch = duration_cast<std::chrono::seconds>(system_clock::now().time_since_epoch()).count();

	long nano_since_epoch = duration_cast<std::chrono::nanoseconds>(system_clock::now().time_since_epoch()).count();

	nano_since_epoch -= sec_since_epoch*1000000000;
	
	(void)arr->Set(getContext(), 0, v8::Number::New(getIsolate(), sec_since_epoch));
	(void)arr->Set(getContext(), 1, v8::Number::New(getIsolate(), nano_since_epoch));
//	std::cout<<"sec, nano: "<<sec_since_epoch<<", "<<nano_since_epoch<<std::endl;
	info.GetReturnValue().Set(arr);
}
/*
 Handle<Value> Include(const Arguments& args) {
	 for (int i = 0; i < args.Length(); i++) {
		 String::Utf8Value str(args[i]);

		 // load_file loads the file with this name into a string,
		 // I imagine you can write a function to do this :)
		 std::string js_file = load_file(*str);

		 if(js_file.length() > 0) {
			 Handle<String> source = String::New(js_file.c_str());
			 Handle<Script> script = Script::Compile(source);
			 return script->Run();
		 }
	 }
	 return Undefined();
 }

 Handle<ObjectTemplate> global = ObjectTemplate::New();

 global->Set(String::New("include"), FunctionTemplate::New(Include));

 */
void V8Host::bindDefaults(Local<ObjectTemplate> global)
{
	auto glob2 = v8::ObjectTemplate::New(isolate);
	
//	auto strConstructor = v8::FunctionTemplate::New(isolate, strConstructor_);
	auto strFields = v8::ObjectTemplate::New(isolate);
//	strFields->Set(isolate, "Str", vstr(""));
	glob2->Set(isolate, "Headers", strFields);
	glob2->Set(isolate, "Response", strFields);
	glob2->Set(isolate, "Request", strFields);
	
//	glob2->Set(isolate, "fetch", FunctionTemplate::New(isolate, fetch));
//
	bindGlobals(global, glob2, isolate);
	
	auto proc = v8::ObjectTemplate::New(isolate);
	proc->Set(isolate, "hrtime", FunctionTemplate::New(isolate, hrtime));

	global->Set(isolate, "process", proc);
	
	global->Set(isolate, "global", glob2);
	global->Set(isolate, "self", glob2);
	global->Set(isolate, "window", glob2);
//	global->Set(isolate, "self", obj);
	
	auto logfunc = FunctionTemplate::New(isolate, LogCallback);
	global->Set(isolate, "log", logfunc);

	auto assertFunc = FunctionTemplate::New(isolate, AssertCallback);
	global->Set(isolate, "assert", assertFunc);
	
	auto savefunc = FunctionTemplate::New(isolate, SaveCallback);
	global->Set(isolate, "saveToFile", savefunc);

//	global->Set(isolate, "global", global);
//	auto cons = v8::ObjectTemplate::New(isolate);
//	cons->SetInternalFieldCount(1);
//	cons->Set(vstr("logpenis"), logfunc);
//	global->Set(isolate, "console", cons);
	
	
//	Console* c = new Console();
//	Local<Object> con = WrapObject(c, context_);
//	auto cstrd = vstr("console");
//	auto consstr = v8::Local<v8::Name>::Cast (cstrd);
//	global->Set(consstr, con);
	
}


void V8Host::spinLoop()
{
//	node::SpinEventLoop(GetEnv());
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
			auto ret = func->Call(ct, global, 0, NULL);

//			auto ret = node::MakeCallback(GetIsolate(), global, func, 0, NULL, {0,0});
			
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
void V8Host::reset()
{
	
}


