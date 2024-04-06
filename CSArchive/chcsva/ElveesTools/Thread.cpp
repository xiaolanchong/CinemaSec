#include "Thread.h"

#include <process.h>
#include <errno.h>

#include "Exceptions.h"
#include "Todo.h"
#include "Utils.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// CThread
///////////////////////////////////////////////////////////////////////////////

CThread::CThread() : m_hThread(0)
{
}
      
CThread::~CThread()
{
	if(m_hThread != 0)
	{
		::CloseHandle(m_hThread);
	}
}

HANDLE CThread::GetHandle() const
{
	return m_hThread;
}

void CThread::Start()
{
	if(!IsStarted())
	{
		unsigned int threadID = 0;

		m_hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (void*)this, 0, &threadID);

		if(m_hThread == 0)
		{
			throw CWin32Exception(TEXT("CThread::Start() - _beginthreadex"), errno);
		}
	}
	else
	{
		throw CException(TEXT("CThread::Start()"), TEXT("Thread already running - you can only call Start() once!"));
	}
}

void CThread::Wait() const
{
   if(!Wait(INFINITE))
   {
      throw CException(TEXT("CThread::Wait()"), TEXT("Unexpected timeout on infinite wait"));
   }
}

bool CThread::Wait(DWORD timeoutMillis) const
{
	#pragma TODO("base class? Waitable?")

	bool ok;

	DWORD result = ::WaitForSingleObject(m_hThread, timeoutMillis);

	if (result == WAIT_TIMEOUT)
	{
		ok = false;
	}
	else if (result == WAIT_OBJECT_0)
	{
		ok = true;
	}
	else
	{
		throw CWin32Exception(TEXT("CThread::Wait() - WaitForSingleObject"), ::GetLastError());
	}
    
	return ok;
}

unsigned int __stdcall CThread::ThreadFunction(void *pV)
{
	int result = 0;

	CThread* pThis = (CThread*)pV;

	CStructuredException::MapSEtoCE();
   
	if(pThis)
	{
		try
		{
			result = pThis->Run();
		}
		catch(const CException& e)
		{
			OutputF(TCritical, TEXT("CThread::Run() - Exception: %s - %s"), e.GetWhere(), e.GetMessage());
		}
		catch(...)
		{
			Output(TCritical, TEXT("CThread::Run() - Unexpected exception"));
		}
	}

	return result;
}

void CThread::Terminate(DWORD exitCode /* = 0 */)
{
	if(!::TerminateThread(m_hThread, exitCode))
	{
		throw CWin32Exception(TEXT("CThread::Terminate() - TerminateThread"), ::GetLastError());
	}
}

bool CThread::IsStarted()
{
	return (WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT) ? true : false;
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
