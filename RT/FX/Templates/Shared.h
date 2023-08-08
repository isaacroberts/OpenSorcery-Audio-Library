

#pragma once

//TODO: Change these to match the Template (isInputSilent)
#define bypassParams bool allowTail, bool inputIsSilent
#define bypassCall allowTail, inputIsSilent


#define fx_proc_start(context) if (isBypassed() || context.isBypassed()) {\
context.bypassProcess(); \
bypassProcess(context, !context.isBypassed(), false); \
return; \
}

/*
 if (isBypassed() || context.isBypassed()) {
	 context.bypassProcess();
	 bypassProcess(context, !context.isBypassed(), false);
	 return;
 }
 */
