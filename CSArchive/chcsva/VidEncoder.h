// VidEncoder.h: interface for the CVidEncoder class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define STRAIGHT_ENCODER_ACCESS
#define STRAIGHT_ENCODER_ACCESS_SAVE

//////////////////////////////////////////////////////////////////////
// VFW Codec DriverProc prototype
//////////////////////////////////////////////////////////////////////

#ifndef _DRIVERPROC_PROTO_
#define _DRIVERPROC_PROTO_

typedef LRESULT (__stdcall *funcDriverProc)(
	DWORD dwDriverId,
	HDRVR hDriver,
	UINT  uMsg,
	LPARAM lParam1,
	LPARAM lParam2);

#endif

//////////////////////////////////////////////////////////////////////
// CVidCompressor
//////////////////////////////////////////////////////////////////////

class CVidEncoder
{
public:
	explicit CVidEncoder(DWORD fccHandler, funcDriverProc fDriverProc = NULL);
	virtual ~CVidEncoder();

	bool InitCompressor(BITMAPINFO *pbiInput,
		long lUsPerFrame = 40000,		// = 25 fps
		long lQuality  = 10000,			// maximum
		long lKeyRate  = 0,				// don't force keyrate
		long lDataRate = 0,
		void *lpState  = NULL,			// state of compressor
		long cbState   = 0,				// size of the state
		long lFrameCount = MAXLONG);	// stream incoding

	bool InitCompressor(COMPVARS* pCodec,
		long lUsPerFrame = 40000,		// = 25 fps
		long lFrameCount = MAXLONG);	// stream incoding

	void FreeCompressor();

	bool StartCompression();
	void StopCompression();

	bool IsCompressionInited();
	bool IsCompressionStarted();

	void* CompressData(const void* pData, long *plSize, bool *pbKeyframe, unsigned *puFrameNum = NULL);
	void  DropFrame();

	DWORD GetFOURCC() const;

	long GetOutFormat(BITMAPINFO* pbi);
	long GetInFormat(BITMAPINFO* pbi);

	long GetMaxPackedSize() const;

protected:
	// Settings
	DWORD          m_fccHandler;	// Codec FOURCC
	funcDriverProc m_fDriverProc;	// Codec proc function

	long m_lQuality;				// Can be changed internally
	long m_lKeyRate;				//	

	void *m_pConfigData;			// Codec state
	int	  m_cbConfigData;			// Codec state size

	// State flags
	bool m_bInited;
	bool m_bStarted;

#ifdef STRAIGHT_ENCODER_ACCESS
	DWORD	m_hic;					// Compressor
	ICOPEN  m_icOpen;
	HMODULE m_hDriver;				// It's module
#else
	HIC	m_hic;
#endif

	long m_lFrameNum;				// Compressed frame count (w/o failed)
	long m_lFrameCompressed;		// Internal codec frame count

	unsigned short m_uFrameSeq;		// Frame counter
	unsigned short m_uIFrameSeq;	// I-Frame counter

	// Datarate control
	long m_lKeyRateCounter;			// Delta frames counter
	long m_lMaxPackedSize;			// Compressor max size
	long m_lMaxFrameSize;			//
	long m_lSlopSpace;				//

	char *m_pOutBuffer;				//
	char *m_pPrvBuffer;				// For compressor such as 'Microsoft Video 1'

	BITMAPINFO *m_pbiInput;			// Init
	BITMAPINFO *m_pbiOutput;		// internally initialized

private:
// Codec Entry point
#ifdef STRAIGHT_ENCODER_ACCESS
	#ifdef STRAIGHT_ENCODER_ACCESS_SAVE
		LRESULT DriverProc(				// Save entry point
			DWORD dwDriverId,
			HDRVR hDriver, 
			UINT uMsg,
			LPARAM lParam1,
			LPARAM lParam2);
	#else
		funcDriverProc DriverProc;		// Codec proc function
	#endif
#endif

	// No copies do not implement
	CVidEncoder(const CVidEncoder &rhs);
	CVidEncoder &operator=(const CVidEncoder &rhs);
};
