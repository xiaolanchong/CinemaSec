// Stream.cpp: implementation of the CStream class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "Stream.h"
#include "VidDecoder.h"

//////////////////////////////////////////////////////////////////////
// CStream
//////////////////////////////////////////////////////////////////////

CStream::CStream(UUID& streamUID)
	: m_StreamUID(streamUID)	
	, m_lStreamID(-1)
	, m_refCount(1)
{
	m_pDecoder = NULL;

#ifdef _DEBUG
	m_pSocket = NULL;
#endif
}

CStream::~CStream()
{
	RemoveFromList();

	if(m_pDecoder)
		delete m_pDecoder;
}

//////////////////////////////////////////////////////////////////////
// IStream
//////////////////////////////////////////////////////////////////////

long CStream::AddRef()
{
//	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace,
//		TEXT("CStream[%X]::AddRef(%ld)"), this, 
//		InterlockedExchange(&m_refCount, m_refCount)));

	long lRefCount = ::InterlockedIncrement(&m_refCount);

	if(!IsLocal() && m_refCount > 2) // 1- orig, +1- writer
	{
		CManager* pManager = GetManager();
		
		if(pManager)
			pManager->StartClientStream(m_stMoniker);
	}

	return lRefCount;
}

long CStream::Release()
{
//	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace,
//		TEXT("CStream[%X]::Release(%ld)"), this,
//		InterlockedExchange(&m_refCount, m_refCount)));

	if(IsBadReadPtr(this, sizeof(CStream)))
	{
		Elvees::Output(Elvees::TError, TEXT("Using CStream::Release() after destroy"));
		return 0;
	}

	long lRefCount = ::InterlockedDecrement(&m_refCount);

	if(0 == lRefCount)
	{
		TCHAR stDevice[64];

		UUID StreamUID = GetUID();
		UuidString pszUuid = 0;

		if(::UuidToString(&StreamUID, &pszUuid) == RPC_S_OK && pszUuid)
		{
			wsprintf(stDevice, TEXT("{%s}"), pszUuid);
			RpcStringFree(&pszUuid);
		}
		else
		{
			lstrcpy(stDevice, TEXT("{UUID conversion failed}"));
		}

		Elvees::OutputF(Elvees::TInfo, TEXT("Stream [%ld] \"%s\" deleted"),
			m_lStreamID, stDevice);

		Delete();
	}

	return lRefCount;
}

UUID CStream::GetUID()
{
	return m_StreamUID;
}

long CStream::GetID()
{
	return m_lStreamID;
}

void CStream::SetID(long streamID)
{
	m_lStreamID = streamID;
}

bool CStream::IsStreamUID(UUID& streamUID)
{
	return IsEqualGUID(streamUID, m_StreamUID) ? true : false;
}

bool CStream::IsDeviceString(LPCTSTR stDevice)
{
	return false;
}

bool CStream::IsValid()
{
	return false;
}

void CStream::SetStreamFormat(BITMAPINFO *pbiOutput)
{
}

void CStream::SetStreamBuffer(void* pvBits, long cbBits)
{
}

void CStream::InitDecompressor(DWORD dwFCC, BITMAPINFO *pbiOutput, BITMAPINFO *pbiInput)
{
	if(!m_pDecoder)
	{
		m_pDecoder = new CVidDecoder(dwFCC);

		if(!m_pDecoder->InitDecompressor(pbiOutput, pbiInput))
		{
			delete m_pDecoder;
			m_pDecoder = NULL;
		}
		else
			SetStreamFormat(pbiOutput);
	}
}

void CStream::DecompressData(const void* pData, long lSize, unsigned uFrameSeq)
{
	if(m_pDecoder && m_refCount > 2) // 1- orig, +1- writer
	{
		LPVOID pImage = m_pDecoder->DecompressData(pData, lSize, uFrameSeq);

#ifdef _DEBUG
		if(pImage == NULL)
		{
			Elvees::OutputF(Elvees::TTrace, TEXT("S[%ld] %c %ld\t%u [F %d, I %d] %c"),
				GetID(),
				uFrameSeq & 0xFFFF ? TEXT('-') : TEXT('*'),
				lSize,
				uFrameSeq,
				(int)LOWORD(uFrameSeq),
				(int)HIWORD(uFrameSeq),
				pImage ? TEXT('+') : TEXT('~'));
		}
#endif

		if(pImage)
			SetStreamBuffer(pImage, m_pDecoder->GetBufferSize());
	}
}

bool CStream::IsLocal()
{
	return true;
}

#ifdef _DEBUG

LPCTSTR CStream::DecoderState()
{
	if(!m_pDecoder)
	{
		lstrcpy(m_stState, TEXT("not created"));
	}
	else if(!m_pDecoder->IsDecompressionInited())
	{
		lstrcpy(m_stState, TEXT("not inited"));
	}
	else if(!m_pDecoder->IsDecompressionStarted())
	{
		lstrcpy(m_stState, TEXT("not started"));
	}
	else
	{
		wsprintf(m_stState, TEXT("%s [F%d,I%d]"),
			m_pDecoder->m_bWaitIFrame ? TEXT("WKF") : TEXT("decoding"),
			m_pDecoder->m_uFrameSeq, m_pDecoder->m_uIFrameSeq);
	}

	return m_stState;
}

#endif