// FrmBuffer.cpp: implementation of the CFrmBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "FrmBuffer.h"
#include "VidDecoder.h"

//////////////////////////////////////////////////////////////////////
// CFrmBuffer
//////////////////////////////////////////////////////////////////////

CFrmBuffer::CFrmBuffer(Allocator &allocator, unsigned size)
	: m_allocator(allocator)
	, m_lrefCount(1)
	, m_size(size)
	, m_used(0)
{
	Empty();
}

void CFrmBuffer::Empty()
{
	m_used = 0;

	m_lSignalLocked = -1;
	m_uFrameTime  = 0;
	m_uSystemTime = 0;
}

void* CFrmBuffer::operator new(unsigned objectSize, unsigned bufferSize)
{
	// Round to 4kb size..
	return malloc((objectSize + bufferSize + 0xFFF) & 0xFFFFF000);
}

void CFrmBuffer::operator delete(void *pObject)
{
	free(pObject);
}

void CFrmBuffer::SetSignalLocked(long lSignalLocked)
{
	m_lSignalLocked = lSignalLocked;
}

void CFrmBuffer::SetFrameData(BYTE* pData, unsigned dataLen)
{
	if(pData)
	{
		m_used = min(dataLen, m_size);
		CopyMemory(m_buffer, pData, m_used);
	}
}

void CFrmBuffer::SetFrameTime(unsigned frameTime, __int64 systemTime)
{
	m_uFrameTime  = frameTime;
	m_uSystemTime = systemTime;
}

//////////////////////////////////////////////////////////////////////
// IFrame
//////////////////////////////////////////////////////////////////////

long CFrmBuffer::AddRef()
{
//	long lrefCount = ::InterlockedIncrement(&m_lrefCount);
//	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("[%X] %ld addref"), this, lrefCount));
//	return lrefCount;

	return ::InterlockedIncrement(&m_lrefCount);
}

long CFrmBuffer::Release()
{
//	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("[%X] %ld release"), this, lrefCount));

	if(IsBadReadPtr(this, sizeof(CFrmBuffer)))
	{
		Elvees::Output(Elvees::TError, TEXT("Using CFrmBuffer::Release() after destroy"));
		return 0;
	}

	if(0 == ::InterlockedDecrement(&m_lrefCount))
	{
		Empty();

		if(IsBadReadPtr(&m_allocator, sizeof(m_allocator)))
			delete this;
		else
			m_allocator.Release(this);

		return 0;
	}

	return m_lrefCount;
}

long CFrmBuffer::GetSourceID()
{
	return m_allocator.AllocatorSourceID();
}

__int64 CFrmBuffer::GetTime()
{
	return m_uSystemTime;
}

unsigned CFrmBuffer::GetRelativeTime()
{
	return m_uFrameTime;
}

long CFrmBuffer::IsSignalLocked()
{
	return m_lSignalLocked;
}

long CFrmBuffer::GetFormat(BITMAPINFO* pbi)
{
	Elvees::CCriticalSection::Owner lock(m_allocator.m_dataSection);

	int cbSize = m_allocator.m_pbiFrm->bmiHeader.biSize + \
		m_allocator.m_pbiFrm->bmiHeader.biClrUsed*4;

	if(pbi == NULL)
		return cbSize;
	
	try
	{
		CopyMemory(pbi, m_allocator.m_pbiFrm, cbSize);
	}
	catch(...)
	{
		return -1;
	}

	return 0;
}

const void* CFrmBuffer::GetBytes()
{
	return m_buffer;
}

bool CFrmBuffer::Draw(HDC hDC, long x, long y, long cx, long cy)
{
	bool bOK = false;

	Elvees::CCriticalSection::Owner lock(m_allocator.m_dataSection);

	DWORD dwFCC = m_allocator.m_pbiFrm->bmiHeader.biCompression;

	if(dwFCC == BI_RGB || dwFCC == BI_BITFIELDS)
	{
		bOK = (GDI_ERROR != StretchDIBits(hDC, x, y, cx, cy, 0, 0,
				m_allocator.m_pbiFrm->bmiHeader.biWidth,
				m_allocator.m_pbiFrm->bmiHeader.biHeight,
				m_buffer, m_allocator.m_pbiFrm, DIB_RGB_COLORS, SRCCOPY));
	}
	else if(m_allocator.m_pDecoder)
	{
		BITMAPINFO biOut;
		m_allocator.m_pDecoder->GetOutFormat(&biOut);

		void* pImage = m_allocator.m_pDecoder->DecompressData(m_buffer,
							m_allocator.m_pbiFrm->bmiHeader.biSizeImage, true);

		if(pImage)
		{
			bOK = (GDI_ERROR != StretchDIBits(hDC, x, y, cx, cy, 0, 0,
				biOut.bmiHeader.biWidth, biOut.bmiHeader.biHeight,
				pImage, &biOut, DIB_RGB_COLORS, SRCCOPY));
		}
	}

	return bOK;
}

bool CFrmBuffer::CreateCopy(CHCS::IFrame **ppFrame)
{
	if(ppFrame == NULL)
		return false;

	CFrmBuffer *pFrame = m_allocator.AllocateFrameBuffer();

	if(pFrame)
	{
		CopyMemory(pFrame->m_buffer, m_buffer, m_used);
		pFrame->m_used = m_used;

		pFrame->m_lSignalLocked = m_lSignalLocked;
		pFrame->m_uFrameTime  = m_uFrameTime;
		pFrame->m_uSystemTime = m_uSystemTime;
	}

	*ppFrame = pFrame;

	return (pFrame != NULL);
}

void CFrmBuffer::ZeroFrame()
{
	ZeroMemory(m_buffer, m_size);
}

///////////////////////////////////////////////////////////////////////////////
// CIOBuffer::Allocator
///////////////////////////////////////////////////////////////////////////////

#pragma TODO("dynamic update maxFreeFrames, depend on frame using...")

CFrmBuffer::Allocator::Allocator(unsigned maxFreeFrames)
	: m_maxFreeFrames(maxFreeFrames)
{
	m_pbiFrm  = NULL;
	m_bufSize = 0;

	m_pDecoder = NULL;
}

CFrmBuffer::Allocator::~Allocator()
{
	if(m_pbiFrm)
	{
		free(m_pbiFrm);
		m_pbiFrm = NULL;
	}

	if(m_pDecoder)
	{
		delete m_pDecoder;
		m_pDecoder = NULL;
	}
}

CFrmBuffer *CFrmBuffer::Allocator::AllocateFrameBuffer()
{
	if(!m_pbiFrm)
		return NULL;

	Elvees::CCriticalSection::Owner lock(m_listSection);

	CFrmBuffer *pBuffer = NULL;

	if(!m_freeList.Empty())
	{
		try
		{
			pBuffer = m_freeList.PopNode();
			pBuffer->AddRef();
		}
		catch(...)
		{
			pBuffer = NULL;
			Elvees::OutputF(Elvees::TWarning, TEXT("[%X] AllocateFrameBuffer m_freeList.PopNode() failed"), this);

		//	m_freeList.m_pHead = NULL;
		//	m_freeList.m_pEnd  = NULL;
		//	m_freeList.m_numNodes = 0;
		}
	}
	
	if(!pBuffer)
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("[%X] AllocateFrameBuffer AL:%ld FL:%ld"), this,
			m_activeList.Count(), m_freeList.Count()));

		pBuffer = new(m_bufSize) CFrmBuffer(*this, m_bufSize);
	}

	if(pBuffer)
	{
		m_activeList.PushNode(pBuffer);
	}

	return pBuffer;
}

CHCS::IFrame* CFrmBuffer::Allocator::AllocateFrame()
{
	return AllocateFrameBuffer();
}

void CFrmBuffer::Allocator::Release(CFrmBuffer *pBuffer)
{
	if(!pBuffer)
		return;

	Elvees::CCriticalSection::Owner lock(m_listSection);

	// unlink from the in-use list

	pBuffer->RemoveFromList();

	if(m_maxFreeFrames == 0 || m_freeList.Count() < m_maxFreeFrames)
	{
		pBuffer->Empty();
      
		// add to the free list
		m_freeList.PushNode(pBuffer);
	}
	else
	{
		DestroyBuffer(pBuffer);
	}
}

void CFrmBuffer::Allocator::FlushFrames()
{
	Elvees::CCriticalSection::Owner lock(m_listSection);

#ifdef _DEBUG
	Elvees::Output(Elvees::TTrace, TEXT("CFrmBuffer::Allocator flushing buffers"));

	if(!m_activeList.Empty())
		Elvees::Output(Elvees::TWarning, TEXT("CFrmBuffer::Allocator::m_activeList not empty"));
#endif
		
	while(!m_activeList.Empty())
		DestroyBuffer(m_activeList.PopNode());

	while(!m_freeList.Empty())
		DestroyBuffer(m_freeList.PopNode());
}

void CFrmBuffer::Allocator::DestroyBuffer(CFrmBuffer *pBuffer)
{
	try
	{
		if(pBuffer)
			delete pBuffer;
	}
	catch(...)
	{
		DEBUG_ONLY(Elvees::Output(Elvees::TTrace,
			TEXT("Unexpected exceptiont on CFrmBuffer::Allocator::DestroyBuffer()")));
	}
}

bool CFrmBuffer::Allocator::SetFrameAllocatorFormat(BITMAPINFO* pbiImg)
{
	if(!pbiImg || IsBadReadPtr(pbiImg, sizeof(BITMAPINFOHEADER)))
	{
		DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("SetFrameAllocatorFormat bad format")));
		return false;
	}

	bool bDone = false;
	
	unsigned    _bufSize = 0;
	BITMAPINFO* _pbiImg  = NULL;

	int cbSize = pbiImg->bmiHeader.biSize + pbiImg->bmiHeader.biClrUsed*4;

	if( cbSize > 0 &&
		pbiImg->bmiHeader.biWidth  > 0 &&
		pbiImg->bmiHeader.biBitCount > 0)
	{
		// allocate enough buffer
		_pbiImg = reinterpret_cast<LPBITMAPINFO>( malloc( max(cbSize, sizeof(BITMAPINFO)) ) );

		// width rounded to 4 bytes..
		_bufSize = (((pbiImg->bmiHeader.biWidth * pbiImg->bmiHeader.biBitCount + 31) & (~31)) >> 3) * abs(pbiImg->bmiHeader.biHeight);

		if(_pbiImg)
		{
			try
			{
				CopyMemory(_pbiImg, pbiImg, cbSize);

				// Adjust image buffer size
				_pbiImg->bmiHeader.biSizeImage = _bufSize;

				bDone = true;
			}
			catch(...)
			{
				DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("SetFrameAllocatorFormat copy failed")));

				free(_pbiImg);
				_pbiImg  = NULL;
				_bufSize = 0;
				bDone = false;
			}
		}
	}
#ifdef _DEBUG
	else
	{
		Elvees::OutputF(Elvees::TTrace, TEXT("SetFrameAllocatorFormat cb=%d, w=%ld, bc=%d"),
			cbSize,
			pbiImg->bmiHeader.biWidth,
			pbiImg->bmiHeader.biBitCount);
	}
#endif

	if(bDone)
	{
		if(m_pbiFrm)
		{
			FlushFrames();
			free(m_pbiFrm);
		}

		if(m_pDecoder)
		{
			delete m_pDecoder;
			m_pDecoder = NULL;
		}

		m_pbiFrm  = _pbiImg;
		m_bufSize = _bufSize;

		if( m_pbiFrm->bmiHeader.biCompression != BI_RGB &&
			m_pbiFrm->bmiHeader.biCompression != BI_BITFIELDS)
		{
			m_pDecoder = new CVidDecoder(m_pbiFrm->bmiHeader.biCompression);

			if(m_pDecoder)
			{
				BITMAPINFO biOut;
				ZeroMemory(&biOut, sizeof(BITMAPINFO));

				biOut.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				biOut.bmiHeader.biCompression = BI_RGB;
				biOut.bmiHeader.biBitCount = 24;
				biOut.bmiHeader.biPlanes = 1;
				biOut.bmiHeader.biWidth  = m_pbiFrm->bmiHeader.biWidth;
				biOut.bmiHeader.biHeight = m_pbiFrm->bmiHeader.biHeight;
				biOut.bmiHeader.biSizeImage = DIBSIZE(biOut.bmiHeader);

				if(!m_pDecoder->InitDecompressor(&biOut, m_pbiFrm))
				{
					Elvees::Output(Elvees::TError, TEXT("SetFrameAllocatorFormat: InitDecompressor failed"));
				}
			}
			else
			{
				DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("DrawDibOpen failed")));
			}
		}
	}

	return bDone;
}

unsigned CFrmBuffer::Allocator::GetFrameCount() const
{
	return m_activeList.Count();
}

unsigned CFrmBuffer::Allocator::GetFrameSize() const
{
	return m_bufSize;
}
