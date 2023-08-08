
#include "JS.h"

//* <NodeJs>

//#define NODE_WANT_INTERNALS 1

#if USING_NODE_JS
#include "node_code_cache_stub.cc"
#include "node_snapshot_stub.cc"
#endif

//* </NodeJs>


#include "VData.cpp"

#include "V8Host.cpp"

#if USING_NODE_JS
#include "V8Host_node.cpp"
#else
#include "V8Host_v8.cpp"
#endif

#include "V8AppRoot.cpp"
#include "V8AppHarness.cpp"
#include "Util.cpp"
