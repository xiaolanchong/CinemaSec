// StreamMMF.h: interface for the CStreamMMF class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Stream.h"
#include "FrmBuffer.h"

//////////////////////////////////////////////////////////////////////
// CStreamDevice
//////////////////////////////////////////////////////////////////////

class CStreamMMF
	: public CStream
	, public CFrmBuffer::Allocator
{
private:
	explicit CStreamMMF(UUID& streamUID, bool bLocal);
	virtual ~CStreamMMF();

public:
	// Create stream
	//

	static CStream* CreateStream(
		UUID& streamID,			// Unique stream ID
		LPCTSTR stMoniker,		// Device UID or IP
		bool bLocal = true);

	// CStream
	//

	virtual void Delete();

	virtual bool IsValid();
	virtual bool IsDeviceString(LPCTSTR stDevice);

	virtual bool IsLocal();

	// IStream implementation
	//

	virtual bool IsStarted();	//
	virtual bool GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout);

	virtual bool Start();
	virtual void Stop();

	virtual long SeekTo(INT64 timePos);

private:
	// Allocator source ID request
	virtual long AllocatorSourceID();
	virtual void SetStreamFormat(BITMAPINFO *pbiOutput);
	virtual void SetStreamBuffer(void* pvBits, long cbBits);

private:
	DWORD m_dwLastUpdated;

	bool m_bLocal;
	bool m_bFormatSet;

	void SetLastFrame(CHCS::IFrame* pFrame);
	bool GetLastFrame(CHCS::IFrame** ppFrame);

	CHCS::IFrame *m_pFrameLast;
	Elvees::CCriticalSection m_lastFrameSect;
	Elvees::CManualResetEvent m_newImage;

private:
	// No copies do not implement
	CStreamMMF(const CStreamMMF &rhs);
	CStreamMMF &operator=(const CStreamMMF &rhs);
};