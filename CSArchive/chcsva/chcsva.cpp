// CHCSVA.cpp : Implementation of DLL Exports.

#include "chcs.h"

#include "resource.h"
#include "DlgConfigure.h"
#include "DlgShowStream.h"

#include <initguid.h>

#ifdef _DEBUG
#define WRITELOG
#endif

#ifdef WRITELOG
	#include <memory>
	#include <iostream>
	#include <fstream>

	#ifdef _UNICODE
	typedef std::wfstream _tfstream;
	#else
	typedef std::fstream _tfstream;
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// One and only manger object
/////////////////////////////////////////////////////////////////////////////

CManager* _Manager = NULL;

/////////////////////////////////////////////////////////////////////////////
// One and only manger object
/////////////////////////////////////////////////////////////////////////////

CManager* GetManager()
{
	return _Manager;
}

//////////////////////////////////////////////////////////////////////////
// DebugOutput
//////////////////////////////////////////////////////////////////////////

DWORD_PTR        procOutputData = 0;
CHCS::OutputProc procOutput     = NULL;

/////////////////////////////////////////////////////////////////////////////
// ElveesTools Output function
/////////////////////////////////////////////////////////////////////////////

namespace Elvees
{

#ifdef WRITELOG
	static _tfstream s_debugOut;
#endif

void Output(TOutput type, LPCTSTR stText)
{
#ifdef _DEBUG
	TCHAR stHeader[24];
	SYSTEMTIME stLocalTime;

	GetLocalTime(&stLocalTime);

	wsprintf(stHeader,
		TEXT("%02hd:%02hd:%02hd [%04ld] %c "),
		stLocalTime.wHour,
		stLocalTime.wMinute,
		stLocalTime.wSecond,
		GetCurrentThreadId(),
		(type == TTrace)    ? TEXT('D') :
		(type == TWarning)  ? TEXT('W') :
		(type == TInfo)     ? TEXT('I') :
		(type == TError)    ? TEXT('E') :
		(type == TCritical) ? TEXT('C') : TEXT('U'));

	_tprintf(TEXT("%s%s\n"), stHeader, stText);
#else
	OutputDebugString(stText);
	OutputDebugString(TEXT("\r\n"));
#endif

#ifdef WRITELOG
	if(!s_debugOut.is_open())
	{
		s_debugOut.open("chcsva.log", std::ios_base::out | std::ios_base::app);

		s_debugOut << TEXT("****************New Log*****************") << std::endl;
	}

	s_debugOut << stHeader << stText << std::endl;
#endif

	if(!procOutput)
		return;

	int nMode = 
		(type == TInfo) ? CHCS_INFO :
		(type == TTrace) ? CHCS_TRACE :
		(type == TError) ? CHCS_ERROR : 
		(type == TWarning) ? CHCS_WARNING :
		(type == TCritical) ? CHCS_CRITICAL : CHCS_INFO;

	__try
	{
	#ifndef _UNICODE
		WCHAR szMsg[1024];
		MultiByteToWideChar(CP_ACP, 0, stText, lstrlen(stText) + 1,
			szMsg, countof(szMsg));

		procOutput(procOutputData, nMode, szMsg);
	#else
		procOutput(procOutputData, nMode, stText);
	#endif
	}
	__except(1)
	{
		OutputDebugString(TEXT("Unexpected exception in DebugOutput\n"));
	}
}

} // End of namespace Elvees

//////////////////////////////////////////////////////////////////////
// Namespace: CHCS

namespace CHCS {

CHCSVA_API bool __stdcall InitStreamManager(LPINITCHCS lpInitCHCS)
{
	DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("=InitStreamManager")));

	// Release manager if any
	//

	if(_Manager)
	{
		Elvees::Output(Elvees::TError, TEXT("InitManager called twice without FreeManager"));
		return false;
	}

	// Check input parameter
	//

	bool bOK = false;
	
	do
	{
		if(!lpInitCHCS)
			break;
	
		if(IsBadReadPtr(lpInitCHCS, sizeof(INITCHCS)))
			break;

		if(lpInitCHCS->dwSize < sizeof(INITCHCS))
			break;

		// Test output proc...

		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Mode=%ld"), lpInitCHCS->dwMode));

		procOutput = lpInitCHCS->procOutput;
		procOutputData = lpInitCHCS->dwUserData;

		if(lpInitCHCS->procOutput)
		{
			try
			{
				lpInitCHCS->procOutput(procOutputData, CHCS_INFO, L"InitManager: testing output");
			}
			catch(...)
			{
				procOutput = NULL;
				procOutputData = 0;

				Elvees::Output(Elvees::TError, TEXT("InitManager invalid output function"));
				break;
			}
		}

		bOK = true;
	}
	while(false);

	if(!bOK)
	{
		Elvees::Output(Elvees::TError, TEXT("InitManager failed. Invalid parameter"));
		return false;
	}

	// Create and start manager
	//

	try
	{
		_Manager = new CManager(lpInitCHCS->dwMode);
		
		if(_Manager)
		{
			_Manager->Start();
			_Manager->WaitStarted();

			return true;
		}
	}
	catch(const Elvees::CWin32Exception& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("InitManager - Exception %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("InitManager - Exception %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Elvees::Output(Elvees::TError, TEXT("InitManager - Unexpected exception"));
	}

	FreeStreamManager();

	return false;
}

CHCSVA_API bool __stdcall FreeStreamManager()
{
	DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("=FreeStreamManager")));

	if(!_Manager)
		return false;

	try
	{
		_Manager->ShutDown();

		delete _Manager;
	}
	catch(const Elvees::CWin32Exception& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("FreeStreamManager - Exception %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("FreeStreamManager - Exception %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Elvees::Output(Elvees::TError, TEXT("FreeStreamManager - Unexpected exception"));
	}

	_Manager = NULL;

	return true;
}

CHCSVA_API bool __stdcall GetStreamByID(IStream** ppStream, long lStream)
{
	if(!ppStream)
		return false;

	if(_Manager)
	{
		*ppStream = _Manager->StreamByID(lStream);
		return (*ppStream == NULL) ? false : true;
	}

	return false;
}

CHCSVA_API bool __stdcall GetStreamByUID(IStream** ppStream, UUID* pStreamUID)
{
	if(!ppStream || !pStreamUID)
		return false;

	if(_Manager)
	{
		*ppStream = _Manager->StreamByUID(*pStreamUID);
		return (*ppStream == NULL) ? false : true;
	}

	return false;
}

CHCSVA_API bool __stdcall GetAvailableStreams(CAUUID* pUIDs)
{
	if(!pUIDs)
		return false;

	if(!_Manager)
	{
		pUIDs->cElems = 0;
		pUIDs->pElems = 0;
		return false;
	}

	return _Manager->GetStreams(pUIDs);
}

CHCSVA_API bool __stdcall ShowStream(UUID* StreamUID, LPCWSTR stTitle)
{
	IStream* ppStream = NULL;

	if(GetStreamByUID(&ppStream, StreamUID))
	{
		CDlgShowStream dlg(ppStream);
		ppStream->Release();

		dlg.DoModal(::GetDesktopWindow(), reinterpret_cast<LPARAM>(stTitle));

		return true;
	}

	return false;
}

CHCSVA_API bool __stdcall ShowStreamByNameA(LPCSTR stStream,  LPCSTR stTitle)
{
	if(!stStream)
		return false;

	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("=ShowStreamByNameA %hS"), stStream));

	WCHAR szStream[56];

	MultiByteToWideChar(CP_ACP, 0, stStream, (int)strlen(stStream) + 1,
		szStream, countof(szStream));

	if(stTitle)
	{
		WCHAR szTitle[256];

		MultiByteToWideChar(CP_ACP, 0, stTitle, (int)strlen(stTitle) + 1,
			szTitle, countof(szTitle));

		return ShowStreamByNameW(szStream, szTitle);
	}

	return ShowStreamByNameW(szStream, NULL);
}

CHCSVA_API bool __stdcall ShowStreamByNameW(LPCWSTR szStream, LPCWSTR szTitle)
{
	if(!szStream || !_Manager)
		return false;

	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("=ShowStreamByNameW %lS"), szStream));

	UUID stUUID;

#ifndef _UNICODE
	CHAR stStream[56];
	
	WideCharToMultiByte(CP_ACP, 0, szStream, (int)wcslen(szStream) + 1,
		stStream, sizeof(stStream), NULL, NULL);

	if(StringToUuid(stStream, &stUUID))
#else
	if(StringToUuid(const_cast<LPTSTR>(szStream), &stUUID))
#endif
	{
		IStream* ppStream = NULL;

		if(GetStreamByUID(&ppStream, &stUUID))
		{
			CDlgShowStream dlg(ppStream);
			ppStream->Release();

			dlg.DoModal(::GetDesktopWindow(), reinterpret_cast<LPARAM>(szTitle));

			return true;
		}
	}

	return false;
}

CHCSVA_API bool __stdcall StartArchiveStream(IStream** ppStream, long lStream, INT64 startPos)
{
	if(_Manager && ppStream)
		return _Manager->CreateArchiveStream(ppStream, lStream, startPos);

	return false;
}

CHCSVA_API void WINAPI Configure(HWND hwnd, HINSTANCE hinst, LPTSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr;
	hr = CoInitialize(NULL);
	if(FAILED(hr))
	{
		::MessageBox(NULL, TEXT("CoInitialize failed"),
			TEXT("Configure"), MB_OK | MB_ICONSTOP);
		return;
	}

	CDlgConfigure dlg;
	dlg.DoModal();

	CoUninitialize();
}

} // End of namespace CHCS

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(hInstance);
		_Module.Init(NULL, hInstance);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		_Module.Term();
	}

	return TRUE;
}

STDAPI DllRegisterServer(void)
{
	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
	return S_OK;
}