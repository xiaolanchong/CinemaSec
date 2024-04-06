// CSBackUp.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "CSBackUp.h"
#include "BackUpCS.h"

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

//extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(hInstance);
	//	_Module.Init(NULL, hInstance);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
	//	_Module.Term();
	}

	return TRUE;
}

HRESULT __cdecl CreateCSBackUp(ICSBackUp** ppBackUp, IDebugOutput* pDebugOut)
{
	if(!pDebugOut)
		return E_POINTER;

	CBackUpCS* pInterface = NULL;

	try
	{
		pInterface = new CBackUpCS(pDebugOut);
	}
	catch(...)
	{
		pInterface = NULL;

		if(pDebugOut)
			pDebugOut->PrintW(IDebugOutput::mt_error, L"Fail to create BackUp interface");
	}

	*ppBackUp = (ICSBackUp*)pInterface;

	return (*ppBackUp == NULL) ? E_FAIL : S_OK;
}
