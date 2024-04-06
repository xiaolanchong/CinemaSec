/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CSBackupImp.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-07-29
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Exception.hpp"
#include "Utility.hpp"
#include "Database.hpp"
#include "Storage.hpp"
#include "CSBackup.h"
#include "CSBackupImp.hpp"
// exception handler
#include "..\dmp\ExceptionHandler.h"

HRESULT __declspec(dllexport) CreateCSBackUp(ICSBackUp** ppBackUp, IDebugOutput* pDebugOut)
{
	try
	{
		assert(pDebugOut);
		*ppBackUp = NULL;
        
		CSBackupImp* pCSBackupImp = new CSBackupImp( pDebugOut );
		*ppBackUp = pCSBackupImp;

		//Start cleaning 		
		pCSBackupImp->Cleanup();
	}
	catch( CException<CSBackupImp>& e )
	{
		pDebugOut->PrintW( IDebugOutput::MessageType::mt_error, L"Create CSBackupImp failed" );
		pDebugOut->PrintW( IDebugOutput::MessageType::mt_error, e.Cause().c_str() );
		return E_FAIL;
	}
	catch( ... )
	{
		pDebugOut->PrintW( IDebugOutput::MessageType::mt_error, L"Unknown error has been occured" );
		return E_FAIL;
	}

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//These methods are called by external thread

CSBackupImp::CSBackupImp( IDebugOutput* pDebug )
{
	m_pDebug = pDebug;
}

CSBackupImp::~CSBackupImp()
{
}

HRESULT CSBackupImp::Release()
{
	if(IsBadReadPtr(this, sizeof(CSBackupImp)))
		return E_FAIL;

	//If thread is started
	if(::WaitForSingleObject( m_nThreadStartedEvent, 0) == WAIT_OBJECT_0 )
	{
		//Try to stop server
		if(::SetEvent( m_hStoppingEvent ))
		{
			// Wait for thread
			WaitForSingleObject( m_nThreadStartedEvent, ( m_nOuterLoopWaitingTime / 2 ) );
		}

		// Still started?
		if(::WaitForSingleObject( m_nThreadStartedEvent, 0) == WAIT_TIMEOUT)
		{
			m_pDebug->PrintW(IDebugOutput::mt_error, L"Terminating backup thread");

			// Kill process
			if(!TerminateThread(m_hThread, 1))
			{
				m_pDebug->PrintW(IDebugOutput::mt_error, L"Terminate failed");
			}
		}
	}
	try
	{
		delete this;
	}
	catch( ... )
	{
		return E_FAIL;
	}

	return S_OK;
}
void CSBackupImp::Cleanup()
{
	//Create stopping event
	m_hStoppingEvent = CreateEvent( NULL, TRUE, FALSE, L"Stopping Event" ); 
	if( m_hStoppingEvent == NULL  )
		throw CException<CSBackupImp>( boost::str( boost::wformat( L"Can't create the event object, error: %1%" ) % GetLastError() ) );

	//Create stopping event
	m_nThreadStartedEvent = CreateEvent( NULL, TRUE, TRUE, L"Starting Event" ); 
	if( m_nThreadStartedEvent == NULL  )
		throw CException<CSBackupImp>( boost::str( boost::wformat( L"Can't create the event object, error: %1%" ) % GetLastError() ) );

	//Create main thread
	m_hThread = CreateThread( NULL, 0, CSBackupImp::ThreadProc, this, 0, &m_dwThreadId);  
	if( m_hThread == NULL )
		throw CException<CSBackupImp>( boost::str( boost::wformat( L"Can't create the thread, error: %1%" ) % GetLastError() ) );

	//Set thread priority
	SetThreadPriority( m_hThread, THREAD_PRIORITY_LOWEST );

}

// 2005-08-11 dump enchancement

DWORD WINAPI CSBackupImp::ThreadProc_Handled( void* pParam )
{
	__try
	{
		return CSBackupImp::ThreadProc( pParam );
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("CSBackup thread")), EXCEPTION_EXECUTE_HANDLER )
	{
		return ~0;
	}
}

//////////////////////////////////////////////////////////////////////////
DWORD WINAPI CSBackupImp::ThreadProc(void* pParam)
{
	HRESULT hr = S_OK;
	unsigned int nCurrentIteration = 0;


	//Initalize COM library
    CoInitialize( NULL );

	assert( pParam );
	CSBackupImp* pBackup = 
		static_cast<CSBackupImp*>( pParam );

	//Thread has been started, raise event
	SetEvent( pBackup->m_nThreadStartedEvent );
	boost::shared_ptr<void> GuardEvent( pBackup->m_nThreadStartedEvent, ResetEvent );

	//Fetch parameters
	while(  WaitForSingleObject( pBackup->m_hStoppingEvent, CSBackupImp::m_nOuterLoopWaitingTime ) != WAIT_OBJECT_0 )
	{

		try
		{

			//Retrieve archive path
			std::wstring sArchivePath;
			CUtility::GetArchivePath( sArchivePath );

			//Get room ids
			std::vector<DWORD> dwRoomIds;
			CUtility::GetRoomIDs( dwRoomIds );

			//Get connection string 
			std::wstring sConnectionString;
			CUtility::GetDatabaseConnectionString( sConnectionString );

			//Initialize DB stuff
			boost::shared_ptr<CCinemaDatabase> pDatabase( new CCinemaDatabase( sConnectionString ) );
			pDatabase->Connect();

			//Retrieve camera ids
			std::wstring sLocalHostIP;
			std::vector<int> nCameraIds;
			CUtility::GetLocalHostIP( sLocalHostIP ); 
			pDatabase->GetCameraIDs( sLocalHostIP, nCameraIds );

			//Nothing to do here :(
			if( sArchivePath.empty() )
				throw CException<CUtility>( L"Archive path is empty" );

			if( nCameraIds.empty() )
				throw CException<CUtility>( L"CameraIDs list is empty" );

			//Initialized storage
			boost::shared_ptr<CCinemaStorage> pStorage( new CCinemaStorage( sArchivePath, nCameraIds, pBackup->GetDebugOutput() ) );

			double dBackupStart;
			double dBackupStop;
			DATE lastDate;

			pDatabase->GetCleaningParams( dwRoomIds, dBackupStart, dBackupStop, lastDate );

			//Update statistics
			pDatabase->UpdateStatistics( dwRoomIds, lastDate );

			//Cleaning by free space on the disk
			if( dBackupStart >= pStorage->GetAvailableSpaceBalance() )
			{
				//Synchronize storage with the physical storage and sort content
				pStorage->Synchronize();
				pStorage->Sort();

				//Start killing files by free space on disk
				for( 
					CCinemaStorage::CFileProxyIt it = pStorage->Begin();
					( it != pStorage->End() ) 
					&& ( dBackupStop >= pStorage->GetAvailableSpaceBalance() )
					&& ( WaitForSingleObject( pBackup->m_hStoppingEvent, CSBackupImp::m_nInnerLoopWaitingTime ) != WAIT_OBJECT_0 );
				)
				{
					try
					{
						//Delete the element and return next element position
						it = pStorage->Delete( it );
					}
					catch( CException<CCinemaStorage>& e )
					{
						//Can't delete file
						pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, e.Cause().c_str() );

						//Move to the next file
						++it;
					}
				}

				//Remove empty directories
				pStorage->RemoveDirectories();
			}
            
			//Cleaning by last date every 50'th iteration
			if(		( ++nCurrentIteration % 50 == 0 )
				&& ( WaitForSingleObject( pBackup->m_hStoppingEvent, CSBackupImp::m_nInnerLoopWaitingTime ) != WAIT_OBJECT_0 ) )
			{
				COleDateTime dtLastDate( lastDate );

				//Synchronize storage with the physical storage and sort content
				pStorage->Synchronize();
				pStorage->Sort();

				//Start killing files by date
				for( 
					CCinemaStorage::CFileProxyIt it = pStorage->Begin();
					( it != pStorage->End() ) 
					&& ( WaitForSingleObject( pBackup->m_hStoppingEvent, CSBackupImp::m_nInnerLoopWaitingTime ) != WAIT_OBJECT_0 );
				)
				{
					try
					{
						COleDateTime dtFileDate = it->GetOleDateTime();

						if( dtFileDate <= dtLastDate )
							it = pStorage->Delete( it );
						else 
							++it;
					}
					catch( CException<CCinemaStorage>& e )
					{
						//Can't delete file
						pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, e.Cause().c_str() );

						//Move to the next file
						++it;
					}
				}

				//Remove empty directories
				pStorage->RemoveDirectories();
			}



		}// Outer while loop

		catch( CException<CCinemaDatabase>& e )
		{
			pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, L"Error in the thread proc" );
			pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, e.Cause().c_str() );
		}
		catch( CException<CCinemaStorage>& e )
		{
			pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, L"Error in the thread proc" );
			pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, e.Cause().c_str() );
		}
		catch( CException<CUtility>& e )
		{
			pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, L"Error in the thread proc" );
			pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, e.Cause().c_str() );
		}
		catch( CException<CSBackupImp>& e )
		{
			pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, L"Error in the thread proc" );
			pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, e.Cause().c_str() );
		}
		catch( ... )
		{
			pBackup->m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, L"Unknown error in the thread proc" );
		}
	}

	CoUninitialize();
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////



