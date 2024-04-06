//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		CSBackup client code
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   28.02.2005                                                                //
//                                                                                     //
//=====================================================================================//

#include "stdafx.h"
#include "BackupServer.h"
#include <process.h>
#include "../../CSChair/dmpstuff/ExceptionHandler.h"
#include <Shlwapi.h>
#include "NetPort.h"

// last header
#include "../../CSChair/public/memleak.h"

//=====================================================================================//
//                                 class BackupServer                                  //
//=====================================================================================//
BackupServer::BackupServer() : m_hStartThread(0)
{
}

BackupServer::~BackupServer()
{
}

void	BackupServer::CreateBackupInterface()
{
	BOOL bUseStub = SHRegGetBoolUSValue( RegSettings::c_szCinemaSecSubkey, _T("BackupStub"), TRUE, FALSE );	
	HRESULT hr = E_FAIL;
	ICSBackUp* pInt = NULL;
	if( ! bUseStub )
	{
		hr = CreateCSBackUp( &pInt, m_Debug.Get() );
	}
	else
	{
		m_Debug.PrintW( IDebugOutput::mt_info, L"BackupStub=true, don't create backup interface" );
	}
	if( hr != S_OK || !pInt )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to create CSBackup, code=%x", hr );
	}
	else
	{
		m_pBackup = boost::shared_ptr<ICSBackUp> ( pInt, ReleaseInterface<ICSBackUp>() );
		m_Debug.PrintW( IDebugOutput::mt_info, L"CSBackup created" );
	}
}

void	BackupServer::Start(IDebugOutput* pInt )
{
	Stop();
	m_Debug.Set( pInt );
	CreateBackupInterfaceInThread();
}

void	BackupServer::Stop()
{
	if( !m_hStartThread ) return;
	DWORD res = WaitForSingleObject( m_hStartThread, 100 );
	if( res == WAIT_TIMEOUT )
	{
		TerminateThread( m_hStartThread, DWORD(-1) );
	}
	CloseHandle(m_hStartThread);
	m_hStartThread = 0;
	m_pBackup.reset();
}

void	BackupServer::CreateBackupInterfaceInThread()
{
	m_Debug.PrintW( IDebugOutput::mt_info, L"Trying to create backup interface..." );
	m_hStartThread = (HANDLE)_beginthreadex( 0, 0, CreateBackupProc, this, 0, NULL  );
}

unsigned int __stdcall BackupServer::CreateBackupProc(void* pParam)
{
	unsigned int Res = unsigned(1);
	__try
	{
		Res = BackupServer::CreateBackupProc_Handled(pParam);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("CreateBackup thread"))
#ifndef _DEBUG
		, EXCEPTION_EXECUTE_HANDLER
#endif
		)
	{
	}
	return Res;
}

unsigned int __stdcall BackupServer::CreateBackupProc_Handled(void* pParam)
{
	HRESULT hr = ::CoInitializeEx( NULL, COINIT_MULTITHREADED );
	BackupServer* pThis = (BackupServer*)pParam;
	pThis->CreateBackupInterface();
	if(hr == S_OK) ::CoUninitialize();
	return 0;
}