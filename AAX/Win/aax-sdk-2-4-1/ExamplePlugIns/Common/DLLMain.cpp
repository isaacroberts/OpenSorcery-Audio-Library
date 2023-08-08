/*================================================================================================*/
/*
 *	Copyright 1998-2015 by Avid Technology, Inc.
 *	All rights reserved.
 *
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 */
/*================================================================================================*/
#include "DLLMain.h"
#include "PlugInOpenAndClose.h"
#include <string.h>
#include <windows.h>

//#include "..\..\..\..\DAEWin\DSPProbe\WinBuildDLL\DSPProbeAPI.h"
//Call LaunchDSPProbe() to execute.

typedef short OSErr;
extern "C" 
{
__declspec(dllexport)   OSErr _stdcall NewPlugIn(void*);
			void  _stdcall ClosePlugIn(void);
void		RegisterByteSwapper(void);
void		RemoveByteSwapper(void);
}
void* gThisModule;  // global for the plugin's module handle
char gModuleName[512];	// global for the plugin's module name
char gResFile[516];		// global for the plugin's .rsr file name

static bool bPlugInAlive = false;
static int iOpenCount = 0;
void (_cdecl *pSetPlugInOpenAndCloseProcs)(OSErr (__stdcall *)(),OSErr (__stdcall *)()) = NULL;


//-----------------------------------------------------------------------
static OSErr __stdcall PlugInOpen()
{
	//Run DSPProbe on plug-in launch
	//LaunchDSPProbe();

	OSErr err = 0;
	iOpenCount++;
	if (iOpenCount == 1)
	{
		DWORD ret = GetModuleFileName( (HINSTANCE) gThisModule, gModuleName, sizeof(gModuleName) ); // ret dropped on floor
		strcpy( gResFile, gModuleName );
		strcat( gResFile, ".rsr" );
		RegisterByteSwapper();
		err = NewPlugIn (0);
		bPlugInAlive = (err == 0);
		if (!bPlugInAlive)
		{
			ClosePlugIn();
			RemoveByteSwapper();
		}
	}
	return err;
}

//-----------------------------------------------------------------------
static OSErr __stdcall PlugInClose()
{
	iOpenCount--;
	if (iOpenCount == 0)
	{
		if (bPlugInAlive)
		{
			ClosePlugIn();
			RemoveByteSwapper();
		}
	}
	return 0;
}

//-----------------------------------------------------------------------
extern "C" BOOL WINAPI DllMain(     
                                    HINSTANCE hInstance, 
                                    DWORD ul_reason_for_call, 
                                    LPVOID lpReserved
                              )
//-----------------------------------------------------------------------
{
	try
	{
		switch( ul_reason_for_call )
		{
			case DLL_PROCESS_ATTACH:
			{
				gThisModule = hInstance;

				// Newer versions of DAE.DLL export a function which is meant
				// to be called by a loading PlugIn.  We pass the address of 
				// functions DAE can then call to intialize and deinitialize 
				// the PlugIn.  The absence of this export in DAE means we
				// must do initialization right here in DllMain.
				*((FARPROC*)&pSetPlugInOpenAndCloseProcs) = 
					GetProcAddress(GetModuleHandle("DAE.DLL"),"SetPlugInOpenAndCloseProcs");

				if (pSetPlugInOpenAndCloseProcs != NULL)
					pSetPlugInOpenAndCloseProcs(PlugInOpen,PlugInClose);
				else
				{
					if (PlugInOpen() != 0)
						return false;
				}
			
				break;
			}

			case DLL_PROCESS_DETACH:
				if (pSetPlugInOpenAndCloseProcs == NULL)
					PlugInClose();
				break;

			default:
				break;
		}
	}
	catch(...)
	{
		return false;
	}
	return true;
}

static bool sUnderTest = false;

bool PlugInIsUnderTest()
{
	return sUnderTest;
}

/*
The following forwarding functions were defined because the internal
versions needed to stay stdcall because they are expected to be
stdcall by SetPlugInOpenAndCloseProcs() but for simplicity of the
exports' names being unmanaged, it is nice to use cdecl.
*/

extern "C" __declspec(dllexport) FWinPlugInOpen  WinPlugInOpen;
extern "C" __declspec(dllexport) FWinPlugInClose WinPlugInClose;

extern "C" __declspec(dllexport) __int32 __cdecl WinPlugInOpen
(
	__int32 iReasonForCall, void* iCallArgs
)
{
	__int32 r = 0;

	sUnderTest = false;
	
	if (iReasonForCall == eReasonForPlugInOpenCall_PlugInUnderTest)
	{
		sUnderTest = true;
		r = PlugInOpen();
	}
	else
	{
		r = -1;
	}

	return r;
}

extern "C" __declspec(dllexport) __int32 __cdecl WinPlugInClose()
{
	return PlugInClose();
}
