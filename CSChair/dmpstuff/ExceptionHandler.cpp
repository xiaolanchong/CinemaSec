// ExceptionHandler.cpp  Version 1.3
//
// Copyright � 1998 Bruce Dawson
//
// This source file contains the exception handler for recording error
// information after crashes. See ExceptionHandler.h for information
// on how to hook it in.
//
// Author:       Bruce Dawson
//               brucedawson@cygnus-software.com
//
// Modified by:  Hans Dietrich
//               hdietrich2@hotmail.com
//
// Version 1.3:  - Added minidump output
//
// Version 1.1:  - reformatted output for XP-like error report
//               - added ascii output to stack dump
//
// A paper by the original author can be found at:
//     http://www.cygnus-software.com/papers/release_debugging.html
//
///////////////////////////////////////////////////////////////////////////////

// 10/11/2004	enchanced by Eugene Gorbachev
//				remove extension from the file name and add the result to current system time 
//				Year-Month-Day-Hour-Minute

// Disable warnings generated by the Windows header files.
#pragma warning(disable : 4514)
#pragma warning(disable : 4201)
#pragma	  warning( disable : 4995 )		//deprecated functions

#define _WIN32_WINDOWS 0x0500	// for IsDebuggerPresent

// if you don't use PCH comment a string below
#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

///////////////////////////////////////////////////////////////////////////////
// lstrrchr (avoid the C Runtime )
static TCHAR * lstrrchr(LPCTSTR string, int ch)
{
	TCHAR *start = (TCHAR *)string;

	while (*string++)                       /* find end of string */
		;
											/* search towards front */
	while (--string != start && *string != (TCHAR) ch)
		;

	if (*string == (TCHAR) ch)                /* char found ? */
		return (TCHAR *)string;

	return NULL;
}

// returns a pointer to the extension of a file.
//
// in:
//      qualified or unqualfied file name
//
// returns:
//      pointer to the extension of this file.  if there is no extension
//      as in "foo" we return a pointer to the NULL at the end
//      of the file
//
//      foo.txt     ==> ".txt"
//      foo         ==> ""
//      foo.        ==> "."
//

#define FILENAME_SEPARATOR_1	_T('\\')
#define FILENAME_SEPARATOR_2	_T('/')

STDAPI_(LPTSTR) MyPathFindExtension(LPCTSTR pszPath)
{
    LPCTSTR pszDot = NULL;
    if (pszPath)
    {
        for (; *pszPath; ++pszPath )
        {
            switch (*pszPath)
            {
                case TEXT('.'):
                    pszDot = pszPath;   // remember the last dot
                    break;

                case FILENAME_SEPARATOR_1:
				case FILENAME_SEPARATOR_2:
                case TEXT(' '):         // extensions can't have spaces
                    pszDot = NULL;      // forget last dot, it was in a directory
                    break;
            }
        }
    }

    // if we found the extension, return ptr to the dot, else
    // ptr to end of the string (NULL extension) (cast->non const)
    return pszDot ? (LPTSTR)pszDot : (LPTSTR)pszPath;
}

///////////////////////////////////////////////////////////////////////////////
// DumpMiniDump
static void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo)
{
	if (excpInfo == NULL) 
	{
		// Generate exception to get proper context in dump
		__try 
		{
			OutputDebugString(_T("raising exception\r\n"));
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(hFile, GetExceptionInformation()),
				 EXCEPTION_CONTINUE_EXECUTION) 
		{
		}
	} 
	else
	{
		OutputDebugString(_T("writing minidump\r\n"));
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = excpInfo;
		eInfo.ClientPointers = FALSE;

		// note:  MiniDumpWithIndirectlyReferencedMemory does not work on Win98
		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpWithIndirectlyReferencedMemory,
			excpInfo ? &eInfo : NULL,
			NULL,
			NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetFilePart
static TCHAR * GetFilePart(LPCTSTR source)
{
	TCHAR *result = lstrrchr(source, _T('\\'));
	if (result)
		result++;
	else
		result = (TCHAR *)source;
	return result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// RecordExceptionInfo
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS pExceptPtrs, 
								LPCTSTR lpszMessage)
{
	UNREFERENCED_PARAMETER(lpszMessage);
	static bool bFirstTime = true;
	if (!bFirstTime)	// Going recursive! That must mean this routine crashed!
		return EXCEPTION_CONTINUE_SEARCH;
	bFirstTime = false;

	// Create a filename to record the error information to.
	// Storing it in the executable directory works well.

	TCHAR szModuleName[MAX_PATH*2];
	ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileName(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpy(szModuleName, _T("Unknown"));

	TCHAR *pszFilePart = GetFilePart(szModuleName);

	///////////////////////////////////////////////////////////////////////////
	//
	// write minidump
	//
	///////////////////////////////////////////////////////////////////////////

	// Replace the filename with our minidump file name
	// don't use MFC
	SYSTEMTIME	SysTime;
	GetLocalTime  ( &SysTime );

	TCHAR FormattedName[MAX_PATH];
	wsprintf(	FormattedName, _T( "_%04d-%02d-%02d-%02d-%02d.dmp"), 
				SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute );
	
	LPTSTR pExt = MyPathFindExtension(pszFilePart);
	if( pExt ) *pExt = _T('\0');
	//NOTENOTE: caution! we can overflow buffer!
	lstrcat(pszFilePart, FormattedName );

	// Create the file
	HANDLE hMiniDumpFile = CreateFile(
		szModuleName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);

	// Write the minidump to the file
	if (hMiniDumpFile != INVALID_HANDLE_VALUE)
	{
		DumpMiniDump(hMiniDumpFile, pExceptPtrs);

		// Close file
		CloseHandle(hMiniDumpFile);
	}

	// return the magic value which tells Win32 that this handler didn't
	// actually handle the exception - so that things will proceed as per
	// normal.
	return EXCEPTION_CONTINUE_SEARCH;
}
