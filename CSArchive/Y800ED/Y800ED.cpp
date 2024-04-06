// Y800ED.cpp : Defines the entry point for the dll application.
//

#include "Y800ED.h"
#include "Y800VfW.h"

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

//////////////////////////////////////////////////////////////////////////
// Global variables

BOOL    g_bDebuginfo  = FALSE;
HMODULE g_hModuleY800 = NULL;

//////////////////////////////////////////////////////////////////////////
//
// DLL EntryPoint
//
//////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(hInstance);
		g_hModuleY800 = hInstance;
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// DumpMiniDump
//
//////////////////////////////////////////////////////////////////////////

static void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo)
{
	MINIDUMP_EXCEPTION_INFORMATION eInfo;

	if(excpInfo == NULL)
	{
		// Generate exception to get proper context in dump
		__try 
		{
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(hFile, GetExceptionInformation()),
			EXCEPTION_EXECUTE_HANDLER) 
		{
			OutputDebugString(TEXT("y800ed!DriverProc: DumpMiniDump - Invalid context\n"));
		}
	} 
	else
	{
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = excpInfo;
		eInfo.ClientPointers = FALSE;

		// note:  MiniDumpWithIndirectlyReferencedMemory does not work on Win98
		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			//	MiniDumpNormal,
			MiniDumpWithIndirectlyReferencedMemory,
			excpInfo ? &eInfo : NULL,
			NULL,
			NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
//
// RecordExceptionInfo
//
//////////////////////////////////////////////////////////////////////////

int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS excpInfo)
{
	if(IsDebuggerPresent())
		return EXCEPTION_CONTINUE_SEARCH;

	// Print exception info
	//

	TCHAR strExceptionInfo[256];
	PEXCEPTION_RECORD pException = excpInfo->ExceptionRecord;

	// If the exception was an access violation, print out some additional
	// information, to the debugger.
	if(pException->ExceptionCode == STATUS_ACCESS_VIOLATION &&
		pException->NumberParameters >= 2)
	{
		wsprintf(strExceptionInfo, TEXT("y800ed!DriverProc: %s location 0x%08X caused an access violation\n"),
			pException->ExceptionInformation[0] ? TEXT("Write to") : TEXT("Read from"),
			pException->ExceptionInformation[1]);
	}
	else
	{
		wsprintf(strExceptionInfo, TEXT("y800ed!DriverProc: Unexpected exception (0x%08X)\n"),
			pException->ExceptionCode);
	}

	OutputDebugString(strExceptionInfo);

	// Disable crush dump
	//

	if(!g_bDebuginfo)
		return EXCEPTION_EXECUTE_HANDLER;

	// Write minidump
	//

	DWORD dwSize;
	SYSTEMTIME st;

	TCHAR szFileName[MAX_PATH];
	TCHAR szTempPath[MAX_PATH];

	HANDLE hMiniDumpFile;

	GetLocalTime(&st);

	dwSize = GetTempPath(MAX_PATH, szTempPath);

	if(dwSize == 0 || dwSize > MAX_PATH)
		lstrcpy(szTempPath, TEXT("c:"));

	dwSize = lstrlen(szTempPath) - 1;

	if(szTempPath[dwSize] == '\\')
		szTempPath[dwSize] = 0;

	//%%TMP%%
	wsprintf(szFileName, TEXT("%s\\y800ed_%d%02d%02d_%02d%02d%02d.dmp"),
		szTempPath,
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);

	// Create the file
	hMiniDumpFile = CreateFile(
		szFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);

	// Write the minidump to the file
	if(hMiniDumpFile != INVALID_HANDLE_VALUE)
	{
		DumpMiniDump(hMiniDumpFile, excpInfo);
		FlushFileBuffers(hMiniDumpFile);

		dwSize = GetFileSize(hMiniDumpFile, NULL); 

		// If we failed ... 
		if(dwSize == INVALID_FILE_SIZE && GetLastError() != NO_ERROR) 
		{
		}

		CloseHandle(hMiniDumpFile);

		// Delete wrong dump file...
		if(dwSize == 0)
		{
			OutputDebugString(TEXT("y800ed!DriverProc: Writing minidump failed\n"));
			DeleteFile(szFileName);
		}
	}

	// return the magic value which tells Win32 that this handler didn't
	// actually handle the exception - so that things will proceed as per
	// normal.

	return EXCEPTION_EXECUTE_HANDLER;
}

//////////////////////////////////////////////////////////////////////////
//
//	DriverProc  -  The entry point for an installable driver.
//
//	PARAMETERS
//	dwDriverId:  For most messages, <dwDriverId> is the DWORD
//		value that the driver returns in response to a <DRV_OPEN> message.
//		Each time that the driver is opened, through the <DrvOpen> API,
//		the driver receives a <DRV_OPEN> message and can return an
//		arbitrary, non-zero value. The installable driver interface
//		saves this value and returns a unique driver handle to the
//		application. Whenever the application sends a message to the
//		driver using the driver handle, the interface routes the message
//		to this entry point and passes the corresponding <dwDriverId>.
//		This mechanism allows the driver to use the same or different
//		identifiers for multiple opens but ensures that driver handles
//		are unique at the application interface layer.
//		The following messages are not related to a particular open
//		instance of the driver. For these messages, the dwDriverId
//		will always be zero.
//
//		DRV_LOAD, DRV_FREE, DRV_ENABLE, DRV_DISABLE, DRV_OPEN
//
//	hDriver: This is the handle returned to the application by the
//		driver interface.
//
//	uiMessage: The requested action to be performed. Message
//		values below <DRV_RESERVED> are used for globally defined messages.
//		Message values from <DRV_RESERVED> to <DRV_USER> are used for
//		defined driver protocols. Messages above <DRV_USER> are used
//		for driver specific messages.
//
//	lParam1: Data for this message.  Defined separately for
//		each message
//
//	lParam2: Data for this message.  Defined separately for
//		each message
//
//	RETURNS
//		Defined separately for each message.
//
//////////////////////////////////////////////////////////////////////////

LRESULT __stdcall DriverProc(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2)
{
	__try
	{
		CY800VFW* pi = (CY800VFW*)(DWORD_PTR)dwDriverID;

		switch(uiMessage)
		{
		// standard driver messages
		//

		case DRV_ENABLE:
		case DRV_DISABLE:
		case DRV_LOAD:
		case DRV_FREE:
			return (LRESULT)DRVCNF_OK;

		case DRV_INSTALL:
		case DRV_REMOVE:
			return (LRESULT)DRV_OK;

		case DRV_OPEN:
			// GAAH! This used to return a pointer to 0xFFFF0000 when lParam==0!
			return (LRESULT)(DWORD_PTR)CY800VFW::Open((ICOPEN*)lParam2);

		case DRV_CLOSE:
			return CY800VFW::Close(pi);

		// state messages
		//

		case DRV_QUERYCONFIGURE:
			return (LRESULT)DRVCNF_OK;

		case DRV_CONFIGURE:
			pi->Configure((HWND)lParam1);
			return DRV_OK;

		case ICM_CONFIGURE:
			if(lParam1 == -1)
				return pi->QueryConfigure() ? ICERR_OK : ICERR_UNSUPPORTED;
			else
				return pi->Configure((HWND)lParam1);

		case ICM_ABOUT:
			if(lParam1 == -1)
				return pi->QueryAbout() ? ICERR_OK : ICERR_UNSUPPORTED;
			else
				return pi->About((HWND)lParam1);

		case ICM_GETSTATE:
			return pi->GetState((LPVOID)lParam1, (DWORD)lParam2);

		case ICM_SETSTATE:
			return pi->SetState((LPVOID)lParam1, (DWORD)lParam2);

		case ICM_GETINFO:
			return pi->GetInfo((ICINFO*)lParam1, (DWORD)lParam2);

		case ICM_GETDEFAULTQUALITY:
			if(lParam1) {
				*((LPDWORD)lParam1) = 10000;
				return ICERR_OK;
			}
			break;

		// compression messages
		//

		case ICM_COMPRESS_QUERY:
			return pi->CompressQuery((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

		case ICM_COMPRESS_BEGIN:
			return pi->CompressBegin((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

		case ICM_COMPRESS_GET_FORMAT:
			return pi->CompressGetFormat((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

		case ICM_COMPRESS_GET_SIZE:
			return pi->CompressGetSize((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

		case ICM_COMPRESS:
			return pi->Compress((ICCOMPRESS*)lParam1, (DWORD)lParam2);

		case ICM_COMPRESS_END:
			return pi->CompressEnd();

		// decompress messages
		//

		case ICM_DECOMPRESS_QUERY:
			return pi->DecompressQuery((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

		case ICM_DECOMPRESS_BEGIN:
			return pi->DecompressBegin((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

		case ICM_DECOMPRESS_GET_FORMAT:
			return pi->DecompressGetFormat((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

		case ICM_DECOMPRESS_GET_PALETTE:
			return pi->DecompressGetPalette((LPBITMAPINFOHEADER)lParam1, (LPBITMAPINFOHEADER)lParam2);

		case ICM_DECOMPRESS:
			return pi->Decompress((ICDECOMPRESS*)lParam1, (DWORD)lParam2);

		case ICM_DECOMPRESS_END:
			return pi->DecompressEnd();
		}

		if(uiMessage < DRV_USER)
			return DefDriverProc(dwDriverID, hDriver, uiMessage, lParam1, lParam2);
	}
	__except(RecordExceptionInfo(GetExceptionInformation()))
	{
		return (LRESULT)ICERR_ERROR;
	}

	return (LRESULT)ICERR_UNSUPPORTED;
}