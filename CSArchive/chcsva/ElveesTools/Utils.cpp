// Utils.cpp

#include <wtypes.h>

#include "Utils.h"
#include "Exceptions.h"
#include "CriticalSection.h"

#include <tchar.h>
#include <stdio.h>

#pragma warning(disable : 4100)

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

bool __cdecl IsFileExists(LPCTSTR stFile)
{
	HANDLE hFindFile;
	WIN32_FIND_DATA wfd;

	if((hFindFile = FindFirstFile(stFile, &wfd)) == INVALID_HANDLE_VALUE)
		return false;

	FindClose(hFindFile);
	return true;
}

LPCTSTR __cdecl ToHex(BYTE c)
{
	static TCHAR hex[3];

	const int val = c;

	wsprintf(hex, TEXT("%02X"), val);

	return hex;
}

bool __cdecl HexToString(LPCBYTE pBuffer, int nBytes, LPTSTR lpDest, int cchDest)
{
	return false;
}

bool __cdecl StringToHex(LPCTSTR lpSource, BYTE *pBuffer, int nBytes)
{
	return false;
}

LPCTSTR __cdecl GetLastErrorMessage(DWORD dwError)
{
	static TCHAR srErrMsg[512];

	if(!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 
		0,
		dwError,
		0,
		srErrMsg,
		511,
		NULL))
	{
		// if we fail, call ourself to find out why and return that error
		return (GetLastErrorMessage(GetLastError()));  
	}

	return srErrMsg;
}

LPCTSTR __cdecl GetCurrentDirectory()
{
	static TCHAR srCurDir[MAX_PATH];

	DWORD size = ::GetCurrentDirectory(0, 0);

	if(size > MAX_PATH || 0 == ::GetCurrentDirectory(size, srCurDir))
	{
		throw CException(TEXT("GetCurrentDirectory()"), TEXT("Failed to get current directory"));
	}

	return srCurDir;
}

LPCTSTR __cdecl GetDateTimeStamp()
{
	static TCHAR srDateTime[32];

	SYSTEMTIME stLocalTime;
	GetLocalTime(&stLocalTime);

	wsprintf(srDateTime, TEXT("%02hd/%02hd/%02hd %02hd:%02hd:%02hd"),
		stLocalTime.wDay,
		stLocalTime.wMonth,
		(1900 + stLocalTime.wYear) % 100,
		stLocalTime.wHour,
		stLocalTime.wMinute,
		stLocalTime.wSecond);

	return srDateTime;
}

LPCTSTR __cdecl DumpData(LPCBYTE pData, int dataLength, int lineLength)
{
	lineLength = min(80, lineLength);
	dataLength = min(256, dataLength);

	const size_t bytesPerLine = lineLength != 0 ? max(1,lineLength/4 - 1) : 16;

	static TCHAR stResult[1024];

	TCHAR stDisplay[32];
	TCHAR stDisplayHex[128];

	long cbResult = 0;
	long cbDisplay = 0;
	long cbDisplayHex = 0;

	int i = 0;
	int line = 0;
	
	while(i < dataLength)
	{
		const BYTE c = pData[i++];
		
		cbDisplay += wsprintf(&stDisplay[cbDisplay],
			TEXT("%c"),	isprint(c) ? (TCHAR)c : TEXT('.'));
		
		cbDisplayHex += wsprintf(&stDisplayHex[cbDisplayHex],
			TEXT("%02X "), c);

		if((bytesPerLine && (i % bytesPerLine == 0 && i != 0)) || i == dataLength)
		{
			if(i == dataLength && (bytesPerLine && (i % bytesPerLine != 0)))
			{
				for(size_t pad = i % bytesPerLine; pad < bytesPerLine; pad++)
				{
					cbDisplayHex += wsprintf(&stDisplayHex[cbDisplayHex], TEXT("   "));
				}
			}

			cbResult += wsprintf(&stResult[cbResult],
				line > 0 ? TEXT("\n%s   %s") : TEXT("%s   %s"),	stDisplayHex, stDisplay);

			cbDisplay = cbDisplayHex = 0;
			line++;
		}
	}

//	wsprintf(&stResult[cbResult], TEXT("%ld bytes"), dataLength);

	return stResult;
}

LPCTSTR __cdecl GetComputerName()
{
	static bool  gotName = false;
	static TCHAR stComputerName[MAX_COMPUTERNAME_LENGTH + 1];

	if(!gotName)
	{
		DWORD dwNameLen = MAX_COMPUTERNAME_LENGTH ;

		if(!::GetComputerName(stComputerName, &dwNameLen))
		{
			lstrcpy(stComputerName, TEXT("UNAVAILABLE"));
		}

		gotName = true;
	}

	return stComputerName;
}

LPCTSTR __cdecl GetUserName()
{
	static bool  gotName = false;
	static TCHAR stUserName[256];

	if(!gotName)
	{
		DWORD dwNameLen = 255;

		if(!::GetUserName(stUserName, &dwNameLen))
			lstrcpy(stUserName, TEXT("UNAVAILABLE"));

		gotName = true;
	}

	return stUserName;
}

///////////////////////////////////////////////////////////////////////////////
// Output functions
///////////////////////////////////////////////////////////////////////////////

void __cdecl OutputF(TOutput type, LPCTSTR stFormat, ...)
{
	int nRet;
	TCHAR szMsg[1024];

	// Format the input string
	va_list pArgs;
	va_start(pArgs, stFormat);
	nRet = _vsntprintf(szMsg, 1023, stFormat, pArgs);
	va_end(pArgs);

	if(nRet < 0)
	{
		Output(TWarning, TEXT("Message trancated"));
		DEBUG_ONLY(DebugBreak());
	}

	Output(type, szMsg);
}

} // End of namespace Elvees