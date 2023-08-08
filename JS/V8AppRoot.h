//
//  V8AppRoot.h
//  Yuzik - App
//
//  Created by Isaac Roberts on 12/5/21.
//

// Created by Isaac Roberts 

#pragma once


//using namespace juce;

#ifdef JUCE_GUI_BASICS_H_INCLUDED


#include <JuceHeader.h>
#include "V8Host.h"

class V8AppRoot : public juce::Component
{
public:
	V8AppRoot();
	virtual ~V8AppRoot();
	
	// =========== js ===============
	juce::var evaluate(const juce::File& bundle);

	void handleRuntimeError(const V8Host::JSException& err);
	void reset();

	void bindRenderingHooks();
	
	
	// ========== component =============
	void paint(juce::Graphics& g) override;
	void resized() override;
	

protected:
	void bindNativeRenderingHooks();
	

protected:
	
	std::unique_ptr<juce::AttributedString> errorText;

	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (V8AppRoot);

};

#endif
