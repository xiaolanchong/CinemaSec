#ifndef ELVEES_SOCKETSERVERUDP_INCLUDED__
#define ELVEES_SOCKETSERVERUDP_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

///////////////////////////////////////////////////////////////////////////////

#include "Thread.h"
#include "IOBuffer.h"
#include "UsesWinsock.h"
#include "IOCP.h"
#include "Events.h"
#include "NodeList.h"
#include "CriticalSection.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// CSocketServerUDP
///////////////////////////////////////////////////////////////////////////////

class CSocketServerUDP
	: protected CThread
	, protected CIOBuffer::Allocator
	, private CUsesWinsock
{
public:
	class Socket;
	friend class Socket;

	virtual ~CSocketServerUDP();

	using CThread::Start;

	void StartAcceptingConnections();
	void StopAcceptingConnections();

	void InitiateShutdown();
	void WaitForShutdownToComplete();

protected:
	class WorkerThread;
	friend class WorkerThread;

	explicit CSocketServerUDP(
		unsigned long addressToListenOn,
		unsigned short portToListenOn,
		unsigned maxFreeSockets,
		unsigned maxFreeBuffers,
		unsigned recvsToPost,
		unsigned bufferSize = 1024,
		unsigned numThreads = 0);

	void ReleaseSockets();
	void ReleaseBuffers();

private:
	virtual int Run();

	// Override this to create your worker thread
	virtual WorkerThread *CreateWorkerThread(CIOCP &iocp);

	// Override this to create the listening socket of your choice
	virtual SOCKET CreateListeningSocket(
		unsigned long address,
		unsigned short port) = 0;

	virtual unsigned GetAddressSize() const;

	// Interface for derived classes to receive state change notifications...
	virtual void OnStartAcceptingConnections() {}
	virtual void OnStopAcceptingConnections()  {}
	virtual void OnShutdownInitiated() {}
	virtual void OnShutdownComplete()  {}

	virtual void OnConnectionCreated()   {}
	virtual void OnConnectionDestroyed() {}

	// CIOBuffer::Allocator
	virtual void OnBufferCreated()   {}
	virtual void OnBufferAllocated() {}
	virtual void OnBufferReleased()  {}
	virtual void OnBufferDestroyed() {}

	// CSocketServerUDP::WorkerThread
	virtual void OnThreadBeginProcessing() {}
	virtual void OnThreadEndProcessing()   {}

	virtual void ReadCompleted(Socket *pSocket, CIOBuffer *pBuffer) = 0;
	virtual void WriteCompleted(Socket *pSocket, CIOBuffer *pBuffer);

	enum IO_Operation 
	{ 
		IO_Accept_Completed,
		IO_Read_Request, 
		IO_Read_Completed, 
		IO_Write_Request, 
		IO_Write_Completed 
	};

	// Socket operations
	Socket *AllocateSocket();
	void CloseSocket(Socket *pSocket);
	void ReleaseSocket(Socket *pSocket);
	void DestroySocket(Socket *pSocket);
   
	// Post IO Operations...

	void Write(
		Socket *pSocket,
		const char *pData,
		unsigned dataLength);

	void Write(
		Socket *pSocket,
		CIOBuffer *pBuffer);

	void PostReadRequest();
	void ReadCompleted();

private:
	typedef TNodeList<Socket> SocketList;

	SocketList m_activeList;
	SocketList m_freeList;

	SOCKET m_listeningSocket;

	CIOCP m_iocp;
	CCriticalSection  m_listManipulationSection;

	CManualResetEvent m_shutdownEvent;
	CManualResetEvent m_acceptConnectionsEvent;
	CManualResetEvent m_stopAcceptingConnectionsEvent;

	const unsigned m_numThreads;
	const unsigned m_maxFreeSockets;
	const unsigned m_recvsToPost;

	const unsigned long m_address;
	const unsigned short m_port;

	// No copies do not implement
	CSocketServerUDP(const CSocketServerUDP &rhs);
	CSocketServerUDP &operator=(const CSocketServerUDP &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServerUDP::Socket
///////////////////////////////////////////////////////////////////////////////

class CSocketServerUDP::Socket : public CNodeList::Node
{
public:
	friend class CSocketServerUDP;
	friend class CSocketServerUDP::WorkerThread;

	void AddRef();
	void Release();

	// No read on unbinded UDP socket
	void Write(const char *pData, unsigned dataLength);
	void Write(CIOBuffer *pBuffer);

private:
	explicit Socket(CSocketServerUDP &server);
	virtual ~Socket();

	void Attach(SOCKET socket);

private:
	CSocketServerUDP &m_server;
	SOCKET m_socket;

	long m_ref;

	const unsigned m_fromLen;
	int m_actualFromLen;
	sockaddr *m_pFrom;

	// No copies do not implement
	Socket(const Socket &rhs);
	Socket &operator=(const Socket &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServerUDP::WorkerThread
///////////////////////////////////////////////////////////////////////////////

class CSocketServerUDP::WorkerThread : public CNodeList::Node, protected CThread
{
public:
	virtual ~WorkerThread();

protected:
	friend class CSocketServerUDP;
	explicit WorkerThread(CSocketServerUDP &server, CIOCP &iocp);

	void WaitForShutdown();

private:
	virtual int Run();

	void Read(
		CSocketServerUDP::Socket *pSocket,
		CIOBuffer *pBuffer) const;

	void Write(
		CSocketServerUDP::Socket *pSocket,
		CIOBuffer *pBuffer) const;

	CIOCP &m_iocp;
	CSocketServerUDP &m_server;

	// No copies do not implement
	WorkerThread(const WorkerThread &rhs);
	WorkerThread &operator=(const WorkerThread &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEES_SOCKETSERVERUDP_INCLUDED__
