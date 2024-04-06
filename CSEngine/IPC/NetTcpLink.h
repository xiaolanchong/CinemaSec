#ifndef __NETTCPLINK_H__
#define __NETTCPLINK_H__

#include "NetStruct.h"
#include <boost/tuple/tuple.hpp>

namespace TCP
{

struct Marker
{
	int nID;
	int Length;
};

class NetSocket
{
protected:

	struct StreamData
	{
		netadr_t			NetAddress;
		int					TotalBytes;
		bool				TotalValid;
		std::vector<BYTE>	Data;
	};

	typedef std::list<StreamData>	StreamDataList_t;
	StreamDataList_t				m_StreamList;

	StreamData* FindData( const netadr_t& NetAddress);
	void		AddData( const netadr_t& NetAddress, const void* pBytes, size_t nSize );

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

	SOCKET		m_ipsocket;

	netadr_t	net_local_adr;
	short		m_nPort;
#ifdef _WIN32
	CRITICAL_SECTION net_cs;
	HANDLE	hNetThread;
	HANDLE		m_hSyncEvent;
#endif
	__int32		m_gSequenceNumber;
	bool		m_bFirst;

	bool		net_thread_initialized;
	bool		use_thread;
	bool		net_configured;

	int			m_nSocketConnection;

	int		NET_WaitForSend( SOCKET sock, DWORD timeout);

	void		NET_ThreadLock();
	void		NET_ThreadUnlock();

//	int			NET_Sleep( void );

	bool		NET_QueuePacket( SOCKET sock, const netadr_t& From);

	SOCKET		GetSocket()			{ return m_ipsocket; }
	void		SetSocket( SOCKET s){ m_ipsocket = s;	}

	NetIncomeBuffer_t	m_IncomeBuffer;

	void	NET_SendPacket	(SOCKET s, int length, void *data);

	SOCKET	NET_IPSocket ( const TCHAR *net_interface, int& port);
	void	NET_Config (bool multiplayer);
	void	NET_GetLocalAddress (bool localhost);
	void	NET_OpenIP (void);

	void		NET_StartThread( void );
	void		NET_StopThread( void );

	virtual std::pair< LPTHREAD_START_ROUTINE, void*>	GetThreadParams() = 0; 

public:
	NetSocket(void);
	~NetSocket(void);

	void	LockIncomeBuffer();
	void	UnlockIncomeBuffer();

	NetIncomeBuffer_t&	GetIncomeBuffer()  { return m_IncomeBuffer; }
	int		GetMaxMessageSize()				const { return NET_REAL_MAX_MESSAGE ;};
};

class NetServer : public NetSocket
{
	typedef std::vector< boost::tuple< netadr_t, SOCKET, INT64 > >	SocketArr_t;
	SocketArr_t			m_Clients;
	enum
	{
		InvalidClient = -1
	};
	int		m_nIdleTime;

	static		DWORD WINAPI NET_ThreadFunc(VOID*);
	int		FindClient( const netadr_t& NetAddr );
	void	CheckIdleSockets();


	virtual std::pair< LPTHREAD_START_ROUTINE, void*>	GetThreadParams() 
	{
		return std::make_pair( NET_ThreadFunc, this );
	}

	int			NET_Sleep(  );
public:
	static const int c_DefSocketConnection = -1;

	NetServer() : m_nIdleTime(0){}
	virtual ~NetServer();

	void	SendMessage( netadr_t NetAddr, const void* pBytes, int nSize  );
	void	Start( short nPort, int nIdleTime, int nSocketConnection = c_DefSocketConnection );
	void	Stop();
	void	CloseClient( const netadr_t& NetAddr );

};

class NetClient : public NetSocket
{
	netadr_t	m_ServerAddress;
	virtual std::pair< LPTHREAD_START_ROUTINE, void*>	GetThreadParams() 
	{
		return std::make_pair( LPTHREAD_START_ROUTINE(NULL), this );
	}
	int			NET_Sleep(  );

#if 0
	bool		m_bSendOnly;
	static		DWORD WINAPI NET_ThreadFunc(VOID*);
#endif
public:

	virtual ~NetClient();
	void			Start( short nPort, netadr_t ServerAddress /*, bool bSendOnly*/ );
	void			Stop();
	void			SendMessage( const void* pBytes, int nSize  );
	const netadr_t&	GetServerAddress() { return m_ServerAddress; }
	bool				WaitForSend(  DWORD timeout)
	{
		return NET_WaitForSend( GetSocket(), timeout ) != 0;
	}
};

class NetSendClient : protected NetClient
{
public:
	NetSendClient( const netadr_t& ServerAddress, const void* pData, size_t nSize )
	{
		Start( PORT_ANY, ServerAddress );
		int res = NET_WaitForSend( GetSocket(), 20 );
//		ASSERT(res != INVALID_SOCKET );
		SendMessage( pData, int(nSize) );
	}
/*	bool				WaitForSend(  DWORD timeout)
	{
		return NetClient::WaitForSend( timeout );
	}*/
};

}

#endif //__NETTCPLINK_H__