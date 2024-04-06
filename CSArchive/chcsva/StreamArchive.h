// StreamArchive.h: interface for the CStreamArchive class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "FrmBuffer.h"

class CVidDecoder;

//////////////////////////////////////////////////////////////////////
// CStream
//////////////////////////////////////////////////////////////////////

class CStreamArchive
	: public CHCS::IStream
	, public CFrmBuffer::Allocator
{
protected:
	CStreamArchive(long lSourceID);
	virtual ~CStreamArchive();

public:
	static CStreamArchive* CreatStream(long lSourceID, INT64 startPos);

	virtual long AddRef();
	virtual long Release();
	virtual bool Start();
	virtual void Stop();
	virtual bool IsStarted();
	virtual long SeekTo(INT64 timePos);
	virtual bool GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout);

	long GetID();
	void SetConnection(Elvees::CSocketServer::Socket* pSocket);

	void InitDecompressor(DWORD dwFCC, BITMAPINFO *pbiOutput, BITMAPINFO *pbiInput);
	void DecompressData(const void* pData, long lSize, unsigned uFrameSeq, unsigned uTime);

private:
	// Allocator source ID request
	virtual long AllocatorSourceID();

	void SetLastFrame(CHCS::IFrame*  pFrame);
	bool GetLastFrame(CHCS::IFrame** ppFrame);

	CHCS::IFrame *m_pFrameLast;
	Elvees::CManualResetEvent m_newImage;
	Elvees::CCriticalSection m_lastFrameSect;

private:
	long m_refCount;

	const long m_lStreamID;
	CVidDecoder* m_pDecoder;

	Elvees::CSocketServer::Socket* m_pSocket;

	// No copies do not implement
	CStreamArchive(const CStreamArchive &rhs);
	CStreamArchive &operator=(const CStreamArchive &rhs);
};