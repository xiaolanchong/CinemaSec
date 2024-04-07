// BaseFrameGrabberVfW.cpp: implementation of the BaseFrameGrabberVfW class.
//
//////////////////////////////////////////////////////////////////////

//#if 0
	#include "stdafx.h"
//#else
	#include <windows.h>

	#include <tchar.h>		// _vsntprintf
	#include <stdio.h>		// formated output

	#include <vfw.h>
	#pragma comment(lib, "Vfw32.lib")

	typedef const BITMAPINFO    *LPCBITMAPINFO;
	typedef const unsigned char *LPCUBYTE;

	#define countof(x) (sizeof(x)/sizeof(x[0]))
//#endif

#include "BaseFrameGrabberVfW.h"

//////////////////////////////////////////////////////////////////////
// Utilities
//////////////////////////////////////////////////////////////////////

BaseFrameGrabberVfW* BaseFrameGrabberVfW::CreateGrabber()
{
	// just to verify, that this is not abstract class
	return new BaseFrameGrabberVfW(0, true, true);
}

BOOL BaseFrameGrabberVfW::IsFileExists(LPCTSTR stFile)
{
	HANDLE hFindFile;
	WIN32_FIND_DATA wfd;

	if((hFindFile = ::FindFirstFile(stFile, &wfd)) == INVALID_HANDLE_VALUE)
		return FALSE;

	::FindClose(hFindFile);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////

BaseFrameGrabberVfW::BaseFrameGrabberVfW(
		int  nBitPerPixel,
		bool bPerFrameMode,
		bool bLoopback,
		DWORD dwWidth,
		DWORD dwHeight)
	: m_nBitPerPixel(nBitPerPixel)
	, m_bPerFrameMode(bPerFrameMode)
	, m_bLoopback(bLoopback)
	, m_dwDesiredWidth(dwWidth)
	, m_dwDesiredHeight(dwHeight)
{
	m_bInited = false;

	m_pStream = NULL;
	m_pbiStream = NULL;
	m_pFrame = NULL;
	m_cbFrame = 0;

	m_stScriptFile[0] = 0;

	m_FPS = 0.0f;
	m_Frame = 0L;
	m_FrameStart = 0L;
	m_FrameTotal = 0L;

	// If library already inited, it will increment counter...
	CoInitialize(NULL);

#ifdef BFG_Y800_OUTPUT

	ZeroMemory(&m_biY800, sizeof(BITMAPINFOHEADER));
	m_biY800.biSize = sizeof(BITMAPINFOHEADER);
	m_biY800.biBitCount = 8;
	m_biY800.biPlanes   = 1;
	m_biY800.biCompression = mmioFOURCC('Y','8','0','0');

	m_cbFrameY800 = 0;
	m_pFrameY800 = NULL;

#endif
}

BaseFrameGrabberVfW::~BaseFrameGrabberVfW()
{
	CloseInterfaces();
	CoUninitialize();
}

//////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////

bool BaseFrameGrabberVfW::CreateScriptFile(LPCTSTR lpFilename)
{
	DWORD dwNumber;
	HANDLE hScriptFile;

	bool bDone = false;
	TCHAR stFullPath[MAX_PATH];
	TCHAR stTempPath[MAX_PATH];

	// Is it full path?
	//
	
	if(::GetCurrentDirectory(countof(stFullPath), stFullPath) == 0)
	{
		Output(TEXT("Fail to get current directory Res=%ld"), ::GetLastError());
		return false;
	}

	dwNumber = lstrlen(lpFilename);
	while(dwNumber && lpFilename[dwNumber - 1] != TEXT('\\')) dwNumber--;

	if(dwNumber)
		lstrcpy(stFullPath, lpFilename);
	else
	{
		lstrcat(stFullPath, TEXT("\\"));
		lstrcat(stFullPath, lpFilename);
	}

	// Get temporal folder path
	//

	::GetTempPath(countof(stTempPath), stTempPath);
	::GetTempFileName(stTempPath, TEXT("BFG"), 0, m_stScriptFile);

	hScriptFile = CreateFile(m_stScriptFile,  // file name 
		GENERIC_READ | GENERIC_WRITE, // open for read/write 
		0,                            // do not share 
		NULL,                         // default security 
		CREATE_ALWAYS,                // overwrite existing file
		FILE_ATTRIBUTE_NORMAL,        // normal file 
		NULL);                        // no template 

	if(hScriptFile != INVALID_HANDLE_VALUE)
	{
		// AviSynth read only ACII files...
		char stTemp[MAX_PATH*2];

	#ifdef _UNICODE
		wsprintfA(stTemp, "AviSource(\"%ls\",audio=false,pixel_type=\"full\",fourCC=\"YUY2\")\r\n", stFullPath);
	#else
		wsprintfA(stTemp, "AviSource(\"%hs\",audio=false,pixel_type=\"YUY2\")\r\n", stFullPath);
	#endif

		::SetFilePointer(hScriptFile, 0, NULL, FILE_END);

		bDone = ::WriteFile(hScriptFile,
			stTemp, (DWORD)strlen(stTemp),
			&dwNumber, NULL) ? true : false;

		::CloseHandle(hScriptFile);
	}

	return bDone;
}

bool BaseFrameGrabberVfW::OpenInterfaces(LPCTSTR lpFilename)
{
	HRESULT hr;
	PAVIFILE paviFile;
	AVISTREAMINFOW avisInfo;
	
	long lFormatSize;
	
	m_bInited = false;
	paviFile = NULL;

	// AviSynth Script Handler Object
	static const CLSID avsHandler = 
		{ 0xE6D6B700, 0x124D, 0x11D4, { 0x86, 0xF3, 0xDB, 0x80, 0xAF, 0xD9, 0x87, 0x78 } };

	// Open stream from video file
	//

	do
	{
		if(!IsFileExists(lpFilename))
		{
			Output(TEXT("File \"%s\" doesn't exists"), lpFilename);
			break;
		}

		if(!CreateScriptFile(lpFilename))
		{
			Output(TEXT("File to create script file"));
			break;
		}

		hr = AVIFileOpen(&paviFile,
			m_stScriptFile,	// file name
			OF_READ,		// mode to open file with
			const_cast<LPCLSID>(&avsHandler));

		if(hr != AVIERR_OK || !paviFile)
		{
			Output(TEXT("Fail to open file \"%s\" hr=0x%08X"),
				lpFilename, hr);
			break;
		}

		hr = paviFile->GetStream(&m_pStream, streamtypeVIDEO, 0L);
		if(hr != AVIERR_OK)
		{
			Output(TEXT("Fail get video stream hr=0x%08X"), hr);
			break;
		}

		hr = m_pStream->Info(&avisInfo, sizeof(avisInfo));
		if(hr != AVIERR_OK)
		{
			Output(_T("Fail get video stream info hr=0x08X"), hr);
			break;
		}

		lFormatSize = 0; 

		hr = m_pStream->ReadFormat(0L, NULL, &lFormatSize);
		if(SUCCEEDED(hr) && lFormatSize > 0)
		{
			m_pbiStream = (LPBITMAPINFOHEADER)malloc(lFormatSize);
			if(!m_pbiStream)
			{
				Output(TEXT("Memory operation failed (format)"));
				break;
			}

			hr = m_pStream->ReadFormat(0L, m_pbiStream, &lFormatSize);
			if(hr != AVIERR_OK)
			{
				free(m_pbiStream);
				m_pbiStream = NULL;

				Output(TEXT("Fail get stream stream format hr=0x08X"), hr);
				break;
			}
		}
		else
		{
			Output(TEXT("Unknown stream format"));
			break;
		}

		// If shit happens AviSynth generate
		//  avi with RGB32 or RGB24 format

		if(m_pbiStream->biCompression != mmioFOURCC('Y','U','Y','2'))
		{
			Output(TEXT("Unexpected: invalid stream format"));
			break;
		}

		// Adjust image size if needed
		m_pbiStream->biSizeImage = ((2 * m_pbiStream->biWidth + 3) & ~3) * labs(m_pbiStream->biHeight);

		m_cbFrame = (m_pbiStream->biSizeImage + 0xFFF) & ~0xFFF; // 4K round
		m_pFrame = malloc(m_cbFrame);
		if(!m_pFrame)
		{
			Output(TEXT("Memory operation failed (frame %ld)"), m_cbFrame);
			break;
		}

	#ifdef BFG_Y800_OUTPUT

		// Alloc buffer for Y800 convertion
		m_biY800.biWidth  = m_pbiStream->biWidth;
		m_biY800.biHeight = m_pbiStream->biHeight;
		m_biY800.biSizeImage = ((m_biY800.biWidth + 3) & ~3) * labs(m_biY800.biHeight);

		m_cbFrameY800 = (m_biY800.biSizeImage + 0xFFF) & ~0xFFF; // 4K round;
		m_pFrameY800 = malloc(m_cbFrameY800);
		if(!m_pFrameY800)
		{
			Output(TEXT("Memory operation failed (frame Y8 %ld)"), m_cbFrameY800);
			break;
		}

	#endif

		m_bInited = true;
	}
	while(false);

	if(paviFile)
		paviFile->Release(); // doesn't need anymore

	if(!m_bInited)
	{
		CloseInterfaces();
		return false;
	}

	m_FrameStart = avisInfo.dwStart;
	m_FrameTotal = avisInfo.dwLength;

	m_FPS = avisInfo.dwRate/(double)avisInfo.dwScale;

	m_Frame = m_FrameStart;

	return true;
}

void BaseFrameGrabberVfW::CloseInterfaces()
{
	if(m_pStream)
		m_pStream->Release();

	if(m_pbiStream)
		free(m_pbiStream);

	if(m_pFrame)
		free(m_pFrame);

	if(IsFileExists(m_stScriptFile))
		::DeleteFile(m_stScriptFile);

	m_pStream = NULL;
	m_pbiStream = NULL;
	m_pFrame = NULL;
	m_cbFrame = 0;

	m_stScriptFile[0] = 0;

	m_FPS = 0.0f;
	m_Frame = 0L;
	m_FrameStart = 0L;
	m_FrameTotal = 0L;

	m_bInited = false;

#ifdef BFG_Y800_OUTPUT

	if(m_pFrameY800)
		free(m_pFrameY800);

	m_biY800.biWidth = 0;
	m_biY800.biHeight = 0;
	m_biY800.biSizeImage = 0;

	m_cbFrameY800 = 0;
	m_pFrameY800 = NULL;

#endif

}

//////////////////////////////////////////////////////////////////////
// BaseFrameGrabber implementation
//////////////////////////////////////////////////////////////////////
 
bool BaseFrameGrabberVfW::Start(LPCTSTR lpFilename, const FrameGrabberParameters& fgFaram)
{
	if(!lpFilename)
		return false;

	return OpenInterfaces(lpFilename);
}

bool BaseFrameGrabberVfW::Stop()
{
	m_Frame = m_FrameStart;
	return true;
}

bool BaseFrameGrabberVfW::LockLatestFrame(const BITMAPINFO* &pHeader, const BYTE* &pImage, int &nImgSize)
{
	if(!m_bInited)
		return false;

	long lSize;
	HRESULT hr;

	bool bDone = false;

	while(m_Frame < m_FrameTotal)
	{
		hr = m_pStream->Read(
			m_Frame,
			1,
			m_pFrame,
			m_cbFrame,
			&lSize,
			NULL);

		if(hr == AVIERR_OK)
		{
			if(lSize > 0)
			{
			#ifdef _DEBUG
				Output(TEXT("Frame %ld locked"), m_Frame);
			#endif

				bDone = true;

			#ifdef BFG_Y800_OUTPUT

				YUY2toY800(m_pbiStream, m_pFrame, m_pFrameY800);

				pHeader = reinterpret_cast<const BITMAPINFO*>(&m_biY800);
				pImage  = reinterpret_cast<const BYTE*>(m_pFrameY800);
				nImgSize = (int)m_biY800.biSizeImage;

			#else

				pHeader = reinterpret_cast<const BITMAPINFO*>(m_pbiStream);
				pImage  = reinterpret_cast<const BYTE*>(m_pFrame);
				nImgSize = (int)m_pbiStream->biSizeImage;

			#endif

				m_Frame++;
				break;
			}
			else
				Output(_T("Frame %ld empty"), m_Frame);
		}
		else
			Output(TEXT("No Frame %ld"), m_Frame);

		m_Frame++;
	}

	return bDone;
}

bool BaseFrameGrabberVfW::UnlockProcessedFrame()
{
	return true;
}

bool BaseFrameGrabberVfW::IsOk() const
{
	return m_bInited;
}

bool BaseFrameGrabberVfW::IsEnd() const
{
	return m_Frame >= m_FrameTotal ? true : false;
}

bool BaseFrameGrabberVfW::HasFreshFrame() const
{
	return !IsEnd();
}

//////////////////////////////////////////////////////////////////////
// BaseFrameGrabberEx implementation
//////////////////////////////////////////////////////////////////////

DWORD BaseFrameGrabberVfW::GetVersion()
{
	return MAKELONG(1,5);
}

DWORD BaseFrameGrabberVfW::GetSize(DWORD& x, DWORD& y)
{
	if(m_pbiStream)
	{
		x = (DWORD)m_pbiStream->biWidth;
		y = (DWORD)m_pbiStream->biHeight;

		return 0;
	}

	return 1;
}

DWORD BaseFrameGrabberVfW::GetPos(float& fPos)
{
	fPos = m_FrameTotal > 0 ? m_Frame/(float)m_FrameTotal : 0.0f;
	return 0;
}

//////////////////////////////////////////////////////////////////////
// IBaseGrabber implementation
//////////////////////////////////////////////////////////////////////

HRESULT BaseFrameGrabberVfW::LockFrame(const BITMAPINFO* &pHeader, const BYTE* &pImage, int &nImgSize)
{
	return LockLatestFrame(pHeader, pImage, nImgSize) ? S_OK : E_FAIL;
}

HRESULT BaseFrameGrabberVfW::UnlockFrame()
{
	return UnlockProcessedFrame() ? S_OK : E_FAIL;
}

void BaseFrameGrabberVfW::Release()
{
#ifdef _DEBUG
	Output(TEXT("Grabber released"));
#endif

	delete this;
}

//////////////////////////////////////////////////////////////////////
// IFileGrabber implementation
//////////////////////////////////////////////////////////////////////

HRESULT BaseFrameGrabberVfW::GetSize(SIZE &sizeFrame)
{
	if(m_pbiStream)
	{
		sizeFrame.cx = (int)m_pbiStream->biWidth;
		sizeFrame.cy = (int)m_pbiStream->biHeight;

		return S_OK;
	}

	return E_FAIL;
}

HRESULT BaseFrameGrabberVfW::GetCurFrame(INT64 &nCurPos)
{
	nCurPos = (INT64)m_Frame;
	return S_OK;
}

HRESULT BaseFrameGrabberVfW::GetTotalFrames(INT64 &nTotal)
{
	nTotal = (INT64)m_FrameTotal;
	return S_OK;
}

HRESULT BaseFrameGrabberVfW::Seek(INT64 nCurPos)
{
	HRESULT hr = S_OK;
	m_Frame = (DWORD)nCurPos;

	if(m_Frame < m_FrameStart)
	{
		m_Frame = m_FrameStart;
		hr = S_FALSE;
	}

	if(m_Frame > m_FrameTotal)
	{
		m_Frame = m_FrameTotal;
		hr = S_FALSE;
	}

	return hr;
}

HRESULT BaseFrameGrabberVfW::GetFPS(double &fFPS)
{
	fFPS = m_FPS;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Output functions
//////////////////////////////////////////////////////////////////////////

inline void BaseFrameGrabberVfW::Output(LPCTSTR stFormat, ...)
{
	int nRet;
	TCHAR stMsg[2*MAX_PATH];

	// Format the input string
	va_list pArgs;
	va_start(pArgs, stFormat);
	nRet = _vsntprintf(stMsg, countof(stMsg) - 1, stFormat, pArgs);
	va_end(pArgs);

	if(nRet < 0)
		stMsg[countof(stMsg) - 1] = 0;

	// Add info
	TCHAR stOut[2*MAX_PATH];

	SYSTEMTIME stLocalTime;
	GetLocalTime(&stLocalTime);

	wsprintf(stOut, TEXT("%02hd:%02hd:%02hd [%X] %s\n"),
		stLocalTime.wHour,
		stLocalTime.wMinute,
		stLocalTime.wSecond,
		this, stMsg);

	OutputDebugString(stOut);
}

//////////////////////////////////////////////////////////////////////////
// Converter
//////////////////////////////////////////////////////////////////////////

#ifdef BFG_Y800_OUTPUT

void BaseFrameGrabberVfW::YUY2toY800(LPBITMAPINFOHEADER lpbiIn, const void* pSrc, void* pDst)
{
	const BYTE *pYUY2;
	BYTE *pY8;

	long lCol, lRow;
	long lWidthY8 = (lpbiIn->biWidth + 3L) & (~3L);

	for(lRow = 0; lRow < labs(lpbiIn->biHeight); lRow++)
	{
		pY8 = reinterpret_cast<BYTE*>(pDst) + lWidthY8 * lRow;
		pYUY2 = reinterpret_cast<const BYTE*>(pSrc) + 2 * lpbiIn->biWidth * lRow;

		for(lCol = 0; lCol < lpbiIn->biWidth; lCol++)
		{
			pY8[0] = pYUY2[0];

			pY8 += 1;
			pYUY2 += 2;
		}
	}
}

#endif