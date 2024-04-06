// ArchiveReader.h: interface for the CArchiveReader class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

// {00020000-0000-0000-C000-000000000046}
DEFINE_GUID(defaultAviHandler,
0x00020000, 0, 0, 0xC0, 0, 0, 0, 0, 0, 0, 0x46);

class CArchiveServer;

///////////////////////////////////////////////////////////////////////////////
// CArchiveReader
///////////////////////////////////////////////////////////////////////////////

class CArchiveReader : public Elvees::CThread
{
public:
	explicit CArchiveReader(CArchiveServer* pServer, Elvees::CSocketServer::Socket *pSocket);
	virtual ~CArchiveReader();

	void ProcessCommand(LPCSTR stCommand, unsigned nSize);
	void Shutdown();

protected:
	virtual int Run();

	long GetFileLength(LPCTSTR stFile);
	bool FindFileByTime(INT64* pTime, LPTSTR stFile, long cbFile, long* plgap);

	bool OpenFile(LPCTSTR stFilename);
	void CloseFile();

private:
	long m_lSourceID;							// Source ID
	CArchiveServer *m_pServer;					// Server

	INT64 m_timePos;

	// avi file data
	long m_cbCompressed;
	void *m_pCompressed;

	PAVISTREAM m_pStream;
	AVISTREAMINFOW m_avisInfo;

	//

	Elvees::CManualResetEvent      m_timeEvent;
	Elvees::CManualResetEvent      m_shutdownEvent;
	
	Elvees::CSocketServer::Socket *m_pSocket;		// Connected socket
	Elvees::CIOBuffer             *m_pFormatPacket;	// Format packet
	
	// No copies do not implement
	CArchiveReader(const CArchiveReader &rhs);
	CArchiveReader &operator=(const CArchiveReader &rhs);
};