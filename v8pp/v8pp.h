/** BEGIN_JUCE_MODULE_DECLARATION

	ID:            v8pp
	vendor:        Pmed
	version:        1.0.0
	name:           V8PP Bindings
	description:
	website:       https://github.com/pmed/v8pp
	license:        MIT

	dependencies:

	END_JUCE_MODULE_DECLARATION
*/

//OSXLibs: v8_libbase v8_libplatform v8
//windowsLibs: v8_libbase v8_libplatform v8	

#ifndef V8PP_MODULE_INCLUDED
#define V8PP_MODULE_INCLUDED

// V8 Includes
//#include "v8.h"
//#include "libplatform/libplatform.h"
//#include "v8-context.h"
//#include "v8-exception.h"
//#include "v8-initialization.h"
//#include "v8-isolate.h"
//#include "v8-local-handle.h"
//#include "v8-script.h"
//#include "v8-template.h"


#include <libplatform/libplatform.h>
#include <v8-context.h>
#include <v8-exception.h>
#include <v8-initialization.h>
#include <v8-isolate.h>
#include <v8-local-handle.h>
#include <v8-script.h>
#include <v8-template.h>

#include <v8.h>


#if defined(_WIN32)
#include <windows.h>
static char const path_sep = '\\';
#else
#include <dlfcn.h>
static char const path_sep = '/';
#endif

#define V8PP_HEADER_ONLY 1

#define V8PP_PLUGIN_INIT_PROC_NAME "v8pp_plugin_initalization"


#if JUCE_WINDOWS
#define V8PP_PLUGIN_SUFFIX "dll"
#else
#define V8PP_PLUGIN_SUFFIX "so"
#endif


#include "config.hpp"



#include "factory.hpp"
#include "ptr_traits.hpp"
#include "utility.hpp"
#include "convert.hpp"
#include "call_from_v8.hpp"
#include "throw_ex.hpp"
#include "function.hpp"
#include "property.hpp"
#include "class.hpp"
#include "context.hpp"
#include "json.hpp"
#include "module.hpp"
#include "object.hpp"
#include "persistent.hpp"
#include "version.hpp"

#endif
