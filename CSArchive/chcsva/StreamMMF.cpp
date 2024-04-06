// StreamMMF.cpp: implementation of the CStreamMMF class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "StreamMMF.h"

//////////////////////////////////////////////////////////////////////
// CStreamMMF
//////////////////////////////////////////////////////////////////////

CStream* CStreamMMF::CreateStream(
	UUID& streamID,
	LPCTSTR stMoniker,
	bool bLocal)
{
	CStreamMMF* pStream = new CStreamMMF(streamID, bLocal);

	if(pStream)
	{
		// Store for new device detection
		lstrcpy(pStream->m_stMoniker, stMoniker);
	}
	else
	{
		Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
	}

	return pStream;
}

CStreamMMF::CStreamMMF(UUID& streamUID, bool bLocal)
	: CStream(streamUID)
	, CFrmBuffer::Allocator(5)
	, m_bLocal(bLocal)
	, m_newImage(false)
{
	m_bFormatSet = false;
	m_dwLastUpdated = 0;

	m_pFrameLast = NULL;
}

CStreamMMF::~CStreamMMF()
{
	if(m_pFrameLast)
		m_pFrameLast->Release();
}

void CStreamMMF::Delete()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////
// CStream
//////////////////////////////////////////////////////////////////////

bool CStreamMMF::IsValid()
{
	return m_bFormatSet;
}

bool CStreamMMF::IsDeviceString(LPCTSTR stDevice)
{
	return lstrcmpi(m_stMoniker, stDevice) == 0;
}

//////////////////////////////////////////////////////////////////////
// Allocator ID request
//////////////////////////////////////////////////////////////////////

long CStreamMMF::AllocatorSourceID()
{
	return GetID(); // CStream::GetID()
}

bool CStreamMMF::Start()
{
	if(m_bLocal)
		Elvees::OutputF(Elvees::TInfo, TEXT("Stream [%ld] started"), GetID());

	return true;
}

void CStreamMMF::Stop()
{
}

bool CStreamMMF::IsStarted()
{
	return m_bFormatSet;
}

long CStreamMMF::SeekTo(INT64 timePos)
{
	return 0;
}

bool CStreamMMF::GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout)
{
	if(ppFrame == NULL)
		return false;

	*ppFrame = NULL;

	DWORD dwTime = timeGetTime();

	if(dwTime - m_dwLastUpdated > 5000)
	{
#ifdef _DEBUG
		ULONG addr = 0;

	#ifndef _UNICODE
		addr = inet_addr(m_stMoniker);
	#else
		char stAddress[56];

		WideCharToMultiByte(CP_ACP, 0, m_stMoniker,
			(int)wcslen(m_stMoniker) + 1,
			stAddress, sizeof(stAddress), NULL, NULL);

		addr = inet_addr(stAddress);
	#endif

		CManager* pManager = GetManager();

		if(pManager->IsClientStreamStarted(addr))
		{
			Elvees::OutputF(Elvees::TWarning, TEXT("GetNextFrame[%ld]: usage=%ld, decoder=%s"),
				GetID(), m_refCount, DecoderState());

			if(m_pSocket)
			{
				if(!m_pSocket->Write("printsockstatus\r\n", 17))
				{
					Elvees::OutputF(Elvees::TWarning, TEXT("GetNextFrame[%ld]: write failed"), GetID());
					m_pSocket->Shutdown();
				}
			}
		}
		else
		{
			Elvees::OutputF(Elvees::TWarning, TEXT("GetNextFrame[%ld]: restarting"), GetID());
			pManager->StartClientStream(m_stMoniker);
		}

#else
		CStream::AddRef();  // Force reconect
		CStream::Release();
#endif

		m_dwLastUpdated = dwTime;
	}

	m_newImage.Wait(min(40, uTimeout));

	return GetLastFrame(ppFrame);
}

void CStreamMMF::SetStreamFormat(BITMAPINFO *pbiOutput)
{
	if(!pbiOutput)
		return;

	if(SetFrameAllocatorFormat(pbiOutput))
	{
		Elvees::OutputF(Elvees::TInfo, TEXT("SetFormat stream[%ld] (%ld x %ld)"),
			GetID(), pbiOutput->bmiHeader.biWidth, pbiOutput->bmiHeader.biHeight);

		m_bFormatSet = true;
	}
	else
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Fail to set allocator format (%ldx%ld)"),
			pbiOutput->bmiHeader.biWidth, pbiOutput->bmiHeader.biHeight));
	}
}

void CStreamMMF::SetStreamBuffer(void* pvBits, long cbBits)
{
	m_dwLastUpdated = timeGetTime();

	// Allocate frame buffer
	CFrmBuffer* pFrame = AllocateFrameBuffer();

	if(pFrame)
	{
		__time64_t ltime;
		_time64(&ltime);

		// Copy data to frame buffer...
		pFrame->SetSignalLocked(-1);
		pFrame->SetFrameTime(m_dwLastUpdated, ltime);

		// Copy frame buffer
		if(pvBits)
			pFrame->SetFrameData((BYTE*)pvBits, GetFrameSize());

		// Update last frame
		SetLastFrame(pFrame);

		m_newImage.Pulse();
	}
	else
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("CStreamMMF::SetStreamBuffer - Fail to alloc buffer")));
	}
}

bool CStreamMMF::IsLocal()
{
	return m_bLocal;
}

//////////////////////////////////////////////////////////////////////
// Last frame
//////////////////////////////////////////////////////////////////////

void CStreamMMF::SetLastFrame(CHCS::IFrame* pFrame)
{
	if(!pFrame)
		return;

	Elvees::CCriticalSection::Owner lock(m_lastFrameSect);

	if(m_pFrameLast)
		m_pFrameLast->Release();

	m_pFrameLast = pFrame;
}

bool CStreamMMF::GetLastFrame(CHCS::IFrame** ppFrame)
{
	if(!ppFrame)
		return false;

	Elvees::CCriticalSection::Owner lock(m_lastFrameSect);

	if(m_pFrameLast)
	{
		m_pFrameLast->AddRef();
		*ppFrame = m_pFrameLast;
		return TRUE;
	}

	return FALSE;
}