//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Implements guard action
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 08.07.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "GuardAction.h"
#include "Launcher.h"

#include "../../../CSChair/public/memleak.h"

BOOL CALLBACK ShowWindowProc(   HWND hwnd,  LPARAM lParam);
BOOL CALLBACK HideWindowProc(   HWND hwnd,  LPARAM lParam);
BOOL CALLBACK EnumWindowProc(   HWND hwnd,  LPARAM lParam, int nCmd);
void	WaitOrKillProcess( PROCESS_INFORMATION& pi );
bool	TrySendMessage( DWORD dwThreadId, LPARAM dwWindowID, LPCTSTR szMessage );

/////// State /////////////

IState*	IState::GetStartState()
{
	return new StartState;
}

IState * StartState::Process( ProcessEnvironment& pe )
{
	STARTUPINFO si;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pe.m_pi, sizeof(pe.m_pi));
	if(CreateProcess(pe.m_szPath, // No module name (use command line). 
		NULL,             // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block.
		NULL,             // Use parent's starting directory. !!!
		&si,              // Pointer to STARTUPINFO structure.
		&pe.m_pi))             // Pointer to PROCESS_INFORMATION structure.
	{
		return new CreatedState;
	}
	else
	{
		Output(_T("Err: Fail to restart server"));
		Sleep(1000);
		return NULL;
	}
}

////////////////////////////////////////////////////
IState * EndProcessState::Process( ProcessEnvironment& pe )
{
	WaitOrKillProcess( pe.m_pi, pe.n_WaitOrKillProcess );
	if(m_bExitAfterEnd )
	{
		return new ExitState;
	}
	else
	{
		Sleep(pe.m_dwTimeout);
		return new StartState;
	}
}

void	EndProcessState::WaitOrKillProcess( PROCESS_INFORMATION& pi, DWORD dwTimeout )
{
	PostThreadMessage( pi.dwThreadId, WM_QUIT, 0, 0  );
	DWORD res = WAIT_TIMEOUT;
	__try
	{
		res = WaitForSingleObject( pi.hProcess, dwTimeout );
	}
	__except( 0, EXCEPTION_EXECUTE_HANDLER )
	{
		
	}
	if( res == WAIT_TIMEOUT )
	{
		Output(_T("WARNING: process did not exit, terminating")  );
		TerminateProcess( pi.hProcess, -1);
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

/////////////////////////////////////////////////////


IState * CreatedState::Process( ProcessEnvironment& pe )
{
	bool bRestart = false;
	bRestart = CheckProcess( pe );
	if(bRestart) return new StartState;
	QueueResult res = CheckQueue( pe );
	if( res == res_restart )
	{
		return new EndProcessState(false);
	}
	else if( res == res_exit )
	{
		return new EndProcessState(true);
	}
	bRestart = PingProcess( pe );
	if(bRestart) return new EndProcessState(false);
	return NULL;
}

bool CreatedState::CheckProcess(ProcessEnvironment& pe)
{
	DWORD res = WaitForSingleObject( pe.m_pi.hProcess, pe.n_CheckProcess );
	if( res  == WAIT_OBJECT_0  || res == WAIT_FAILED )
	{
		DWORD dwExit = 0;
		GetExitCodeProcess(pe.m_pi.hProcess, &dwExit);

		OutputF(_T("Process exited with code 0x%lX"), dwExit);
		Output(_T("Restarting server after timeout (0.5c)"));
		Sleep(pe.m_dwTimeout);

		CloseHandle(pe.m_pi.hThread);
		CloseHandle(pe.m_pi.hProcess);
		return true;
	}
	else
		return false;
}

BOOL CALLBACK EnumWindowProc(   HWND hwnd,  LPARAM lParam, int nCmd)
{ 
	if( lParam != 0 )
	{
		HANDLE ID = GetProp( hwnd, _T("WindowID") );
		if( (LPARAM)ID == lParam ) ShowWindow( hwnd, nCmd );
	}
	else
		ShowWindow( hwnd, nCmd );
	return TRUE; 
} 

BOOL CALLBACK ShowWindowProc(   HWND hwnd,  LPARAM lParam)
{
	return EnumWindowProc( hwnd, lParam, SW_SHOW );
}

BOOL CALLBACK HideWindowProc(   HWND hwnd,  LPARAM lParam)
{
	return EnumWindowProc( hwnd, lParam, SW_HIDE );
}

CreatedState::QueueResult CreatedState::CheckQueue(ProcessEnvironment& pe)
{
	CreatedState::QueueResult res = res_ok;
	MessagePipe::MsgQueue_t* pQueue = GetMessagePipe().LockQueue( true );
	if( pQueue )
	{
		while ( !pQueue->empty() )
		{
			int Msg = pQueue->front();
			switch( Msg )
			{
			case MessagePipe::MsgExit: 
				res = res_exit;
				break;
			case MessagePipe::MsgRestart:
				Output(_T("Restarting server after \"Restart\" command"));
				res = res_restart;
				break;
			case MessagePipe::MsgShow:
				EnumThreadWindows( pe.m_pi.dwThreadId, ShowWindowProc, pe.m_WindowID ); 
				Output(_T("Show application's main window"));
				res = res_ok;
				break;
			case MessagePipe::MsgHide:
				EnumThreadWindows( pe.m_pi.dwThreadId, HideWindowProc, pe.m_WindowID ); 
				Output(_T("Hide application's main window"));
				res = res_ok;
				break;
			default: ;
			}
			pQueue->pop();
		}
		GetMessagePipe().UnlockQueue();
	}
	return res;
}

typedef std::pair<LPARAM, HWND> Arg_t;

BOOL CALLBACK GetMainWindowProc(   HWND hwnd,  LPARAM lParam)
{ 
	Arg_t*	p = (Arg_t*)lParam;
	HANDLE ID = GetProp( hwnd, _T("WindowID") );
	if( (LPARAM)ID == p->first ) 
	{
		p->second = hwnd;
		return FALSE;
	}
	return TRUE; 
} 

static HWND	GetMainThreadWindow(DWORD dwThreadId, LPARAM WindowID)
{
	Arg_t	p ( WindowID, NULL );
	EnumThreadWindows( dwThreadId, GetMainWindowProc, (LPARAM)&p );
	return p.second;
}

bool CreatedState::PingProcess(ProcessEnvironment& pe)
{
	if( !pe.m_WindowMsg ) return false;
	HWND hWnd = GetMainThreadWindow( pe.m_pi.dwThreadId, pe.m_WindowID );
	if( hWnd  )
	{
		const WPARAM	dwCookie = 0xdeadbeef;
		DWORD_PTR		res = 0;
		LPARAM lRes = SendMessageTimeout( hWnd, pe.m_WindowMsg, dwCookie, 0, SMTO_ABORTIFHUNG, pe.n_SendMessageTimeout, &res );
		if( !lRes )
		{
			Output(_T("Timeout when message was sent. Application will be restarted"));
			return true;
		}
		else if( dwCookie != res )
		{
			Output(_T("WARNING: Wrong answer"));
			return false;
		}
		else return false;
	}
	else if( !m_nTimeAfterNoWindow )
	{
		m_nTimeAfterNoWindow = _time64(NULL);
	}
	else if( ( _time64(NULL) - *m_nTimeAfterNoWindow.get_ptr() ) > pe.n_TimeAfterNoWindow/1000 )
	{
		Output(_T("WARNING: Application's main window does not exist, but the process still is active. Restart"));
		return true;
	}
	return false;
}