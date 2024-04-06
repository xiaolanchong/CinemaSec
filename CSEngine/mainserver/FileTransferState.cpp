//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Интерфейс состояния сервера при передачи файла
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 01.08.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "FileTransferState.h"
#include "FileTransferServer.h"
#include "../IPC/NetTcpLink.h"
#include "../mainserver/NetProto.h"
#include <shlwapi.h>
#include <boost/crc.hpp>

/**
    Состояние подготовки данных
 */

class	FileTransferStatePrepare : public IFileTransferState
{
#if 0
	int		m_nOperation;
#endif
	virtual IFileTransferState*	Process();
	virtual void Cancel();
public:
	FileTransferStatePrepare(FileTransferServer* pTransfer, int nCameraID):
		IFileTransferState(pTransfer, nCameraID)
#if 0
		,m_nOperation(0)
#endif
		{}
};

/**
	Состояние передачи данных
*/

class	FileTransferStateSend : public IFileTransferState
{
	virtual IFileTransferState*	Process();
	virtual void Cancel();

	int					m_nAttemptNo;
	DWORD64				m_nOffset;	
	boost::crc_32_type	m_CRC;
public:
	FileTransferStateSend(FileTransferServer* pTransfer, int nCameraID):
	  IFileTransferState(pTransfer, nCameraID),
	  m_nAttemptNo(0),
	  m_nOffset(0)
	  {
		  m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_debug_info, L"[FileTransferServer]start send data for - %hs", NET_AdrToStringA(m_pTransfer->m_NetAddr).c_str() );
	  }
};

/**
	Состояние передачи конца сеанса
*/

class	FileTransferStateFinish : public IFileTransferState
{
	virtual IFileTransferState*	Process();
	virtual void Cancel();

	DWORD64				m_nCurBlock ;
//	std::vector<BYTE>	m_Msg;
	int					m_nResultCode;
	DWORD32				m_nCRC;

	int					m_nAttemptNo;
public:
	FileTransferStateFinish(FileTransferServer* pTransfer, int nCameraID, int nResultCode, DWORD32 nCRC):
	  IFileTransferState(pTransfer, nCameraID),
		  m_nResultCode(nResultCode),
		  m_nCRC(nCRC),
		  m_nAttemptNo(0)
	  {
	  }

	  static bool SendResultCode( TCP::NetClient& cl, int nCameraID, int nResult, DWORD32 nCRC, DWORD nTimeout );
};

/**
	Конечное состояние
*/

class	FileTransferStateEnd : public  IFileTransferState
{
	virtual FileTransferStateEnd*	Process() { return NULL;};
	virtual void Cancel() {}
public:
	FileTransferStateEnd(FileTransferServer* pTransfer, int nCameraID):
	  IFileTransferState(pTransfer, nCameraID){}
};

//======================================================================================//
//                               class IFileTransferState                               //
//======================================================================================//

IFileTransferState*	IFileTransferState::GetStartState(FileTransferServer* pTransfer, int nCameraID)
{
	return new FileTransferStatePrepare(pTransfer, nCameraID);
}

bool	IFileTransferState::IsEndState( IFileTransferState* pState )
{
	return dynamic_cast<FileTransferStateEnd*>(pState) != 0;
}

//======================================================================================//
//                               class FileTransferStatePrepare                         //
//======================================================================================//

IFileTransferState* FileTransferStatePrepare::Process()
{
	std::map<int, boost::shared_ptr<IArchiveProvider> >::iterator it = m_pTransfer->m_FileMap.find( m_nCameraID );
	if( it == m_pTransfer->m_FileMap.end() )
	{
		try
		{
			boost::shared_ptr< IArchiveProvider > p( 
				new 
#if 0
				FakeArchiveProvider
#else
				FileArchiveProvider
#endif
				( m_pTransfer->m_Debug.Get(), GetCameraID(), m_pTransfer->m_nBeginTime, m_pTransfer->m_nEndTime ) 
				);
			m_pTransfer->m_FileMap.insert( std::make_pair( GetCameraID(), p ) );
			m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_debug_info, L"[FileTransferServer]camera=%d, prepare file for - %hs", 
				GetCameraID(),
				NET_AdrToStringA(m_pTransfer->m_NetAddr).c_str() );
			return NULL;
		}
		catch( FileArchiveProviderException )
		{
			m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_error, L"[FileTransferServer]<cam=%d> source exception", 
				GetCameraID());
			return new FileTransferStateFinish(m_pTransfer, GetCameraID(), NetProto::FileResponseEndHdr::Res_SourceError, 0);
		}
	}
	else 
	{
		try
		{
			bool res = it->second->IsReady();
			return res ? new FileTransferStateSend( m_pTransfer, GetCameraID() ) : NULL;
		}
		catch ( FileArchiveProviderException ) 
		{
			m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_error, L"[FileTransferServer]<cam=%d> source exception", 
				GetCameraID());
			return new FileTransferStateFinish(m_pTransfer, GetCameraID(), NetProto::FileResponseEndHdr::Res_SourceError, 0);
		}
	}
}

void	FileTransferStatePrepare::Cancel()
{

}

//======================================================================================//
//                               class FileTransferStateSend                            //
//======================================================================================//

IFileTransferState* FileTransferStateSend::Process()
{
	const DWORD c_MaxAttemptNo	= 10;
	const DWORD	c_BlockSize		= 512*1024;

	std::map<int, boost::shared_ptr<IArchiveProvider> >::iterator it = 
		m_pTransfer->m_FileMap.find( m_nCameraID );
	ASSERT( m_pTransfer->m_FileMap.end() != it );

	DWORD64	nMsgSize = it->second->GetFileSize( );

	std::vector<BYTE>	Msg;
	bool bLastPiece = false;
	try
	{
		bLastPiece = it->second->Read( m_nOffset, Msg, 512* 1024 );
	}
	catch(FileArchiveProviderException)
	{
		m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_error, L"[FileTransferServer]<cam=%d> source exception while reading", GetCameraID());
		return new FileTransferStateFinish(m_pTransfer, GetCameraID(), NetProto::FileResponseEndHdr::Res_SourceError, 0);
	}
	int	dwSize = (int)Msg.size();

	bool bAllSent = false;
	try
	{
		WCHAR szBufSent[20], szBufAll[20], szBufOff[20];
		StrFormatByteSizeW( dwSize,		szBufSent, 20);
		StrFormatByteSizeW( m_nOffset,	szBufOff, 20);
		StrFormatByteSizeW( nMsgSize,	szBufAll, 20);
		BYTE* pData = &Msg[0];
		NetSendFilePortion FilePortion( GetCameraID(), nMsgSize, pData, dwSize );
		FilePortion.End();
		bool res = m_pTransfer->m_ClientSocket.WaitForSend( 1000 + 500 * m_nAttemptNo );
		if(res)
		{
			m_CRC.process_bytes( pData, Msg.size() );
			m_pTransfer->m_ClientSocket.SendMessage( &FilePortion.m_Message[0], (int)FilePortion.m_Message.size() );
			m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_debug_info, L"[FileTransferServer]<cam=%d> sent=%ws/off=%ws/total=%ws", 
				GetCameraID(), szBufSent, szBufOff, szBufAll );
			m_nAttemptNo = 0;
			m_nOffset += Msg.size();
			bAllSent = bLastPiece;
		}
		else
		{
			m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_warning, L"[FileTransferServer]<cam=%d> socket is not ready for write, attempt=%d",
				GetCameraID(), m_nAttemptNo );
			++m_nAttemptNo;
			bAllSent = false;
		}
		if( bAllSent ) 
		{
			InterlockedIncrement( &m_pTransfer->m_nSentCameras ); 
			return new FileTransferStateFinish(m_pTransfer, GetCameraID(), NetProto::FileResponseEndHdr::Res_Ok, m_CRC.checksum() );
		}
	}
	catch(NetException & ex)
	{
		m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_error, L"[FileTransferServer]<cam=%d> data sending failed, reason=%hs", GetCameraID(), ex.what() );
		return new FileTransferStateEnd(m_pTransfer, GetCameraID());
	}
	return NULL;
}

void	FileTransferStateSend::Cancel()
{
	try
	{
		bool res = FileTransferStateFinish::SendResultCode( 
			m_pTransfer->m_ClientSocket, GetCameraID(), NetProto::FileResponseEndHdr::Res_Cancelled, 0, 1000 
			);
		res ?	m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_debug_info, L"[FileTransferServer]<cam=%d> sent cancel", GetCameraID() ):
				m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_debug_info, L"[FileTransferServer]<cam=%d> socket transmitting timeout", GetCameraID() );
	}
	catch(NetException & ex)
	{
		m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_error, L"[FileTransferServer]cancel sending failed, reason=%hs", ex.what() );
	}
}

//======================================================================================//
//                               class FileTransferStateSend                            //
//======================================================================================//

bool FileTransferStateFinish::SendResultCode( TCP::NetClient& cl, int nCameraID, int nResult, DWORD32 nCRC, DWORD nTimeout )
{
	if( cl.WaitForSend( nTimeout ) )
	{
		NetSendFileEnd SendEnd( nCameraID, nResult, nCRC );
		cl.SendMessage( &SendEnd.m_Message[0], (int)SendEnd.m_Message.size() );
		return true;
	}
	else return false;
}

IFileTransferState*	FileTransferStateFinish::Process()
{
	try
	{

		NetSendFileEnd SendEnd( GetCameraID(), NetProto::FileResponseEndHdr::Res_Cancelled, 0 );
		bool res = FileTransferStateFinish::SendResultCode( 
			m_pTransfer->m_ClientSocket, GetCameraID(), m_nResultCode, m_nCRC, 1000 * (m_nAttemptNo + 1) 
			);
		if( !res )
		{
			++m_nAttemptNo;
			m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_warning, L"[FileTransferServer]<cam=%d> socket is not ready for write, attempt=%d",
				GetCameraID(), m_nAttemptNo );
			return ( m_nAttemptNo > 6 )? new FileTransferStateEnd( m_pTransfer, GetCameraID() ) : NULL ;
		}
		else
		{
			m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_debug_info, L"[FileTransferServer]<cam=%d> sent file end, res=%u desc=%ws, crc=%I32x", 
				GetCameraID(), m_nResultCode, FileTransferServer::ErrorDesc(m_nResultCode), m_nCRC );
		}
	}
	catch(NetException & ex)
	{
		m_pTransfer->m_Debug.PrintW(IDebugOutput::mt_error, L"[FileTransferServer]end data sending failed, reason=%hs", ex.what() );
	}
	return new FileTransferStateEnd( m_pTransfer, GetCameraID() );
}

void FileTransferStateFinish::Cancel()
{

}

