//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Реализация интерфейса передачи файлов от сервера к клиенту
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 27.07.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "FileTransferImpl.h"
#include "../IPC/NetTcpLink.h"
#include "../../CSChair/dmpstuff/ExceptionHandler.h"
#include "../../CSChair/interfaceEx.h"
#include <shlwapi.h>
#include <strsafe.h>
#include "..\mainserver\NetPort.h"
#include "..\Timeouts.h"

//======================================================================================//
//                                class FileTransferImpl                                //
//======================================================================================//
FileTransferImpl::FileTransferImpl():
	m_bFileTransfered(FALSE)
{
	
}

FileTransferImpl::~FileTransferImpl()
{
}

HRESULT	FileTransferImpl::Start( IDebugOutput* pDbgInt, std::vector<CamRequest_t>&	CamRequest, INT64 nBeginTime, INT64 nEndTime )
{
	m_Debug.Set( pDbgInt );
	if( CamRequest.empty() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Empty request list");
		return E_INVALIDARG;
	}
	if( WaitForSingleObject( m_Thread.m_hThread , 1) == WAIT_TIMEOUT  )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"The thread has already started, try later");
		return E_PENDING;
	}
	m_RequestedCam	= CamRequest;
	m_nBeginTime	= nBeginTime;
	m_nEndTime		= nEndTime;
	m_CameraFileName.clear();

	m_CameraState.clear();
	for(size_t i = 0; i < CamRequest.size(); ++i)
	{
		m_CameraState.insert( std::make_pair( boost::get<0>(CamRequest[i]), CameraState() ) );
	}

	m_NetServer.LockIncomeBuffer();
	m_NetServer.GetIncomeBuffer().clear();
	m_NetServer.UnlockIncomeBuffer();

	typedef unsigned ( __stdcall *start_address_t )( void * );
	start_address_t sa = ThreadProc_Handled<FileTransferImpl, &FileTransferImpl::ThreadProc, ~0 >;
	HANDLE hThread = (HANDLE)_beginthreadex( 0, 0, sa, this, 0, NULL  );
	m_Thread.Start( hThread );
	return S_OK;
}

std::auto_ptr<IFileTransfer::IState>	FileTransferImpl::GetState( int nCameraID )
{
	AutoLockCriticalSection acs( m_Sync );
	
	if( m_TotalState.get() )
	{
		return std::auto_ptr<IFileTransfer::IState>( m_TotalState->Clone() );
	}

	std::map<int, CameraState>::const_iterator it = m_CameraState.find( nCameraID );

	if( it == m_CameraState.end() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[GetState]No such camera=%d", nCameraID);
		return std::auto_ptr<IFileTransfer::IState>();
	}

	const CameraState& cs = it->second;

	IFileTransfer::IState* pState = NULL;
	switch( cs.m_nState )
	{
	case st_NotStarted:
		pState = new IFileTransfer::NotStarted();
		break;
	case st_Working:
		pState = new IFileTransfer::Working
			( 
				cs.m_dwTotalFileSize ? double( cs.m_dwCurrentOffset )/cs.m_dwTotalFileSize: 0.0, 
				cs.m_dwTotalFileSize 
			) ;
		break;
	case st_Finished:
		pState = new IFileTransfer::Finished( cs.m_nReturnCode );
		break;
	}
	return pState ? std::auto_ptr<IFileTransfer::IState>( pState ) : std::auto_ptr<IFileTransfer::IState>();
}

std::auto_ptr<IFileTransfer::IState>	FileTransferImpl::GetTotalState(  )
{
	AutoLockCriticalSection acs( m_Sync );

	return  (m_TotalState.get()?
				std::auto_ptr<IFileTransfer::IState>( m_TotalState->Clone() ) : 
				std::auto_ptr<IFileTransfer::IState>() ) ;
}

HRESULT	FileTransferImpl::Cancel()
{
	bool res = m_Thread.Stop( ThreadTimeout_FileTransferClient );
	return S_OK;
}

bool	FileTransferImpl::IsBusy()
{
	return WaitForSingleObject( m_Thread.m_hThread, 0 ) == WAIT_TIMEOUT;
}

unsigned int FileTransferImpl::ThreadProc()
{
	NetProto Proto( this, this );

	m_CameraFile.clear();
	m_CameraFileName.clear();
	ServerMap_t ServerMap;
	for( size_t i = 0; i < m_RequestedCam.size(); ++i )
	{
		int nCameraID = boost::get<0>(m_RequestedCam[i]);
		const std::wstring& ServerName	= boost::get<1>(m_RequestedCam[i]) ;
		const std::wstring& FileName	= boost::get<2>(m_RequestedCam[i]) ;
		ServerMap_t::iterator it = ServerMap.find( ServerName );
		if( it != ServerMap.end() )
		{
			it->second.push_back( nCameraID );
		}
		else
		{
			std::vector<int> z(1, nCameraID );
			ServerMap.insert( std::make_pair( ServerName, z ) );
		}
		m_CameraFileName.insert( std::make_pair( nCameraID, boost::get<2>(m_RequestedCam[i]) ) );

		try
		{
			m_CameraFile.insert( std::make_pair( nCameraID,
					boost::shared_ptr<FileArchiveWriter>( new FileArchiveWriter(FileName, m_Debug.Get() ) ) ) );
		}
		catch(  FileArchiveWriterException )
		{
			m_Sync.Lock();
			m_TotalState = std::auto_ptr<IFileTransfer::IState>( new CriticalErrorFile(FileName) );
			m_Sync.Unlock();
			return 0;
		}
	}

	DumpFileRequest( ServerMap );

	unsigned nRes = SendFileRequest( ServerMap );
	if( nRes ) 
	{
		CancelAll();
		return nRes;
	}

	try
	{
		m_NetServer.Start( NetSettings::c_FilePort , 60 );
	}
	catch( NetException& ex )
	{
		AutoLockCriticalSection acs( m_Sync );
		m_TotalState = std::auto_ptr<IFileTransfer::IState>
			( new CriticalErrorSocket(   ) ) ;
		m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to start server, reason=%hs", ex.what() );
	}

	m_bFileTransfered	= false;
	m_bMustExit			= 0;
	while(true)
	{
		if( m_Thread.CanThreadWork( 50 ))
		{
			if(   !m_bFileTransfered )
			{
				m_NetServer.LockIncomeBuffer();
				NetIncomeBuffer_t& MsgBuf = m_NetServer.GetIncomeBuffer();
				NetIncomeBuffer_t::const_iterator itMsg = MsgBuf.begin();
				for( ; itMsg != MsgBuf.end(); ++itMsg )
				{
					Proto.PumpMessage( itMsg->first, itMsg->second ); 
				}
				MsgBuf.clear();
				m_NetServer.UnlockIncomeBuffer();
			}
		}
		else if ( !m_bFileTransfered )
		{
			SendClientCancel( ServerMap );
			break;
		}
		else if( m_bMustExit ) break;
		else break;
	}
	return 0;
}

void	FileTransferImpl::OnErrorMessage(netadr_t NetAddr, const void* Msg, size_t nSize  )
{
	m_Debug.PrintW( IDebugOutput::mt_error, L"Wrong message from %hs", NET_AdrToStringA(NetAddr).c_str());
	m_NetServer.CloseClient( NetAddr );
}

void	FileTransferImpl::OnFileResponse( netadr_t NetAddr, int nCameraID, DWORD64 nFileSize, const BYTE* pData, size_t nSize )
{
//	AutoLockCriticalSection acs( m_Sync );
	m_Sync.Lock();
	// push in set
	m_ClientAddr = NetAddr;
	std::map<int, CameraState>::iterator it = m_CameraState.find( nCameraID );
	if( it == m_CameraState.end() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[OnFileResponse]No such camera=%d from %hs", nCameraID, NET_AdrToStringA(NetAddr).c_str());
		return ;
	}

	std::map<int, boost::shared_ptr<FileArchiveWriter> >::iterator itFile =  m_CameraFile.find( nCameraID );
	if( itFile == m_CameraFile.end() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[OnFileResponse]No such camera=%d in file array", nCameraID);
		return ;
	}
	FileArchiveWriter& fw = *itFile->second.get();

	try
	{
		fw.Write(pData, nSize);
	}
	catch( FileArchiveWriterException )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[OnFileResponse]<cam=%d> write to file failed", nCameraID);
		return ;
	}

	CameraState& cs = it->second;
	cs.m_nState			= st_Working;	
	cs.m_dwTotalFileSize = nFileSize;
	cs.m_dwCurrentOffset += nSize;
	WCHAR szBufTotal[20], szBufCur[20], szBufOff[20];
	StrFormatByteSizeW(  nSize , szBufCur, 20 );
	StrFormatByteSizeW(  nFileSize , szBufTotal, 20 );
	StrFormatByteSizeW(  cs.m_dwCurrentOffset , szBufOff, 20 );
	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"File portion for cam=%d arrived =%ws/%ws, offset=%ws", nCameraID, szBufCur, szBufTotal, szBufOff);

	m_Sync.Unlock();

	if( cs.m_dwTotalFileSize < cs.m_dwCurrentOffset  )
	{
		ASSERT(FALSE);
	}
#if 0
	if( cs.m_dwTotalFileSize == cs.m_dwCurrentOffset )
	{
		OnFileResponseEnd( NetAddr, nCameraID, 0, 0 );
	}
#endif
}

void	FileTransferImpl::OnFileResponseEnd( netadr_t NetAddr, int nCameraID, UINT32 nResult, DWORD32 nCRC )
{
	AutoLockCriticalSection acs( m_Sync );
	m_NetServer.CloseClient(m_ClientAddr);

	if( nResult == NetProto::FileResponseEndHdr::Res_ServerBusy )
	{
		CancelAll( ErrorServerBusy );
		return;
	}

	std::map<int, CameraState>::iterator it = m_CameraState.find( nCameraID );
	if( it == m_CameraState.end() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[OnFileResponseEnd]No such camera=%d from %hs", nCameraID, NET_AdrToStringA(NetAddr).c_str());
		return ;
	}
	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"End of file received w/ res=%I32u", nResult );
	it->second.m_nState			= st_Finished;
	switch(nResult) 
	{
	case NetProto::FileResponseEndHdr::Res_Ok:
			it->second.m_nReturnCode = Ok;
			break;
	case NetProto::FileResponseEndHdr::Res_Cancelled:
			it->second.m_nReturnCode = ErrorTransfer;
			CancelAll();
			break;
	case NetProto::FileResponseEndHdr::Res_NoSuchCamera:
	case NetProto::FileResponseEndHdr::Res_WrongCamera:
			it->second.m_nReturnCode = ErrorRequest;
			CancelAll();
			break;	
	case NetProto::FileResponseEndHdr::Res_SourceError:
			it->second.m_nReturnCode = ErrorSourceError;
			CancelAll();
			break;	
	default:
			it->second.m_nReturnCode = ErrorTransfer;
			CancelAll();
	}
	m_CameraFile.erase( nCameraID );
	InterlockedExchange( &m_bFileTransfered , IsAllFileTransfered() ? TRUE : FALSE );
}

bool	FileTransferImpl::IsAllFileTransfered() const
{
	std::map<int, CameraState>::const_iterator it = m_CameraState.begin( );
	for( ; it != m_CameraState.end(); ++it )
	{
		if ( it->second.m_nState != st_Finished ) return false;
	}
	return true;
}

unsigned	FileTransferImpl::SendFileRequest( const ServerMap_t& ServerMap )
{
	ServerMap_t::const_iterator it = ServerMap.begin();

	const std::vector<int>&	Cams = it->second;
	netadr_t NetAddrServer;
	NET_StringToAdr( _T("eugene:13420"), &NetAddrServer );
	NetFileRequest Request( m_nBeginTime, m_nEndTime, Cams);
	NetProto::GenericHdr * pHdr = (NetProto::GenericHdr *) &Request.m_Message[0];
	// FIXME
	pHdr->nReserved = c_FileTransferID;
	try
	{
		TCP::NetSendClient	cl( NetAddrServer, &Request.m_Message[0], Request.m_Message.size() ) ;
	}
	catch(NetException& ex)
	{
		AutoLockCriticalSection acs( m_Sync );
		m_Debug.PrintW(IDebugOutput::mt_error, L"File request failed server=%hs, error=%hs", 
						NET_AdrToStringA(NetAddrServer).c_str(), ex.what() );
		// FIXME
		m_TotalState = std::auto_ptr<IFileTransfer::IState>
			( 
			new CriticalErrorRequest( it->first  ) 
			) ;
		return 1;
	}
	return 0;
}

void	FileTransferImpl::DumpFileRequest( const ServerMap_t& ServerMap )
{
	WCHAR szBuf[20];
	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"Requested cameras:" );
	ServerMap_t::const_iterator it = ServerMap.begin();
	for( ; it != ServerMap.end(); ++it )
	{
		std::wstring s;
		const std::vector<int>& Cameras = it->second;
		for( size_t i = 0; i < Cameras.size(); ++i )
		{
			StringCchPrintfW( szBuf, 20, L" %d", Cameras[i] );
			s += szBuf;
		}
		m_Debug.PrintW( IDebugOutput::mt_debug_info, L"%s :%s", it->first.c_str(), s.c_str() );
	}
}

void	FileTransferImpl::SendClientCancel( const ServerMap_t& ServerMap )
{
	NetSendFileClientCancel Packet;
	netadr_t NetAddr;
#if 0
	ServerMap_t::const_iterator it = ServerMap.begin();
	for( ; it != ServerMap.end(); ++it )
	{
		const std::wstring& ServerName = it->first;
		NET_StringToAdr( ServerName.c_str(), c_ServerPort, &NetAddr );
		m_Debug.PrintW( IDebugOutput::mt_debug_info, L"Send cancel to %hs", NET_AdrToStringA(NetAddr).c_str() );
		TCP::NetSendClient cl( NetAddr, &Packet.m_Message[0], Packet.m_Message.size() );
	}
#else
	try
	{
		NET_StringToAdr( _T("eugene"), NetSettings::c_ServerPort, &NetAddr );
		TCP::NetSendClient cl( NetAddr, &Packet.m_Message[0], Packet.m_Message.size() );
	}
	catch( NetException )
	{
	}
#endif

	CancelAll();
}

void FileTransferImpl::CancelAll()
{
	std::map<int, boost::shared_ptr<FileArchiveWriter> >::iterator 
		itFile =  m_CameraFile.begin( );
	for(; itFile != m_CameraFile.end(); ++itFile )
	{
		itFile->second->Cancel();
	}
	InterlockedExchange( &m_bMustExit, 1 );
}

void		FileTransferImpl::CancelAll(int nResCode)
{
	std::map<int, CameraState>::iterator it = m_CameraState.begin( );
	for( ; it != m_CameraState.end(); ++it )
	{
		it->second.m_nState			= st_Finished;
		it->second.m_nReturnCode = nResCode;
	}
	CancelAll();
}

EXPOSE_INTERFACE( FileTransferImpl, IFileTransfer, FILE_TRANSFER_INTERFACE )