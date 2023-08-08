//
//  Util.cpp
//  Yuzik - App
//
//  Created by Isaac Roberts on 12/9/21.
//

// Created by Isaac Roberts


#include "Util.h"


#include "V8Host.h"

using namespace v8;


namespace js {
juce::var jvar(v8::Local<v8::Value> val, v8::Local<v8::Context> ct)
{
	if (val.IsEmpty())
		return juce::var("empty");
	if (val->IsNull())
		return juce::var("null");
	if (val->IsString())
		return juce::var(jstr(val->ToString(ct)));
	if (val->IsInt32())
		return juce::var((**val->ToInt32(ct).ToLocalChecked()).Value());
	if (val->IsUint32())
		return juce::var((juce::int64)(**val->ToUint32(ct).ToLocalChecked()).Value());
	if (val->IsNumber())
		return juce::var(val->NumberValue(ct).ToChecked());
	if (val->IsBoolean())
		return juce::var((*val->ToBoolean(V8Host::getIsolate()))->Value());
	if (val->IsObject())
	{
		auto json = v8::JSON::Stringify(ct, val);
		auto jjs = jstr(json);
		return "{"+jjs+"}";
	}
	auto type = val->TypeOf(V8Host::getIsolate());
	juce::String jst = jstr(type);
	return juce::var("val["+jst+"]");
}
	
std::string cstr(const v8::Local<v8::Value> val, Local<Context> ct) {
	//	return std::string(jstr(val, ct).toRawUTF8());
	 if (val.IsEmpty())
		 return "empty";
	 if (val->IsNull())
		 return "null";
	 if (val->IsString())
		 return cstr(val->ToString(ct));
	 if (val->IsInt32())
		 return std::to_string((**val->ToInt32(ct).ToLocalChecked()).Value());
	 if (val->IsUint32())
		 return std::to_string((juce::int64)(**val->ToUint32(ct).ToLocalChecked()).Value());
	 if (val->IsNumber())
		 return std::to_string(val->NumberValue(ct).ToChecked());
	 if (val->IsBoolean())
		 return std::to_string((*val->ToBoolean(V8Host::getIsolate()))->Value());
	 if (val->IsObject())
	 {
		 auto json = v8::JSON::Stringify(ct, val);
		 return "{"+cstr(json)+"}";
	 }
	 auto type = val->TypeOf(V8Host::getIsolate());
	 auto cst = cstr(type);
	 return std::string("val[")+cst+"]";

}
juce::var jvar(v8::MaybeLocal<v8::Value> mval, v8::Local<v8::Context> ct)
{
	if (mval.IsEmpty())
	{
		return juce::var();
	}
	else {
		return jvar(mval.ToLocalChecked(), ct);
	}
}



v8::Local<v8::String> vstr(juce::String str) {
	auto data = str.toRawUTF8();
	return v8::String::NewFromUtf8(V8Host::getIsolate(), data).ToLocalChecked();
}
v8::Local<v8::String> vstr(std::string str)
{
	return v8::String::NewFromUtf8(V8Host::getIsolate(), str.data()).ToLocalChecked();
}
v8::Local<v8::String> vstr(const char* str)
{
	return v8::String::NewFromUtf8(V8Host::getIsolate(), str).ToLocalChecked();
}

juce::String jstr(v8::Local<v8::String> vstr) {
//	return juce::String(vstr.
	v8::String::Utf8Value str(V8Host::getIsolate(), vstr);
//	std::string cppStr(*str);
	juce::String jstr(*str);
	return jstr;
	// back to v8
//	v8::Local<v8::String> v8String = v8::String::NewFromUtf8(isolate, cppStr.c_str(), v8::String::kNormalString);
}
juce::String jstr(v8::MaybeLocal<v8::String> vstr) {
	v8::Local<v8::String> local;
	bool sxs = vstr.ToLocal(&local);
	if (sxs)
		return jstr(local);
	else
		return juce::String();
}
juce::String jstr(v8::Local<v8::Value> vstr, v8::Local<v8::Context> ct) {
	return jvar(vstr, ct).toString();
}
juce::String jstr(v8::MaybeLocal<v8::Value> vstr, v8::Local<v8::Context> ct) {
	return jvar(vstr, ct).toString();
}


std::string cstr(const juce::String& s) {
	return s.toStdString();
}
std::string cstr(juce::String s) {
	return s.toStdString();
}

std::string cstr(const v8::MaybeLocal<v8::Value> mv, Local<Context> ct) {
	Local<v8::Value> val;
	if (mv.ToLocal(&val))
		return cstr(val, ct);
	else
		return "";
}
std::string cstr(v8::Local<v8::String> vstr) {
	//	return juce::String(vstr.
	v8::String::Utf8Value str(V8Host::getIsolate(), vstr);
	return std::string(*str);
}
std::string cstr(const v8::MaybeLocal<v8::String> mv) {
	Local<v8::String> val;
	if (mv.ToLocal(&val))
		return cstr(val);
	else
		return "";
}
// back to v8
//	v8::Local<v8::String> v8String = v8::String::NewFromUtf8(isolate, cppStr.c_str(), v8::String::kNormalString);



//extern juce::String jstr(v8::String);
//extern juce::String jstr(std::string);
//
//extern std::string cstr(v8::String);
//extern std::string cstr(juce::String);
	
extern juce::String jstr(v8::Local<v8::Value> v) {
	return jstr(v, V8Host::getContext());
}
extern juce::String jstr(v8::MaybeLocal<v8::Value> v) {
	return jstr(v, V8Host::getContext());
}
extern std::string cstr(const v8::Local<v8::Value> v) {
	return cstr(v, V8Host::getContext());
}
extern std::string cstr(const v8::MaybeLocal<v8::Value> v) {
	return cstr(v, V8Host::getContext());
}

}
