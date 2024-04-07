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
#ifndef _FILE_TRANSFER_IMPL_2377945520953125_
#define _FILE_TRANSFER_IMPL_2377945520953125_

/**  implemented interface */
#include "IFileTransfer.h"
/**  net server & net stuff */
#include "../IPC/NetTcpLink.h"
/**  debug interface */
#include "../../CSChair/debug_int.h"
/**  general thread stuff */
#include "..\mainserver\ThreadError.h"

#include "..\..\CSChair\dmpstuff\ExceptionHandler.h"
#include "../mainserver/NetProto.h"

#include "FileArchiveWriter.h"

//======================================================================================//
//                                class FileTransferImpl                                //
//======================================================================================//

/*!
	\version	1.1
	\date		2005-08-01
*/

class FileTransferImpl :	public IFileTransfer,
							private INetProtoCallback,
							private INetFileCallback
{
public:
	FileTransferImpl();
	virtual ~FileTransferImpl();
private:

	virtual void	Release()	{ delete this; };
	virtual HRESULT	Start( IDebugOutput* pDbgInt, std::vector<CamRequest_t>&	CamRequest, INT64 nBeginTime, INT64 nEndTime );
	virtual	std::auto_ptr<IFileTransfer::IState>	GetState( int nCameraID );  
	virtual	std::auto_ptr<IFileTransfer::IState>	GetTotalState(  )	; 
	virtual	HRESULT	Cancel();
	virtual bool	IsBusy();


	/*! debug wrap for IDebugOutput */
	MyDebugOutputImpl		m_Debug;
	/*! synchronize stuff for the working thread */
	ThreadSync				m_Thread;

	//! requested cameras
	std::vector<CamRequest_t>	m_RequestedCam;
	//! begin time
	INT64						m_nBeginTime;
	//! end time
	INT64						m_nEndTime;

	
	/*! the working thread function */
	unsigned ThreadProc();

	virtual void OnRequestData( netadr_t /*NetAddr*/, const std::vector<int>& /*CameraIDs*/ ){};
	virtual void OnResponseData( netadr_t /*NetAddr*/, const std::vector< ResponseDataStruct >& /*Data*/ ){};
	virtual void OnStartWork( netadr_t /*NetAddr*/, const GUID& /*SessionID*/, __int64 /*nStartSec*/, __int64 /*nEndSec*/ ) {};
	virtual void OnStopWork( netadr_t /*NetAddr*/ )	{};
	virtual void OnCurrentState(netadr_t /*NetAddr*/, int /*nRoomID*/,  const GUID& /*SessionID*/, 
								__int64 /*nBeginSec*/, __int64 /*nEndSec*/,
								const std::vector< CurrentStateType >& /*Devices*/) {};
	// for UDP datagrams ( ping, start, state )
	virtual void SendMessage( netadr_t /*NetAddr*/, const void* /*Msg*/, size_t /*nSize*/ ) {};
	// for TCP packets ( data, requests )
	virtual void SendReliableMessage( netadr_t /*NetAddr*/, const void* /*Msg*/, size_t /*nSize*/ ) {};

	virtual void OnHallData( netadr_t NetAddr, const std::vector<BYTE>& Data  ){}

//	virtual void OnExchangeReconnect(netadr_t NetAddr) {};
//	virtual void OnExchangeReport(netadr_t NetAddr) {};
	virtual void OnErrorMessage(netadr_t NetAddr, const void* Msg, size_t nSize  );

	virtual void	OnFileRequest( netadr_t NetAddr, const std::vector<int>& CameraIDs, INT64 nBeginTime, INT64 nEndTime) {};
	virtual void	OnFileResponse( netadr_t NetAddr, int nCameraID, DWORD64 nFileSize, const BYTE* pData, size_t nSize )	;
	virtual void	OnFileResponseEnd( netadr_t NetAddr, int nCameraID, UINT32 nResult, DWORD32 nCRC );

	TCP::NetServer	m_NetServer;
	netadr_t		m_ClientAddr;

	SyncCriticalSection		m_Sync;
	//! current thread state

	//! download progress
	enum
	{
		st_NotStarted,
		st_Working,
		st_Finished
	};
	struct CameraState
	{
		LONG			m_nState;
		DWORD64			m_dwTotalFileSize;
		DWORD64			m_dwCurrentOffset;
		int				m_nReturnCode;

		CameraState( ) :
			m_nState(st_NotStarted),
			m_nReturnCode( IFileTransfer::ErrorTransfer ),
			m_dwTotalFileSize(0),
			m_dwCurrentOffset(0)
		{}
	};

	std::auto_ptr<IFileTransfer::IState>	m_TotalState;

	std::map<int, CameraState>			m_CameraState;
	std::map<int, boost::shared_ptr<FileArchiveWriter> >	m_CameraFile;
	std::map<int, std::wstring>			m_CameraFileName;
	LONG								m_bFileTransfered;
	LONG								m_bMustExit;

	bool	IsAllFileTransfered() const;
	typedef std::map< std::wstring, std::vector<int> >	ServerMap_t;
	unsigned	SendFileRequest( const ServerMap_t& ServerMap ); 
	void		DumpFileRequest( const ServerMap_t& ServerMap ); 
	void		CancelAll();
	void		CancelAll(int nResCode);
	void		SendClientCancel( const ServerMap_t& ServerMap );
};

#endif // _FILE_TRANSFER_IMPL_2377945520953125_