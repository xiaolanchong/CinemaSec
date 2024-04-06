// VidEncoder.cpp: implementation of the CVidEncoder class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "VidEncoder.h"

// >= 30day x 25fps
//#define MAXFRAMENUM  0x06FFFFFF

// XviD crushes on frame 98311
#define MAXFRAMENUM 0x00018000

// Display codec configure dialog if no state provided
//#define ENABLE_CODEC_CONFIGURE

// Enable handle message loop
// not recommended if you use special thread messages
//#define ENABLE_CODEC_OUTPUT

//////////////////////////////////////////////////////////////////////
// CVidEncoder
//////////////////////////////////////////////////////////////////////

CVidEncoder::CVidEncoder(DWORD fccHandler, funcDriverProc fDriverProc)
	: m_fccHandler(fccHandler)
	, m_fDriverProc(fDriverProc)
{
	m_hic = 0;

	m_pOutBuffer = NULL;
	m_pPrvBuffer = NULL;

	m_pbiInput  = NULL;
	m_pbiOutput = NULL;

	m_cbConfigData = 0;
	m_pConfigData  = NULL;

	m_bInited  = false;
	m_bStarted = false;

#ifdef STRAIGHT_ENCODER_ACCESS
	m_hDriver = NULL;

	#ifndef STRAIGHT_ENCODER_ACCESS_SAVE
	DriverProc = NULL;
	#endif
#endif
}

CVidEncoder::~CVidEncoder()
{
	FreeCompressor();

#ifdef STRAIGHT_ENCODER_ACCESS
	if(m_hDriver)
		FreeLibrary(m_hDriver);
#endif
}

bool CVidEncoder::InitCompressor(COMPVARS* pCodec, long lUsPerFrame, long lFrameCount)
{
	if(!pCodec)
		return false;

	return InitCompressor(
		pCodec->lpbiIn,
		lUsPerFrame, 
		pCodec->lQ, pCodec->lKey, pCodec->lDataRate, pCodec->lpState, pCodec->cbState,
		lFrameCount);
}

bool CVidEncoder::InitCompressor(BITMAPINFO *pbiInput,
	long lUsPerFrame,
	long lQuality, long lKeyRate, long lDataRate,
	void *lpState, long cbState,
	long lFrameCount)
{
	if(!pbiInput)
		return false;

	if(lUsPerFrame < 10)
		return false;

	if(m_bInited)
		FreeCompressor();

	m_lQuality = lQuality;
	m_lKeyRate = lKeyRate;

	m_lSlopSpace    = 0;
	m_lMaxFrameSize = 0;
	m_lKeyRateCounter = 1;

	m_uFrameSeq  = 0;
	m_uIFrameSeq = 0;
	m_lFrameNum  = 0;
	m_lFrameCompressed = 0;

	int   cbSize;
	DWORD dwFlags;
	LRESULT	lRes;
	ICINFO  info;
	ICCOMPRESSFRAMES icf;

	do
	{
		cbSize = pbiInput->bmiHeader.biSize + pbiInput->bmiHeader.biClrUsed*4;

		m_pbiInput = (LPBITMAPINFO)malloc( max(cbSize, sizeof BITMAPINFO) );

		if(!m_pbiInput)
			break;

		CopyMemory(m_pbiInput, pbiInput, cbSize);
		
		// adjust image data
		m_pbiInput->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_pbiInput->bmiHeader.biSizeImage = DIBSIZE(m_pbiInput->bmiHeader);

	#ifdef STRAIGHT_ENCODER_ACCESS
		if(!m_fDriverProc)
		{
			HIC hCodec = ICOpen(ICTYPE_VIDEO, m_fccHandler, ICMODE_COMPRESS);

			if(hCodec)
			{
				ZeroMemory(&info, sizeof(info));
				info.dwSize = sizeof(info);

				ICGetInfo(hCodec, &info, sizeof(info));
				ICClose(hCodec);

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
			}
		}

		if(!m_fDriverProc)
		{
			Elvees::Output(Elvees::TError, TEXT("DriverProc undefined"));
			break;
		}

	#ifndef STRAIGHT_ENCODER_ACCESS_SAVE
		DriverProc = m_fDriverProc;
	#endif

		ZeroMemory(&m_icOpen, sizeof(m_icOpen));

		m_icOpen.dwSize     = sizeof(m_icOpen);
		m_icOpen.fccType    = ICTYPE_VIDEO;
		m_icOpen.fccHandler = m_fccHandler;
		m_icOpen.dwFlags    = ICMODE_COMPRESS;

		m_hic = (DWORD)DriverProc(0, 0, DRV_OPEN, 0, (LPARAM)&m_icOpen);
	#else
		if(m_fDriverProc)
			m_hic = ICOpenFunction(ICTYPE_VIDEO, m_fccHandler,
							ICMODE_COMPRESS, (FARPROC)m_fDriverProc);
		else
			m_hic = ICOpen(ICTYPE_VIDEO, m_fccHandler, ICMODE_COMPRESS);
	#endif
		if(!m_hic)
		{
			Elvees::Output(Elvees::TError, TEXT("Cant open compressor"));
			break;
		}

		// Query compress

	#ifdef STRAIGHT_ENCODER_ACCESS
		lRes = DriverProc(m_hic, 0, ICM_COMPRESS_QUERY, (LPARAM)m_pbiInput, NULL);
	#else
		lRes = ICCompressQuery(m_hic, m_pbiInput, NULL);
	#endif
		if(lRes != ICERR_OK)
		{
			Elvees::OutputF(Elvees::TError, TEXT("Input format not supported lRes=%ld"), lRes);
			break;
		}

		// Set compressor state

		if(cbState < 0)
		{
	#ifdef ENABLE_CODEC_CONFIGURE
		#ifdef STRAIGHT_ENCODER_ACCESS
			if(DriverProc(m_hic, 0, ICM_CONFIGURE, (LRESULT)-1, ICMF_CONFIGURE_QUERY) == ICERR_OK)
				DriverProc(m_hic, 0, ICM_CONFIGURE, (LRESULT)(GetDesktopWindow()), 0);
		#else
			if(ICQueryConfigure(m_hic))
				ICConfigure(m_hic, GetDesktopWindow());
		#endif
	#else
			Elvees::Output(Elvees::TWarning, TEXT("Compressor not configured, using default"));
	#endif
		}
		else if(cbState > 0)
		{
		#ifdef STRAIGHT_ENCODER_ACCESS
			lRes = DriverProc(m_hic, 0, ICM_SETSTATE, (LPARAM)lpState, cbState);
		#else
			lRes = ICSetState(m_hic, lpState, cbState);
		#endif

			if(lRes != ICERR_OK && lRes != cbState)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Cant set compressor state lRes=%ld"), lRes);
				break;
			}
		}

	#ifdef STRAIGHT_ENCODER_ACCESS
		cbSize = (DWORD)DriverProc(m_hic, 0, ICM_COMPRESS_GET_FORMAT, (LPARAM)m_pbiInput, 0);
	#else
		cbSize = ICCompressGetFormatSize(m_hic, (LPBITMAPINFO)m_pbiInput);
	#endif
		if(cbSize < 0)
		{
			Elvees::OutputF(Elvees::TError, TEXT("Error getting compressor output format size cbSize=%ld"), cbSize);
			break;
		}

		cbSize = max(cbSize, sizeof BITMAPINFO);
		m_pbiOutput = (LPBITMAPINFO)malloc(cbSize);

		if(!m_pbiOutput)
			break;

		ZeroMemory(m_pbiOutput, cbSize);

	#ifdef STRAIGHT_ENCODER_ACCESS
		if(ICERR_OK != DriverProc(m_hic, 0, ICM_COMPRESS_GET_FORMAT, (LPARAM)(LONG_PTR)m_pbiInput, (LPARAM)(LONG_PTR)m_pbiOutput))
	#else
		if(ICERR_OK != ICCompressGetFormat(m_hic, m_pbiInput, m_pbiOutput))
	#endif
		{
			Elvees::Output(Elvees::TError, TEXT("Compressor error"));
			break;
		}

		// Retrieve compressor information.
		//

		ZeroMemory(&info, sizeof(info));
		info.dwSize = sizeof(info);

	#ifdef STRAIGHT_ENCODER_ACCESS
		lRes = DriverProc(m_hic, 0, ICM_GETINFO, (LPARAM)&info, sizeof(info));
	#else
		lRes = ICGetInfo(m_hic, &info, sizeof(info));
	#endif
		if(!lRes)
		{
			Elvees::Output(Elvees::TError, TEXT("Unable to get codec info"));
			break;
		}

		// Analyze compressor.
		dwFlags = info.dwFlags;

		// Set quality
		if(!(dwFlags & VIDCF_QUALITY))
			m_lQuality = 0;

		// Does it use prev buffer
		//

		if(dwFlags & VIDCF_TEMPORAL)
		{
			if(!(dwFlags & VIDCF_FASTTEMPORALC))
			{
				// Allocate prevbuffer
				m_pPrvBuffer = (char*)malloc(m_pbiInput->bmiHeader.biSizeImage);

				if(!m_pPrvBuffer)
				{
					Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
					break;
				}
			}
		}

		// Allocate destination buffer
		//
	
	#ifdef STRAIGHT_ENCODER_ACCESS
		m_lMaxPackedSize = (long)DriverProc(m_hic, 0, ICM_COMPRESS_GET_SIZE, (LPARAM)m_pbiInput, (LPARAM)m_pbiOutput);
	#else
		m_lMaxPackedSize = ICCompressGetSize(m_hic, m_pbiInput, m_pbiOutput);
	#endif
//		if(m_lMaxPackedSize > m_pbiInput->bmiHeader.biSizeImage)
//		{
//			Elvees::Output(Elvees::TWarning,
//				TEXT("Compressor::MaxPackedSize bigger then image size."));
//
//			m_lMaxPackedSize = m_pbiInput->bmiHeader.biSizeImage;
//		}

		m_pOutBuffer = (char*)malloc(m_lMaxPackedSize);

		if(!m_pOutBuffer)
		{
			Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
			break;
		}

		// Save configuration state.
		//

	#ifdef STRAIGHT_ENCODER_ACCESS
		m_cbConfigData = (int)DriverProc(m_hic, 0, ICM_GETSTATE, 0, 0);
	#else
		m_cbConfigData = ICGetStateSize(m_hic);
	#endif
		if(m_cbConfigData > 0)
		{
			m_pConfigData = malloc(m_cbConfigData);
			if(!m_pConfigData)
			{
				Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
				break;
			}

	#ifdef STRAIGHT_ENCODER_ACCESS
			m_cbConfigData = (int)DriverProc(m_hic, 0, ICM_GETSTATE, (LPARAM)m_pConfigData, m_cbConfigData);

			if(m_cbConfigData)
				DriverProc(m_hic, 0, ICM_SETSTATE, (LPARAM)m_pConfigData, m_cbConfigData);
	#else
			m_cbConfigData = (int)ICGetState(m_hic, m_pConfigData, m_cbConfigData);

			if(m_cbConfigData)
				ICSetState(m_hic, m_pConfigData, m_cbConfigData);
	#endif
		}

		// Set DataRate
		//

		if(lDataRate && (dwFlags & VIDCF_CRUNCH))
			m_lMaxFrameSize = MulDiv(lDataRate, lUsPerFrame, 1000000);
		else
			m_lMaxFrameSize = 0;

		ZeroMemory(&icf, sizeof(icf));

		icf.dwFlags		= (DWORD)(LONG_PTR)&icf.lKeyRate;
		icf.lStartFrame = 0;
		icf.lFrameCount = lFrameCount;
		icf.lQuality	= m_lQuality;
		icf.lDataRate	= lDataRate;
		icf.lKeyRate	= m_lKeyRate;

		icf.dwRate		= 1000000;     //
		icf.dwScale		= lUsPerFrame; //

	#ifdef STRAIGHT_ENCODER_ACCESS
		DriverProc(m_hic, 0, ICM_COMPRESS_FRAMES_INFO, (LPARAM)&icf, sizeof(ICCOMPRESSFRAMES));
	#else
		ICSendMessage(m_hic, ICM_COMPRESS_FRAMES_INFO, (LPARAM)(LPVOID)&icf, sizeof(ICCOMPRESSFRAMES));
	#endif

		m_bInited = true;
	}
	while(false);

	if(!m_bInited)
		FreeCompressor();

#ifdef ENABLE_CODEC_OUTPUT
	// message loop. 
	// (Un)Initialize any codec output windows

	MSG msg;
	while(PeekMessage(&msg, NULL,  0, 0, PM_REMOVE)) 
	{ 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}
#endif

	return m_bInited;
}

void CVidEncoder::FreeCompressor()
{
	if(m_bStarted)
		StopCompression();

	if(m_pOutBuffer)
		free(m_pOutBuffer);

	if(m_pPrvBuffer)
		free(m_pPrvBuffer);

	if(m_pConfigData)
		free(m_pConfigData);

	if(m_pbiInput)
		free(m_pbiInput);

	if(m_pbiOutput)
		free(m_pbiOutput);
	
#ifdef STRAIGHT_ENCODER_ACCESS
	if(m_hic)
		DriverProc(m_hic, 0, DRV_CLOSE, 0, (LPARAM)(LONG_PTR)&m_icOpen);
#else
	if(m_hic)
		ICClose(m_hic);
#endif

	m_hic = 0;

	m_pOutBuffer = NULL;
	m_pPrvBuffer = NULL;

	m_pbiInput  = NULL;
	m_pbiOutput = NULL;

	m_cbConfigData = 0;
	m_pConfigData  = NULL;

	m_bInited  = false;
}

bool CVidEncoder::StartCompression()
{
	if(!m_bInited)
		return false;

	if(m_bStarted)
		StopCompression();

	LRESULT	lRes;

#ifdef STRAIGHT_ENCODER_ACCESS
	lRes = DriverProc(m_hic, 0, ICM_COMPRESS_BEGIN, (LPARAM)m_pbiInput, (LPARAM)m_pbiOutput);
#else
	lRes = ICCompressBegin(m_hic, m_pbiInput, m_pbiOutput);
#endif	

	if(lRes != ICERR_OK)
		return false;

	m_bStarted = true;

	return true;
}

void CVidEncoder::StopCompression()
{
	// Stop and reset MPEG-4 compressor
	//

	if(m_bStarted)
	{
	#ifdef STRAIGHT_ENCODER_ACCESS
		DriverProc(m_hic, 0, ICM_COMPRESS_END, 0, 0);

		if(m_cbConfigData)
			DriverProc(m_hic, 0, ICM_SETSTATE, (LPARAM)m_pConfigData, m_cbConfigData);
	#else
		ICCompressEnd(m_hic);

		if(m_cbConfigData)
			ICSetState(m_hic, m_pConfigData, m_cbConfigData);
	#endif
	}

	m_lKeyRateCounter = 1;

	m_lFrameNum = 0;
	m_lFrameCompressed = 0;

	m_bStarted = false;
}

void* CVidEncoder::CompressData(const void* pData, long *plSize, bool *pbKeyframe, unsigned *puFrameNum)
{
	if(!m_bStarted)
	{
		if(!StartCompression())
			return NULL;
	}

	LRESULT lRes;

	long lSize;
	bool bKeyframe;
	
	DWORD dwFlags   = 0;
	DWORD dwFlagsIn = ICCOMPRESS_KEYFRAME;

	// Check for codec counter
	if(m_lFrameCompressed > MAXFRAMENUM)
	{
		DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("Compressor internal re-init")));

		StopCompression();
		StartCompression();
	}

	// Figure out if we should force a keyframe.  If we don't have any
	// keyframe interval, force only the first frame.  Otherwise, make
	// sure that the key interval is lKeyRate or less.  We count from
	// the last emitted keyframe, since the compressor can opt to
	// make keyframes on its own.

	long lKeyRateCounterSave = m_lKeyRateCounter;

	if(m_lKeyRate)
	{
		if (--m_lKeyRateCounter)
			dwFlagsIn = 0;
		else
			m_lKeyRateCounter = m_lKeyRate;
	}
	else
	{
		if(m_lFrameNum)
			dwFlagsIn = 0;
	}

	// Figure out how much space to give the compressor, if we are using
	// data rate stricting.  If the compressor takes up less than quota
	// on a frame, save the space for later frames.  If the compressor
	// uses too much, reduce the quota for successive frames, but do not
	// reduce below half data rate.

	long lAllowableFrameSize = 0;//xFFFFFF;

	if(m_lMaxFrameSize && m_lFrameNum)
	{
		lAllowableFrameSize = m_lMaxFrameSize + (m_lSlopSpace>>2);

		if(lAllowableFrameSize < m_lMaxFrameSize/2)
			lAllowableFrameSize = m_lMaxFrameSize/2;
	}

	// A couple of notes:
	//
	// ICSeqCompressFrame() passes 0x7FFFFFFF when data rate control
	// is inactive.  Docs say 0.  We pass 0x7FFFFFFF here to avoid
	// a bug in the Indeo 5 QC driver, which page faults if
	// keyframe interval=0 and max frame size = 0.

	// Compress!

	if(dwFlagsIn)
		dwFlags = AVIIF_KEYFRAME;

	DWORD dwChunkId = 0; // Ignored
	DWORD sizeImage = m_pbiOutput->bmiHeader.biSizeImage;

#ifdef STRAIGHT_ENCODER_ACCESS
	ICCOMPRESS icCompress;

	icCompress.dwFlags    = dwFlagsIn;
	icCompress.lpbiOutput = reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiOutput);
	icCompress.lpOutput   = m_pOutBuffer;
	icCompress.lpbiInput  = reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiInput);
	icCompress.lpInput    = const_cast<void*>(pData);
	icCompress.lpckid     = &dwChunkId;
	icCompress.lpdwFlags  = &dwFlags;
	icCompress.lFrameNum  = m_lFrameNum;
	icCompress.dwQuality  = m_lQuality;
	icCompress.dwFrameSize = lAllowableFrameSize;
	icCompress.lpbiPrev   = (dwFlagsIn & ICCOMPRESS_KEYFRAME) ? NULL : reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiInput);
	icCompress.lpPrev     = (dwFlagsIn & ICCOMPRESS_KEYFRAME) ? NULL : m_pPrvBuffer;

	lRes = DriverProc(m_hic, 0, ICM_COMPRESS, (LPARAM)&icCompress, sizeof(icCompress));
#else
	lRes = ICCompress(m_hic, dwFlagsIn,
		reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiOutput), m_pOutBuffer,
		reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiInput), const_cast<void*>(pData),
		&dwChunkId,
		&dwFlags,
		m_lFrameNum,
		lAllowableFrameSize,
		m_lQuality,
		(dwFlagsIn & ICCOMPRESS_KEYFRAME) ? NULL : reinterpret_cast<LPBITMAPINFOHEADER>(m_pbiInput),
		(dwFlagsIn & ICCOMPRESS_KEYFRAME) ? NULL : m_pPrvBuffer);
#endif
	// Compressor internal frame counter
	// differ from m_lFrameNum
	// Internal ReInit will help to avoid codec failure
	// if this counter next to MAX_LONG

	// counter changed only in ICCompress
	m_lFrameCompressed++;

	lSize = m_pbiOutput->bmiHeader.biSizeImage;

	m_pbiOutput->bmiHeader.biSizeImage = sizeImage;

#ifdef ENABLE_CODEC_OUTPUT
	// Message loop for codec output windows
	MSG msg;
	while(PeekMessage(&msg, NULL,  0, 0, PM_REMOVE)) 
	{ 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	} 
#endif

	if(lRes != ICERR_OK)
		return NULL;

	// Special handling for DivX 5 and XviD codecs:
	//
	// A one-byte frame starting with 0x7f should be discarded
	// (lag for B-frame).

	if( m_pbiOutput->bmiHeader.biCompression == '05xd' ||
		m_pbiOutput->bmiHeader.biCompression == '05XD' ||
		m_pbiOutput->bmiHeader.biCompression == 'divx' ||
		m_pbiOutput->bmiHeader.biCompression == 'DIVX')
	{
		if(m_pbiOutput->bmiHeader.biSizeImage == 1 && *m_pOutBuffer == 0x7f)
		{
			m_lKeyRateCounter = lKeyRateCounterSave;
			return NULL;
		}
	}

	m_lFrameNum++;

	// If we're using a compressor such as (Microsoft Video 1),
	// we have to store the frame to compress the next one....

	if(lRes == ICERR_OK && m_pPrvBuffer && (!m_lKeyRate || m_lKeyRateCounter>1))
		CopyMemory(m_pPrvBuffer, pData, m_pbiInput->bmiHeader.biSizeImage);

	// Update quota.
	if(m_lMaxFrameSize)
		m_lSlopSpace += m_lMaxFrameSize - lSize;

	// Was it a keyframe?
	
	if(dwFlags & AVIIF_KEYFRAME)
	{
		bKeyframe = true;
		m_lKeyRateCounter = m_lKeyRate;

		m_uIFrameSeq++;
		m_uFrameSeq = 0;
	}
	else
	{
		bKeyframe = false;
		
		// Just be on the safe side
		// I don't know compressor what can make 0xFFFF delta frames
		if(++m_uFrameSeq == 0) m_uFrameSeq = 1;
	}

	if(plSize)
		*plSize = lSize;

	if(pbKeyframe)
		*pbKeyframe = bKeyframe;

	if(puFrameNum)
		*puFrameNum = MAKELONG(m_uFrameSeq, m_uIFrameSeq);

	return m_pOutBuffer;
}

void CVidEncoder::DropFrame()
{
	if(m_lKeyRate && m_lKeyRateCounter > 1)
		--m_lKeyRateCounter;

	m_lFrameNum++;
}

bool CVidEncoder::IsCompressionInited()
{
	return m_bInited;
}

bool CVidEncoder::IsCompressionStarted()
{
	return m_bStarted;
}

DWORD CVidEncoder::GetFOURCC() const
{
	if(m_pbiOutput && m_pbiOutput->bmiHeader.biCompression != 0)
		return m_pbiOutput->bmiHeader.biCompression;

	return m_fccHandler;
}

long CVidEncoder::GetInFormat(BITMAPINFO* pbi)
{
	if(m_pbiInput == NULL)
		return -1;

	int cbSize = m_pbiInput->bmiHeader.biSize + \
		m_pbiInput->bmiHeader.biClrUsed*4;

	__try
	{
		if(pbi)
			CopyMemory(pbi, m_pbiInput, cbSize);

		return cbSize;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return -1;
}

long CVidEncoder::GetOutFormat(BITMAPINFO* pbi)
{
	if(m_pbiOutput == NULL)
		return -1;

	int cbSize = m_pbiOutput->bmiHeader.biSize + \
		m_pbiOutput->bmiHeader.biClrUsed*4;

	__try
	{
		if(pbi)
			CopyMemory(pbi, m_pbiOutput, cbSize);

		return cbSize;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return -1;
}

long CVidEncoder::GetMaxPackedSize() const
{
	return m_lMaxFrameSize;
}

//////////////////////////////////////////////////////////////////////
// DriverProc - spy on codec and protect from exceptions
// usefully for debug
//////////////////////////////////////////////////////////////////////

#ifdef STRAIGHT_ENCODER_ACCESS_SAVE

LRESULT CVidEncoder::DriverProc(
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
			Elvees::Output(Elvees::TWarning, TEXT("Unexpected exception in DriverProc"));
		}
	}

	if(uMsg != ICM_COMPRESS)
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("DriverProc(%d,%d,%u,%u,%u)=%ld"),
			dwDriverId, hDriver,
			uMsg, lParam1, lParam2, lRes));
	}
	else if(lRes == ICERR_ERROR)
	{
		Elvees::OutputF(Elvees::TWarning, TEXT("Restarting encoder [%08X] after failure on frame [F %d, I %d] {%ld/%ld}"),
			this, m_uFrameSeq, m_uIFrameSeq, m_lFrameNum, m_lFrameCompressed);

		StopCompression();
		StartCompression();
	}

	return lRes;
}

#endif