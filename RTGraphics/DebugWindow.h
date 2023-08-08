//
//  DebugWindow.h
//  
//
//  Created by Isaac Roberts on 8/21/20.
//

// Created by Isaac Roberts 

#pragma once


#ifndef DEBUG_WINDOW
#define DEBUG_WINDOW 0
#endif

#if DEBUG_WINDOW

#include <JuceHeader.h>
//using namespace juce;

class DebugWindow
{
public:
	class DebugInstance : public juce::TextEditor, juce::Timer
	{
	public:
		DebugInstance();
		void timerCallback() override;
		void start();
		~DebugInstance();
//	private:
	};
	
	static DebugWindow* getInst() {
		static DebugWindow inst;
		return &inst;
	}
	
	//Base case for template list
	static void print() {
		getInst()->println();
	}
	//Template list variadic
	template<typename Arg1, typename ... Arguments>
	static void print(Arg1 a, Arguments... args) {
		getInst()->printArg(a);
		print(args...);
	}
	
	static void println() {
		getInst()->_println();
	}
	
	static bool hasInstancesActive() { return getInst()->instances > 0; }
	
	static void addInstance()  {getInst()->instances++; }
	static void removeInstance()  {getInst()->instances--; }
	
private:
	//Template prints single argument
	template<typename Arg>
	void printArg(Arg a) {
		currentLine += juce::String(a);
		currentLine += " ";
	}
	template<>
	void printArg<void*>(void* a) {
		currentLine += "0x";
		currentLine += juce::String::toHexString((long)a);
		currentLine += " ";
	}
	//Prints string argument
	template<>
	void printArg<juce::String>(juce::String a) {
		currentLine+= a;
		currentLine += " ";
	}
	
	juce::String addedText();
	void _println();
	void flush();
	
	DebugWindow();

	
	
	DebugWindow(DebugWindow& ) = delete;
	void operator=(const DebugWindow &) = delete;
	virtual ~DebugWindow();

	
private:
	juce::StringArray lines;
	juce::String currentLine;
	int instances;
//	int length;
	
	std::mutex editLock;
};
#elif DEBUG


#include <JuceHeader.h>
class DebugWindow
{
public:
	class DebugInstance
	{};
	static DebugWindow* getInst() {
		static DebugWindow inst;
		return &inst;
	}
	static void print() {
		getInst()->println();
	}
	//Template list variadic
	template<typename Arg1, typename ... Arguments>
	static void print(Arg1 a, Arguments... args) {
		getInst()->printArg(a);
		print(args...);
	}
	
	static void println() {
		getInst()->_println();
	}

	static bool hasInstancesActive() { return true; }
	static void addInstance()  {}
	static void removeInstance()  {}
	//Member ---------
private:
	template<typename Arg>
	void printArg(Arg a) {
		std::cout<< a << " ";
	}
	template<>
	void printArg<void*>(void* a) {
		std::cout <<"0x"<<juce::String::toHexString((long)a)<<" ";
	}
	void _println() {
		std::cout<<std::endl;
	}
	
public:
	void setBounds(int x, int y, int w, int h) { }
	juce::TextEditor* getComponent() { return NULL; }
	juce::TextEditor* getTextEditor() { return NULL; }
};

#else

#include <JuceHeader.h>
class DebugWindow
{
public:
	class DebugInstance
	{
	};
	static DebugWindow* getInst() { return NULL; }
//	static void end() {}
	//Base case for template list
	static void print() { }
	
	//Template list variadic
	template<typename Arg1, typename ... Arguments>
	static void print(Arg1 a, Arguments... args) { }
	
	static void println() { }
	
	static bool hasInstancesActive() { return false; }
	//Member ---------
public:
	void setBounds(int x, int y, int w, int h) { }
	juce::TextEditor* getComponent() { return NULL; }
	juce::TextEditor* getTextEditor() { return NULL; }
};

#endif
typedef DebugWindow Debug;
