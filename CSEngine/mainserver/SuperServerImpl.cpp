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
#include "stdafx.h"
#include "SuperServerImpl.h"
#include "../../CSChair/interfaceEx.h"
#include "../CSEngine.h"
#include "NetProto.h"
#include "NetPort.h"
#include <shlwapi.h>

#include "../../CSChair/public/memleak.h"
//======================================================================================//
//                                class SuperServerImpl                                 //
//======================================================================================//
SuperServerImpl::SuperServerImpl() : 
	m_bInitialized(FALSE),
	m_pCallbackInt(NULL)
{
}

SuperServerImpl::~SuperServerImpl()
{
	AutoLockCriticalSection al( m_MainServerCS );
	BackupServer::Stop();
	FileTransferServer::Stop();

	ServerMap_t::iterator it = m_MainServerMap.begin( );
	for( ; it != m_MainServerMap.end(); ++it)
	{
		it->second.m_pServer->Stop(true);
		it->second.m_pServer.reset();
//		it = m_MainServerMap.erase(it);
	}
	m_MainServerMap.clear();

	try
	{
		m_ReliableLink.Stop();
		m_UnreliableLink.StopNetCommunication();
	}
	catch( NetException & )
	{

	}
}

static void DestroyMainServer(IMainServer * p)
{
	p->Release();
}

HRESULT SuperServerImpl::StartServer(	int nRoomID, IDebugOutput *pInt, LPCWSTR	szDirName )
{
//	AutoLockCriticalSection al(m_ExternalCS);
	AutoLockCriticalSection al(m_MainServerCS);
	if( !m_bInitialized ) 
	{
		return E_FAIL;
	}

	ServerMap_t::iterator it = m_MainServerMap.find( nRoomID );
	if( it != m_MainServerMap.end() )
	{
		return E_INVALIDARG;
	}

	HRESULT hr;
	DWORD res;

	IMainServer* pMainInt;
	res = CreateEngineInterface( MAIN_SERVER_INTERFACE_0, (void**)&pMainInt );
	if( res )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to create IMainServer interface for room=%d", nRoomID );
		return E_FAIL;
	}
	MainServerPtr_t tmp( pMainInt, ::DestroyMainServer ) ;
	pMainInt->SetDebugInterface( pInt );
	hr = pMainInt->Initialize( m_pGrabberFactory.get() , this );
	if( hr != S_OK )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to initialize server interface for room=%d", nRoomID );
		return hr;
	}

	m_MainServerMap.insert( std::make_pair(nRoomID, MainServerThread( tmp ) ) );
	return S_OK;
}

HRESULT SuperServerImpl::StopServer		( int nRoomID )	
{
//	AutoLockCriticalSection al1(m_ExternalCS);
	AutoLockCriticalSection al2(m_MainServerCS);
	ServerMap_t::iterator it = m_MainServerMap.find( nRoomID );
	if( it == m_MainServerMap.end() )
	{
		return E_INVALIDARG;
	}
	it->second.m_pServer->Stop(true);
	m_MainServerMap.erase(it);
	return S_OK;
}

//#define TEST_SOCKET
#undef TEST_SOCKET
#ifdef  TEST_SOCKET
void CreateIdleSocket()
{
	SOCKET newsocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	ASSERT( newsocket != INVALID_SOCKET );
	unsigned long		_true = 1;
	if (ioctlsocket (newsocket, FIONBIO, &_true) == -1)
	{
		ASSERT(FALSE);
	}

	sockaddr_in	address;
	LPCTSTR net_interface = L"eugene:0";
	LPCTSTR net_serv_interface = L"eugene:13420";
	NET_StringToSockaddr (net_interface, (struct sockaddr *)&address);
	address.sin_family = AF_INET;

	int port = PORT_ANY;
	int portmax = 10;
	bool success = false;
	int p;
	for ( p = 0; p < portmax; p++ )
	{
		if (port == PORT_ANY)
		{
			address.sin_port = 0;
			//address.sin_addr.s_addr = INADDR_ANY; 
		}
		else
		{
			address.sin_port = htons((short)( port + p ));
		}

		if ( bind (newsocket, (struct sockaddr *)&address, sizeof(address)) != -1 )
		{
			success = true;
			if ( port != PORT_ANY && p != 0 )
			{
				port = port + p;
			}
			break;
		}
	}

	sockaddr_in	srv_address;
	NET_StringToSockaddr (net_serv_interface, (struct sockaddr *)&srv_address);
	int err = connect( newsocket, (struct sockaddr *)&srv_address, sizeof(srv_address) );
	if(err == SOCKET_ERROR)
	{
		int net_error = WSAGetLastError();
		if( net_error != WSAEWOULDBLOCK ) ASSERT(FALSE);
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(  newsocket, &fd );
		while( ! select( 0, NULL, &fd, NULL, NULL ) )
		{
		}
		closesocket( newsocket );
	}
}

#endif

HRESULT	SuperServerImpl::Initialize( IDebugOutput* pDbgInt, IDebugOutput* pGrabberDbgInt, bool bInitVideo )
{
	//AutoLockCriticalSection al(m_ExternalCS);
	AutoLockCriticalSection al(m_MainServerCS);

	m_Debug.Set( pDbgInt );
	// FIXME to DB_CONFIG
	const WORD	BasePortNumber = 13420;
	unsigned long dwMultiCastGroup = inet_addr( "225.6.7.8");

	try
	{
		m_ReliableLink.Start( BasePortNumber, 1 * 60 );

#ifdef TEST_SOCKET		// socket test  !!!
		CreateIdleSocket();
#endif

	}
	catch( NetException& ex )
	{
		m_Debug.PrintA( IDebugOutput::mt_error, ex.what() );
		return E_FAIL;
	}

	try
	{
		m_UnreliableLink.StartNetCommunication( BasePortNumber, 0);
	}
	catch( NetException& ex )
	{
		m_Debug.PrintA( IDebugOutput::mt_error, ex.what() );
		m_ReliableLink.Stop();
		return E_FAIL;
	}
	m_bInMulticast = false;
	// at the computer w/o network failed
	try
	{
		m_UnreliableLink.AddToGroup( dwMultiCastGroup );
		m_bInMulticast = true;
	}
	catch( NetException& ex )
	{
		m_Debug.PrintA( IDebugOutput::mt_error, ex.what() );
	}

	if( bInitVideo )
	{
		IGrabberFactory* pFactory;
		BOOL bUseStub = SHRegGetBoolUSValue( RegSettings::c_szCinemaSecSubkey, _T("VideoStub"), TRUE, FALSE );
		DWORD dwRes = CreateInterface( 
			bUseStub? STUB_SERVER_GRABBER_FACTORY : SERVER_GRABBER_FACTORY,
			(void**)&pFactory );
		if( dwRes || !pFactory)
		{
			m_Debug.PrintW( IDebugOutput::mt_error, L"Failed create IGrabberFactory" );
			return E_FAIL;
		}
		boost::shared_ptr<IGrabberFactory> Tmp( pFactory, ReleaseInterface<IGrabberFactory>() );
		HRESULT hr;
		// bLocalVideo is nevermind
		hr = pFactory->Initialize( pGrabberDbgInt, false );
		if( hr != S_OK ) 
		{
			m_Debug.PrintW( IDebugOutput::mt_error, L"Failed initialize IGrabberFactory" );
			return E_FAIL;		
		}
		m_pGrabberFactory = Tmp;
		BackupServer::Start( pDbgInt );
	}
	InterlockedExchange( &m_bInitialized, TRUE );
	return S_OK;
}

void	SuperServerImpl::PumpMessage( NetIncomeBuffer_t& MsgBuf, bool bErrorIfNotFound )
{
	NetIncomeBuffer_t::iterator itMsg = MsgBuf.begin();
	for( ; itMsg != MsgBuf.end(); ++itMsg )
	{
		const std::vector<BYTE>& Msg = itMsg->second;
		size_t nSize = Msg.size();
		if( nSize < sizeof( NetProto::GenericHdr ) ) 
		{
			// wrong message 
			m_Debug.PrintW( IDebugOutput::mt_error, L"ISuperServer::PumpMessage - too short message received, %d bytes", (int)nSize  );
			continue;
		}
		else
		{
			NetProto::GenericHdr * pHdr = (NetProto::GenericHdr *) &Msg[0];
			int nRoomID = pHdr->nReserved;
			if( nRoomID == -99 )
			{
				ParseNetworkMessage( itMsg->first, Msg );
			}
			else if( pHdr->Command == NetProto::Hdr_ExchangeReport  )
			{
				ReceiveExchangeReport();
			}
			else if( pHdr->Command == NetProto::Hdr_ExchangeReconnect )
			{
				ReceiveExchangeReconnect();
			}
			else
			{
			
				ServerMap_t::iterator it = m_MainServerMap.find( nRoomID );
				if( it == m_MainServerMap.end())
				{
					// we can ignore incoming message from unknown roomid or send to upper level
					// now just skip it
					if( bErrorIfNotFound )
					m_Debug.PrintW( IDebugOutput::mt_error, L"ISuperServer::PumpMessage - no suitable server with RoomID=%d", nRoomID  );
				}
				else
				{
			//		size_t nSize = itMsg->second.size();
					it->second.m_MessageBuffer.push_back( *itMsg );
				}
			}
		}
	}
	MsgBuf.clear();
}

void	SuperServerImpl::LockIncomeBuffer(int nRoomID)	
{
	m_MainServerCS.Lock();
	
	m_ReliableLink.LockIncomeBuffer();
	NetIncomeBuffer_t& MsgBuf = m_ReliableLink.GetIncomeBuffer();
	PumpMessage( MsgBuf, true);
	m_ReliableLink.UnlockIncomeBuffer();

	m_UnreliableLink.LockIncomeBuffer();
	NetIncomeBuffer_t& MsgBufUDP = m_UnreliableLink.GetIncomeBuffer();
	// can be multicast, so ignore
	PumpMessage( MsgBufUDP, false );
	m_UnreliableLink.UnlockIncomeBuffer();	
}

void	SuperServerImpl::UnlockIncomeBuffer(int nRoomID) 
{	
	m_MainServerCS.Unlock();
}

NetIncomeBuffer_t&	SuperServerImpl::GetIncomeBuffer(int nRoomID)
{
	ServerMap_t::iterator it = m_MainServerMap.find( nRoomID );
	if( it == m_MainServerMap.end() )
	{
		// throw an exception
		m_Debug.PrintW( IDebugOutput::mt_debug_info, L"[SuperServerImpl::GetIncomeBuffer]wrong RoomID=%d", nRoomID );
		return m_EmptyStub;
	}
	else
	{
		return it->second.m_MessageBuffer;
	}
}

void		SuperServerImpl::TCP_CloseClient( int nRoomID, netadr_t NetAddr )
{
	m_ReliableLink.CloseClient( NetAddr );
}

void		SuperServerImpl::UDP_SendMessage(int nRoomID,  netadr_t NetAddr, const void* pBytes, size_t nSize  ) 
{
	SetRoomID( nRoomID, pBytes, nSize );
	std::vector<BYTE> NewMessage( (const BYTE*) pBytes, (const BYTE*) pBytes + nSize );
	
	const NetProto::GenericHdr* pHdr = (const NetProto::GenericHdr*)pBytes;
	if( nSize >= sizeof(NetProto::CurrentStateHdr) && 
		pHdr->Command == NetProto::Hdr_CurrentState		)
	{
		AddExchangeState( NewMessage );
	}

	m_UnreliableLink.SendMessageFromServer( NetAddr, &NewMessage[0], NewMessage.size() );
}

void		SuperServerImpl::TCP_SendFromClient( int nRoomID, netadr_t ServerAddress, const void* pData, size_t nSize )
{
	SetRoomID( nRoomID, pData, nSize );
	TCP::NetSendClient Nc( ServerAddress, pData, nSize );
}

void	SuperServerImpl::SetRoomID( int nRoomID, const void* pBytes, size_t nSize)
{
	if( nSize < sizeof( NetProto::GenericHdr ) ) throw NetException("Invalid message size") ;
	NetProto::GenericHdr* pHdr = (NetProto::GenericHdr*)pBytes;
	pHdr->nReserved = nRoomID;
}

HRESULT	SuperServerImpl::SetCameraWindow( int nRoomID, int nCameraNo, HWND hWnd )
{
	AutoLockCriticalSection al(m_MainServerCS);
	ServerMap_t::iterator it = m_MainServerMap.find( nRoomID );
	if( it == m_MainServerMap.end() )
	{
		return E_INVALIDARG;
	}
	else
	{
		MainServerThread& svt = it->second;
		if( !svt.m_pServer ) return E_FAIL;
		return svt.m_pServer->SetCameraWindow( nCameraNo, hWnd );  
	}
}

void SuperServerImpl::OnImageAccStart( int nRoomID )
{
	m_bSkipMessage = true;
}

void SuperServerImpl::OnImageAccStop( bool bSaveImage )
{
	m_bSkipMessage = true;
}

void SuperServerImpl::OnImageAccResponse( int nResult )
{
	m_bSkipMessage = true;
}

void	SuperServerImpl::OnFileRequest(netadr_t NetAddr, const std::vector<int>& CameraIDs, 
									   INT64 nBeginTime, INT64 nEndTime)
{
	m_ReliableLink.CloseClient( NetAddr );
	NetAddr.port = NET_HostToNetShort( NetSettings::c_FilePort );
	m_Debug.PrintW(IDebugOutput::mt_debug_info, L"File request from %hs", NET_AdrToStringA(NetAddr).c_str()  );
#if 0
	if( FileTransferServer::IsWorking() )
	{
#endif
		m_Debug.PrintW( IDebugOutput::mt_error, L"[OnFileRequest]Server is busy, try later" );
		SendServerBusy( NetAddr );
#if 0
	}
	else if( CameraIDs.empty() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[OnFileRequest]Empty request camera list" );
		SendWrongCamera( NetAddr );
	}
	else if( nBeginTime >= nEndTime )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[OnFileRequest]nBeginTime >= nEndTime" );
		SendWrongTime( NetAddr );
	}
	else 
		FileTransferServer::Start( m_Debug.Get(), NetAddr, CameraIDs, nBeginTime, nEndTime );
#endif
}

void	SuperServerImpl::OnFileResponse(	netadr_t NetAddr, int nCameraID, DWORD64 nFileSize, const BYTE* pData, size_t nSize)	
{
	m_Debug.PrintW(IDebugOutput::mt_error, L"OnFileResponse from %hs", NET_AdrToStringA(NetAddr).c_str()  );
}

void	SuperServerImpl::OnFileResponseEnd( netadr_t NetAddr, int nCameraID, UINT32 nResult, DWORD32 nCRC )
{
//	m_Debug.PrintW(IDebugOutput::mt_error, L"OnFileResponseEnd from %hs", NET_AdrToStringA(NetAddr).c_str()  );
	switch( nResult )
	{
	case NetProto::FileResponseEndHdr::Res_ClientCancel:
		m_Debug.PrintW(IDebugOutput::mt_debug_info, L"[OnFileResponseEnd]cancel from %hs", NET_AdrToStringA(NetAddr).c_str()  );
		FileTransferServer::Stop();
		break;
	}
}

void	SuperServerImpl::ParseNetworkMessage( netadr_t NetAddr, const std::vector<BYTE>& Msg )
{
	NetProto np( this, this) ;
	np.PumpMessage( NetAddr, Msg );
}

void	SuperServerImpl::AddExchangeState( std::vector<BYTE>& Msg )
{
	int nState = INetProtoCallback::CurrentStateType::StateExchange_Dead;
	NetProto::CurrentStateHdr::SubHdr cs = {	INetProtoCallback::CurrentStateType::SubTypeExchange, 
												-1, 
												INetProtoCallback::CurrentStateType::StateExchange_Dead };
	// no  interface - no info
	int nCbState;
	if( m_pCallbackInt  && m_pCallbackInt->GetState(nCbState) == S_OK )
	{
		switch( nCbState )
		{
		case ICallbackServer::StateOk:
			cs.State = INetProtoCallback::CurrentStateType::StateExchange_Working;
			break;
		case ICallbackServer::StateDisconnected:
			cs.State = INetProtoCallback::CurrentStateType::StateExchange_Disconnect;
			break;
		case ICallbackServer::StateNotCreated:
			break;
		}
	}
	Msg.insert( Msg.end(), (const BYTE*)&cs, (const BYTE*)&cs + sizeof(NetProto::CurrentStateHdr::SubHdr) );
}

void SuperServerImpl::RegisterCallback( ICallbackServer * pInt)
{
	InterlockedExchangePointer( (void**)&m_pCallbackInt, pInt );
}

void SuperServerImpl::UnregisterCallback( )
{
	InterlockedExchangePointer( (void**)&m_pCallbackInt, NULL );
}

void	SuperServerImpl::ReceiveExchangeReport()
{
	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"[SuperServerImpl]Received create report..." );
	if(m_pCallbackInt) m_pCallbackInt->Report();
}

void	SuperServerImpl::ReceiveExchangeReconnect()
{
	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"[SuperServerImpl]Received create reconnect..." );
	if(m_pCallbackInt) m_pCallbackInt->Reconnect();
}

EXPOSE_INTERFACE( SuperServerImpl, ISuperServer, SUPER_SERVER_INTERFACE )