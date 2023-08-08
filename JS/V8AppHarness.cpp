//
//  V8AppHarness.cpp
//  Yuzik - App
//
//  Created by Isaac Roberts on 12/5/21.
//

// Created by Isaac Roberts

#include "V8AppHarness.h"

//#include <v8.h>

#include "VData.h"

#ifdef NODE_APPROOT_INCLUDED
V8AppHarness::V8AppHarness(V8AppRoot& v8AppRoot)
: onBeforeAll([&](){}),
onAfterAll([&](){}),
appRoot(v8AppRoot)
{
	fileWatcher = std::make_unique<FileWatcher>([this]() { handleFilesChanged(); });
}
#else
V8AppHarness::V8AppHarness()
: onBeforeAll([&](){}),
onAfterAll([&](){})
{
	fileWatcher = std::make_unique<FileWatcher>([this]() { handleFilesChanged(); });
}

#endif

void V8AppHarness::watch (const juce::File& f)
{
	if (fileWatcher)
	{
		fileWatcher->watch(f);
	}
}



/** Run the initial evaluation step and then watch for file changes. */
void V8AppHarness::start()
{
	// Nothing to do if we haven't watched any files
	if (fileWatcher == nullptr)
		return;

	// Run initial callbacks and file eval.
	handleFilesChanged();

	// Finally, kick off the file watch process
	fileWatcher->start();
}

/** Stop any running file watch. */
void V8AppHarness::stop()
{
	if (fileWatcher)
		fileWatcher->stop();
}

/** Run the initial evaluation step exactly once.
 *
 *  It may be helpful to use AppHarness::start in DEBUG builds when you want
 *  hot reloading for development, then switch in RELEASE builds to using
 *  AppHarness::once to ignore file watching when you don't need it.
 */
void V8AppHarness::once()
{
	// Nothing to do if we haven't watched any files
	if (fileWatcher == nullptr)
		return;

	// Run callback and eval step once.
	handleFilesChanged();
}
#ifdef NODE_APPROOT_INCLUDED
void V8AppHarness::handleFilesChanged()
{
	appRoot.reset();
	appRoot.bindRenderingHooks();

	if (onBeforeAll) { onBeforeAll(); }

	for (const auto& f : fileWatcher->getWatchedFiles())
	{
		try
		{
			appRoot.evaluate(f);
		}
		catch (const V8Host::JSException& err)
		{
			// We may fall in here in Release builds when ReactApplicationRoot hasn't
			// caught the error for us to show the red screen. In this case, we call
			// a user supplied error handler if we have one, else just break the loop
			// and await the next file change event.
//			if (onEvalError)
//			{
//				onEvalError(err);
//				break;
//			}
//			else
//			{
				throw err;
//			}
		}

	}

	if (onAfterAll) { onAfterAll(); }
	
}
#else


void V8AppHarness::handleFilesChanged()
{
	V8Host::getInst().reset();
	//bindRenderingHooks();

	if (onBeforeAll) { onBeforeAll(); }

	for (const auto& f : fileWatcher->getWatchedFiles())
	{
		try
		{
//			V8Host::getInst().host(f);
		}
		catch (const V8Host::JSException& err)
		{
			throw err;
		}

	}

	if (onAfterAll) { onAfterAll(); }
}

#endif

//Destructor
V8AppHarness::~V8AppHarness()
{
	
}


juce::File V8AppHarness::relativeFromBundle(juce::String path, int upDirAmt)
{
	//Unused - needs to figure out location of Resources/ folder on OSX Release and location on Windows debug/release
	jassertfalse;
//	juce::String main_name = "js-source/js/src/App.js";
//	#if DEBUG
//		#if JUCE_MAC
//			/* MAC:
//
//				This works ONLY if the application is in /Builds/MacOSX/build/Debug/
//
//				When we bundle this for release, we'll have to package the javascript in the application folder
//			 */
//			auto file = juce::File::getSpecialLocation(juce::File::currentApplicationFile);
//			file = file.getChildFile("../../../../../");
//		#else
//			/* Windows:
//				Same, the application must be in /Builds/VisualStudio/whatever/whatever
//
//				This will have to be bundled with the windows application as well
//			 */
//			auto file = File::getCurrentWorkingDirectory().getChildFile("../../");
//		#endif
//		if (upDirAmt > 0)
//		{
//			juce::String ups = juce::String::repeatedString("../", upDirAmt);
//			file = file.getChildFile(ups);
//		}
//	#else
//		#if JUCE_MAC
//			//TODO
//			#error "Find location of Resources/ folder in release build"
//		#else
//			#error "Find location of js folder on Windows"
//		#endif
//	#endif
//
//	file = file.getChildFile(path);
//
//	return file;
}
