#include "StdAfx.h"
#include ".\netlink.h"
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

#define	PORT_ANY	-1


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

void NetLink::NET_ThreadLock( void )
{
#if defined( _WIN32 )
	if ( use_thread && net_thread_initialized )
	{
		EnterCriticalSection( &net_cs );
	}
#endif
}

void NetLink::NET_ThreadUnlock( void )
{
#if defined( _WIN32 )
	if ( use_thread && net_thread_initialized )
	{
		LeaveCriticalSection( &net_cs );
	}
#endif
}

void NetLink::NET_StartThread( void )
{
	if ( !use_thread )
		return;

	if ( net_thread_initialized )
		return;

	net_thread_initialized = true;

#if defined( _WIN32 )


	DWORD dwNetThreadId;
	hNetThread = CreateThread (NULL,  0, (LPTHREAD_START_ROUTINE) NET_ThreadFunc, this, 0, &dwNetThreadId );
	if ( !hNetThread )
	{
		DeleteCriticalSection( &net_cs );
		net_thread_initialized = false;
		use_thread = 0;
		Sys_Error( "Couldn't initialize network thread, run without -net_thread" );
		return;
	}
#endif
}

void NetLink::NET_StopThread( void )
{
	if ( !use_thread )
		return;

	if ( !net_thread_initialized )
		return;
#if defined( _WIN32 )
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
#if 0
	TerminateThread( hNetThread, 0 );
#endif
#endif

	net_thread_initialized = false;
}



// sleeps until one of the net sockets has a message.  Lowers thread CPU usage immensely
int NetLink::NET_Sleep( void )
{
	fd_set	fdset;
	int i;
	int sock;
	struct timeval tv;
	int number;

	FD_ZERO(&fdset);
	i = 0;
	for ( sock = 0; sock < NUMBER_OF_SOCKETS ; sock++ )
	{
		if ( GetSocket(sock) != INVALID_SOCKET )
		{
			FD_SET( GetSocket(sock) , &fdset); // network socket
			if ( int( GetSocket(sock) ) > i )
				i = (int)GetSocket(sock);
		}
	}

	tv.tv_sec = 0;
	tv.tv_usec = 20 * 1000;

	// Block infinitely until a message is in the queue
	number = select( i+1, &fdset, NULL, NULL, net_sleepforever ? NULL : &tv );
	return number;
}

#if defined( _WIN32 )
DWORD WINAPI  NetLink::NET_ThreadFunc( LPVOID pv )
{
	NetLink* pThis = (NetLink*)pv;
	//Plat_RegisterThread("NET_ThreadFunc");

	bool done = false;
	bool queued;
	int i;
	int sockets_ready;

	while ( WaitForSingleObject( pThis->m_hSyncEvent, 20 ) == WAIT_TIMEOUT ) 
	{

		// Wait for messages
		sockets_ready = pThis->NET_Sleep();
		// Service messages
		//
		done = false;
		while ( !done && sockets_ready )
		{
			done = true;
			for ( i = 0; i < NUMBER_OF_SOCKETS; i++ )
			{
				// Link
				pThis->NET_ThreadLock();

				queued = pThis->NET_QueuePacket( i );
				if ( queued )
				{
					done = false;
				}

				pThis->NET_ThreadUnlock();
				if( WaitForSingleObject( pThis->m_hSyncEvent,0 ) != WAIT_TIMEOUT ) return 0;
			}
		}
	}

	return 0;
}
#endif



//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void NetLink::NET_DiscardStaleSplitpackets( void )
{
	int i;
	for ( i = int( g_SplitPacketEntries.size() ) - 1; i >= 0; i-- )
	{
		CSplitPacketEntry *entry = &g_SplitPacketEntries[ i ];
		Assert( entry );

		if ( Plat_FloatTime() < ( entry->lastactivetime + SPLIT_PACKET_STALE_TIME ) )
			continue;

		g_SplitPacketEntries.erase( g_SplitPacketEntries.begin() + i );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *from - 
// Output : CSplitPacketEntry
//-----------------------------------------------------------------------------
NetLink::CSplitPacketEntry *NetLink::NET_FindOrCreateSplitPacketEntry( netadr_t *from )
{
	size_t i, count = g_SplitPacketEntries.size();
	CSplitPacketEntry *entry = NULL;
	for ( i = 0; i < count; i++ )
	{
		entry = &g_SplitPacketEntries[ i ];
		Assert( entry );

		if ( NET_CompareAdr( entry->from, *from ) )
			break;
	}

	if ( i >= count )
	{
		CSplitPacketEntry newentry;
		newentry.from = *from;

		g_SplitPacketEntries.push_back( newentry );

		entry = &g_SplitPacketEntries[ g_SplitPacketEntries.size() - 1 ];
	}

	Assert( entry );
	return entry;
}

bool	NetLink::NET_QueuePacket (int sock)
{
	netadr_t		in_from;
	int				ret;
	struct sockaddr	from;
	int				fromlen;
	SOCKET			net_socket = INVALID_SOCKET;
	int				err;
	unsigned char	buf[ NET_MAX_MESSAGE ];

	net_socket = GetSocket(sock);
	if (net_socket != INVALID_SOCKET)
	{
		fromlen = sizeof(from);
		ret = recvfrom(net_socket, (char *)buf, NET_MAX_MESSAGE, 0, (struct sockaddr *)&from, (int *)&fromlen );
		if ( ret != -1 )
		{
			SockadrToNetadr( &from, &in_from );

			if( ret < sizeof(SPLITPACKET) )
			{
				Sys_Debug ( "NET_QueuePacket:  Too short packet from %s\n", NET_AdrToStringA (in_from).c_str() );
				return false;
			}
			else if( ret >= NET_MAX_MESSAGE )
			{
				Sys_Debug ( "NET_QueuePacket:  Oversize packet from %s\n", NET_AdrToStringA (in_from).c_str() );
				return false;
			}

			SPLITPACKET* pSplitPacket = (SPLITPACKET*)buf;
			if(! Header_Check( *pSplitPacket ) )
			{
				Sys_Debug ( "NET_QueuePacket:  Unknown packet from %s\n", NET_AdrToStringA (in_from).c_str() );
				return false;
			}

			//if (  )
			{
				// Check for split message
				
				if ( pSplitPacket->netID == -2 )
				{
					int size;
					bool res = NET_GetLong( &in_from, buf, ret, &size );
					if(res)
					{
						Sys_Debug ( "NET_QueuePacket:  Save packet from %s %d bytes\n", NET_AdrToStringA (in_from).c_str(), size );
						m_IncomeBuffer.push_back( std::make_pair( in_from, std::vector<BYTE>( buf, buf + size ) )  );
					}
					return res;
				}
				else
				{
					m_IncomeBuffer.push_back( std::make_pair( in_from, std::vector<BYTE>( buf + sizeof(SPLITPACKET), buf + ret ) )  );
					return false;
				}
				
#ifdef NET_LAG_PACKET
				// Lag the packet, if needed
				return NET_LagPacket( true, sock, &in_from, &in_message );
#else
				return true;
#endif
			}

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
#ifdef NET_LAG_PACKET
	// Allow lagging system to return a packet
	return NET_LagPacket( false, sock, NULL, NULL );
#else
	return true;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pData - 
//			size - 
//			*outSize - 
// Output : qboolean
//-----------------------------------------------------------------------------
bool NetLink::NET_GetLong( netadr_t *from, byte *pData, int size, int *outSize )
{
	int				packetNumber, packetCount, sequenceNumber, offset;
	int				packetID;
	SPLITPACKET		*pHeader;

	CSplitPacketEntry *entry = NET_FindOrCreateSplitPacketEntry( from );
	Assert( entry );
	if ( !entry )
		return false;

	entry->lastactivetime = Plat_FloatTime();
	Assert( NET_CompareAdr( entry->from, *from ) );

	pHeader = ( SPLITPACKET * )pData;
	sequenceNumber	= pHeader->sequenceNumber;
	packetID		= pHeader->packetID;
	// High byte is packet number
	packetNumber	= ( packetID >> 16 );	
	// Low byte is number of total packets
	packetCount		= ( packetID & 0xffff );	

	if ( packetNumber >= MAX_SPLITPACKET_SPLITS ||
		packetCount > MAX_SPLITPACKET_SPLITS )
	{
		Sys_Debug( "NET_GetLong:  Split packet from %s with too many split parts (number %i/ count %i) where %i is max count allowed\n", 
			NET_AdrToStringA( *from ).c_str(), 
			packetNumber, 
			packetCount, 
			MAX_SPLITPACKET_SPLITS );
		return false;
	}

	// First packet in split series?
	if ( entry->netsplit.currentSequence == -1 || 
		sequenceNumber != entry->netsplit.currentSequence )
	{
		entry->netsplit.currentSequence	= sequenceNumber;
		entry->netsplit.splitCount		= packetCount;
	}

	size -= sizeof(SPLITPACKET);

	if ( entry->splitflags[ packetNumber ] != sequenceNumber )
	{
		// Last packet in sequence? set size
		if ( packetNumber == (packetCount-1) )
		{
			entry->netsplit.totalSize = (packetCount-1) * SPLIT_SIZE + size;
			if( entry->netsplit.splitCount > 1 )
				Sys_Debug( "<== ERROR packet from %s was dropped, count = %d\n", NET_AdrToStringA( *from ).c_str(), entry->netsplit.splitCount -1 );
		}

		entry->netsplit.splitCount--;		// Count packet
		entry->splitflags[ packetNumber ] = sequenceNumber;

	#ifdef _DEBUG
		Sys_Debug( "<-- Split packet %i of %i from %s\n", packetNumber+1, packetCount, NET_AdrToStringA( *from ).c_str() );
	//	Sys_Debug( "Splitcount = %d\n", entry->netsplit.splitCount);
	#endif
	}
	else
	{
#ifdef _DEBUG
		Sys_Debug( "NET_GetLong:  Ignoring duplicated split packet %i of %i ( %i bytes ) from %s\n", packetNumber + 1, packetCount, size, NET_AdrToStringA( *from ).c_str() );
#endif
	}


	// Copy the incoming data to the appropriate place in the buffer
	offset = (packetNumber * SPLIT_SIZE);
	memcpy( entry->netsplit.buffer + offset, pData + sizeof(SPLITPACKET), size );

	// Have we received all of the pieces to the packet?
	if ( entry->netsplit.splitCount <= 0 )
	{
		entry->netsplit.currentSequence = -1;	// Clear packet
		if ( entry->netsplit.totalSize > sizeof(entry->netsplit.buffer) )
		{
			Sys_Debug("Split packet too large! %d bytes from %s\n", entry->netsplit.totalSize, NET_AdrToStringA( *from ).c_str() );
			return false;
		}

		memcpy( pData, entry->netsplit.buffer, entry->netsplit.totalSize );
		*outSize = entry->netsplit.totalSize;
		return true;
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
void NetLink::NET_SendPacket (int sock, int length, void *data, netadr_t to)
{
	int		ret;
	struct sockaddr	addr;
	SOCKET		net_socket;

	if ( to.type == NA_LOOPBACK )
	{
		NET_SendLoopPacket (sock, length, data, to);
		return;
	}

	if (to.type == NA_BROADCAST)
	{
		net_socket = GetSocket(sock);
		if (net_socket == INVALID_SOCKET)
			return;
	}
	else if (to.type == NA_IP)
	{
		net_socket = GetSocket(sock);
		if (net_socket == INVALID_SOCKET)
			return;
	}
	else
	{
		Sys_Error ("NET_SendPacket: bad address type");
		return;
	}

	NetadrToSockadr (&to, &addr);

	ret = NET_SendLong( sock, net_socket, (const char *)data, length, 0, &addr, sizeof(addr) );
	if (ret == -1)
	{
		int err = SockLastError();
		// wouldblock is silent
		if (err == WSAEWOULDBLOCK)
			return;

		if ( err == WSAECONNRESET )
			return;

		// some PPP links dont allow broadcasts
		if ( (err == WSAEADDRNOTAVAIL) && ( to.type == NA_BROADCAST ) )
			return;

			if ( err == WSAEADDRNOTAVAIL )
			{
				Sys_Error("NET_SendPacket Warning: %s : %s\n", NET_ErrorString(err), NET_AdrToStringA (to).c_str() );
			}
			else
			{
				Sys_Error ( "NET_SendPacket ERROR: %s\n", NET_ErrorString(err));
			}
	}
}

void NetLink::NET_SendLoopPacket (int sock, int length, void *data, netadr_t to)
{
	NET_ThreadLock();
	m_IncomeBuffer.push_back( NetMessage_t( to, std::vector< BYTE>( (BYTE*)data, (BYTE*)data + length ) )  );
	NET_ThreadUnlock();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : sock - 
//			s - 
//			buf - 
//			len - 
//			flags - 
//			to - 
//			tolen - 
// Output : int
//-----------------------------------------------------------------------------
int NetLink::NET_SendLong( int sock, SOCKET s, const char FAR * buf, int len, int flags, const struct sockaddr FAR * to, int tolen )
{
	if( len > NET_REAL_MAX_MESSAGE)
	{
		Sys_Error( "Too long packet packet of %i bytes and maximum %i bytes can be sent", len, NET_REAL_MAX_MESSAGE );
		return 0;
	}
	else if ( len > MAX_ROUTEABLE_PACKET )	// Do we need to break this packet up?
	{
		// yep
		char packet[MAX_ROUTEABLE_PACKET];
		int totalSent, ret, size, packetCount, packetNumber;
		SPLITPACKET *pPacket;
		int originalSize = len;

		m_gSequenceNumber++;
		if ( m_gSequenceNumber < 0 )
		{
			m_gSequenceNumber = 1;
		}

		pPacket = (SPLITPACKET *)packet;
		Header_Fill( *pPacket );
		pPacket->netID = -2;
		pPacket->sequenceNumber = m_gSequenceNumber;
		packetNumber = 0;
		totalSent = 0;
		packetCount = (len + SPLIT_SIZE - 1) / SPLIT_SIZE;

#if defined( _DEBUG )
		if ( packetCount > 2 )
		{
			Sys_Debug( "Saving split packet of %i bytes and %i packets",
					len, packetCount);

		}
#else
		Sys_Debug( "Splitting packet of %i bytes to %i pieces", len, packetCount  );
#endif

		while ( len > 0 )
		{
			size = std::min<int>( SPLIT_SIZE, len );

			pPacket->packetID = ( packetNumber << 16 ) | (packetCount & 0xffff);

			memcpy( packet + sizeof(SPLITPACKET), buf + (packetNumber * SPLIT_SIZE), size );

			ret = NET_SendTo( false, s, packet, size + sizeof(SPLITPACKET), flags, to, tolen );
			if ( ret < 0 )
			{
				return ret;
			}

			if ( ret >= size )
			{
				totalSent += size;
			}
			len -= size;
			packetNumber++;

#ifdef _DEBUG
			{
				netadr_t adr;
				memset( &adr, 0, sizeof( adr ) );
				SockadrToNetadr( (struct sockaddr *)to, &adr );
				Sys_Debug( "Split packet %i/%i (size %i dest: %s)\n",
					packetNumber, 
					packetCount, 
					originalSize, 
					NET_AdrToStringA( adr ).c_str() );
			}
#endif

			// FIXME:  This was 15, but if you have a lot of packets, that will pause the server for a long time
#ifdef _WIN32
			Sleep( 1 );
#elif _LINUX
			usleep( 1 );
#endif

		}

		return totalSent;
	}
	else
	{
		int nSend = 0;
		char packet[MAX_ROUTEABLE_PACKET + sizeof(SPLITPACKET)];
		memset( packet, 0, sizeof(SPLITPACKET));
		Header_Fill( *((SPLITPACKET*)&packet) );
		memcpy( packet + sizeof(SPLITPACKET), buf, len  );
		nSend = NET_SendTo( true, s, packet, len + sizeof(SPLITPACKET), flags, to, tolen );
		return nSend;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : sock - 
//			s - 
//			buf - 
//			len - 
//			flags - 
//			to - 
//			tolen - 
// Output : int
//-----------------------------------------------------------------------------
int NetLink::NET_SendTo( bool verbose, SOCKET s, const char FAR * buf, int len, int flags, const struct sockaddr FAR * to, int tolen )
{	
	int nSend = 0;
	nSend = sendto( s, buf, len, flags, to, tolen );

#if defined( DEBUG )
	if ( ( tolen > MAX_ROUTEABLE_PACKET ) )
	{
		Sys_Error ( "NET_SendTo:  Packet length (%i) > (%i) bytes\n", tolen, MAX_ROUTEABLE_PACKET);
	}
#endif
	return nSend;
}

/*
====================
NET_IPSocket
====================
*/
SOCKET NetLink::NET_IPSocket ( const TCHAR *net_interface, int& port, bool bReuse)
{
	SOCKET				newsocket;
	struct sockaddr_in	address;
	unsigned long		_true = 1;
	int					i = 1;
	int					err;

	if ((newsocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		err = SockLastError();
		if (err != WSAEAFNOSUPPORT)
			Sys_Error ("WARNING: UDP_OpenSocket: socket: %s", NET_ErrorString(err));
		return 0;
	}

	// make it non-blocking
	if (ioctlsocket (newsocket, FIONBIO, &_true) == -1)
	{
		err = SockLastError();
		Sys_Error ("WARNING: UDP_OpenSocket: ioctl FIONBIO: %s\n", NET_ErrorString(err));
		return 0;
	}
#if 0	// we dont use broadcast
	// make it broadcast capable
	if (setsockopt(newsocket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) == -1)
	{
		err = SockLastError();
		Sys_Error ("WARNING: UDP_OpenSocket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString(err));
		return 0;
	}
#endif

	int ttl = 1 ; // Limits to subnet.
	int res = setsockopt( newsocket, IPPROTO_IP, IP_MULTICAST_TTL,	(char *)&ttl, sizeof(ttl));
	if(res)
	{
		err = SockLastError();
		Sys_Error ("WARNING: UDP_OpenSocket: setsockopt IP_MULTICAST_TTL: %s\n", NET_ErrorString(err));
	}

	// make it reusable
	if ( /*CommandLine()->FindParm( "-reuse" )*/bReuse )
	{
		if (setsockopt(newsocket, SOL_SOCKET, SO_REUSEADDR, (char *)&_true, sizeof(DWORD)) == -1)
		{
			err = SockLastError();
			Sys_Error ("WARNING: UDP_OpenSocket: setsockopt SO_REUSEADDR: %s\n", NET_ErrorString(err));
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
				Sys_Error( "UDP Socket bound to non-default port %i because original port was already in use.\n", port + p );
				port = port + p;
			}
			break;
		}

		err = SockLastError();
		if ( port == PORT_ANY || err != WSAEADDRINUSE )
		{
			Sys_Error ("WARNING: UDP_OpenSocket: bind: %s\n", NET_ErrorString(err));
			closesocket (newsocket);
			return 0;
		}

		// Try next port
	}

	if ( !success )
	{
		Sys_Error( "WARNING: UDP_OpenSocket: unable to bind socket\n" );
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
void NetLink::NET_GetLocalAddress (bool localhost)
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
		char tmp[128];
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
		if ( getsockname ( GetSocket(NS_SERVER), (struct sockaddr *)&address, (socklen_t *)&namelen) != 0)
		{
			int err = SockLastError();
			Sys_Error("Could not get TCP/IP address, TCP/IP disabled\nReason:  %s\n", NET_ErrorString(net_error));
		}
		else
		{
			net_local_adr.port = address.sin_port;
			std::string ps = NET_AdrToStringA (net_local_adr);
			Sys_Debug("Server IP address %s\n", ps.c_str() );
		//	net_address.SetValue( va( "%s", NET_AdrToString( net_local_adr ) ) );
		}
}

void	NetLink::NET_Config (bool multiplayer)
{
	int		i;
	static	bool	old_config;
	static bool bFirst = true;

	if (old_config == multiplayer)
		return;

	old_config = multiplayer;

	if (!multiplayer)
	{	// shut down any existing sockets
		NET_ThreadLock();

		for (i=0 ; i< NUMBER_OF_SOCKETS ; i++)
		{
			if (GetSocket(i) != INVALID_SOCKET)
			{
				closesocket (GetSocket(i));
				SetSocket(i, INVALID_SOCKET);
			}
		}

		NET_ThreadUnlock();
	}
	else
	{	// open sockets

		NET_OpenIP ();

		// Get our local address, if possible
		if ( bFirst )
		{
			bFirst = false;
			NET_GetLocalAddress (false);
		}
	}

	net_configured = multiplayer ? 1 : 0;
}


#if STUB_PORT
#define PORT_SERVER	1234
#define PORT_CLIENT	1235
#endif

LPCTSTR GetStubIP()
{
#ifdef STUB_NAME
	return _T("localhost");
#endif
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
void NetLink::NET_OpenIP (void)
{
	int		port;
	int		dedicated = false;
#ifndef DONT_USE_CLIENT
	int     sv_port = 0, cl_port = 0;
#else
	int		sv_port = 0;
#endif
	static bool bFirst = true;

	NET_ThreadLock();

	if (GetSocket(NS_SERVER) == INVALID_SOCKET)
	{
		SOCKET s;
		port = m_nServerPort;
		s = NET_IPSocket (GetStubIP() , port, m_bReuseSocket);
		SetSocket(NS_SERVER, s);
		if (!s && dedicated)
		{
			Sys_Error ("Couldn't allocate dedicated server IP port");
		}

		sv_port = port;
	}
#ifndef DONT_USE_CLIENT
	if (GetSocket(NS_CLIENT) == INVALID_SOCKET )
	{
		port = m_nClientPort;
		SOCKET s = NET_IPSocket ( GetStubIP() , port, m_bReuseSocket);
		SetSocket( NS_CLIENT, s ); 
		if (s == INVALID_SOCKET)
		{
			port = PORT_ANY;
			SOCKET s= NET_IPSocket (GetStubIP() , port, m_bReuseSocket);
			SetSocket(NS_CLIENT, s); 
		}
		cl_port = port;
	}
#endif
	NET_ThreadUnlock();

	if ( bFirst )
	{
		bFirst = false;
#ifndef DONT_USE_CLIENT
		Sys_Debug( "Networking ports (%i sv / %i cl)\n", sv_port, cl_port );
#else
		Sys_Debug( "Networking port %i sv\n", sv_port );
#endif
	}
}

char const *NET_GetHostName()
{
	static char buff[ 512 ];
	gethostname(buff, 512);
	return buff;
}


void	NetLink::SendMessageFromServer( netadr_t NetAddr, const void* pBytes, size_t nSize  )
{
	NET_SendPacket( NS_SERVER, (int)nSize, (void*)pBytes, NetAddr );
}

#ifndef DONT_USE_CLIENT
void	NetLink::SendMessageFromClient( netadr_t NetAddr, const void* pBytes, size_t nSize  )
{
	NET_SendPacket( NS_CLIENT, (int)nSize, (void*)pBytes, NetAddr );
}
#endif

void	NetLink::StartNetCommunication(short nServerPort, short nClientPort, 
										bool bReuse)
{
	m_bReuseSocket = bReuse;
	m_nServerPort = nServerPort;
#ifndef DONT_USE_CLIENT
	m_nClientPort = nClientPort;
#endif
	NET_Config(true);
	NET_StartThread();
}

void	NetLink::StopNetCommunication()
{
	NET_StopThread();
	NET_Config(false);
}

#define DEFINE_HEADER(a,b,c,d)		(( d <<24)+( c <<16)+( b <<8)+ a)
#define PROTO_MAGIC					( 'c', 's', 'n', 'p' )
#define PROTO_VERSION				1

void	NetLink::Header_Fill(SPLITPACKET& sp)
{

	const __int32 cur_proto_version = PROTO_VERSION;
	sp.protoVersion = cur_proto_version;
	sp.magicID		= PROTO_MAGIC;
}

bool	NetLink::Header_Check(const SPLITPACKET& sp  ) const
{
	return sp.magicID == PROTO_MAGIC && sp.protoVersion <= PROTO_VERSION;
}

NetLink::NetLink(void) : 
	m_gSequenceNumber (1), 
	m_hSyncEvent( CreateEvent( 0, TRUE, FALSE, 0 ) ),
	m_bReuseSocket(false)
{
	// for timer init
	Plat_FloatTime();
	InitializeCriticalSection( &net_cs );
	use_thread				= true;
	net_thread_initialized	= false;
	SetSocket(NS_SERVER, INVALID_SOCKET);
#ifndef DONT_USE_CLIENT
	SetSocket(NS_CLIENT, INVALID_SOCKET);
#endif
	SocketInit();
}

NetLink::~NetLink(void)
{
	StopNetCommunication();
	SocketExit();
	DeleteCriticalSection( &net_cs );
	CloseHandle( m_hSyncEvent );
}


void	NetLink::LockIncomeBuffer()
{
	NET_ThreadLock();
}

void	NetLink::UnlockIncomeBuffer()
{
	NET_ThreadUnlock();
}

void	NetLink::AddToGroup( unsigned long nGroup)
{
	// mreq is the ip_mreqstructure 
	SOCKET newsocket = GetSocket(NS_SERVER);
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = nGroup;
	mreq.imr_interface.s_addr = INADDR_ANY;
	if( setsockopt(newsocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) )
	{
		int err = SockLastError();
		Sys_Error ("WARNING: UDP_OpenSocket: setsockopt IP_ADD_MEMBERSHIP: %s", NET_ErrorString(err));
	}
}