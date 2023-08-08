//
//  V8Host.h
//  Yuzik - App
//
//  Created by Isaac Roberts on 12/5/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "VData.h"

#include "Util.h"

typedef v8::Local<v8::Value> Lval;
typedef v8::MaybeLocal<v8::Value> Mval;


#if NODE_JS
#define OpenHandle jassert(!v8::Locker::IsLocked(V8Host::getIsolate()));v8::Locker locker(V8Host::getIsolate());v8::HandleScope handle_scope(V8Host::getIsolate());
#else
#define OpenHandle v8::HandleScope handle_scope(V8Host::getIsolate());
#endif

#if DEBUG
#define CheckHandle V8Host::check_handles__();
#else
#define CheckHandle
#endif

class V8Host : public VData
{
	
public:
	static void check_handles__()
	{
#if NODE_JS
		bool locked = v8::Locker::IsLocked(getIsolate());
		jassert(locked);
#else
		//hope i guess
#endif
	}

	
private:
	static V8Host mainInst;
public:
	static inline V8Host& getInst() { return mainInst;}
	static void init(int argc, const char* const* argv);
	static void end();
		
	//Setup thread
public:
	void start();
	void reset();
	
	static juce::File relativeFile(juce::String f);
	
	Lval evaluate(const juce::File& file);
	Lval evaluate(const char* script, const char* origin="cstr_literal\0");
	Lval evaluate(std::string script, std::string origin="str_literal");
	Lval evaluate(v8::Local<v8::String> script);
	Lval evaluate(v8::Local<v8::String> script, v8::Local<v8::String> origin);

	Mval call(std::string name);
	Mval call(std::string object, std::string name);
	
	Mval callAsync(std::string name);
	
	Mval await(Mval);
	Lval await(Lval);
	Lval await(v8::Local<v8::Promise>);
	
private:
	template <typename T>
	static void typedMemberCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		auto& i = getInst();
		auto ct = i.GetContext();
		auto arr = info.Data()->ToObject(ct).ToLocalChecked();
		T* member = static_cast<T*>(arr->Get(ct, 0).ToLocalChecked().As<v8::External>()->Value());
		void* vp = arr->Get(ct,1).ToLocalChecked().As<v8::External>()->Value();
		void (T::*f)(const v8::FunctionCallbackInfo<v8::Value>&);
		
		// convert it to void** which is also an object pointer
		void ** ppv = reinterpret_cast<void**>(&f);
		*ppv = vp;

		((*member).*f)(info);
	}
public:
	template<typename T>
	v8::Local<v8::Function> makeFunction(T* obj, void (T::*callback)(const v8::FunctionCallbackInfo<v8::Value>&))
	{
		auto arr = v8::Array::New(GetIsolate(), 2);
		(void)arr->Set(GetContext(), 0, v8::External::New(GetIsolate(), (void*)obj));
		auto fpp= &callback;
		void ** vpp = reinterpret_cast<void**>(fpp);
		auto vp = *vpp;
		(void)arr->Set(GetContext(), 1, v8::External::New(GetIsolate(), vp));
		auto mf = v8::Function::New(GetContext(), typedMemberCallback<T>, arr);
		//confidence
		return mf.ToLocalChecked();
	}
	v8::Local<v8::Function> makeFunction(v8::FunctionCallback callback)
	{
		auto mf = v8::Function::New(GetContext(), callback);
		//confidence
		return mf.ToLocalChecked();
	}

	
	
	void spinLoop();
	
	void resolvePromise(Lval promise, v8::FunctionCallback callback, Lval data=Lval());
	
	
	v8::Local<v8::Object> getGlobal();
	v8::MaybeLocal<v8::Object> getObject(std::string name);
	v8::MaybeLocal<v8::Object> getObject(v8::Local<v8::Object>, std::string name);
	v8::MaybeLocal<v8::Value> getValue(v8::Local<v8::Object>, std::string name);
	v8::MaybeLocal<v8::Array> getArray(v8::Local<v8::Object>, std::string name);
	bool getInt(v8::Local<v8::Object>, std::string name, int& value);
	
	v8::MaybeLocal<v8::Value> getValue(v8::Local<v8::Array>, int ix);
	v8::MaybeLocal<v8::Object> getObject(v8::Local<v8::Array>, int ix);
	

	v8::Local<v8::Object> hardCastObject(Mval v) { return hardCastObject(v.ToLocalChecked()); }
	v8::Local<v8::Object> hardCastObject(Lval v) { return v->ToObject(GetContext()).ToLocalChecked(); }
	
#if NODE_JS
	static node::CommonEnvironmentSetup* getCES() { return mainInst.GetCES(); }
	static node::Environment* getEnv() { return mainInst.GetEnv(); }
#endif
	static v8::Local<v8::Context> getContext() { return mainInst.GetContext(); }
	static v8::Isolate* getIsolate() { return mainInst.GetIsolate(); }

	
protected:
#if NODE_JS
	void startExecution();
	int RunNodeInstance(node::MultiIsolatePlatform* platform,
							   const std::vector<std::string>& args,
							   const std::vector<std::string>& exec_args);

	void bindDefaults();	
#else
	void bindDefaults(v8::Local<v8::ObjectTemplate>);
	
#endif
	void ReportException(v8::TryCatch* try_catch);
	void ReportException(v8::Local<v8::Message> message);
	
	static void PromiseRejected(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void PromiseRejectCallback(v8::PromiseRejectMessage msg) { getInst().PromiseRejectCallback_(msg); }
	void PromiseRejectCallback_(v8::PromiseRejectMessage);

//	void PromiseRejected_(const v8::FunctionCallbackInfo<v8::Value>& args);
	
	
public:
	static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
	void setOutputDir(std::string path);
	static void SaveCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void AssertCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
#if !NODE_JS
//	static void fetch(const v8::FunctionCallbackInfo<v8::Value>& info);
	static void hrtime(const v8::FunctionCallbackInfo<v8::Value>& info);
#endif
	
	
private:
	V8Host();
	virtual ~V8Host();
	void end_();
	
	V8Host(const V8Host&) = delete;
	V8Host& operator=(const V8Host&) = delete;
	
	
protected:
	static std::string cmdLineArgs;
	
	bool errorSinceRestart=false;
	
	bool hasFailed() {
		return false;
//		jassert(!errorSinceRestart);
//		return errorSinceRestart;
	}
	
	
public:
	
	std::string loggingDir;
#if NODE_JS
	
#else
	std::function<void(v8::Local<v8::ObjectTemplate> globMain, v8::Local<v8::ObjectTemplate> glob2, v8::Isolate*)> bindGlobals;
#endif
	
	std::function<void()> onFail;
	void restart();
	
public:
	
	
public:
	
	template <class Obj>
	void printValue(v8::Local<Obj> val)
	{
		if (val->IsPromise())
		{
			std::cout<<"[Promise]"<<std::endl;
		}
		else
		{
			auto json = v8::JSON::Stringify(GetContext(), val);
			std::cout<<js::cstr(json)<<std::endl;
		}
	}
	template <class Obj>
	void printValue(v8::MaybeLocal<Obj> mval)
	{
		Lval val;
		if (mval.ToLocal(&val))
		{
			printValue(val);
		}
		else
			std::cout<<"{}"<<std::endl;
	}
	
	template <class Obj>
	void printValue(std::string name, v8::Local<Obj> val)
	{
		jassert(!val.IsEmpty());
		if (val->IsPromise())
		{
			std::cout<<name<<"[Promise]"<<std::endl;
		}
		else
		{
			auto json = v8::JSON::Stringify(GetContext(), val);
			std::cout<<name<<" = "<<js::cstr(json)<<std::endl;
		}
	}
	template <class Obj>
	void printValue(std::string name, v8::MaybeLocal<Obj> mval)
	{
		Lval val;
		if (mval.ToLocal(&val))
		{
			printValue(name, val);
		}
		else
			std::cout<<name<<"{}"<<std::endl;
	}
	
//	void printValue(Lval val);
//	void printValue(Mval val);
	
	void printDict(std::string objectName="");
	void printDict(v8::MaybeLocal<v8::Value> val, std::string label="");
	void printDict(v8::Local<v8::Value> val, std::string label="");
	void printDict(v8::MaybeLocal<v8::Object> val, std::string label="");
	void printDict(v8::Local<v8::Object> val, std::string label="");

	template <typename dtype>
	Lval toLval(dtype p) {
		return v8pp::to_v8(GetIsolate(), p);
	}

	
	
public:
	
	template <typename dtype>
	Mval call(std::string name, dtype param1) {
		CheckHandle;
		Lval vals[1] {toLval(param1)};
		return call_internal(name, 1, vals);
	}
	template <typename dtype, typename d2>
	Mval call(std::string name, dtype param1, d2 param2) {
		CheckHandle;
		return call_internal(name, 2, {toLval(param1), toLval(param2)});
	}
	template <typename dtype, typename d2, typename d3>
	Mval call(std::string name, dtype param1, d2 param2, d3 param3) {
		CheckHandle;
		return call_internal(name, 3, {toLval(param1), toLval(param2), toLval(param3)});
	}
	template <typename dtype>
	Mval call(std::string object, std::string name, dtype param1) {
		CheckHandle;
		Lval vals = toLval(param1);
		return call_internal(object, name, 1, &vals);
	}
	template <typename dtype, typename d2>
	Mval call(std::string object, std::string name, dtype param1, d2 param2) {
		CheckHandle;
		return call_internal(object, name, 2, {toLval(param1), toLval(param2)});
	}
	template <typename dtype, typename d2, typename d3>
	Mval call(std::string object, std::string name, dtype param1, d2 param2, d3 param3) {
		CheckHandle;
		return call_internal(object, name, 3, {toLval(param1), toLval(param2), toLval(param3)});
	}
	
protected:
	Mval call_internal(std::string name, int argc, Lval* argv);
	Mval call_internal(std::string object, std::string name, int argc, Lval* argv);

	Mval callWithData(v8::Local<v8::Function>, Lval data, Lval extra);
	
	
public:
	struct JSException : public std::exception
	{
//		JSException() : std::exception() {}
		JSException(v8::TryCatch&, v8::Local<v8::Context>);
		virtual const char* what() const noexcept { return what_str.c_str();}
//		virtual String* where() { return where_str; }
		
		std::string what_str;
		std::string stack;
	};
};
