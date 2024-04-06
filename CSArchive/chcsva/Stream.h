// Stream.h: interface for the CStream class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CVidDecoder;

//////////////////////////////////////////////////////////////////////
// CStream
//////////////////////////////////////////////////////////////////////

class CStream
	: public Elvees::CNodeList::Node
	, public CHCS::IStream
{
protected:
	explicit CStream(UUID& streamUID);
	virtual ~CStream();

	// Set UID to ID
	void SetID(long streamID);
	friend class CManager;

public:
	// IStream
	virtual long AddRef();		// Increment reference counter
	virtual long Release();		// Decrement it

	// CStream
	virtual void Delete() = 0;	// Destroy object

	virtual bool IsDeviceString(LPCTSTR stDevice);
	virtual bool IsValid();

	virtual bool IsLocal();

	virtual void SetStreamFormat(BITMAPINFO *pbiOutput);
	virtual void SetStreamBuffer(void* pvBits, long cbBits);

	bool IsStreamUID(UUID& streamUID);
	
	UUID GetUID();
	long GetID();

	void InitDecompressor(DWORD dwFCC, BITMAPINFO *pbiOutput, BITMAPINFO *pbiInput);
	void DecompressData(const void* pData, long lSize, unsigned uFrameSeq);

protected:
	// Stream settings
	bool  m_bEnabled;
	TCHAR m_stMoniker[MAX_PATH]; // Stream specific data

#ifndef _DEBUG
private:
#else
	TCHAR m_stState[64];
	LPCTSTR DecoderState();

	friend class CStreamClient;
	Elvees::CSocketServer::Socket* m_pSocket;
#endif

	long m_lStreamID;	// Stream ID
	UUID m_StreamUID;	// Unique stream ID

	CVidDecoder* m_pDecoder;

	// Reference count
	long m_refCount;

private:
	// No copies do not implement
	CStream(const CStream &rhs);
	CStream &operator=(const CStream &rhs);
};

typedef Elvees::TNodeList<CStream> CStreamList;