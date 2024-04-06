//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	ISuperServer implementation, don't use threads
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 15.03.2005
//                                                                                      //
//======================================================================================//
#ifndef _SUPER_SERVER_IMPL_3837106031146859_
#define _SUPER_SERVER_IMPL_3837106031146859_
//======================================================================================//
//                                class SuperServerImpl                                 //
//======================================================================================//

#include "../ISuperServer.h"
#include "../main_server.h"
#include "ThreadError.h"
#include "../IPC/NetLink.h"
#include "../IPC/NetTcpLink.h"
#include "NetworkLayer.h"
#include "../../CSChair/Grabber/IGrabberFactory.h"
#include "BackupServer.h"
#include "FileTransferServer.h"
#include <boost/shared_ptr.hpp>
#include "NetProto.h"

typedef boost::shared_ptr< IMainServer >	MainServerPtr_t;

struct MainServerThread
{
//	int					m_nRoomID;
	MainServerPtr_t		m_pServer;
	NetIncomeBuffer_t	m_MessageBuffer;
	bool				m_UseReliable;
	bool				m_UseUnrealiable;

	MainServerThread(  const MainServerPtr_t& p ):
		m_UseReliable(false), m_UseUnrealiable(false),
		m_pServer(p)
		{}
};

class SuperServerImpl	:	public		ISuperServer,
							public		INetworkLayer,
							protected	BackupServer,
							private		INetProtoCallbackServer,
							private		INetFileCallback,
							private		FileTransferServer
{
public:
	SuperServerImpl();
	virtual ~SuperServerImpl();

private:

	virtual void	Release() { delete this;}

	virtual HRESULT	Initialize( IDebugOutput* pDbgInt, IDebugOutput* pGrabberDbgInt, bool bInitVideo );	

	virtual HRESULT StartServer		( int nRoomID, IDebugOutput *pInt, LPCWSTR szDirName );
	virtual HRESULT StopServer		( int nRoomID )	;					

	virtual HRESULT RestartServer	( int nRoomID)	{ return E_NOTIMPL; }

	virtual HRESULT	SetCameraWindow( int nRoomID, int nCameraNo, HWND hWnd ); 

	virtual void RegisterCallback( ICallbackServer * pInt);
	virtual void UnregisterCallback( );

	// Network
	virtual void				LockIncomeBuffer(int nRoomID)	;
	virtual void				UnlockIncomeBuffer(int nRoomID) ;
	virtual NetIncomeBuffer_t&	GetIncomeBuffer(int nRoomID)	;

//	virtual void				TCP_SendMessage(int nRoomID,  netadr_t NetAddr, const void* pBytes, size_t nSize  ) ;
	virtual void				TCP_CloseClient( int nRoomID, netadr_t NetAddr );
	virtual void				UDP_SendMessage(int nRoomID,  netadr_t NetAddr, const void* pBytes, size_t nSize  ) ;
	// for client 
	virtual void				TCP_SendFromClient( int nRoomID, netadr_t ServerAddress, const void* pData, size_t nSize ) ;

	virtual void OnImageAccStart( int nRoomID ) ;
	virtual void OnImageAccStop( bool bSaveImage );
	virtual void OnImageAccResponse( int nResult ) ;

	virtual void	OnFileRequest( netadr_t NetAddr, const std::vector<int>& CameraIDs, INT64 nBeginTime, INT64 nEndTime);
	virtual void	OnFileResponse( netadr_t NetAddr, int nCameraID, DWORD64 nFileSize, const BYTE* pData, size_t nSize )	;
	virtual void	OnFileResponseEnd( netadr_t NetAddr, int nCameraID, UINT32 nResult, DWORD32 nCRC );

	void			ParseNetworkMessage( netadr_t NetAddr, const std::vector<BYTE>& Msg);
	void			AddExchangeState( std::vector<BYTE>& Msg );

	void	PumpMessage( NetIncomeBuffer_t& MsgBuf, bool bErrorIfNotFound );
	void	SetRoomID( int nRoomID, const void* pBytes, size_t nSize);

	void	ReceiveExchangeReport();
	void	ReceiveExchangeReconnect();

	MyDebugOutputImpl							m_Debug;
	SyncCriticalSection							m_MainServerCS;

	LONG										m_bInitialized;
	
	typedef std::map<int, MainServerThread>		ServerMap_t;
	ServerMap_t									m_MainServerMap;	
	NetLink										m_UnreliableLink;
	TCP::NetServer								m_ReliableLink;
	std::set<int>								m_RefReliable;
	std::set<int>								m_RefUnreliable;

	boost::shared_ptr< IGrabberFactory >		m_pGrabberFactory;

	NetIncomeBuffer_t							m_EmptyStub;

	bool										m_bInMulticast;

	bool										m_bSkipMessage;
	ICallbackServer*							m_pCallbackInt;
};

#endif // _SUPER_SERVER_IMPL_3837106031146859_