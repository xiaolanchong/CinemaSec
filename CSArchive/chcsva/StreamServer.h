// StreamServer.h: interface for the CStreamServer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CStreamServerEvents;

///////////////////////////////////////////////////////////////////////////////
// CStreamServer
///////////////////////////////////////////////////////////////////////////////

class CStreamServer : public Elvees::CSocketServer
{
public:
	explicit CStreamServer(
		CStreamServerEvents* pCallback,
		unsigned long addressToListenOn,
		unsigned short portToListenOn,
		unsigned maxFreeSockets,
		unsigned maxFreeBuffers,
		unsigned bufferSize,
		unsigned numThreads = 0);

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

public:
	bool SendFrame(Elvees::CIOBuffer *pData);

private:
	CStreamServerEvents *m_pCallback;

	// No copies do not implement
	CStreamServer(const CStreamServer &rhs);
	CStreamServer &operator=(const CStreamServer &rhs);
};