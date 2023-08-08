//
//  Util.h
//  Yuzik - App
//
//  Created by Isaac Roberts on 12/9/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

//using namespace juce;

//#include <v8.h>

#include "VData.h"

namespace js {
	extern v8::Local<v8::String> vstr(juce::String);
	extern v8::Local<v8::String> vstr(std::string);
	extern v8::Local<v8::String> vstr(const char*);

	//extern v8::Local<v8::Value> vval(juce::String);
	//extern v8::Local<v8::Value> vstr(std::string);

	extern juce::var jvar(v8::Local<v8::Value>, v8::Local<v8::Context>);
	extern juce::var jvar(v8::MaybeLocal<v8::Value>, v8::Local<v8::Context>);


	extern juce::String jstr(v8::Local<v8::String>);
	extern juce::String jstr(v8::MaybeLocal<v8::String>);

	extern juce::String jstr(v8::Local<v8::Value>, v8::Local<v8::Context>);
	extern juce::String jstr(v8::MaybeLocal<v8::Value>, v8::Local<v8::Context>);
	extern juce::String jstr(v8::Local<v8::Value>);
	extern juce::String jstr(v8::MaybeLocal<v8::Value>);

	//extern juce::String jstr(std::string);

	extern std::string cstr(const juce::String&);
	//extern std::string cstr(juce::String);

	extern std::string cstr(v8::Local<v8::String>);
	extern std::string cstr(const v8::MaybeLocal<v8::String>);
	extern std::string cstr(const v8::Local<v8::Value>, v8::Local<v8::Context>);
	extern std::string cstr(const v8::MaybeLocal<v8::Value>, v8::Local<v8::Context>);
	extern std::string cstr(const v8::Local<v8::Value>);
	extern std::string cstr(const v8::MaybeLocal<v8::Value>);

}
