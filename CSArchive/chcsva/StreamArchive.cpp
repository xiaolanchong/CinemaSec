// StreamArchive.h: implementation of the CStreamArchive class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "StreamArchive.h"
#include "VidDecoder.h"

CStreamArchive* CStreamArchive::CreatStream(long lSourceID, INT64 startPos)
{
	CStreamArchive* pStream = new CStreamArchive(lSourceID);
	return pStream;
}

CStreamArchive::CStreamArchive(long lSourceID)
	: CFrmBuffer::Allocator(5)
	, m_lStreamID(lSourceID)
	, m_refCount(1)
	, m_newImage(false)
{
	m_pSocket = NULL;
	m_pDecoder = NULL;
	m_pFrameLast = NULL;
}

CStreamArchive::~CStreamArchive()
{
}

long CStreamArchive::AddRef()
{
	return ::InterlockedIncrement(&m_refCount);
}

long CStreamArchive::Release()
{
	if(IsBadReadPtr(this, sizeof(CStream)))
	{
		Elvees::Output(Elvees::TError, TEXT("Using CStreamArchive::Release() after destroy"));
		return 0;
	}

	if(0 == ::InterlockedDecrement(&m_refCount))
	{
		Elvees::OutputF(Elvees::TInfo, TEXT("Archive stream [%ld] deleted"),
			m_lStreamID);

		if(m_pSocket)
		{
			m_pSocket->Shutdown();
			m_pSocket->Release();
		}

		delete this;
		return 0;
	}

	return m_refCount;
}

bool CStreamArchive::Start()
{
	if(m_pSocket)
	{
		char stInit[56];
		wsprintfA(stInit, "init %ld\r\n", m_lStreamID);

		return m_pSocket->Write(stInit, (int)strlen(stInit));
	}

	return false;
}

void CStreamArchive::Stop()
{
}

bool CStreamArchive::IsStarted()
{
	return true;
}

long CStreamArchive::SeekTo(INT64 timePos)
{
	if(m_pSocket && timePos < 0xF4862CCF)
	{
		char stSeek[56];
		sprintf(stSeek, "seek %I64d\r\n", timePos);

		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace,
			TEXT("Cmd: %ld %hs"), m_lStreamID, stSeek));

		return m_pSocket->Write(stSeek, (int)strlen(stSeek)) ? 0 : 1;
	}

	Elvees::OutputF(Elvees::TTrace, TEXT("SeekTo %I64d failed"), timePos);

	return 1;
}

bool CStreamArchive::GetNextFrame(CHCS::IFrame** ppFrame, unsigned uTimeout)
{
	m_newImage.Wait(min(40, uTimeout));
	return GetLastFrame(ppFrame);
}

long CStreamArchive::GetID()
{
	return m_lStreamID;
}

long CStreamArchive::AllocatorSourceID()
{
	return m_lStreamID;
}

void CStreamArchive::SetConnection(Elvees::CSocketServer::Socket* pSocket)
{
	if(m_pSocket)
		m_pSocket->Release();

	m_pSocket = pSocket;

	if(m_pSocket)
		m_pSocket->AddRef();
}

void CStreamArchive::InitDecompressor(DWORD dwFCC, BITMAPINFO *pbiOutput, BITMAPINFO *pbiInput)
{
	if(m_pDecoder)
	{
		BITMAPINFO bihOld;
		m_pDecoder->GetInFormat(&bihOld);

		if( bihOld.bmiHeader.biWidth != pbiOutput->bmiHeader.biWidth ||
			bihOld.bmiHeader.biHeight != pbiOutput->bmiHeader.biHeight)
		{
			delete m_pDecoder;
			m_pDecoder = NULL;
		}
	}

	if(!m_pDecoder)
	{
		m_pDecoder = new CVidDecoder(dwFCC);

		if(!m_pDecoder->InitDecompressor(pbiOutput, pbiInput))
		{
			delete m_pDecoder;
			m_pDecoder = NULL;
		}

		SetFrameAllocatorFormat(pbiOutput);
	}
}

void CStreamArchive::DecompressData(const void* pData, long lSize, unsigned uFrameSeq, unsigned uTime)
{
	if(lSize <= 0)
	{
		Elvees::OutputF(Elvees::TInfo, TEXT("A[%ld] last frame received"),	m_lStreamID);

		// Allocate frame buffer
		CFrmBuffer* pFrame = AllocateFrameBuffer();

		if(pFrame)
		{
			LARGE_INTEGER lnTime;

			lnTime.LowPart = uTime;
			lnTime.HighPart = 0;

			// Copy data to frame buffer...
			pFrame->SetSignalLocked(1);
			pFrame->SetFrameTime(timeGetTime(), lnTime.QuadPart);
			pFrame->ZeroFrame();

			// Update last frame
			SetLastFrame(pFrame);

			m_newImage.Pulse(); // Disable wait
		}		

		return;
	}

	if(m_pDecoder)
	{
		LPVOID pImage = m_pDecoder->DecompressData(pData, lSize, uFrameSeq);

		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("A[%ld] %c %ld\t%u [F %d, I %d] %c %u"),
			GetID(),
			uFrameSeq & 0xFFFF ? TEXT('-') : TEXT('*'),
			lSize,
			uFrameSeq,
			(int)LOWORD(uFrameSeq),
			(int)HIWORD(uFrameSeq),
			pImage ? TEXT('+') : TEXT('~'),
			uTime));

		// Allocate frame buffer
		CFrmBuffer* pFrame = AllocateFrameBuffer();

		if(pFrame)
		{
			LARGE_INTEGER lnTime;
			
			lnTime.LowPart = uTime;
			lnTime.HighPart = 0;

			// Copy data to frame buffer...
			pFrame->SetSignalLocked(1);
			pFrame->SetFrameTime(timeGetTime(), lnTime.QuadPart);

			// Copy frame buffer
			pFrame->SetFrameData( (LPBYTE)(LPCBYTE)(pImage), m_pDecoder->GetBufferSize());

			// Update last frame
			SetLastFrame(pFrame);

			m_newImage.Pulse();
		}		
	}
}

//////////////////////////////////////////////////////////////////////
// Last frame
//////////////////////////////////////////////////////////////////////

void CStreamArchive::SetLastFrame(CHCS::IFrame* pFrame)
{
	if(!pFrame)
		return;

	Elvees::CCriticalSection::Owner lock(m_lastFrameSect);

	if(m_pFrameLast)
		m_pFrameLast->Release();

	m_pFrameLast = pFrame;
}

bool CStreamArchive::GetLastFrame(CHCS::IFrame** ppFrame)
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