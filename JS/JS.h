/** BEGIN_JUCE_MODULE_DECLARATION

	ID:            JS
	vendor:        Pmed
	version:        1.0.0
	name:           JS Hosting
	description: V8 or NodeJS
	website:
	license:        MIT

	dependencies:

	END_JUCE_MODULE_DECLARATION
*/



//==============================================================================
/** Config: USING_NODE_JS

	If this is enabled, uses node js instead of V8
 
 */
#ifndef USING_NODE_JS
 #define USING_NODE_JS 0
#endif

#define NODE_JS USING_NODE_JS

/* ----------------------------------------------------
			  JUCE Options:
   ----------------------------------------------------
*/

/*
 For V8:
	USING_NODE_JS = Disabled
 	
	Extra Compiler Flags:
		-DV8_COMPRESS_POINTERS
		-Wno-non-virtual-dtor
	
	External Libraries to Link:
		v8_libbase
		v8_libplatform
		v8
 
	Header Search Paths:
		/usr/local/opt/v8/include
		/usr/local/include
 
	Extra Library Search Paths:
		/usr/local/opt/v8/lib/
 
*/

/*
 For Node:
	USING_NODE_JS = Disabled
	
	Preprocessor definitions
		NODE_WANT_INTERNALS=1
 
	Compiler Flags:
		-Wno-non-virtual-dtor
		-Wno-implicit-int-conversion
 
	Libraries:
		 cares
		 brotli
		 gtest_main
		 gtest
		 histogram
		 zlib
		 icui18n
		 icudata
		 icutools
		 icuucx
		 llhttp
		 nghttp2
		 nghttp3
		 ngtcp2
		 uv
		 uvwasi
		 openssl
		 v8_zlib
		 v8_snapshot
		 v8_base_without_compiler
		 v8_libbase
		 v8_libplatform
		 v8_init
		 v8_initializers
		 v8_compiler
		 node
 
	Header search paths:
		 /Users/isaacroberts/Documents/node_js/node/deps/v8/include
		 /Users/isaacroberts/Documents/node_js/node/deps/uv/include
		 /Users/isaacroberts/Documents/node_js/node/src/
		 /Users/isaacroberts/Documents/node_js/ (??)
 
	Library search paths:
		 /Users/isaacroberts/Documents/node_js/node/out/Release/
 
*/


#ifndef NODEJS_MODULE_INCLUDED
#define NODEJS_MODULE_INCLUDED



//#include "config.hpp"


#endif
