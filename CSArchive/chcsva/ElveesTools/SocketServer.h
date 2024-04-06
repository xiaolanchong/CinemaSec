#ifndef ELVEES_SOCKETSERVER_INCLUDED__
#define ELVEES_SOCKETSERVER_INCLUDED__

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
// CSocketServer
///////////////////////////////////////////////////////////////////////////////

class CSocketServer : 
	protected CThread, 
	protected CIOBuffer::Allocator,
	private CUsesWinsock
{
public:
	class Socket;
	friend class Socket;

	virtual ~CSocketServer();

	using CThread::Start;

	void StartAcceptingConnections();
	void StopAcceptingConnections();

	void InitiateShutdown();
	void WaitForShutdownToComplete();

	Socket *Connect(const SOCKADDR_IN &address);

protected:
	class WorkerThread;
	friend class WorkerThread;

	CSocketServer(
		unsigned long addressToListenOn,
		unsigned short portToListenOn,
		unsigned maxFreeSockets,
		unsigned maxFreeBuffers,
		unsigned bufferSize = 1024,
		unsigned numThreads = 0,
		bool postZeroByteReads = false);

	static void  __cdecl SetServerDataPtr(Socket *pSocket, void *pData);
	static void* __cdecl GetServerDataPtr(const Socket *pSocket);

	void BroadcastMessage(Elvees::CIOBuffer *pData);
	
	enum ConnectionErrorSource
	{
		ZeroByteReadError,
		ReadError,
		WriteError
	};

private:
	virtual int Run();

	// Override this to create your worker thread
	virtual WorkerThread *CreateWorkerThread(CIOCP &iocp);

	// Override this to create the listening socket of your choice
	virtual SOCKET CreateListeningSocket(
		unsigned long address,
		unsigned short port);

	// Override this to create the outbound socket of your choice
	virtual SOCKET CreateOutboundSocket(
		unsigned long address,
		unsigned short port);

	virtual void ReadCompleted(Socket *pSocket, CIOBuffer *pBuffer) = 0;
	virtual void WriteCompleted(Socket *pSocket, CIOBuffer *pBuffer);

	// Interface for derived classes to receive state change notifications...
	virtual void OnStartAcceptingConnections();
	virtual void OnStopAcceptingConnections();
	virtual void OnShutdownInitiated();
	virtual void OnShutdownComplete();

	virtual void OnConnectionCreated();
	virtual void OnConnectionDestroyed();

	virtual void OnConnectionEstablished(Socket *pSocket, CIOBuffer *pAddress) = 0;
	
	virtual void OnConnectionClientClose(Socket* pSocket);
	virtual bool OnConnectionClosing(Socket*);
	virtual void OnConnectionClosed(Socket*);
	virtual void OnConnectionReset(Socket*);

	virtual void OnConnectionError(
		ConnectionErrorSource source,
		Socket *pSocket,
		CIOBuffer *pBuffer,
		DWORD lastError);

	// CSocketServer::WorkerThread
	virtual void OnThreadBeginProcessing() {}
	virtual void OnThreadEndProcessing()   {}

	// Sockets manipulations

	virtual Socket* CreateSocket(SOCKET theSocket);
	virtual void DestroySocket(Socket *pSocket);

	Socket* AllocateSocket(SOCKET theSocket);

	void ReleaseSocket(Socket *pSocket);
	void ReleaseSockets();

	enum IO_Operation 
	{ 
		IO_Zero_Byte_Read_Request,
		IO_Zero_Byte_Read_Completed,
		IO_Read_Request,
		IO_Read_Completed,
		IO_Write_Request,
		IO_Write_Completed
	};

	void PostIoOperation(
		Socket *pSocket,
		CIOBuffer *pBuffer,
		IO_Operation operation);

private:
	const unsigned long m_address;
	const unsigned short m_port;
	const unsigned m_maxFreeSockets;
	const unsigned m_numThreads;
	const bool m_postZeroByteReads;

	SOCKET m_listeningSocket;

	CIOCP m_iocp;
	CManualResetEvent m_shutdownEvent;
	CManualResetEvent m_acceptConnectionsEvent;

	typedef TNodeList<Socket> SocketList;
	SocketList m_activeList;
	SocketList m_freeList;

	CCriticalSection m_listManipulationSection;

	// No copies do not implement
	CSocketServer(const CSocketServer &rhs);
	CSocketServer &operator=(const CSocketServer &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::Socket
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::Socket : public CNodeList::Node
{
protected:
	explicit Socket(CSocketServer &server, SOCKET socket);
	virtual ~Socket();

	friend class CSocketServer;
	friend class CSocketServer::WorkerThread;

public:
	bool Read(CIOBuffer *pBuffer = 0, bool throwOnFailure = false);
	bool Write(const char *pData, unsigned dataLength, bool throwOnFailure = false);
	bool Write(const BYTE *pData, unsigned dataLength, bool throwOnFailure = false);
	bool Write(CIOBuffer *pBuffer, bool throwOnFailure = false);

	void AddRef();
	void Release();

	void Close();
	void AbortiveClose();

	void Shutdown(int how = SD_BOTH);

	bool IsConnected(int how = SD_BOTH) const;

	Socket* Connect(const sockaddr_in &address);

	long GetSessionID() const;
	void SetSessionID(long lSessionID);

	long GetWriteCount() const;
	long GetWriteBytesCount() const;

private:
	void Attach(SOCKET socket);

	bool WritePending(long lBytes);
	void WriteCompleted(long lBytes);

	void OnClientClose();
	void OnConnectionReset();

	bool IsValid();

	void InternalClose();

	void OnConnectionError(
		CSocketServer::ConnectionErrorSource source,
		CIOBuffer *pBuffer,
		DWORD lastError);

	CCriticalSection m_crit;

	CSocketServer &m_server;
	SOCKET m_socket;

	long m_ref;
	long m_outstandingWrites;
	long m_outstandingBytes;

	// TODO we could store all of these 1 bit flags in with the outstanding write count..

	bool m_readShutdown;
	bool m_writeShutdown;
	bool m_closing;
	long m_clientClosed;

	///////////////////////////////////////

	long m_lSessionID;

	///////////////////////////////////////

	void *GetServerDataPtr() const;
	void  SetServerDataPtr(void *pData);

	void *m_pServerData;

	///////////////////////////////////////

	unsigned GetReadSequenceNumber();
	unsigned GetWriteSequenceNumber();

	unsigned m_ReadSequenceNo;
	unsigned m_WriteSequenceNo;

	CIOBuffer::InOrderBufferList m_outOfSequenceWrites;

	///////////////////////////////////////

	// No copies do not implement
	Socket(const Socket &rhs);
	Socket &operator=(const Socket &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::WorkerThread
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::WorkerThread : public CNodeList::Node, protected CThread
{
protected:
	friend class CSocketServer;
	explicit WorkerThread(CSocketServer &server, CIOCP &iocp);

	void WaitForShutdownToComplete();

private:
	virtual int Run();

	void HandleOperation(
		CSocketServer::Socket *pSocket,
		CIOBuffer *pBuffer,
		DWORD dwIoSize,
		bool weClosedSocket);

	void ZeroByteRead(
		CSocketServer::Socket *pSocket,
		CIOBuffer *pBuffer) const;

	void Read(
		CSocketServer::Socket *pSocket,
		CIOBuffer *pBuffer) const;

	void Write(
		CSocketServer::Socket *pSocket,
		CIOBuffer *pBuffer) const;

	CIOCP &m_iocp;
	CSocketServer &m_server;

	// No copies do not implement
	WorkerThread(const WorkerThread &rhs);
	WorkerThread &operator=(const WorkerThread &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEES_SOCKETSERVER_INCLUDED__

