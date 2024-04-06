// StreamDevice.cpp: implementation of the CStreamDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "StreamDevice.h"

#include "VidEncoder.h"
#include "NetProtocol.h"

#include <initguid.h>
#include <math.h>

// {00020000-0000-0000-C000-000000000046}
DEFINE_GUID(defaultAviHandler,
	0x00020000, 0, 0, 0xC0, 0, 0, 0, 0, 0, 0, 0x46);

//////////////////////////////////////////////////////////////////////
// CStreamDevice
//////////////////////////////////////////////////////////////////////

CStream* CStreamDevice::CreateStream(
	UUID& streamID,
	LPCTSTR stMoniker,
	long lWidth,
	long lHeight,
	long lFrmRate,
	long lInput,
	long lVStandard,
	bool bBWMode)
{
	CStreamDevice* pDevice = new CStreamDevice(streamID);

	if(pDevice)
	{
		pDevice->InitDevice(stMoniker, lWidth, lHeight, lFrmRate, lInput, lVStandard, bBWMode);

		// Store for new device detection
		lstrcpy(pDevice->m_stMoniker, stMoniker);
	}
	else
	{
		Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
	}

	return pDevice;
}

CStreamDevice::CStreamDevice(UUID& streamUID)
	: CStream(streamUID)
	, CFrmBuffer::Allocator(5)
	, m_newImage(false)
	, m_bSendKey(true)
	, m_CompressedFrames(81920, 10)
	, fThresh(0.01)
	, fCoef(0.2)
{
	m_uTimeStart = 0;
	m_lLastFrame = -1;

	m_pEncoder = NULL;

	// Archive

	m_pArchive = NULL;

	m_lVideoSize = 0;
	m_lStartFrame = -1;

	// Settings

	m_lFPM = 0;
	m_lVMS = 0;

	m_pFrameLast = NULL;
	m_pFormatData = NULL;

	// -------

	m_dwLastUpdated = 0;

	// -------

	m_pFrameOut = NULL;
	m_arHisto = NULL;
}

CStreamDevice::~CStreamDevice()
{
	SAFE_RELEASE(m_pArchive);
	SAFE_RELEASE(m_pFormatData);

	if(m_pEncoder)
		delete m_pEncoder;

	if(m_pFrameOut)
		free(m_pFrameOut);

	if(m_arHisto)
		free(m_arHisto);

	if(m_pFrameLast)
		m_pFrameLast->Release();
}

void CStreamDevice::Delete()
{
	StopDevice();
	Sleep(100);

	delete this;
}

//////////////////////////////////////////////////////////////////////
// CStream
//////////////////////////////////////////////////////////////////////

bool CStreamDevice::IsValid()
{
	return IsDeviceValid();
}

bool CStreamDevice::IsDeviceString(LPCTSTR stDevice)
{
	return lstrcmpi(m_stMoniker, stDevice) == 0;
}

//////////////////////////////////////////////////////////////////////
// Allocator ID request
//////////////////////////////////////////////////////////////////////

long CStreamDevice::AllocatorSourceID()
{
	return GetID(); // CStream::GetID()
}

//////////////////////////////////////////////////////////////////////
// Accept stream format from CDevice
//////////////////////////////////////////////////////////////////////

bool CStreamDevice::OnImageFormat(BITMAPINFOHEADER* pbiImg)
{
	if(!pbiImg || IsBadReadPtr(pbiImg, sizeof(BITMAPINFOHEADER)))
		return false;

	// Capture only YUY2 images
	if(pbiImg->biCompression != mmioFOURCC('Y','U','Y','2'))
		return false;

	// Normalize/Resize image
	ZeroMemory(&m_bihOut, sizeof(m_bihOut));

	m_bihOut.biSize = sizeof(BITMAPINFOHEADER);
	m_bihOut.biCompression = mmioFOURCC('Y','V','1','2');
	m_bihOut.biPlanes = 1;
	m_bihOut.biBitCount = 12;
	m_bihOut.biWidth  = pbiImg->biWidth / 2;
	m_bihOut.biHeight = pbiImg->biHeight / 2;
	m_bihOut.biSizeImage = DIBSIZE(m_bihOut);

	if(m_pFrameOut)	free(m_pFrameOut);
	if(m_arHisto) free(m_arHisto);

	m_pFrameOut = (BYTE*)malloc(m_bihOut.biSizeImage);
	m_arHisto = (DWORD*)malloc(256 * sizeof(DWORD));

	memset(m_pFrameOut, 0x80, m_bihOut.biSizeImage);

	// CFrmBuffer::Allocator
	return SetFrameAllocatorFormat(reinterpret_cast<BITMAPINFO*>(pbiImg));
}

//////////////////////////////////////////////////////////////////////
// Called in context of SampleGrabber filter thread
//////////////////////////////////////////////////////////////////////

void CStreamDevice::OnBufferCB(double fTime, BYTE *pData, long lLen)
{
	// Called after destroy
	if(IsBadReadPtr(this, sizeof(CStreamDevice)))
	{
		Elvees::Output(Elvees::TWarning, TEXT("CStreamDevice::OnBufferCB() called after destroy"));
		return;
	}

	m_dwLastUpdated = timeGetTime();

	// Allocate frame buffer
	CFrmBuffer* pFrame = AllocateFrameBuffer();

	if(pFrame)
	{
		__time64_t ltime;
		_time64(&ltime);

		// Copy data to frame buffer...
		pFrame->SetSignalLocked(IsSignalLocked());
		pFrame->SetFrameTime(m_dwLastUpdated, ltime);
		
		// Copy frame buffer
		pFrame->SetFrameData(pData, lLen);

		// Update last frame
		SetLastFrame(pFrame);

		m_newImage.Pulse();

		if(IsFrameNeeded(pFrame)) // Send frame archive
			CompressFrame(pFrame);
	}
}

//////////////////////////////////////////////////////////////////////
// Last frame
//////////////////////////////////////////////////////////////////////

void CStreamDevice::SetLastFrame(CHCS::IFrame* pFrame)
{
	if(!pFrame)
		return;

	Elvees::CCriticalSection::Owner lock(m_lastFrameSect);

	if(m_pFrameLast)
		m_pFrameLast->Release();

	m_pFrameLast = pFrame;
}

bool CStreamDevice::GetLastFrame(CHCS::IFrame** ppFrame)
{
	if(!ppFrame)
		return false;

	Elvees::CCriticalSection::Owner lock(m_lastFrameSect);

	if(m_pFrameLast)
	{
		m_pFrameLast->AddRef();
		*ppFrame = m_pFrameLast;
		return true;
	}
	
	return false;
}

//////////////////////////////////////////////////////////////////////
// IStream
//////////////////////////////////////////////////////////////////////

bool CStreamDevice::Start()
{
	if(!IsDeviceValid())
		return false;

	bool bRet = StartDevice();
	
	Elvees::OutputF(bRet ? Elvees::TInfo : Elvees::TWarning,
		TEXT("Stream [%ld] %s"), GetID(),
		bRet ? TEXT("started") : TEXT("start failed"));

	return bRet;
}

void CStreamDevice::Stop()
{
	if(IsDeviceValid())
	{
		StopDevice();
		
		Elvees::OutputF(Elvees::TInfo, TEXT("Stream [%ld] stopped"), GetID());
	}
}

bool CStreamDevice::IsStarted()
{
	return IsDeviceStarted(500);
}

long CStreamDevice::SeekTo(INT64 timePos)
{
	return 0;
}

bool CStreamDevice::GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout)
{
	DWORD dwTime = timeGetTime();

	if(dwTime - m_dwLastUpdated > 5000)
	{
		DEBUG_ONLY(Elvees::Output(Elvees::TTrace,
			TEXT("Frame timeout. Restarting device")));

		if(IsDeviceStarted(500))
			StopDevice();

		if(!StartDevice())
			Elvees::Output(Elvees::TError, TEXT("Restart device failed"));

		m_dwLastUpdated = dwTime;
	}
	else
	{
		m_newImage.Wait(min(40, uTimeout));
	}

	return GetLastFrame(ppFrame);
}

//////////////////////////////////////////////////////////////////////////
// Video archiving
//////////////////////////////////////////////////////////////////////////

bool CStreamDevice::IsFrameNeeded(CHCS::IFrame* pFrame)
{
	CManager* pManager = GetManager();

	if(pManager && pManager->IsArchiveEnabled())
	{
		long lFrame;
		unsigned uTime;
		
		uTime  = pFrame->GetRelativeTime() - m_uTimeStart;
		lFrame = MulDiv(uTime, m_lFPM, 60000);

		if(lFrame > m_lLastFrame || m_lLastFrame < 0)
			return true;
	}

	return false;
}

void CStreamDevice::CompressFrame(CHCS::IFrame* pFrame)
{
	long     lFrame;
	unsigned uTime;

	uTime  = pFrame->GetRelativeTime() - m_uTimeStart;
	lFrame = MulDiv(uTime, m_lFPM, 60000);

	if(lFrame <= m_lLastFrame)
		return;

	CManager* pManager = GetManager();
	if(!pManager) return;

	//////////////////////////////////////////////////////////////////////////
	// Normalize image

	long lSize, i, w, h;
	long minY, maxY, aveY;
	long pixels, limit;

	DWORD *YUY2;
	const BYTE* pFrameBytes = reinterpret_cast<const BYTE*>(pFrame->GetBytes());

	double fHalfPt, fGamma, out;
	BYTE YT[256];

	ZeroMemory(m_arHisto, 256*sizeof(DWORD));

	// YUY2 Format
	// Y0 U0 Y1 V0 | Y2 U2 Y3 V2 | 
	// 0x80YY80YY - grey

	// lpbiOut->biWidth = lpbiIn->biWidth / 2
	// lpbiOut->biHeight = lpbiIn->biHeight / 2

	for(h=0; h < labs(m_bihOut.biHeight); h++)
	{
		YUY2 = (DWORD*)(pFrameBytes + 8 * m_bihOut.biWidth * h);

		for(w=0; w < m_bihOut.biWidth; w++)
			m_arHisto[ YUY2[w] & 0xFF ]++;
	}

	lSize = m_bihOut.biWidth * labs(m_bihOut.biHeight);

	/////////////////////////////////////////////////////////

	limit = (long)(fThresh*lSize + 0.5);

	for(pixels=0, minY=0; minY<256; minY++)
	{
		if((pixels += m_arHisto[minY]) >= limit)
			break;
	}

	for(pixels=0, maxY=255; maxY>=minY; maxY--)
	{
		if((pixels += m_arHisto[maxY]) >= limit)
			break;
	}

	for(pixels=0, limit=lSize/2, aveY=0; aveY<256; aveY++)
	{
		if((pixels += m_arHisto[aveY]) >= limit)
			break;
	}

	/////////////////////////////////////////////////////////

	if(minY == maxY)
		fGamma = 1.0;
	else
	{
		// compute halfpoint... if inputlo/hi range is even,
		// drop down by 1/2 to allow for halfpoint

		if((minY + maxY) & 1)
			fHalfPt = ((aveY <= (minY + maxY)/2 ? +1 : -1) + 2*(aveY - minY))
			/ (2.0*(maxY - minY - 1));
		else
			fHalfPt = (aveY - minY) / (double)(maxY - minY);

		// halfpt ^ (1/gc) = 0.5
		// 1/gc = log_halfpt(0.5)
		// 1/gc = log(0.5) / log(halfpt)
		// gc = log(halfpt) / log(0.5)

		// clamp gc to [0.01...10.0]

		if (fHalfPt > 0.9930925)
			fHalfPt = 0.9930925;
		else if (fHalfPt < 0.0009765625)
			fHalfPt = 0.0009765625;

		fGamma = log(fHalfPt) / -0.693147180559945309417232121458177; // log(0.5);
	}

	/////////////////////////////////////////////////////////

	for(i=0; i<256; i++)
	{
		if(i < minY)
			YT[i] = 0;
		else if(i > maxY)
			YT[i] = 255;
		else
		{
			out = 255 * pow((i - minY) / double(maxY - minY), 1.0/fGamma);
			YT[i] = min(255, int(out + 0.5));
		}
	}

	/////////////////////////////////////////////////////////

	for(h=0; h < labs(m_bihOut.biHeight); h++)
	{
		YUY2 = (DWORD*)(pFrameBytes + 8 * m_bihOut.biWidth * h);

		for(w=0; w < m_bihOut.biWidth; w++)
		{
			i = m_bihOut.biWidth * h + w;
#if 1
			m_pFrameOut[i] = (BYTE)(fCoef* YT[ YUY2[w] & 0xFF ] + (1 - fCoef)*m_pFrameOut[i] + 0.5);
#else
			m_pFrameOut[i] = YT[ YUY2[w] & 0xFF ];
#endif
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Create encoder

	if(m_pEncoder == NULL)
	{
	//	BITMAPINFO biFrame;
	//	pFrame->GetFormat(&biFrame);
	//	m_pEncoder = pManager->CreateCompressor(&biFrame);

		m_pEncoder = pManager->CreateCompressor((LPBITMAPINFO)&m_bihOut);

		m_lFPM = pManager->GetVideoRate();
		m_lVMS = pManager->GetVideoMaxSize();

		m_lFPM = m_lFPM > 0 ? m_lFPM : 60;
		m_lVMS = m_lVMS > 0 ? m_lVMS : 300;
	}

	if(m_pEncoder == NULL)
		return;

	//////////////////////////////////////////////////////////////////////////
	// Create format packet

	if(!m_pFormatData)
	{
		long lFormatSize;
		LPBITMAPINFO pbiComp;
		
		lFormatSize = m_pEncoder->GetOutFormat(NULL);
		pbiComp = reinterpret_cast<LPBITMAPINFO>(malloc(lFormatSize));

		if(pbiComp)
		{
			m_pEncoder->GetOutFormat(pbiComp);

			m_pFormatData = m_CompressedFrames.Allocate();
			if(m_pFormatData)
			{
				NetPacketHeader header;
				NetStreamInfo   datainfo;

				datainfo.uStreamUID  = GetUID();
				datainfo.uFOURCC     = m_pEncoder->GetFOURCC();
				datainfo.uFormatSize = lFormatSize;

				header.uSynchro = CHCS_NET_SYNCHRO;
				header.uPacket  = NET_PACKET_STREAMINFO;
				header.uTime    = pFrame->GetRelativeTime();
				header.uLength  = sizeof(NetPacketHeader) + sizeof(NetStreamInfo) + lFormatSize;
				header.uCheckSum = header.uPacket + header.uTime + header.uLength;

				m_pFormatData->AddData(&header, sizeof(header));
				m_pFormatData->AddData(&datainfo, sizeof(datainfo));
				m_pFormatData->AddData(pbiComp, lFormatSize);
			}

			free(pbiComp);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	
	HRESULT hr;
	bool bKey  = true;
	void* pCompressed = NULL;
	unsigned uFrameNum;

	lSize = 0;

	pCompressed = m_pEncoder->CompressData(
	//	pFrame->GetBytes(),
		m_pFrameOut,
		&lSize, &bKey, &uFrameNum);

	if(pCompressed)
	{
		if(m_lLastFrame < 0)
			m_uTimeStart = pFrame->GetRelativeTime();

		m_lLastFrame = lFrame;
	}
	else
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Compress frame[%ld] failed"), GetID()));
		return;
	}

	//////////////////////////////////////////////////////////////////////////

	long lVideoFrame = m_lStartFrame < 0 ? 0 : lFrame - m_lStartFrame;

	// Segment archive check by time or by file size
	//

	if((MulDiv(lVideoFrame, 60, m_lFPM) >= m_lVMS || m_lVideoSize >= 0x2BC00000L) && bKey)
	{
		lVideoFrame   = 0;

		m_lVideoSize  = 0;
		m_lStartFrame = -1;

		SAFE_RELEASE(m_pArchive);
	}

	// Do we need to open the AVI file?
	//  start only from I-Frame
	if(m_pArchive == NULL && bKey && GetID() > 0)
	{
		__time64_t ltime   = pFrame->GetTime();
		struct tm* tmFrame = _localtime64(&ltime);

		int cbFilePath;
		TCHAR stFilePath[MAX_PATH];
			
		bool bOpenFile   = false;
		bool bDeleteFile = false;

		IAVIFile *pAVIFile = NULL;
		BITMAPINFO *pbiComp = NULL;
		AVISTREAMINFOW strHdr;
		
		do 
		{
			if(!tmFrame)
				break;

			cbFilePath = wsprintf(stFilePath, TEXT("%s\\%ld"),
				pManager->GetVideoBasePath(), pFrame->GetSourceID());

			if(!::CreateDirectory(stFilePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Fail to create \"%s\" directory"), stFilePath);
				break;
			}

			cbFilePath += wsprintf(&stFilePath[cbFilePath], TEXT("\\%02d-%02d-%02d"),
				tmFrame->tm_year%100, tmFrame->tm_mon + 1, tmFrame->tm_mday);

			if(!::CreateDirectory(stFilePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Fail to create \"%s\" directory"), stFilePath);
				break;
			}

			cbFilePath += wsprintf(&stFilePath[cbFilePath], TEXT("\\%02d-%02d-%02d.avi"),
				tmFrame->tm_hour, tmFrame->tm_min, tmFrame->tm_sec);

			if(Elvees::IsFileExists(stFilePath))
			{
				Elvees::OutputF(Elvees::TError, TEXT("AVIFileOpen file \"%s\" already exists"), stFilePath);
				break;
			}

			// Get encoder format
			long lFormatSize = m_pEncoder->GetOutFormat(NULL);

			pbiComp = reinterpret_cast<LPBITMAPINFO>(malloc(lFormatSize));
			if(!pbiComp)
			{
				Elvees::Output(Elvees::TError, TEXT("Fail to alloc format block"));
				break;
			}

			m_pEncoder->GetOutFormat(pbiComp);

			// Open the movie file for writing....
			// I know that i must call AviFileInit but it only call CoInitilize()
			
			bDeleteFile = true;

			hr = AVIFileOpen(&pAVIFile,		// Address to contain the new file interface pointer
				stFilePath,					// Null-terminated string containing the name of the file to open
				OF_CREATE | OF_READWRITE,	// Access mode to use when opening the file
				const_cast<LPCLSID>(&defaultAviHandler));	// use handler determined from file extension
			if(hr != S_OK)
			{
				Elvees::OutputF(Elvees::TError, TEXT("AVIFileOpen failed. Check filename \"%s\". hr=0x%X"), stFilePath, hr);
				break;
			}

			ZeroMemory(&strHdr, sizeof(strHdr));
			strHdr.fccType    = streamtypeVIDEO;					// video stream type
			strHdr.fccHandler = m_pEncoder->GetFOURCC();			// Codec FCC
			strHdr.dwScale    = m_lFPM > 60 ? 1 : 60;				// should be one for video
			strHdr.dwRate     = m_lFPM > 60 ? m_lFPM/60 : m_lFPM;	// fps
			strHdr.dwSuggestedBufferSize = m_pEncoder->GetMaxPackedSize();	// Recommended buffer size, in bytes, for the stream.

			::SetRect(&strHdr.rcFrame, 0, 0,						// rectangle for stream
				pbiComp->bmiHeader.biWidth,
				pbiComp->bmiHeader.biHeight);

			hr = pAVIFile->CreateStream(&m_pArchive, &strHdr);
			if(hr != AVIERR_OK)
			{
				Elvees::OutputF(Elvees::TError, TEXT("AVI Video stream creation failed. hr=0x%X"), hr);
				break;
			}

			hr = m_pArchive->SetFormat(0, pbiComp, lFormatSize);
			if(hr != S_OK)
			{
				Elvees::OutputF(Elvees::TError, TEXT("AVI Stream format setting failed. hr=0x%X"), hr);
				break;
			}

			m_lStartFrame = lFrame;
			bOpenFile = true;
		}
		while(false);

		if(pbiComp)
			free(pbiComp);

		// IAVIFile don't need anymore...
		SAFE_RELEASE(pAVIFile);

		if(!bOpenFile)
		{
			if(bDeleteFile)
				::DeleteFile(stFilePath);

			m_lVideoSize  = 0;
			m_lStartFrame = -1;

			SAFE_RELEASE(m_pArchive);
		}
	}

	if(m_pArchive)
	{
		m_lVideoSize += lSize;

		hr = m_pArchive->Write(
				lVideoFrame,				// time of this frame
				1, 							// one frame
				pCompressed, 				// frame data
				lSize,						// frame size
				bKey ? AVIIF_KEYFRAME : 0,	// I-Frame or not?
				NULL,
				NULL);

		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail write frame to avi hr=0x%X size=%ld"), hr, m_lVideoSize);

			m_lVideoSize  = 0;
			m_lStartFrame = -1;

			SAFE_RELEASE(m_pArchive);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	
	if((m_bSendKey && bKey) || !m_bSendKey)
	{
		Elvees::CIOBuffer *pData = NULL;

		if(bKey && m_pFormatData)
		{
			pData = m_pFormatData->CreateCopy();
			if(pData)
			{
				pManager->SendFrame(pData);
				pData->Release();
			}
		}

		pData = m_CompressedFrames.Allocate();

		if(pData)
		{
			if(lSize + sizeof(NetPacketHeader) + sizeof(NetStreamDataInfo)
				<= m_CompressedFrames.GetBufferSize())
			{
				NetPacketHeader   header;
				NetStreamDataInfo datainfo;

				header.uSynchro = CHCS_NET_SYNCHRO;
				header.uPacket  = NET_PACKET_STREAMDATA;
				header.uTime    = pFrame->GetRelativeTime();
				header.uLength  = sizeof(NetPacketHeader) + sizeof(NetStreamDataInfo) + lSize;
				header.uCheckSum = header.uPacket + header.uTime + header.uLength;

				datainfo.uStreamUID = GetUID();
				datainfo.uFrameSize = lSize;
				datainfo.uFrameSeq  = uFrameNum;

				pData->AddData(&header, sizeof(header));
				pData->AddData(&datainfo, sizeof(datainfo));
				pData->AddData(pCompressed, lSize);

				m_bSendKey = !pManager->SendFrame(pData);
			}

			pData->Release();
		}
	}
}