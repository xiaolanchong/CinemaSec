// VidDecoder.cpp: implementation of the CVidDecoder class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "VidDecoder.h"

//////////////////////////////////////////////////////////////////////
// CVidDecoder
//////////////////////////////////////////////////////////////////////

CVidDecoder::CVidDecoder(DWORD fccHandler, funcDriverProc fDriverProc)
	: m_fccHandler(fccHandler)
	, m_fDriverProc(fDriverProc)
{
	m_hic = 0;

	m_pbiInput   = NULL;
	m_pbiOutput  = NULL;

	m_pOutBuffer = NULL;
	m_cbOutBuffer = 0;

#ifdef STRAIGHT_DECODER_ACCESS
	m_hDriver = NULL;

	if(m_fccHandler == mmioFOURCC('Y','U','Y','2') && m_fDriverProc == NULL)
	{
		m_hDriver = LoadLibrary(TEXT("msyuv.dll"));
		m_fDriverProc = (funcDriverProc)::GetProcAddress(m_hDriver, "DriverProc");
	}
#endif

	m_bInited  = false;
	m_bStarted = false;

	m_bWaitIFrame = true;

	m_uFrameSeq  = 0;
	m_uIFrameSeq = 0;
}

CVidDecoder::~CVidDecoder()
{
	FreeDecompressor();

#ifdef STRAIGHT_DECODER_ACCESS
	if(m_hDriver)
		FreeLibrary(m_hDriver);
#endif
}

bool CVidDecoder::InitDecompressor(BITMAPINFO *pbiOutput, BITMAPINFO *pbiInput)
{
	if(!pbiOutput)
		return false;

	if(m_bInited)
		FreeDecompressor();
	
	int cbSize;

	do
	{
		// Alloc output format
		//

		if(!pbiOutput->bmiHeader.biSize)
			break;

		cbSize = pbiOutput->bmiHeader.biSize + pbiOutput->bmiHeader.biClrUsed*4;
		m_pbiOutput = (LPBITMAPINFO)calloc( max(cbSize, sizeof BITMAPINFO), 1 );

		if(!m_pbiOutput)
			break;

		CopyMemory(m_pbiOutput, pbiOutput, cbSize);

		m_pbiOutput->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_pbiOutput->bmiHeader.biSizeImage = DIBSIZE(m_pbiOutput->bmiHeader);

		// Alloc input format
		//

		cbSize = pbiInput ? (pbiInput->bmiHeader.biSize + pbiInput->bmiHeader.biClrUsed*4) : 0;
		m_pbiInput = (LPBITMAPINFO)calloc( max(cbSize, sizeof BITMAPINFO), 1 );

		if(!m_pbiInput)
			break;

		if(pbiInput)
		{
		//	m_pbiInput->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		//	m_pbiInput->bmiHeader.biSizeImage = DIBSIZE(m_pbiInput->bmiHeader);
		// ... HuffYUY codec store its settings in pbiInput
			
			CopyMemory(m_pbiInput, pbiInput, cbSize);
		}
		else
		{
			ZeroMemory(m_pbiInput, sizeof BITMAPINFO);

			m_pbiInput->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			m_pbiInput->bmiHeader.biCompression = m_fccHandler;
			m_pbiInput->bmiHeader.biWidth  = m_pbiOutput->bmiHeader.biWidth;
			m_pbiInput->bmiHeader.biHeight = m_pbiOutput->bmiHeader.biHeight;
			// ignored
			m_pbiInput->bmiHeader.biPlanes   = 1;
			m_pbiInput->bmiHeader.biBitCount = 24;
		}

		m_cbOutBuffer = m_pbiOutput->bmiHeader.biSizeImage;
		m_pOutBuffer = (char*)malloc(m_cbOutBuffer);
		if(!m_pOutBuffer)
		{
			Elvees::OutputF(Elvees::TError, TEXT("Memory operation failed buf(%ld)"), m_cbOutBuffer);
			break;
		}

	#ifdef STRAIGHT_DECODER_ACCESS
		if(!m_fDriverProc)
		{
			HIC hDecoder = ICOpen(ICTYPE_VIDEO, m_fccHandler, ICMODE_DECOMPRESS);

			if(!hDecoder || ICERR_OK != ICDecompressQuery(hDecoder, m_pbiInput, m_pbiOutput))
			{
				if(hDecoder)
					ICClose(hDecoder);

				hDecoder = ICLocate(ICTYPE_VIDEO, NULL,
					reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiInput),
					reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiOutput),
					ICMODE_DECOMPRESS);
			}

			if(hDecoder)
			{
				ICINFO info;

				ZeroMemory(&info, sizeof(info));
				info.dwSize = sizeof(info);

				ICGetInfo(hDecoder, &info, sizeof(info));

				if(m_hDriver)
					FreeLibrary(m_hDriver);

			#ifdef _UNICODE
				m_hDriver = LoadLibrary(info.szDriver);
			#else
				char stDriver[MAX_PATH];

				WideCharToMultiByte(CP_ACP, 0, info.szDriver, (int)wcslen(info.szDriver) + 1,
					stDriver, sizeof(stDriver), NULL, NULL);

				m_hDriver = LoadLibrary(stDriver);
			#endif

				if(m_hDriver)
					m_fDriverProc = (funcDriverProc)GetProcAddress(m_hDriver, "DriverProc");

				ICClose(hDecoder);
			}
		}

		if(!m_fDriverProc)
		{
			Elvees::Output(Elvees::TError, TEXT("DriverProc undefined"));
			break;
		}

		ZeroMemory(&m_icOpen, sizeof(m_icOpen));

		m_icOpen.dwSize     = sizeof(m_icOpen);
		m_icOpen.fccType    = ICTYPE_VIDEO;
		m_icOpen.fccHandler = m_fccHandler;
		m_icOpen.dwFlags    = ICMODE_DECOMPRESS;

		m_hic = (DWORD)DriverProc(0, 0, DRV_OPEN, 0, (LPARAM)&m_icOpen);
	#else
		if(m_fDriverProc)
			m_hic = ICOpenFunction(ICTYPE_VIDEO, m_fccHandler,
				ICMODE_DECOMPRESS, (FARPROC)m_fDriverProc);
		else
		{
			m_hic = ICOpen(ICTYPE_VIDEO, m_fccHandler, ICMODE_DECOMPRESS);

			if(!m_hic || ICERR_OK != ICDecompressQuery(m_hic, m_pbiInput, m_pbiOutput))
			{
				if(m_hic)
					ICClose(m_hic);

				m_hic = ICLocate(ICTYPE_VIDEO, NULL,
					reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiInput),
					reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiOutput),
					ICMODE_DECOMPRESS);
			}
		}
	#endif
		if(!m_hic)
		{
			Elvees::Output(Elvees::TError, TEXT("Cant open decompressor"));
			break;
		}

		m_bInited = true;
	}
	while(false);

	if(!m_bInited)
		FreeDecompressor();

	return m_bInited;
}

void CVidDecoder::FreeDecompressor()
{
	if(m_bStarted)
		StopDecompression();

	if(m_pbiInput)
		free(m_pbiInput);

	if(m_pbiOutput)
		free(m_pbiOutput);

	if(m_pOutBuffer)
		free(m_pOutBuffer);

#ifdef STRAIGHT_DECODER_ACCESS
	if(m_hic)
		DriverProc(m_hic, 0, DRV_CLOSE, 0, (LPARAM)&m_icOpen);
#else
	if(m_hic)
		ICClose(m_hic);
#endif

	m_hic = 0;

	m_pbiInput   = NULL;
	m_pbiOutput  = NULL;
	m_pOutBuffer = NULL;

	m_bInited  = false;
}

bool CVidDecoder::StartDecompression()
{
	if(!m_bInited)
		return false;

	if(m_bStarted)
		StopDecompression();

	LRESULT	res;

#ifdef STRAIGHT_DECODER_ACCESS
	res = DriverProc(m_hic, 0, ICM_DECOMPRESS_BEGIN, (LPARAM)m_pbiInput, (LPARAM)m_pbiOutput);
#else
	res = ICDecompressBegin(m_hic, m_pbiInput, m_pbiOutput);
#endif	

	if(res != ICERR_OK)
		return false;

	m_bStarted = true;

	return true;
}

void CVidDecoder::StopDecompression()
{
	if(m_bStarted)
	{
#ifdef STRAIGHT_DECODER_ACCESS
		DriverProc(m_hic, 0, ICM_DECOMPRESS_END, 0, 0);
#else
		ICDecompressEnd(m_hic);
#endif
	}

	m_bStarted = false;
}

void* CVidDecoder::DecompressData(const void* pData, long lSize, unsigned uFrameSeq)
{
	// Check frames sequence
	bool bKeyframe = !(uFrameSeq & 0x0000FFFF);

	if(bKeyframe)
	{
		m_bWaitIFrame = false;
		m_uFrameSeq  = 0;
		m_uIFrameSeq = HIWORD(uFrameSeq);
	}
	else if(!m_bWaitIFrame)
	{
		if(HIWORD(uFrameSeq) == m_uIFrameSeq)
		{
			if(++m_uFrameSeq == 0)
				m_uFrameSeq = 1;

			if(LOWORD(uFrameSeq) != m_uFrameSeq)
				m_bWaitIFrame = true;  // Lost frame
		}
		else
		{
			m_bWaitIFrame = true;      // Lost I-Frame
			DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Lost I-Frame")));
		}
	}
	
	return m_bWaitIFrame ? NULL : DecompressData(pData, lSize, bKeyframe);
}

void* CVidDecoder::DecompressData(const void* pData, long lSize, bool bKeyframe)
{
	if(!m_bStarted)
	{
		if(!StartDecompression())
			return NULL;
	}

	LRESULT res;

	// Decompress!

	m_pbiInput->bmiHeader.biSizeImage = lSize;

#ifdef STRAIGHT_DECODER_ACCESS
	ICDECOMPRESS icDecompress;

	icDecompress.ckid = 0;
	icDecompress.dwFlags = bKeyframe ? 0 : ICDECOMPRESS_NOTKEYFRAME;

	icDecompress.lpbiInput  = reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiInput);
	icDecompress.lpbiOutput = reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiOutput);
	
	icDecompress.lpInput  = const_cast<void*>(pData);
	icDecompress.lpOutput = m_pOutBuffer;

	res = DriverProc(m_hic, 0, ICM_DECOMPRESS, (LPARAM)&icDecompress, sizeof(icDecompress));
#else
	res = ICDecompress(m_hic, 
		bKeyframe ? 0 : ICDECOMPRESS_NOTKEYFRAME,
		reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiInput),  const_cast<void*>(pData),
		reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiOutput), m_pOutBuffer);
#endif

	if(res != ICERR_OK)
	{
		m_bWaitIFrame = true; // Wait for I-Frame if decompression failed
		return NULL;
	}

	return m_pOutBuffer;
}

bool CVidDecoder::IsDecompressionInited()
{
	return m_bInited;
}

bool CVidDecoder::IsDecompressionStarted()
{
	return m_bStarted;
}

long CVidDecoder::GetInFormat(BITMAPINFO* pbi)
{
	if(m_pbiInput == NULL)
		return -1;

	int cbSize = m_pbiInput->bmiHeader.biSize + \
		m_pbiInput->bmiHeader.biClrUsed*4;

	if(pbi == NULL)
		return cbSize;

	__try
	{
		CopyMemory(pbi, m_pbiInput, cbSize);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return 0;
}

long CVidDecoder::GetOutFormat(BITMAPINFO* pbi)
{
	if(m_pbiOutput == NULL)
		return -1;

	int cbSize = m_pbiOutput->bmiHeader.biSize + \
		m_pbiOutput->bmiHeader.biClrUsed*4;

	if(pbi == NULL)
		return cbSize;

	__try
	{
		CopyMemory(pbi, m_pbiOutput, cbSize);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return 0;
}

long CVidDecoder::GetBufferSize()
{
	return m_cbOutBuffer;
}

//////////////////////////////////////////////////////////////////////
// DriverProc - spy on codec and protect from exceptions
// usefully for debug
//////////////////////////////////////////////////////////////////////

LRESULT CVidDecoder::DriverProc(
	DWORD dwDriverId,
	HDRVR hDriver, 
	UINT  uMsg,
	LPARAM lParam1,
	LPARAM lParam2)
{
	LRESULT lRes = ICERR_ERROR;

	if(m_fDriverProc)
	{
		__try
		{
			lRes = m_fDriverProc(dwDriverId, hDriver, uMsg, lParam1, lParam2);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			Elvees::Output(Elvees::TWarning, TEXT("Unexpected exception in DriverProc()"));
		}
	}

	if(uMsg != ICM_DECOMPRESS)
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("DriverProc(%d,%d,%u,%u,%u)=%ld"),
			dwDriverId, hDriver,
			uMsg, lParam1, lParam2, lRes));
	}
	else if(lRes == ICERR_ERROR)
	{
		Elvees::OutputF(Elvees::TWarning, TEXT("Restarting decoder [%08X] after failure on frame [F %d, I %d]"),
			this, m_uFrameSeq, m_uIFrameSeq);

		StopDecompression();
		StartDecompression();
	}

	return lRes;
}
