// FrmBuffer.h: interface for the CFrmBuffer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CVidDecoder;

#pragma warning(disable: 4200) // nonstandard extension used : zero-sized array in struct/union

class CFrmBuffer
	: public CHCS::IFrame
	, public Elvees::CNodeList::Node
{
public:
	class Allocator;
	friend class Allocator;
	
	// CHCS::IFrame Implementation
	//
	
	virtual long AddRef();
	virtual long Release();

	virtual long GetSourceID();
	virtual long IsSignalLocked();

	virtual __int64  GetTime();
	virtual unsigned GetRelativeTime();
		
	virtual const void* GetBytes();
	virtual long GetFormat(BITMAPINFO* pbi);

	virtual bool Draw(HDC hDC, long x, long y, long cx, long cy);
	virtual bool CreateCopy(CHCS::IFrame **ppFrame);

	// Frame data access
	//

	void SetSignalLocked(long lSignalLocked);
	void SetFrameData(BYTE* pData, unsigned dataLen);
	void SetFrameTime(unsigned frameTime, __int64 systemTime);

	void ZeroFrame();
	
protected:
	void Empty();

private:
	Allocator &m_allocator;

	long m_lrefCount;
	
	unsigned m_used;
	const unsigned m_size;
	
	// IFrame data
	__int64  m_uSystemTime;
	unsigned m_uFrameTime;

	long m_lSignalLocked;

	BYTE m_buffer[0]; // start of the actual buffer, must remain the last
                      // data member in the class.
private:
	static void *operator new(size_t bufferSize);
	static void operator delete(void *pObj, size_t bufferSize);
      
	CFrmBuffer(Allocator &m_allocator, unsigned size);

	// No copies do not implement
	CFrmBuffer(const CFrmBuffer &rhs);
	CFrmBuffer &operator=(const CFrmBuffer &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CFrmBuffer::Allocator
///////////////////////////////////////////////////////////////////////////////

class CFrmBuffer::Allocator
{
public:
	friend class CFrmBuffer;

	explicit Allocator(unsigned maxFreeFrames);
	virtual ~Allocator();

	virtual long AllocatorSourceID() = 0;

	// Set format for frames to allocate
	bool SetFrameAllocatorFormat(BITMAPINFO* pbiImg);

	// Return null untill SetFrameAllocatorFormat called
	CHCS::IFrame* AllocateFrame();

	unsigned GetFrameCount() const;
	unsigned GetFrameSize() const;

protected:
	CFrmBuffer *AllocateFrameBuffer();
	
	// Destroy all frames...
	void FlushFrames();

private:
	void Release(CFrmBuffer *pBuffer);
	void DestroyBuffer(CFrmBuffer *pBuffer);

private:	
	const unsigned m_maxFreeFrames;
	
	typedef Elvees::TNodeList<CFrmBuffer> BufferList;
      
	BufferList m_freeList;
	BufferList m_activeList;

	// Frame format
	BITMAPINFO* m_pbiFrm;
	unsigned    m_bufSize;

	// Video decoder
	CVidDecoder *m_pDecoder;

	// List operation synchronization
	Elvees::CCriticalSection m_listSection;
	Elvees::CCriticalSection m_dataSection;
	
	// No copies do not implement
	Allocator(const Allocator &rhs);
	Allocator &operator=(const Allocator &rhs);
};
