// StreamClient.h: interface for the CStreamClient class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CManager;

///////////////////////////////////////////////////////////////////////////////
// CArchiveClient
///////////////////////////////////////////////////////////////////////////////

class CStreamClient : public Elvees::CSocketServer
{
public:
	explicit CStreamClient(
		CManager* pManager,
		unsigned nConnections,		// 0 - unlimited
		unsigned maxMessageSize,	// max packet size
		unsigned bufferSize = 0,	// IO buffer size
		unsigned numThreads = 0);

	bool StartStream(unsigned long address, unsigned short port);

private:
	class ClientSocket;

	// Create my sockets...
	virtual Elvees::CSocketServer::Socket* CreateSocket(
		SOCKET theSocket);

	virtual void DestroySocket(
		Elvees::CSocketServer::Socket *pSocket);

	// Notifications
	virtual void OnConnectionEstablished(
		Elvees::CSocketServer::Socket *pSocket,
		Elvees::CIOBuffer *pAddress);

	virtual bool OnConnectionClosing(
		Elvees::CSocketServer::Socket *pSocket);

	virtual void OnConnectionClosed(
		Elvees::CSocketServer::Socket *pSocket);

	virtual void ReadCompleted(
		Elvees::CSocketServer::Socket *pSocket,
		Elvees::CIOBuffer *pBuffer);

	// Handle incoming buffers
	// work in context of Elvees::CSocketServer::WorkerThread

	unsigned GetMessageSize(const Elvees::CIOBuffer *pBuffer) const;

	void ProcessDataStream(
		ClientSocket* pSocket,
		Elvees::CIOBuffer *pBuffer);

	void ProcessCommand(
		ClientSocket* pSocket,
		Elvees::CIOBuffer *pBuffer);

private:
	// const variables
	const unsigned m_maxMessageSize;

	// Connection data operation
	Elvees::CIOBuffer::Allocator m_allocator;
	Elvees::CCriticalSection m_criticalSection;

private:
	CManager* m_pManager;

	// No copies do not implement
	CStreamClient(const CStreamClient &rhs);
	CStreamClient &operator=(const CStreamClient &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CStreamClient::ClientSocket
///////////////////////////////////////////////////////////////////////////////

class CStreamClient::ClientSocket : public Elvees::CSocketServer::Socket
{
protected:
	friend class CStreamClient;

	explicit ClientSocket(
		CStreamClient& server, SOCKET socket,
		Elvees::CCriticalSection &criticalSection,
		Elvees::CIOBuffer::Allocator &allocator);

	virtual ~ClientSocket();

	Elvees::CIOBuffer* GetTransferBuffer();
	Elvees::CIOBuffer* GetNextBuffer(Elvees::CIOBuffer* pBuffer);
	Elvees::CIOBuffer* ProcessAndGetNext();

	void RemoveTransferBuffer();
	void ResetClientSocket();

	unsigned long m_address;

private:
	Elvees::CIOBuffer *m_pTransferBuffer;
	Elvees::CIOBuffer::Allocator &m_allocator;
	Elvees::CIOBuffer::InOrderBufferList m_bufferSequence;

	// No copies, do not implement
	ClientSocket(const ClientSocket &rhs);
	ClientSocket &operator=(const ClientSocket &rhs);
};