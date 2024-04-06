#ifndef __NETLINK_H__
#define __NETLINK_H__

#include "NetAddr.h"
#include <vector>
#include <list>
#include "../../CSChair/Common/Exception.h"
#include "NetStruct.h"

#define DONT_USE_CLIENT

enum 
{
	NS_SERVER = 0,
#ifdef DONT_USE_CLIENT
	NUMBER_OF_SOCKETS = 1
#else
	NS_CLIENT = 1,
	NUMBER_OF_SOCKETS = 2
#endif
};

class NetLink
{
	class CSplitPacketEntry
	{
	public:
		CSplitPacketEntry()
		{
			memset( &from, 0, sizeof( from ) );

			int i;
			for ( i = 0; i < MAX_SPLITPACKET_SPLITS; i++ )
			{
				splitflags[ i ] = -1;
			}

			memset( &netsplit, 0, sizeof( netsplit ) );
			lastactivetime = 0.0f;
		}

	public:
		netadr_t		from;
		int				splitflags[ MAX_SPLITPACKET_SPLITS ];
		LONGPACKET		netsplit;
		// host_time the last time any entry was received for this entry
		double			lastactivetime;
	};

	std::vector< CSplitPacketEntry >	g_SplitPacketEntries;

	void				NET_DiscardStaleSplitpackets( void );
	CSplitPacketEntry *	NET_FindOrCreateSplitPacketEntry( netadr_t *from );

	WORD	wSockVersion;
	void SocketInit( void )
	{
		WSADATA wsData;

		WORD wVersionRequested = MAKEWORD(2, 0);
		int nResult = WSAStartup(wVersionRequested, &wsData);
		wSockVersion = wsData.wHighVersion;
	}
	void SocketExit( void )
	{
		WSACleanup();
	}

	netadr_t	net_local_adr;
#ifdef DONT_USE_CLIENT
	short		m_nServerPort;
#else
	short		m_nServerPort, m_nClientPort;
#endif

#ifdef _WIN32
	CRITICAL_SECTION net_cs;
	HANDLE	hNetThread;
	HANDLE		m_hSyncEvent;
#endif
	__int32		m_gSequenceNumber;

	bool	net_thread_initialized;
	bool	use_thread;
	bool	net_configured;
	bool	m_bReuseSocket;

	// Start up networking
	void		NET_Init( void );
	// Shut down networking
	void		NET_Shutdown (void);

	void		NET_StartThread( void );
	void		NET_StopThread( void );

	void		NET_ThreadLock();
	void		NET_ThreadUnlock();

	static		DWORD WINAPI NET_ThreadFunc(VOID*);
	int			NET_Sleep( void );
    
	bool		NET_QueuePacket(int sock);
	bool		NET_GetLong( netadr_t *from, byte *pData, int size, int *outSize );

	SOCKET		m_ipsocket[ NUMBER_OF_SOCKETS ];

	SOCKET		GetSocket(int i)			{ return m_ipsocket[i]; }
	void		SetSocket(int i, SOCKET s)	{ m_ipsocket[i] = s;	}
	
	netadr_t	GetSockAddr(int i)			{ return m_AddrBuf[i] ; }

	NetIncomeBuffer_t	m_IncomeBuffer;
	AddrBuffer_t		m_AddrBuf;

	void	NET_SendPacket (int sock, int length, void *data, netadr_t to);
	void	NET_SendLoopPacket (int sock, int length, void *data, netadr_t to);
	int		NET_SendLong( int sock, SOCKET s, const char FAR * buf, int len, int flags, const struct sockaddr FAR * to, int tolen );
	int		NET_SendTo( bool verbose, SOCKET s, const char FAR * buf, int len, int flags, const struct sockaddr FAR * to, int tolen );

	SOCKET	NET_IPSocket ( const TCHAR *net_interface, int& port, bool bReuse);
	void	NET_Config (bool multiplayer);
	void	NET_GetLocalAddress (bool localhost);
	void	NET_OpenIP (void);

	void	Header_Fill(SPLITPACKET& sp);
	bool	Header_Check(const SPLITPACKET& sp  ) const;
public:
	NetLink(void);
	~NetLink(void);

	void	LockIncomeBuffer();
	void	UnlockIncomeBuffer();

	NetIncomeBuffer_t&	GetIncomeBuffer()  { return m_IncomeBuffer; }

	void	SendMessageFromServer( netadr_t NetAddr, const void* pBytes, size_t nSize  );
#ifdef DONT_USE_CLIENT
#else
	void	SendMessageFromClient( netadr_t NetAddr, const void* pBytes, size_t nSize  );
#endif
	void	StartNetCommunication(	short nServerPort, short nClientPort, bool bReuse = true);

	void	AddToGroup( unsigned long pGroup);
	void	StopNetCommunication();

	int		GetMaxMessageSize() const { return NET_REAL_MAX_MESSAGE ;};
};

#endif //__NETLINK_H__