// VidDecoder.h: interface for the CVidDecoder class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define STRAIGHT_DECODER_ACCESS

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

class CVidDecoder
{
public:
	explicit CVidDecoder(DWORD fccHandler, funcDriverProc fDriverProc = NULL);
	virtual ~CVidDecoder();

	bool InitDecompressor(BITMAPINFO *pbiOutput, BITMAPINFO *pbiInput = NULL);
	void FreeDecompressor();

	bool StartDecompression();
	void StopDecompression();

	bool IsDecompressionInited();
	bool IsDecompressionStarted();

	long GetInFormat(BITMAPINFO* pbi);
	long GetOutFormat(BITMAPINFO* pbi);
	
	void* DecompressData(const void* pData, long lSize, bool bKeyframe);
	void* DecompressData(const void* pData, long lSize, unsigned uFrameSeq);

	long GetBufferSize();

protected:

	LRESULT DriverProc(
		DWORD dwDriverId, HDRVR hDriver, 
		UINT uMsg, LPARAM lParam1, LPARAM lParam2);

private:

	// Decoder data
	DWORD          m_fccHandler;		// Codec FOURCC
	funcDriverProc m_fDriverProc;		// Codec proc function

#ifdef STRAIGHT_DECODER_ACCESS
	DWORD	m_hic;						// Compressor
	ICOPEN  m_icOpen;
	HMODULE m_hDriver;					// It's module
#else
	HIC		m_hic;
#endif

	BITMAPINFO	*m_pbiInput;			// internally initialized
	BITMAPINFO	*m_pbiOutput;			// Init

	long m_cbOutBuffer;
	char *m_pOutBuffer;					// Decompress to my buffer

	bool m_bInited;						// Decoder state
	bool m_bStarted;

#ifdef _DEBUG
public:
#endif

	bool m_bWaitIFrame;					// for sequence decoding
	unsigned short m_uFrameSeq;			// Last frame sequence
	unsigned short m_uIFrameSeq;		// Last key frame sequence
	
private:
	// No copies do not implement
	CVidDecoder(const CVidDecoder &rhs);
	CVidDecoder &operator=(const CVidDecoder &rhs);
};
