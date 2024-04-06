// ArchiveServer.h: interface for the CArchiveServer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////
// CArchiveServer
///////////////////////////////////////////////////////////////////////////////

class CArchiveServer : public Elvees::CSocketServer
{
public:
	explicit CArchiveServer(
		unsigned long addressToListenOn,
		unsigned short portToListenOn,
		unsigned maxMessageSize,
		unsigned maxFreeSockets,
		unsigned maxFreeBuffers,
		unsigned bufferSize,
		unsigned numThreads = 0);
	virtual ~CArchiveServer();

	LPCTSTR GetVideoBasePath();
	Elvees::CIOBuffer *AllocateFrame();

private:
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

	// Process request

	unsigned GetMessageSize(const Elvees::CIOBuffer *pBuffer) const;

	Elvees::CIOBuffer* ProcessDataStream(
		Elvees::CSocketServer::Socket *pSocket,
		Elvees::CIOBuffer *pBuffer);

	void ProcessCommand(
		Elvees::CSocketServer::Socket *pSocket,
		Elvees::CIOBuffer *pBuffer);

private:
	long  m_lSessionRef;			// Sessions counter
	TCHAR m_stBasePath[MAX_PATH];	// Archive base path

	Elvees::CIOBuffer::Allocator m_videoFrames;

	// No copies do not implement
	CArchiveServer(const CArchiveServer &rhs);
	CArchiveServer &operator=(const CArchiveServer &rhs);
};