// SmpGrbHlp.cpp
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "SmpGrbHlp.h"

#include <qedit.h>

#define CheckPointer(p,ret) {if((p)==NULL) return (ret);}

//  Use this define to enable exceptions handling
// inside of Callback helper...

//////////////////////////////////////////////////////////////////////////
//
// DumpMiniDump
//
//////////////////////////////////////////////////////////////////////////

static void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo)
{
	MINIDUMP_EXCEPTION_INFORMATION eInfo;

	if(excpInfo == NULL)
	{
		// Generate exception to get proper context in dump
		__try 
		{
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(hFile, GetExceptionInformation()),
			EXCEPTION_EXECUTE_HANDLER) 
		{
			OutputDebugString(TEXT("DumpMiniDump - Invalid context\n"));
		}
	} 
	else
	{
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = excpInfo;
		eInfo.ClientPointers = FALSE;

		// note:  MiniDumpWithIndirectlyReferencedMemory does not work on Win98
		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			//	MiniDumpNormal,
			MiniDumpWithIndirectlyReferencedMemory,
			excpInfo ? &eInfo : NULL,
			NULL,
			NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
//
// RecordExceptionInfo
//
//////////////////////////////////////////////////////////////////////////

int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS excpInfo)
{
	DWORD dwSize;
	HANDLE hMiniDumpFile;

	TCHAR szFileName[MAX_PATH];

	SYSTEMTIME st;
	GetLocalTime(&st);

	wsprintf(szFileName, TEXT("chcsva_%d%02d%02d_%02d%02d%02d%03d.dmp"),
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	// Create the file
	hMiniDumpFile = CreateFile(
		szFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);

	// Write the minidump to the file
	if(hMiniDumpFile != INVALID_HANDLE_VALUE)
	{
		DumpMiniDump(hMiniDumpFile, excpInfo);
		FlushFileBuffers(hMiniDumpFile);

		dwSize = GetFileSize(hMiniDumpFile, NULL); 

		// If we failed ... 
		if(dwSize == INVALID_FILE_SIZE && GetLastError() != NO_ERROR) 
		{
		}

		CloseHandle(hMiniDumpFile);

		// Delete wrong dump file...
		if(dwSize == 0)
		{
			OutputDebugString(TEXT("Writing minidump failed\n"));
			DeleteFile(szFileName);
		}
	}

	// return the magic value which tells Win32 that this handler didn't
	// actually handle the exception - so that things will proceed as per
	// normal.

	return EXCEPTION_EXECUTE_HANDLER;
}

#define SAVE_SAMPLEGRABBER_CALLBACK

//////////////////////////////////////////////////////////////////////
// CSampleGrabberCallback
//////////////////////////////////////////////////////////////////////

class CSampleGrabberCallback : public ISampleGrabberCB
{
private:
	explicit CSampleGrabberCallback(CSampleGrabberHelper* pCallback)
		: m_pCallback(pCallback)
		, m_lRefCount(1)
		, m_lErrors(0)
	{
	#ifdef _DEBUG
		TCHAR stMsg[128];
		wsprintf(stMsg, TEXT("CSampleGrabberCallback Constructor(%08lX)\n"), m_pCallback);

		OutputDebugString(stMsg);
	#endif
	}

	virtual ~CSampleGrabberCallback()
	{
	#ifdef _DEBUG
		TCHAR stMsg[128];
		wsprintf(stMsg, TEXT("CSampleGrabberCallback Destructor(%08lX)\n"), m_pCallback);

		OutputDebugString(stMsg);
	#endif
	}

	friend class CSampleGrabberHelper;

public:
	virtual DWORD STDMETHODCALLTYPE AddRef()
	{
		return ::InterlockedIncrement(&m_lRefCount);
	}

	virtual DWORD STDMETHODCALLTYPE Release()
	{
		if(0 == ::InterlockedDecrement(&m_lRefCount))
			delete this;

		return m_lRefCount;
	}

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv)
	{
		CheckPointer(ppv, E_POINTER);

		if(riid == IID_ISampleGrabberCB || riid == IID_IUnknown)
		{
			*ppv = (void *)static_cast<ISampleGrabberCB *>(this);
		}
		else
		{
			*ppv = NULL;
			return E_NOINTERFACE;
		}

		AddRef();

		return S_OK;
	}

    virtual HRESULT STDMETHODCALLTYPE SampleCB(double fTime, IMediaSample *pSample)
	{
		CheckPointer(m_pCallback, E_POINTER);
		CheckPointer(pSample, E_POINTER);
		
	#ifdef SAVE_SAMPLEGRABBER_CALLBACK
		CSampleGrabberCallback::SendSample(this, fTime, pSample);
	#else
		m_pCallback->OnSampleCB(fTime, pSample);
	#endif

		return S_OK;
	}

    virtual HRESULT STDMETHODCALLTYPE BufferCB(double fTime, BYTE *pData, long lLen)
	{
		CheckPointer(m_pCallback, E_POINTER);
		CheckPointer(pData, E_POINTER);

	#ifdef SAVE_SAMPLEGRABBER_CALLBACK
		CSampleGrabberCallback::SendBuffer(this, fTime, pData, lLen);
	#else
		m_pCallback->OnBufferCB(fTime, pData, lLen);
	#endif

		return S_OK;
	}

private:

#ifdef SAVE_SAMPLEGRABBER_CALLBACK

	// SendBuffer
	//
	static void __cdecl SendBuffer(CSampleGrabberCallback *pThis, double fTime, BYTE *pData, long lLen)
	{
		__try
		{
			pThis->m_pCallback->OnBufferCB(fTime, pData, lLen);
		}
		__except(pThis->m_lErrors < 10 ? HandleBufferException(GetExceptionInformation(), pData, lLen) : EXCEPTION_EXECUTE_HANDLER)
		{
			pThis->m_lErrors++;
		}
	}

	// SendSample
	//
	static void __cdecl SendSample(CSampleGrabberCallback *pThis, double fTime, IMediaSample *pSample)
	{
		__try
		{
			pThis->m_pCallback->OnSampleCB(fTime, pSample);
		}
		__except(pThis->m_lErrors < 10 ? HandleSampleException(GetExceptionInformation()) : EXCEPTION_EXECUTE_HANDLER)
		{
			pThis->m_lErrors++;
		}
	}

	// HandleBufferException
	//
	static int __cdecl HandleBufferException(PEXCEPTION_POINTERS pExcInfo, BYTE *pData, long lLen)
	{
		if(IsDebuggerPresent())
			return EXCEPTION_CONTINUE_SEARCH;

		RecordExceptionInfo(pExcInfo);

		TCHAR strExceptionInfo[256];
		PEXCEPTION_RECORD pException = pExcInfo->ExceptionRecord;
			
		// If the exception was an access violation, print out some additional
		// information, to the debugger.
		if(pException->ExceptionCode == STATUS_ACCESS_VIOLATION &&
					pException->NumberParameters >= 2)
		{
			wsprintf(strExceptionInfo, TEXT("%s location 0x%08X caused an access violation in CSampleGrabberCallback::SendBuffer(Time, 0x%08p, %ld)."),
				pException->ExceptionInformation[0] ? TEXT("Write to") : TEXT("Read from"),
				pException->ExceptionInformation[1],
				pData, lLen);
		}
		else
		{
			wsprintf(strExceptionInfo, TEXT("Unexpected exception (0x%08X) in CSampleGrabberCallback::SendBuffer(Time, 0x%08p, %ld)."),
				pException->ExceptionCode,
				pData, lLen);
		}

		Elvees::OutputF(Elvees::TError, TEXT("%s"), strExceptionInfo);

		return EXCEPTION_EXECUTE_HANDLER;
	}

	// HandleSampleException
	//
	static int __cdecl HandleSampleException(PEXCEPTION_POINTERS pExcInfo)
	{
		if(IsDebuggerPresent())
			return EXCEPTION_CONTINUE_SEARCH;

		RecordExceptionInfo(pExcInfo);
			
		Elvees::OutputF(Elvees::TError, TEXT("Unexpected exception (0x%08X) in CSampleGrabberCallback::SendSample()"),
			pExcInfo->ExceptionRecord->ExceptionCode);
		
		return EXCEPTION_EXECUTE_HANDLER;
	}

#endif

private:
	long m_lErrors;
	long m_lRefCount;
	CSampleGrabberHelper *m_pCallback;

	// No copies do not implement
	CSampleGrabberCallback(const CSampleGrabberCallback &rhs);
	CSampleGrabberCallback &operator=(const CSampleGrabberCallback &rhs);
};

//////////////////////////////////////////////////////////////////////
// CSampleGrabberHelper
//////////////////////////////////////////////////////////////////////

CSampleGrabberHelper::CSampleGrabberHelper()
{
}

CSampleGrabberHelper::~CSampleGrabberHelper()
{
}

ISampleGrabberCB* CSampleGrabberHelper::CreateSampleGrabberCB()
{
	return static_cast<ISampleGrabberCB *>(new CSampleGrabberCallback(this));
}

bool CSampleGrabberHelper::SetCallback(
	ISampleGrabber *pSG,
	long WhichMethodToCallback,
	bool bBufferSamples,
	bool bOneShot)
{
	HRESULT hr;
	ISampleGrabberCB* pCallback = NULL;

	bool bDone = false;

	do
	{
		if(!pSG) break;

		hr = pSG->SetBufferSamples(bBufferSamples);
		if(FAILED(hr)) break;

		hr = pSG->SetOneShot(bOneShot);
		if(FAILED(hr)) break;

		pCallback = CreateSampleGrabberCB();
		if(!pCallback) break;

		hr = pSG->SetCallback(pCallback, WhichMethodToCallback);
		pCallback->Release();
			
		if(SUCCEEDED(hr))
			bDone = true;
	}
	while(FALSE);

	return bDone;
}

//////////////////////////////////////////////////////////////////////
// Callback events
//////////////////////////////////////////////////////////////////////

void CSampleGrabberHelper::OnSampleCB(double /*fTime*/, IMediaSample* /*pSample*/)
{
#ifdef _DEBUG
	OutputDebugString(TEXT("Unhandled CSampleGrabberHelper::OnSampleCB()\n"));
#endif
}

void CSampleGrabberHelper::OnBufferCB(double /*fTime*/, BYTE* /*pData*/, long /*lLen*/)
{
#ifdef _DEBUG
	OutputDebugString(TEXT("Unhandled CSampleGrabberHelper::OnBufferCB()\n"));
#endif
}
