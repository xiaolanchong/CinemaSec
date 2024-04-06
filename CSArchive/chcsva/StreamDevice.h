// StreamDevice.h: interface for the CStreamDevice class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Device.h"
#include "Stream.h"
#include "FrmBuffer.h"

class  CVidEncoder;
struct IAVIStream;

//////////////////////////////////////////////////////////////////////
// CStreamDevice
//////////////////////////////////////////////////////////////////////

class CStreamDevice
	: public CFrmBuffer::Allocator
	, public CDevice
	, public CStream
{
private:
	explicit CStreamDevice(UUID& streamUID);
	virtual ~CStreamDevice();

public:
	// Create stream
	//

	static CStream* CreateStream(
		UUID& streamID,			// Unique stream ID
		LPCTSTR stMoniker,		// Device moniker
		long lWidth = 0,		// Width
		long lHeight = 0,		// Height
		long lFrmRate = 0,		// Frame rate
		long lInput = 0,		// Input
		long lVStandard = 0,	// Video Standart (see DX documentation)
		bool bBWMode = false);	// Force Black-White video

	// CStream
	//

	virtual void Delete();

	virtual bool IsValid();
	virtual bool IsDeviceString(LPCTSTR stDevice);

	// IStream implementation
	//

	virtual bool IsStarted();	//
	virtual bool GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout);

	virtual bool Start();
	virtual void Stop();

	virtual long SeekTo(INT64 timePos);

	// CDevice

	virtual long GetDeviceID() { return GetID(); };

private:
	// Called in context of SampleGrabber filter thread
	virtual void OnBufferCB(double fTime, BYTE *pData, long lLen);
	// Accept stream format
	virtual bool OnImageFormat(BITMAPINFOHEADER* pbiImg);
	// Allocator source ID request
	virtual long AllocatorSourceID();

	void SetLastFrame(CHCS::IFrame*  pFrame);
	bool GetLastFrame(CHCS::IFrame** ppFrame);

	CHCS::IFrame *m_pFrameLast;

	//////////////////////////////////////////////////////////////////////////
	// Compress

	bool IsFrameNeeded(CHCS::IFrame* pFrame);
	void CompressFrame(CHCS::IFrame* pFrame);

	long m_lFPM;
	long m_lVMS;

	long m_lLastFrame;
	unsigned m_uTimeStart;
	
	CVidEncoder* m_pEncoder;

	//////////////////////////////////////////////////////////////////////////
	// Normalize image

	DWORD *m_arHisto;
	BYTE  *m_pFrameOut;

	BITMAPINFOHEADER m_bihOut;

	const double fThresh;
	const double fCoef;

	//////////////////////////////////////////////////////////////////////////
	// Archive

	long m_lVideoSize;
	long m_lStartFrame;
	
	IAVIStream *m_pArchive;

private:
	DWORD m_dwLastUpdated;
	Elvees::CManualResetEvent m_newImage;

	bool m_bSendKey;
	Elvees::CIOBuffer::Allocator m_CompressedFrames;
	Elvees::CIOBuffer* m_pFormatData;

	Elvees::CCriticalSection m_lastFrameSect;
	
	// No copies do not implement
	CStreamDevice(const CStreamDevice &rhs);
	CStreamDevice &operator=(const CStreamDevice &rhs);
};