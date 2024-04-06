//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Transfer a file through a client socket
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 26.07.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "FileTransferServer.h"
#include "../../CSChair/dmpstuff/ExceptionHandler.h"
#include "NetProto.h"
#include <shlwapi.h>
#include "FileTransferState.h"
#include "..\Timeouts.h"

//======================================================================================//
//                               class FileTransferServer                               //
//======================================================================================//
FileTransferServer::FileTransferServer():
	m_nSentCameras(0)
{
}

FileTransferServer::~FileTransferServer()
{
}

HRESULT		FileTransferServer::Start(	IDebugOutput* pDbgInt, netadr_t NetAddr, 
										const std::vector<int>& CameraIDs, 
										INT64 nBeginTime, INT64 nEndTime )
{
	m_Debug.Set(pDbgInt);
	m_NetAddr		= NetAddr;
	m_CameraIDs		= CameraIDs;
	m_nBeginTime	= nBeginTime;
	m_nEndTime		= nEndTime;
	m_nSentCameras = 0;
	
	if( m_CameraIDs.empty() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[FileTransferServer::Start]Empty request camera list" );
		return E_INVALIDARG;
	}
	if( nBeginTime >= nEndTime )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[FileTransferServer::Start]nBeginTime >= nEndTime" );
		return E_INVALIDARG;
	}
	if( IsWorking() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[FileTransferServer::Start]server is transferring the file" );
		return E_PENDING;
	}

	HANDLE hThread = (HANDLE)_beginthreadex
		(	0, 0, 
			ThreadProc_Handled< FileTransferServer, FileTransferServer::FileTransferProc, ~0 >, 
			this, 0, NULL  
		);
	if( hThread )
	{
		m_Thread.Start( hThread );
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

bool FileTransferServer::IsWorking()
{
	return WaitForSingleObject( m_Thread.m_hThread , 1 ) == WAIT_TIMEOUT;
}

HRESULT FileTransferServer::Stop()
{
	bool res = m_Thread.Stop( ThreadTimeout_FileTransferServer );
	return res ? S_OK : E_FAIL;
}

unsigned int FileTransferServer::FileTransferProc()
{
	m_ClientSocket.Start( PORT_ANY, m_NetAddr );
	
	StateArr_t			States;
	States.reserve( m_CameraIDs.size() );
	for( size_t i = 0; i < m_CameraIDs.size(); ++i )
	{
		States.push_back(
			boost::shared_ptr<IFileTransferState> ( IFileTransferState::GetStartState(this, m_CameraIDs[i] )  )
			);
	}
	while( true) 
	{
		if( !m_Thread.CanThreadWork( 50 ) )
		{
			CancellAll(States);
			break;
		}
		else 
		{
			if( ProcessAll(States) ) break;
		}
	}
	m_ClientSocket.Stop();
	m_FileMap.clear();
	if( m_nSentCameras == m_CameraIDs.size() )
	{
		m_Debug.PrintW(IDebugOutput::mt_debug_info, L"[FileTransferServer]transfer successfully");
		return Res_Ok;
	}
	else
		return  Res_NotWholeDataHaveBeenSent;
}

void	FileTransferServer::CancellAll( StateArr_t& States )
{
	for( size_t i =0; i < States.size(); ++i )
	{
		States[i]->Cancel();
	}
}

bool	FileTransferServer::ProcessAll( StateArr_t& States )
{
	bool bExit = true;
	for( size_t i =0; i < States.size(); ++i )
	{
		IFileTransferState* pNewState = States[i]->Process();
		if( pNewState )
		{
			States[i] = boost::shared_ptr<IFileTransferState>(pNewState);
		}
		if(! IFileTransferState::IsEndState( States[i].get() ) ) bExit = false;
	}	
	return bExit;
}

void		FileTransferServer::SendServerBusy(netadr_t NetAddr)
{
	SendReply<NetSendFileServerBusy> ( NetAddr, L"server busy" );
}

void		FileTransferServer::SendWrongTime(netadr_t NetAddr)
{
	SendReply<NetSendFileWrongTime> ( NetAddr, L"wrong time" );
}

void		FileTransferServer::SendWrongCamera(netadr_t NetAddr)
{
	SendReply<NetSendFileWrongCamera> ( NetAddr, L"wrong camera" );
}

LPCWSTR	FileTransferServer::ErrorDesc(int nResCode)
{
	switch (nResCode)
	{
	case NetProto::FileResponseEndHdr::Res_Ok:				return L"ok";
	case NetProto::FileResponseEndHdr::Res_Cancelled:		return L"transfer was cancelled";
	case NetProto::FileResponseEndHdr::Res_NoSuchCamera:	return L"no such camera";
	case NetProto::FileResponseEndHdr::Res_ServerBusy:		return L"server is busy";
	case NetProto::FileResponseEndHdr::Res_SourceError:		return L"source error";
	case NetProto::FileResponseEndHdr::Res_ClientCancel:	return L"client cancel the operation";
	case NetProto::FileResponseEndHdr::Res_WrongTime:		return L"wrong time";
	case NetProto::FileResponseEndHdr::Res_WrongCamera:		return L"wrong camera";
	default:												return L"undefined error";
	}
}