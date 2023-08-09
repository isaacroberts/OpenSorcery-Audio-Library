

#ifdef JUCE_OSC_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
	already included any other headers - just put it inside a file on its own, possibly with your config
	flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
	header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif


//STL Headers
#include <algorithm>
#include <cassert>
#include <climits>
#include <cstring>
#include <fstream>
#include <functional>
#include <limits>
#include <memory>
#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "v8pp.h"

#include "class.cpp"
#include "context.cpp"
#include "convert.cpp"
#include "json.cpp"
#include "throw_ex.cpp"
#include "version.cpp"

#include "class.ipp"
#include "json.ipp"
#include "throw_ex.ipp"
#include "version.ipp"
