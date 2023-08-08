//
//  V8AppRoot.cpp
//  Yuzik - App
//
//  Created by Isaac Roberts on 12/5/21.
//

// Created by Isaac Roberts

#ifdef JUCE_GUI_BASICS_H_INCLUDED

#include <JuceHeader.h>

#include "V8AppRoot.h"
#include "Util.h"

using namespace v8;

V8AppRoot::V8AppRoot()
: Component()
{
	bindRenderingHooks();
}

juce::var V8AppRoot::evaluate(const juce::File& bundle)
{
	try
	{
		V8Host::getInst().evaluate(bundle);
		return juce::var();
	}
	catch (V8Host::JSException& e)// (const EcmascriptEngine::Error& err)
	{
		handleRuntimeError(e);
		return juce::var();
	}
}





void V8AppRoot::paint(juce::Graphics& g)
{
	if (errorText)
	{
//		g.fillAll(juce::Colour(0xffe14c37));
		g.fillAll(juce::Colour::hex(0xEDAE40));
		errorText->draw(g, getLocalBounds().toFloat().reduced(10.f));
	}
	else
	{
//		V8Host::getInst().evaluate("global.RenderToString({})", "react-renderer");
//		v8::String rdr = v8::String::NewFromUtf8Literal(V8Host::GetIsolate(),
//				"global.RenderToString({})");
//		v8::Script
	}
}
void V8AppRoot::bindRenderingHooks()
{
	
}

void V8AppRoot::handleRuntimeError(const V8Host::JSException& err)
{
#if ! JUCE_DEBUG
		// In release builds, we don't catch errors and show the red screen,
		// we allow the exception to raise up to the user to be handled properly
		// for a production app.
		throw err;
#endif

		JUCE_ASSERT_MESSAGE_THREAD

		DBG("");
		DBG("==== Error in JavaScript runtime. Context: ====");
//		DBG(err.context);
		DBG("Context not implemented yet. (Isaac)");
		DBG("");
		DBG(err.what());

		errorText = std::make_unique<juce::AttributedString>(err.what());
		errorText->append("\n\n");
		errorText->append("Stack not implemented yet. (Isaac)");
		errorText->append(err.stack);

#if JUCE_WINDOWS
		errorText->setFont(juce::Font("Lucida Console", 18, juce::Font::FontStyleFlags::plain));
#elif JUCE_MAC
		errorText->setFont(juce::Font("Monaco", 18, juce::Font::FontStyleFlags::plain));
#else
		errorText->setFont(18);
#endif

		// Lastly, kill the ViewManager to tear down existing views and prevent
		// further view interaction
//		viewManager.clearViewTables();

		repaint();
}
void V8AppRoot::reset()
{
//	viewManager.clearViewTables();
	V8Host::getInst().reset();
	errorText = nullptr;
}


void V8AppRoot::resized()
{
	
}


V8AppRoot::~V8AppRoot()
{
	
}


#endif 
