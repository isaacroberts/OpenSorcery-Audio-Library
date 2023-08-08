//
//  V8AppHarness.h
//  Yuzik - App
//
//  Created by Isaac Roberts on 12/5/21.
//

// Created by Isaac Roberts 

#ifndef V8AppHarness_h
#define V8AppHarness_h

#include <JuceHeader.h>

//#include "react_juce/FileWatcher.h"

#include "V8AppRoot.h"
#include "FileWatcher.h"

class V8AppHarness {
public:
#ifdef NODE_APPROOT_INCLUDED
	V8AppHarness(V8AppRoot& appRoot);
#else
	V8AppHarness();
#endif
	void watch (const juce::File& f);

	static juce::File relativeFromBundle(juce::String path, int upDirAmt=0);
	
	/** Run the initial evaluation step and then watch for file changes. */
	void start();
	
	/** Stop any running file watch. */
	void stop();
	
	/** Run the initial evaluation step exactly once.
	 *
	 *  It may be helpful to use AppHarness::start in DEBUG builds when you want
	 *  hot reloading for development, then switch in RELEASE builds to using
	 *  AppHarness::once to ignore file watching when you don't need it.
	 */
	void once();

	
	std::function<void(void)> onBeforeAll;
	std::function<void(void)> onAfterAll;

	
	virtual ~V8AppHarness();
	
private:
	V8AppHarness(const V8AppHarness& other) = delete;
	V8AppHarness& operator=(const V8AppHarness& other) = delete;
private:
	
	void handleFilesChanged();

	
	//==============================================================================
#ifdef NODE_APPROOT_INCLUDED
	V8AppRoot& appRoot;
#endif
	std::unique_ptr<FileWatcher> fileWatcher;
//	std::unordered_map<juce::String, bool> sourceFileTypeMap;

	
};

#endif /* V8AppHarness_h */
