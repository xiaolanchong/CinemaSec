// Launcher.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Launcher.h"
#include "IAction.h"
#include <strsafe.h>

#include <memory>
#include <iostream>
#include <fstream>
#include <boost/shared_ptr.hpp>

#ifdef _UNICODE
typedef std::wfstream _tfstream;
#else
typedef std::fstream _tfstream;
#endif

///// log function //////

//PostMessage(HWND_BROADCAST)

static _tfstream s_debugOut;

void Output(LPCTSTR stText)
{
	TCHAR stHeader[32];
	SYSTEMTIME stLocalTime;
	GetLocalTime(&stLocalTime);

	// To use buffers larger than 1024 bytes, use _snwprintf
	StringCchPrintf(stHeader, 32, TEXT("%02hd-%02hd %02hd:%02hd:%02hd "),
		stLocalTime.wMonth,
		stLocalTime.wDay,
		stLocalTime.wHour,
		stLocalTime.wMinute,
		stLocalTime.wSecond);

	// printf fail if %s in text
	_tprintf(TEXT("%s%s\n"), stHeader, stText);

	// log to file...
	if(!s_debugOut.is_open())
	{
		s_debugOut.open(("Launcher.log"), std::ios_base::out | std::ios_base::app);

		s_debugOut << TEXT("****************New Log*****************") << std::endl;
	}

	s_debugOut << stHeader << stText << std::endl;
}

void OutputF(LPCTSTR stFormat, ...)
{
	int nRet;
	TCHAR szMsg[1024];

	// Format the input string
	va_list pArgs;
	va_start(pArgs, stFormat);
	nRet = _vsntprintf(szMsg, 1023, stFormat, pArgs);
	va_end(pArgs);

	if(nRet < 0)
		Output(TEXT("Message trancated:"));

	Output(szMsg);
}

//////////////////////////////////////////////////////////////////////////
// Unhandles Exception Filter
//////////////////////////////////////////////////////////////////////////

long WINAPI UnhandledExceptionFlt(PEXCEPTION_POINTERS ExceptionInfo)
{
	::MessageBox(NULL, TEXT("UNHANDLED EXCEPTION, Closing process"),
		TEXT("Launcher"), MB_OK | MB_ICONSTOP);

	::ExitProcess(ExceptionInfo->ExceptionRecord->ExceptionCode);

	return EXCEPTION_EXECUTE_HANDLER;
}

//////////////////////////////////////////////////////////////////////////
// Entry
//////////////////////////////////////////////////////////////////////////

unsigned int __stdcall  LauncherProc( LPVOID pParam )
{
	const DWORD	c_WaitOrKillProcess = 5 * 1000; // ms
	const DWORD c_CheckProcess		= 500;		// ms
	const DWORD c_TimeAfterNoWindow	= 30 * 1000;		// ms
	const DWORD c_SendMessageTimeout = 5 * 1000; //ms

	UNREFERENCED_PARAMETER( pParam );
	ProcessEnvironment pe;

	TCHAR c_szAppName[MAX_PATH] = _T("%WINDIR%\\notepad.exe");
	SetUnhandledExceptionFilter(UnhandledExceptionFlt);

	//////////////////////////////////////////////////////////////////////////

	TCHAR szWindowMsg[MAX_PATH];

	GetPrivateProfileString(_T("Main"), _T("App"), c_szAppName,	pe.m_szPath, MAX_PATH, _T(".\\Launcher.ini"));
	DoEnvironmentSubst( pe.m_szPath, MAX_PATH);

	pe.m_dwTimeout	= GetPrivateProfileInt(_T("Main"), _T("Timeout"), 5000, _T(".\\Launcher.ini"));
	pe.m_WindowID	= GetPrivateProfileInt(_T("Main"), _T("WindowID"), 0, _T(".\\Launcher.ini"));

	pe.n_WaitOrKillProcess	= GetPrivateProfileInt(_T("Timeout"), _T("WaitOrKillProcess"),	c_WaitOrKillProcess, _T(".\\Launcher.ini"));
	pe.n_CheckProcess		= GetPrivateProfileInt(_T("Timeout"), _T("CheckProcess"),		c_CheckProcess, _T(".\\Launcher.ini"));
	pe.n_TimeAfterNoWindow	= GetPrivateProfileInt(_T("Timeout"), _T("TimeAfterNoWindow"),	c_TimeAfterNoWindow, _T(".\\Launcher.ini"));
	pe.n_SendMessageTimeout	= GetPrivateProfileInt(_T("Timeout"), _T("SendMessageTimeout"),	c_SendMessageTimeout, _T(".\\Launcher.ini"));

	GetPrivateProfileString(_T("Main"), _T("WindowMsg"), NULL,	szWindowMsg, MAX_PATH, _T(".\\Launcher.ini"));
	pe.m_WindowMsg	= !szWindowMsg || _tcslen(szWindowMsg) == 0 ? 0 : ::RegisterWindowMessage( szWindowMsg ); 

	OutputF(_T("Running process \"%s\" in monitoring mode"), pe.m_szPath );

	BOOL bDone = FALSE;

	boost::shared_ptr<IState>	CurrentState( IState::GetStartState() );

	while(!bDone)
	{
		IState* pNextState = CurrentState->Process( pe );
		if( pNextState )
		{
			if( dynamic_cast<ExitState*>( pNextState ) ) bDone = TRUE;
			CurrentState = boost::shared_ptr<IState>( pNextState );
		}
	}

	Output(_T("Stop monitoring"));
	return 0;
}


///////////////

MessagePipe&	GetMessagePipe()
{
	static MessagePipe mp;
	return mp;
}

void		MessagePipe::SendMessage( int msg)
{
	MsgQueue_t* pQueue = LockQueue( false );
	ASSERT(pQueue);
	pQueue->push( msg );
	UnlockQueue();
}

MessagePipe::MsgQueue_t*	MessagePipe::LockQueue(bool bOnlyTry)
{
	if( bOnlyTry )
	{
		BOOL bRes = TryEnterCriticalSection( &m_cs );
		if(bRes) return &m_MsgQueue;
		else return 0;
	}
	else
	{
		EnterCriticalSection( &m_cs );
		return &m_MsgQueue;
	}
}

void		MessagePipe::UnlockQueue()
{
	LeaveCriticalSection( &m_cs );
}