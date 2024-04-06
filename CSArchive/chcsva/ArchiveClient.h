// ArchiveClient.h: interface for the CArchiveClient class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CManager;
class CStreamArchive;

///////////////////////////////////////////////////////////////////////////////
// CArchiveClient
///////////////////////////////////////////////////////////////////////////////

class CArchiveClient : public Elvees::CSocketServer
{
public:
	explicit CArchiveClient(
		CManager* pManager,
		unsigned nConnections,		// 0 - unlimited
		unsigned maxMessageSize,	// max packet size
		unsigned bufferSize = 0,	// IO buffer size
		unsigned numThreads = 0);

	bool StartStream(CStreamArchive* pStream,
			unsigned long address, unsigned short port);

private:
	// Pure virtual - No input
	virtual void OnConnectionEstablished(
		Elvees::CSocketServer::Socket *pSocket,
		Elvees::CIOBuffer *pAddress);

	// Notifications
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
		Elvees::CSocketServer::Socket *pSocket,
		Elvees::CIOBuffer *pBuffer);

	void ProcessCommand(
		Elvees::CSocketServer::Socket *pSocket,
		Elvees::CIOBuffer *pBuffer);

private:
	class CStreamData;

	// Using server data to store CStreamData
	static void SetStreamData(
		Elvees::CSocketServer::Socket *pSocket,
		CStreamData *pData);
      
	static CStreamData *GetStreamData(
		const Elvees::CSocketServer::Socket *pSocket);

	// const variables

	const unsigned m_maxMessageSize;

	// Connection data operation
	Elvees::CIOBuffer::Allocator m_allocator;
	Elvees::CCriticalSection m_criticalSection;

private:
	CManager* m_pManager;

	// No copies do not implement
	CArchiveClient(const CArchiveClient &rhs);
	CArchiveClient &operator=(const CArchiveClient &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CArchiveClient::CStreamData
///////////////////////////////////////////////////////////////////////////////

class CArchiveClient::CStreamData
{
public:
	explicit CStreamData(
		CStreamArchive* pStream,
		Elvees::CCriticalSection &criticalSection,
		Elvees::CIOBuffer::Allocator &allocator);
	virtual ~CStreamData();

	Elvees::CIOBuffer* GetTransferBuffer();
	void RemoveTransferBuffer();

	Elvees::CIOBuffer* GetNextBuffer(Elvees::CIOBuffer* pBuffer);
	Elvees::CIOBuffer* ProcessAndGetNext();

public:
	CStreamArchive* m_pStream;

private:
	Elvees::CIOBuffer *m_pTransferBuffer;
	Elvees::CIOBuffer::Allocator &m_allocator;

	Elvees::CIOBuffer::InOrderBufferList m_bufferSequence;

	// No copies, do not implement
	CStreamData(const CStreamData &rhs);
	CStreamData &operator=(const CStreamData &rhs);
};