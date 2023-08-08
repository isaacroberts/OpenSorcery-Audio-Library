


#include "V8Host.h"
#include "Util.h"

using namespace js;
using namespace v8;

V8Host V8Host::mainInst;
std::string V8Host::cmdLineArgs="";

#include "v8-object.h"

V8Host::V8Host()
: VData()
, errorSinceRestart(false)
#if !NODE_JS
, bindGlobals([&](v8::Local<v8::ObjectTemplate>, v8::Local<v8::ObjectTemplate>, v8::Isolate*){})
#endif
, onFail([&](){})
{}
void V8Host::init(int argc, const char* const* argv)
{
	cmdLineArgs = std::string(argv[0]);
	
	
	//	void SetPromiseRejectCallback(PromiseRejectCallback callback);

}

void V8Host::end()
{
	V8Host::getInst().end_();
}

V8Host::JSException::JSException(v8::TryCatch& tryCatch, Local<Context> context)
: what_str(cstr(tryCatch.Message()->Get())),
stack(cstr(tryCatch.StackTrace(context), context))
{
	jassert(tryCatch.HasCaught());
}


Lval V8Host::evaluate(const juce::File& file)
{
	CheckHandle;
	auto jc = file.loadFileAsString();
	auto vs = vstr(jc);
	auto fname = vstr(file.getFullPathName());
	return evaluate(vs, fname);
}
Lval V8Host::evaluate(const char* script, const char* origin)
{
	CheckHandle;
	Local<v8::String> vscr = vstr(script);
	Local<v8::String> vorg = vstr(origin);
	return evaluate(vscr, vorg);
}
Lval V8Host::evaluate(std::string script, std::string origin)
{
	CheckHandle;
	Local<v8::String> vscr = vstr(script);
	Local<v8::String> vorg = vstr(origin);
	return evaluate(vscr, vorg);
}
Lval V8Host::evaluate(v8::Local<v8::String> script)
{
	CheckHandle;
	return evaluate(script, vstr("code_literal_v8_str"));
}




Lval V8Host::evaluate(v8::Local<v8::String> script, v8::Local<v8::String> originName)
{
	if (hasFailed()) return Lval();
	
	auto isolate = GetIsolate();
	v8::Isolate::Scope isolate_scope(isolate);
	CheckHandle;
	
	TryCatch try_catch(GetIsolate());

//	Local<Context> context(GetIsolate()->GetCurrentContext());
	auto context = GetContext();
	v8::Context::Scope context_scope(context);

	// Compile the script and check for errors.
	Local<Script> compiled_script;

	ScriptOrigin origin(isolate, originName);

//	std::cout<<"Compile:"<<std::endl;
	if (!Script::Compile(context, script, &origin).ToLocal(&compiled_script)) {
		ReportException(&try_catch);
		return Lval();
	}
	else
	{
		// Run the script!
		Lval result;
		if (!compiled_script->Run(context).ToLocal(&result)) {
			// The TryCatch above is still in effect and will have caught the error.
			std::cout<<"run error"<<std::endl;
//			std::cout<<*error<<std::endl;
			ReportException(&try_catch);
		}
		else
		{
			return result;
		}

	}
	return Lval();
}

Mval V8Host::call_internal(std::string funcName, int argC, Lval* argv)
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
//		V8Host::getInst().printValue(funcl);
		if (!funcl.IsEmpty() && !funcl->IsUndefined() && !funcl->IsNull())
		{
			Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(funcl);
			Lval reciever (global);

			TryCatch try_catch(GetIsolate());
			auto ret = func->Call(ct, global, argC, argv);
			
			if (try_catch.HasCaught())
			{
				ReportException(&try_catch);
				jassertfalse;
				return Mval();
			}
			else
				return ret;
		}
	}
	std::cout<<"function \""<<funcName<<"\" not found"<<std::endl;
	jassertfalse;
	return Mval();
}
Mval V8Host::call_internal(std::string objName, std::string funcName, int argC, Lval* argv)
{
	CheckHandle;
	if (hasFailed()) return Mval();

	auto ct = GetContext();
	
	MaybeLocal<v8::Object> objm = getObject(objName);
	Local<Object> object;
	if (objm.ToLocal(&object))
	{
		Mval funcm = object->Get(ct, vstr(funcName));
		Lval funcl;
		if (funcm.ToLocal(&funcl))
		{
//			V8Host::getInst().printValue(funcl);
			if (!funcl.IsEmpty() && !funcl->IsUndefined() && !funcl->IsNull())
			{
				Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(funcl);
				Lval reciever (object);
				TryCatch try_catch(GetIsolate());
				auto ret = func->Call(ct, object, argC, argv);
				if (try_catch.HasCaught())
				{
					ReportException(&try_catch);
					jassertfalse;
					return Mval();
				}
				else
					return ret;
			}
		}
		std::cout<<"function \""<<funcName<<"\" not found"<<std::endl;
		jassertfalse;
		return Mval();
	}
	std::cout<<"object \""<<objName<<"\" not found"<<std::endl;
	jassertfalse;
	return Mval();
}



Mval V8Host::call(std::string name) {
	CheckHandle;
	return call_internal(name, 0, NULL);
}
Mval V8Host::call(std::string object, std::string name) {
	CheckHandle;
	return call_internal(object, name, 0, NULL);
}
Mval V8Host::callWithData(v8::Local<v8::Function> func, Lval data, Lval extra)
{
	if (data.IsEmpty() || data->IsNullOrUndefined())
	{
		(void)func->Call(GetContext(), getGlobal(), 1, &extra);
	}
	else if (data->IsArray())
	{
		auto arr = data.As<v8::Array>();
		Maybe<bool> didSet = arr->Set(GetContext(), arr->Length(), extra);
		didSet.Check();
		(void)func->Call(GetContext(), getGlobal(), arr->Length(), &data);
	}
	else
	{
		Lval params[2] { data, extra};
		(void)func->Call(GetContext(), getGlobal(), 2, params);
	}
	return Mval();
}

Mval V8Host::await(Mval mval)
{
	if (mval.IsEmpty())
		return mval;
	else {
		return Mval(await(mval.ToLocalChecked()));
	}
}
Lval V8Host::await(Lval val)
{
	if (val->IsNullOrUndefined())
		return val;
	else if (val->IsPromise())
		return await(val.As<Promise>());
	else
		return val;
}
Lval V8Host::await(Local<Promise> promise)
{
	if (promise->HasHandler()) {
		while (promise->State() == Promise::kPending) {
//			spinLoop();
//			Isolate::getCurrent()->
			Isolate::GetCurrent()->PerformMicrotaskCheckpoint();
			std::cout<<"microtasks performed"<<std::endl;
//			auto result = promise->Result();
//			return result;
		}
		if (promise->State() == Promise::kRejected) {
//				Nan::ThrowError(promise->Result());
//				errorSinceRestart=true;
			jassertfalse;
			return Lval();
		}
		else
		{
			return promise->Result();
			// ... procses promise->Result() ...
		}
	}
	else {
		jassertfalse;
		return Lval();
	}
}

//Local<Function> V8Host::getFunction(std::function<void(const FunctionCallbackInfo<Value>& info)> func)
//{
//	return v8::Function::New(GetContext(), )
//}
void V8Host::resolvePromise(Lval pval, v8::FunctionCallback callback, Lval data)
{
	if (pval.IsEmpty() || pval->IsNullOrUndefined())
	{
		jassertfalse;
		return;
	}
	MaybeLocal<Function> funcm = v8::Function::New(GetContext(), callback, data);
	Local<Function> func;
	if (funcm.ToLocal(&func)) {
		if (pval->IsPromise())
		{
			Local<Promise> promise = pval.As<Promise>();
					
			if (promise->HasHandler())
			{
				auto onReject = v8::Function::New(GetContext(), PromiseRejected).ToLocalChecked();
				
				(void)promise->Then(GetContext(), func, onReject);
			}
			else
			{
				MaybeLocal<Promise> ret = promise->Catch(GetContext(), func);
				if (ret.IsEmpty())
				{
					std::cout<<"empty result"<<std::endl;
				}
				else
				{
					Local<Promise> prom2 = ret.ToLocalChecked();
					std::cout<<prom2->HasHandler()<<std::endl;
//					auto p2ret = prom2->Result();
//					printValue("p2ret",p2ret);
				}
			}
		}
		else
		{
			//Immediately call callback
			callWithData(func, data, pval);
		}
	}
	else {
		jassertfalse;
	}
}

Local<v8::Object> V8Host::getGlobal()
{
	CheckHandle;
	return GetContext()->Global();
}
MaybeLocal<v8::Object> V8Host::getObject(std::string name)
{
	CheckHandle;
	if (name=="")
		return MaybeLocal<v8::Object>(getGlobal());
	auto lval = evaluate(name);
	return lval->ToObject(GetContext());
}


//Getters from Object
MaybeLocal<v8::Value> V8Host::getValue(v8::Local<v8::Object> object, std::string name)
{
	CheckHandle;
	auto ct = GetContext();
	if (name=="")
	{
		jassertfalse;
		return MaybeLocal<v8::Value>(object);
	}
	else
	{
		return object->Get(ct, vstr(name));
	}
}
MaybeLocal<v8::Object> V8Host::getObject(v8::Local<v8::Object> object, std::string name)
{
	CheckHandle;
	auto ct = GetContext();

	Mval objm = getValue(object, name);
	Lval objl;
	if (objm.ToLocal(&objl))
	{
		return objl->ToObject(ct);
	}
	else
		return MaybeLocal<v8::Object>();
}
MaybeLocal<v8::Array> V8Host::getArray(v8::Local<v8::Object> object, std::string name)
{
	CheckHandle;

	Mval objm = getValue(object, name);
	Lval objl;
	if (objm.ToLocal(&objl))
	{
		return objl.As<v8::Array>();
	}
	else
		return MaybeLocal<v8::Array>();
}
bool V8Host::getInt(v8::Local<v8::Object> object, std::string name, int& value)
{
	CheckHandle;
	auto ct = GetContext();

	Mval objm = getValue(object, name);
	Lval objl;
	if (objm.ToLocal(&objl))
	{
//		std::cout<<"Type of int value: "<<cstr(objl->TypeOf(GetIsolate()))<<std::endl;
		MaybeLocal<Integer> ivalm = objl->ToInteger(ct);
		Local<Integer> ival;
		if (ivalm.ToLocal(&ival))
		{
			value = (int)ival->Value();
			return true;
		}
	}
	return false;
}

//Getters from Array
MaybeLocal<v8::Value> V8Host::getValue(v8::Local<v8::Array> array, int ix)
{
	CheckHandle;
	auto ct = GetContext();
	return array->Get(ct, ix);
}
MaybeLocal<v8::Object> V8Host::getObject(v8::Local<v8::Array> array, int ix)
{
	CheckHandle;
	auto ct = GetContext();	
	
	Mval objm = getValue(array, ix);
	Lval objl;
	if (objm.ToLocal(&objl))
	{
		return objl->ToObject(ct);
	}
	else
		return MaybeLocal<v8::Object>();
}

void V8Host::printDict(std::string objectName)
{
	if (hasFailed()) return;
	CheckHandle;
	if (objectName=="")
		printDict(getGlobal());
	else
	{
		Lval val = evaluate(objectName);
		if (val.IsEmpty())
			std::cout<<"No value "<<objectName<<std::endl;
		else
			printDict(val, objectName);
	}
}
void V8Host::printDict(Mval valm, std::string label)
{
	Lval val;
	if (valm.ToLocal(&val))
	{
		printDict(val, label);
	}
	else
		std::cout<<"No value "<<label<<std::endl;;
}
void V8Host::printDict(Lval val, std::string label)
{
	MaybeLocal<Object> objm = val->ToObject(GetContext());
	Local<Object> object;
	if (objm.ToLocal(&object))
	{
		printDict(object, label);
	}
	else
	{
		std::cout<<"Not an object - printing value:"<<std::endl;
		printValue(label, val);
	}
}
void V8Host::printDict(MaybeLocal<Object> objm, std::string label)
{
//	if (hasFailed()) return;
//	CheckHandle;

	Local<Object> object;
	if (objm.ToLocal(&object))
	{
		printDict(object, label);
	}
	else
	{
		std::cout<<"No object found "<<label<<std::endl;
	}
}
void V8Host::printDict(Local<Object> object, std::string label)
{
//	printValue(object);
	if (object->IsNullOrUndefined())
		return;

//	auto rec = JSReceiver::cast(*object);
//
//	Local<Object> global = GetContext()->Global();
//	MaybeLocal<Object> gm = MaybeLocal<Object>(global);
//
//	Local<Object> g2;
//	gm.ToLocal(&g2);
	
	MaybeLocal<v8::Array> arrm = object->GetOwnPropertyNames(GetContext());
	Local<v8::Array> arr;
	if (label!="")
		std::cout<<label<<": {"<<std::endl;
	else
		std::cout<<"{"<<std::endl;
	if (arrm.ToLocal(&arr))
	{
		for (int n=0; n < arr->Length(); ++n)
		{
			Mval name = arr->Get(GetContext(), n);
			std::cout<<"\t"<<n<<": "<<cstr(name)<<std::endl;
		}
	}
	std::cout<<"}"<<std::endl;
}

void V8Host::LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() >= 1)
		HandleScope scope(args.GetIsolate());
//	std::cout<<"Log: ";
	for (int n=0; n < args.Length(); ++n)
	{
		Lval arg = args[n];
		std::string cc = cstr(arg);
		std::cout<<cc<<" ";
	}
	std::cout<<""<<std::endl;
}
void V8Host::AssertCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
#if DEBUG
	if (args.Length()==0)
		jassertfalse;
	else
	{
		if (args[0]->ToBoolean(getIsolate())->Value()==false)
		{
			if (args.Length()>1)
			{
				auto msg = cstr(args[1], getContext());
				std::cout<<msg<<std::endl;
				jassertfalse;
			}
			else
				jassertfalse;
		}
	}
#endif
}
void V8Host::setOutputDir(std::string path)
{
	loggingDir = path;
	if (loggingDir[0]=='/') {}
	else if (loggingDir[0]=='~')
	{
		loggingDir = loggingDir.substr(1);
		loggingDir = "/Users/isaacroberts"+loggingDir;
	}
	else
	{
		loggingDir = "/Users/isaacroberts/Desktop/"+loggingDir;
	}
	//TODO: Create file if does not exist
}
void V8Host::SaveCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() >= 1)
		HandleScope scope(args.GetIsolate());
	std::string filename;
	if (args.Length()>=2)
		filename = cstr(args[1]);
	else
		filename = "output.txt";
	if (filename[0]=='/') {}
	else if (filename[0]=='~') {
		filename = filename.substr(1);
		filename = "/Users/isaacroberts"+filename;
	}
	else
	{
		filename = getInst().loggingDir+'/'+filename;
	}
	
	std::string cc = cstr(args[0]);
	std::ofstream file;
	file.open (filename);
	file << cc;
	file.close();
}

void V8Host::ReportException(v8::TryCatch* try_catch) {
	v8::HandleScope handle_scope(GetIsolate());
	auto message = try_catch->Message();
	if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error; just
		// print the exception.
		std::cerr<<" -- no hecking exception -- "<<std::endl;
		v8::String::Utf8Value exception(isolate, try_catch->Exception());
		auto exception_string = *exception;
		fprintf(stderr, "%s\n", exception_string);
	}
	else {
		ReportException(try_catch->Message());
		auto context = GetContext();
		Lval stack_trace_string;
		if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
			stack_trace_string->IsString() &&
			v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
			v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
			auto stack_trace_string = *(stack_trace);
			fprintf(stderr, "Msg: %s\n", stack_trace_string);
		}
	}
}
void V8Host::ReportException(Local<v8::Message> message)
{
	v8::HandleScope handle_scope(GetIsolate());
	//Thanks StackOverflow
	auto isolate = GetIsolate();
	if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error; just
		// print the exception.
		std::cerr<<" -- no hecking message -- "<<std::endl;
		std::cerr<<"Weep ye all who enter here"<<std::endl;
	} else {
		// Print (filename):(line number): (message).
		
		auto msgstrv = message->Get();
		auto msgstr = cstr(msgstrv);
		std::cerr<<"Msg: "<<msgstr<<std::endl;
		
		std::cerr<<"Stack trace:"<<std::endl;
		
		v8::String::Utf8Value filename(isolate,
									   message->GetScriptOrigin().ResourceName());
		auto context = GetContext();
		const char* filename_string = *(filename);
		const char* exception_string = "";
		int linenum = message->GetLineNumber(context).FromJust();
		int start = message->GetStartColumn(context).FromJust();
		int end = message->GetEndColumn(context).FromJust();
		fprintf(stderr, "%s:%i:%i: %s\n", filename_string, linenum, start, exception_string);
		// Print line of source code.
		v8::String::Utf8Value sourceline(
										 isolate, message->GetSourceLine(context).ToLocalChecked());
		
		char* sourceline_string;
		

		//Clip overly long strings from webpack
		if (sourceline.length() > 200)
		{
			int line_start = std::max(0, start-20);
			int line_end = std::min(sourceline.length(), end + 25) - line_start;
			
			sourceline_string = *(sourceline) + line_start;
			sourceline_string[line_end] = '\0';
			
			start -= line_start;
			end = std::min(end-line_start, line_end);
			
		}
		else
		{
			sourceline_string = *(sourceline);
		}
		fprintf(stderr, "%s\n", sourceline_string);
		// Print wavy underline (GetUnderline is deprecated).
		for (int i = 0; i < start; i++) {
			fprintf(stderr, " ");
		}
		for (int i = start; i < end; i++) {
			fprintf(stderr, "^");
		}
		fprintf(stderr, "\n");
	}
	
	errorSinceRestart = true;
	onFail();
}


void V8Host::PromiseRejected(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	LogCallback(args);
	getInst().errorSinceRestart=true;
	jassertfalse;
}
void V8Host::PromiseRejectCallback_(v8::PromiseRejectMessage reject_message) {
//	auto context = GetContext();
	std::cout<<"Promise rejected"<<std::endl;
	switch (reject_message.GetEvent()) {
		case v8::kPromiseRejectWithNoHandler: {
			v8::Local<v8::Message> message = v8::Exception::CreateMessage(
							  GetIsolate(), reject_message.GetValue());
			ReportException(message);
		}
		case v8::kPromiseHandlerAddedAfterReject: {
			std::cout<<"Promise handler added after reject"<<std::endl;
			break;
		}
		case v8::kPromiseRejectAfterResolved: {
			std::cout<<"Promise rejected after resolve"<<std::endl;
			break;
		}
		case v8::kPromiseResolveAfterResolved: {
			std::cout<<"Promise resolved after resolve"<<std::endl;
			break;
		}
	}
}

void V8Host::restart()
{
	errorSinceRestart=false;
}

juce::File V8Host::relativeFile(juce::String path)
{
#if DEBUG
	#if JUCE_MAC
		/* MAC:
		 
			This works ONLY if the application is in /Builds/MacOSX/build/Debug/
			
			When we bundle this for release, we'll have to package the javascript in the application folder
		 */
		auto file = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
		return file.getChildFile("../../../../../js-source/"+path);
	#else
		/* Windows:
			Same, the application must be in /Builds/VisualStudio/whatever/whatever
		 
			This will have to be bundled with the windows application as well
		 */
		return juce::File::getCurrentWorkingDirectory().getChildFile("../../js-source/" + path);
	#endif
#else
	assert(false);
	return juce::File();
#endif
}

