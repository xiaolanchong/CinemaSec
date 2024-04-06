#include "StdAfx.h"
#include "NetTcplink.h"
#include "NetUtil.h"

#if defined( _WIN32 )

//#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>

typedef int socklen_t;

#else

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define WSAEWOULDBLOCK		EWOULDBLOCK
#define WSAEMSGSIZE			EMSGSIZE
#define WSAEADDRNOTAVAIL	EADDRNOTAVAIL
#define WSAEAFNOSUPPORT		EAFNOSUPPORT
#define WSAECONNRESET		ECONNRESET
#define WSAECONNREFUSED     ECONNREFUSED
#define WSAEADDRINUSE EADDRINUSE

#define ioctlsocket ioctl
#define closesocket close
#ifndef INVALID_SOCKET
#define	INVALID_SOCKET (~0)
#endif

#undef SOCKET
typedef int SOCKET;
#define FAR
#endif

namespace TCP
{

// FIXME
const bool net_sleepforever = false;

int SockLastError()
{
#if defined( _WIN32 )
	return WSAGetLastError();
#else
	return errno;
#endif
}

NetSocket::StreamData* NetSocket::FindData( const netadr_t& NetAddress)
{
	StreamDataList_t::iterator it = m_StreamList.begin();
	for( ; it != m_StreamList.end(); ++it )
	{
		if( NET_CompareAdr( it->NetAddress, NetAddress) ) return &(*it);
	}
	return NULL;
}

void		NetSocket::AddData( const netadr_t& NetAddress, const void* pBytes, size_t nSize )
{
	StreamData* pData = FindData(NetAddress);
	if( !pData )
	{
		StreamData sd;
		sd.NetAddress	= NetAddress;
		sd.TotalBytes	= 0;
		sd.TotalValid	= false;
		m_StreamList.push_back( sd );
		pData = &m_StreamList.back();
	}
	std::vector<BYTE> & Arr = pData->Data;
	while( nSize )
	{
	
	if( pData->TotalValid )
	{
		int nDiff = pData->Data.size() + nSize -  pData->TotalBytes;
		if( nDiff == 0)
		{
			Sys_Debug( "<-- + %d bytes\n", nSize );
			pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, 
				(const BYTE*)pBytes + nSize );	
			Sys_Debug( "<-- Save message %Id bytes\n", Arr.size() );
			m_IncomeBuffer.push_back( std::make_pair( NetAddress, pData->Data ) );
			pData->Data.clear();
			pData->TotalValid = false;
			break;
			
		}
		else if( nDiff < 0 )
		{
			Sys_Debug( "<-- + %d bytes\n", nSize );
			pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, 
				(const BYTE*)pBytes + nSize);
			break;
		}
		else if( nDiff > 0 )
		{
			pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, 
				(const BYTE*)pBytes + nSize - nDiff );
			Sys_Debug( "<-- Save message %Id bytes\n", Arr.size() );
			m_IncomeBuffer.push_back( std::make_pair( NetAddress, pData->Data ) );
			Arr.clear();
			pBytes = (const BYTE*)pBytes + nSize - nDiff;
			nSize = nDiff;
			pData->TotalValid = false;
			continue;
		}
	}
	else
	{
		int ForTotal = nSize + pData->Data.size();
		if( ForTotal >= sizeof(int) )
		{
			int OldSize = Arr.size() ;
			Arr.insert( pData->Data.end(), 
								(const BYTE*)pBytes, (const BYTE*)pBytes + sizeof(int) - Arr.size() );
			pData->TotalBytes = *(int*)&Arr[0];
			Sys_Debug( "<-- New total %d bytes\n", pData->TotalBytes  );
			Arr.clear();
			nSize -= sizeof(int) - Arr.size();
			pBytes = (const BYTE*)pBytes + sizeof(int) - Arr.size();
			pData->TotalValid = true;
			continue;
		}
		else
		{
			Sys_Debug( "<-- + total %Id bytes\n", nSize  );
			pData->Data.insert( pData->Data.end(), (const BYTE*)pBytes, (const BYTE*)pBytes + nSize);
			break;
		}
	}

	}
}

void NetSocket::NET_ThreadLock( void )
{
	if ( use_thread && net_thread_initialized )
	{
		EnterCriticalSection( &net_cs );
	}
}

void NetSocket::NET_ThreadUnlock( void )
{
	if ( use_thread && net_thread_initialized )
	{
		LeaveCriticalSection( &net_cs );
	}
}

void NetSocket::NET_StartThread( void )
{
	if ( !use_thread )
		return;

	std::pair< LPTHREAD_START_ROUTINE, void*> p = GetThreadParams();

	if( !p.first ) return;
	if ( net_thread_initialized )
		return;

	net_thread_initialized = true;

	DWORD dwNetThreadId;
	hNetThread = CreateThread (NULL,  0, p.first, p.second, 0, &dwNetThreadId );
	if ( !hNetThread )
	{
		DeleteCriticalSection( &net_cs );
		net_thread_initialized = false;
		use_thread = 0;
		Sys_Error( "Couldn't initialize network thread, run without -net_thread" );
		return;
	}
}

void NetSocket::NET_StopThread( void )
{
	if ( !use_thread )
		return;

	if ( !net_thread_initialized )
		return;

	SetEvent( m_hSyncEvent );
	DWORD res = WaitForSingleObject( hNetThread, 1000 );
	if( res != WAIT_OBJECT_0 )
	{
		TerminateThread( hNetThread, -1 );
		Sys_Debug( "Thread was terminated\n" );
	}
	else
	{
		Sys_Debug( "Thread exits normally\n" );
	}
	CloseHandle( hNetThread );
	ResetEvent( m_hSyncEvent );

	net_thread_initialized = false;
}

DWORD WINAPI  NetServer::NET_ThreadFunc( LPVOID pv )
{
	NetServer* pThis = (NetServer*)pv;

	bool done = false;
	bool queued;
	int sockets_ready;

	while ( WaitForSingleObject( pThis->m_hSyncEvent, 1 ) == WAIT_TIMEOUT ) 
	{
		// Wait for messages
		struct sockaddr address;
		int addrlen = sizeof( struct sockaddr );
		pThis->NET_ThreadLock();
		// 2005-07-13 bugfix for idle sockets
		pThis->CheckIdleSockets();
		SOCKET sock = accept( pThis->GetSocket(), &address, &addrlen );
		if( sock != INVALID_SOCKET )
		{
			netadr_t NetAddress;
			SockadrToNetadr( &address, &NetAddress );
			pThis->m_Clients.push_back( boost::make_tuple( NetAddress, sock, _time64(NULL) ) );
		}
		pThis->NET_ThreadUnlock();
		sockets_ready = pThis->NET_Sleep();
		// Service messages
		//
		done = false;
		while ( !done && sockets_ready )
		{
			done = true;
			
				// Link
				pThis->NET_ThreadLock();
				
				for( size_t i = 0; i < pThis->m_Clients.size(); ++i )
				{
					SOCKET sock = boost::get<1>(pThis->m_Clients[i]);
					queued = pThis->NET_QueuePacket( sock , boost::get<0>( pThis->m_Clients[i] )  );
					if ( queued )
					{
						boost::get<2>( pThis->m_Clients[i] ) = _time64(0);
						done = false;
					}
				}
				pThis->NET_ThreadUnlock();
				if( WaitForSingleObject( pThis->m_hSyncEvent,0 ) != WAIT_TIMEOUT ) return 0;
		}
//		Sleep( 1 );
	}

	return 0;
}

bool	NetSocket::NET_QueuePacket ( SOCKET sock, const netadr_t& From)
{
	int				ret;
	int				err;
	unsigned char	buf[ NET_MAX_MESSAGE ];

	if (sock != INVALID_SOCKET)
	{
		ret = recv(sock, (char *)buf, NET_MAX_MESSAGE, 0);
		if ( /*ret != -1 && */ret> 0)
		{
			AddData( From,  buf, ret );
			return true;
		}
		if( ret == 0 )
		{
			return false;
		}
		else
		{
			err = SockLastError();

			switch ( err )
			{
			case WSAEWOULDBLOCK:
			case WSAECONNRESET:
			case WSAECONNREFUSED:
			case WSAEMSGSIZE:
				break;
			default:
				// Let's continue even after errors
				Sys_Debug( "NET_QueuePacket: %s\n", NET_ErrorString(err));
				break;
			}
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : sock - 
//			length - 
//			*data - 
//			to - 
// Output : void NET_SendPacket
//-----------------------------------------------------------------------------
void NetSocket::NET_SendPacket ( SOCKET sock, int length, void *data)
{
	int		ret;
	const char* pMarker = (const char*)&length;
	send( sock, pMarker, sizeof(int) , 0 );
	ret = send(  sock, (const char *)data, length, 0 );
	if (ret == -1)
	{
		int err = SockLastError();
		// wouldblock is silent
		if (err == WSAEWOULDBLOCK)
			return;

		if ( err == WSAECONNRESET )
			return;

		// some PPP links dont allow broadcasts
/*		if ( (err == WSAEADDRNOTAVAIL) && ( to.type == NA_BROADCAST ) )
			return;*/

			{
				Sys_Error ( "NET_SendPacket ERROR: %s\n", NET_ErrorString(err));
			}
	}
}

/*
====================
NET_IPSocket
====================
*/
SOCKET NetSocket::NET_IPSocket ( const TCHAR *net_interface, int& port)
{
	SOCKET				newsocket;
	struct sockaddr_in	address;
	unsigned long		_true = 1;
	int					i = 1;
	int					err;

	if ((newsocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		err = SockLastError();
		if (err != WSAEAFNOSUPPORT)
			Sys_Error ("WARNING: TCP_OpenSocket: socket: %s", NET_ErrorString(err));
		return 0;
	}

	// make it non-blocking
	if (ioctlsocket (newsocket, FIONBIO, &_true) == -1)
	{
		err = SockLastError();
		Sys_Error ("WARNING: TCP_OpenSocket: ioctl FIONBIO: %s\n", NET_ErrorString(err));
		return 0;
	}
#if 0
	// make it broadcast capable
	if (setsockopt(newsocket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) == -1)
	{
		err = SockLastError();
		Sys_Error ("WARNING: TCP_OpenSocket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString(err));
		return 0;
	}
#endif

	// make it reusable
	if ( /*CommandLine()->FindParm( "-reuse" )*/0 )
	{
		if (setsockopt(newsocket, SOL_SOCKET, SO_REUSEADDR, (char *)&_true, sizeof(DWORD)) == -1)
		{
			err = SockLastError();
			Sys_Error ("WARNING: TCP_OpenSocket: setsockopt SO_REUSEADDR: %s\n", NET_ErrorString(err));
			return 0;
		}
	}

	if (!net_interface || !net_interface[0] || !_tcsicmp(net_interface, _T("localhost")))
		address.sin_addr.s_addr = INADDR_ANY;
	else
		NET_StringToSockaddr (net_interface, (struct sockaddr *)&address);

	address.sin_family = AF_INET;

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
				Sys_Error( "TCP_OpenSocket bound to non-default port %i because original port was already in use.\n", port + p );
				port = port + p;
			}
		/*	else
			{
				struct sockaddr_in	bindto;
				socklen_t		bindtolen = sizeof( struct sockaddr );
				if ( getsockname ( newsocket, (struct sockaddr*)&bindto, &bindtolen) != 0)
				{
					int net_error = SockLastError();
					Sys_Error("Could not get TCP/IP address\nReason:  %s\n", NET_ErrorString(net_error));
				}
				else
				{
					port = ntohs( bindto.sin_port );
				}
			}*/
			break;
		}

		err = SockLastError();
		if ( port == PORT_ANY || err != WSAEADDRINUSE )
		{
			Sys_Error ("WARNING: TCP_OpenSocket: bind: %s\n", NET_ErrorString(err));
			closesocket (newsocket);
			return 0;
		}
		// Try next port
	}
	if ( !success )
	{
		Sys_Error( "WARNING: TCP_OpenSocket: unable to bind socket\n" );
		closesocket( newsocket );
		return 0;
	}
	return newsocket;
}

/*
================
NET_GetLocalAddress

Returns the servers' ip address as a string.
================
*/
void NetSocket::NET_GetLocalAddress (bool localhost)
{
	TCHAR	buff[512];
	struct sockaddr_in	address;
	int		namelen;
	int     net_error = 0;
	memset( &net_local_adr, 0, sizeof(netadr_t));


	// If we have changed the ip var from the command line, use that instead.
	if (localhost)
	{
		_tcscpy(buff, _T("localhost"));
	}
	else
	{
#if defined(UNICODE) || defined(UNICODE)
		char tmp[512];
		gethostname(tmp, 512);
		mbstowcs( buff, tmp, 512 );
#else
		gethostname(buff, 512);
#endif
	}

		// Ensure that it doesn't overrun the buffer
		buff[512-1] = 0;

		NET_StringToAdr (buff, &net_local_adr);

		namelen = sizeof(address);
		if ( getsockname ( GetSocket(), (struct sockaddr *)&address, (socklen_t *)&namelen) != 0)
		{
			int err = SockLastError();
			Sys_Error("Could not get TCP/IP address, TCP/IP disabled\nReason:  %s\n", NET_ErrorString(net_error));
		}
		else
		{
			net_local_adr.port = address.sin_port;
			std::string ps = NET_AdrToStringA (net_local_adr);
			Sys_Debug("Socket IP address %s\n", ps.c_str() );
		}
}

void	NetSocket::NET_Config (bool multiplayer)
{
//	int		i;


	if (!multiplayer)
	{	// shut down any existing sockets
		NET_ThreadLock();


			if (GetSocket() != INVALID_SOCKET)
			{
				closesocket (GetSocket());
				SetSocket( INVALID_SOCKET);
			}
	

		NET_ThreadUnlock();
	}
	else
	{	// open sockets

		NET_OpenIP ();

		// Get our local address, if possible
		if ( m_bFirst )
		{
			m_bFirst = false;
			NET_GetLocalAddress (false);
		}
	}

	net_configured = multiplayer ? 1 : 0;
}



LPCTSTR GetStubIP()
{
	static TCHAR buff[512];
#if defined(UNICODE) || defined(UNICODE)
	char tmp[512];
	gethostname(tmp, 512);
	mbstowcs( buff, tmp, 512 );
#else
	gethostname(buff, 512);
#endif
	return buff;
}

/*
====================
NET_OpenIP
====================
*/
void NetSocket::NET_OpenIP (void)
{
	int		port;
	int		dedicated = false;
//	static bool bFirst = true;

	NET_ThreadLock();

	if (GetSocket() == INVALID_SOCKET )
	{
		port = m_nPort;
		SOCKET s = NET_IPSocket ( GetStubIP() , port);
		SetSocket(  s ); 
		if (s == INVALID_SOCKET)
		{
			port = PORT_ANY;
			SOCKET s= NET_IPSocket (GetStubIP() , port);
			SetSocket( s); 
		}
		m_nPort = port;
	}

	NET_ThreadUnlock();
}

char const *NET_GetHostName()
{
	static char buff[ 512 ];
	gethostname(buff, 512);
	return buff;
}
/*
#define DEFINE_HEADER(a,b,c,d)		(( d <<24)+( c <<16)+( b <<8)+ a)
#define PROTO_MAGIC					( 'c', 's', 'n', 'p' )
#define PROTO_VERSION				1

void	NetSocket::Header_Fill(SPLITPACKET& sp)
{

	const __int32 cur_proto_version = PROTO_VERSION;
	sp.protoVersion = cur_proto_version;
	sp.magicID		= PROTO_MAGIC;
}

bool	NetSocket::Header_Check(const SPLITPACKET& sp  ) const
{
	return sp.magicID == PROTO_MAGIC && sp.protoVersion <= PROTO_VERSION;
}*/

NetSocket::NetSocket(void) : 
	m_gSequenceNumber (1), 
	m_hSyncEvent( CreateEvent( 0, TRUE, FALSE, 0 ) ),
	m_bFirst(true)
{
	// for timer init
	Plat_FloatTime();
	InitializeCriticalSection( &net_cs );
	use_thread				= true;
	net_thread_initialized	= false;
	SetSocket( INVALID_SOCKET);
	SocketInit();
}

NetSocket::~NetSocket(void)
{
//	Stop();
	SocketExit();
	DeleteCriticalSection( &net_cs );
	CloseHandle( m_hSyncEvent );
}


void	NetSocket::LockIncomeBuffer()
{
	NET_ThreadLock();
}

void	NetSocket::UnlockIncomeBuffer()
{
	NET_ThreadUnlock();
}

int	NetSocket::NET_WaitForSend( SOCKET sock, DWORD timeout)
{
	fd_set	fdset;
	struct timeval tv;
	int number;

	FD_ZERO(&fdset);
	FD_SET( sock, &fdset );

	tv.tv_sec = 0;
	tv.tv_usec = timeout * 1000;

	// Block infinitely until a message is in the queue
	number = select( 1, NULL, &fdset, NULL, /*net_sleepforever */ FALSE ? NULL : &tv );
	return number;
}

///////////////////

int		NetServer::FindClient( const netadr_t& NetAddr )
{
	for(size_t i = 0; i < m_Clients.size(); ++i)
	{
		if(  NET_CompareAdr( boost::get<0>(m_Clients[i]), NetAddr) ) return int(i);
	}
	return InvalidClient;
}

void	NetServer::SendMessage( netadr_t NetAddr, const void* pBytes, int nSize  )
{
	int cl = FindClient(NetAddr);
	if( cl != InvalidClient )
	{
		SOCKET sock = boost::get<1>(m_Clients[cl]);
		NET_SendPacket( sock, nSize, (void*)pBytes );
	}
	else 
		Sys_Error("TCP_SendMessage: no such client socket %s", NET_AdrToStringA( NetAddr ).c_str());
}

void	NetServer::CheckIdleSockets()
{
	if( m_nIdleTime <= 0 ) return;
	SocketArr_t::iterator it = m_Clients.begin();
	for(; it != m_Clients.end();)
	{
		INT64 nLastAccessTime =  boost::get<2>( *it );
		if( (_time64(NULL) - nLastAccessTime ) > m_nIdleTime )
		{
			closesocket( boost::get<1>(*it) );
			std::string ps = NET_AdrToStringA (boost::get<0>(*it));
			Sys_Debug("Idle socket timeout for %s.Socket will be closed explicitly", ps.c_str() );
			it = m_Clients.erase( it );
		}
		else
			 ++it;
	}
}

void	NetServer::Start( short nPort, int nIdleTime, int nSocketConnection )
{
	m_nPort		= nPort;
	m_nIdleTime	= nIdleTime;
	NET_Config(true);
	int err = listen ( GetSocket(), nSocketConnection < 0 ? SOMAXCONN : nSocketConnection);
	if(err == SOCKET_ERROR)
	{
		int net_error = SockLastError();
		Sys_Error("Could not listen, reason:  %s\n", NET_ErrorString(net_error) );
	}
	NET_StartThread();
}

void	NetServer::Stop()
{
	NET_StopThread();
	NET_Config(false);
}

void	NetServer::CloseClient( const netadr_t& NetAddr )
{
	NET_ThreadLock();
	int cl = FindClient(NetAddr);
	if( cl != InvalidClient )
	{
		closesocket( boost::get<1>(m_Clients[cl]) );
		m_Clients.erase( m_Clients.begin() + cl );
	}
	NET_ThreadUnlock();
}

// sleeps until one of the net sockets has a message.  Lowers thread CPU usage immensely
int NetServer::NET_Sleep(  )
{
	fd_set	fdset;
	int i;
	struct timeval tv;
	int number;

//	NET_ThreadLock();
	FD_ZERO(&fdset);
	i = 0;
	int cl = m_Clients.size();
	for( i = 0; i < (int)m_Clients.size(); ++i)
	{
		SOCKET sock = boost::get<1>(m_Clients[i]);
		FD_SET( sock, &fdset );
		if ( int( sock) > i )
			i = (int)sock;
	}
//	NET_ThreadUnlock();
	tv.tv_sec = 0;
	tv.tv_usec = 20 * 1000;

	// Block infinitely until a message is in the queue
	number = select( i+1, &fdset, NULL, NULL, /*net_sleepforever ? NULL :*/ &tv );
	return number;
}


NetServer::~NetServer()
{
	Stop();
}

void	NetClient::Start( short nPort, netadr_t ServerAddress )
{
	m_nPort			= nPort;
	m_ServerAddress	= ServerAddress;	
	NET_Config(true);
	struct sockaddr sa;
	NetadrToSockadr( &ServerAddress, &sa );
	int err = connect( GetSocket(), &sa, sizeof( struct sockaddr ) );
	if(err == SOCKET_ERROR)
	{
		int net_error = SockLastError();
		if( net_error != WSAEWOULDBLOCK )
			Sys_Error("Could not connect socket, reason:  %s\n", NET_ErrorString(net_error) );
	}
}

void	NetClient::Stop()
{
	NET_Config(false);
}

void	NetClient::SendMessage( const void* pBytes, int nSize  )
{
	NET_SendPacket(  GetSocket(), nSize, (void*)pBytes );
}


// sleeps until one of the net sockets has a message.  Lowers thread CPU usage immensely
int NetClient::NET_Sleep(  )
{
	fd_set	fdset;
	int i;
//	int sock;
	struct timeval tv;
	int number;

	FD_ZERO(&fdset);
	i = 0;
	
	if ( GetSocket() != INVALID_SOCKET )
	{
	FD_SET( GetSocket() , &fdset); // network socket
	if ( int( GetSocket() ) > i )
	i = (int)GetSocket();
	}

	tv.tv_sec = 0;
	tv.tv_usec = 20 * 1000;

	// Block infinitely until a message is in the queue
	number = select( i+1, &fdset, NULL, NULL, /*net_sleepforever ? NULL :*/ &tv );
	return number;
}

NetClient::~NetClient()
{
	Stop();
}

} // namespace TCP