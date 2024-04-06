#include "ThreadPool.h"
#include "Exceptions.h"
#include "Utils.h"

#pragma warning( disable : 4127 ) // conditional expression is constant

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// CThreadPool
//////////////////////////////////////////////////////////////////////////////

CThreadPool::CThreadPool(
	unsigned initialThreads,
	unsigned minThreads,
	unsigned maxThreads,
	unsigned maxDormantThreads,
	unsigned poolMaintPeriod,
	unsigned dispatchTimeout,
	unsigned maxThreadsDispatchTimeout)
	: m_workPort(0)
	, m_dispatchPort(0)
	, m_activeThreads(0)
	, m_initialisedThreads(0)
	, m_processingThreads(0)
	, m_initialThreads(initialThreads)
	, m_minThreads(minThreads)
	, m_maxThreads(maxThreads)
	, m_maxDormantThreads(maxDormantThreads)
	, m_poolMaintPeriod(poolMaintPeriod)
	, m_dispatchTimeout(dispatchTimeout)
	, m_maxThreadsDispatchTimeout(maxThreadsDispatchTimeout)
{
	m_poolEmptyEvent.Set();
}

CThreadPool::~CThreadPool()
{
	try
	{
		if(!m_ThreadList.Empty())
		{
			DEBUG_ONLY(Output(TTrace, TEXT("~CThreadPool() - Shutting down worker threads")));
			WaitForShutdownToComplete();
		}

		if(!m_ThreadList.Empty())
		{
			DEBUG_ONLY(Output(TTrace, TEXT("~CThreadPool() - Unexpected - thread pool still contains threads")));
		}
	}
	catch(...)
	{
	}
}

void CThreadPool::Dispatch(
	DWORD completionKey, 
	DWORD dwNumBytes /*= 0*/, 
	OVERLAPPED *pOverlapped /*= 0*/) 
{
	if(completionKey == 0)
	{
		throw CException(TEXT("CThreadPool::Dispatch()"), TEXT("0 is an invalid value for completionKey"));
	}

	m_dispatchPort.PostStatus(completionKey, dwNumBytes, pOverlapped); 
}

void CThreadPool::InitiateShutdown()
{
	m_shutdownEvent.Set();
}

void CThreadPool::WaitForShutdownToComplete(
	unsigned timeout /* = INFINITE */,
	bool forceTermination /* = false */)
{
	InitiateShutdown();

	if(!Wait(DWORD(timeout)) && forceTermination)
	{
		DEBUG_ONLY(Output(TTrace, TEXT("Terminating worker threads")));

		TerminateWorkerThreads();
	}

	Wait();
}

int CThreadPool::Run()
{
	try
	{
		for(unsigned i = 0; i < m_initialThreads; ++i)
		{
			StartWorkerThread();
		}

		DWORD lastMaintainance = GetTickCount();

		while(true)
		{
			if(m_shutdownEvent.Wait(0))
			{
				// time to shutdown...
				break;
			}

			DWORD completionKey;
			DWORD dwNumBytes;
			OVERLAPPED *pOverlapped;

			bool ok = m_dispatchPort.GetStatus(
				&completionKey, 
				&dwNumBytes, 
				&pOverlapped, 
				DWORD(m_poolMaintPeriod));

			if(m_shutdownEvent.Wait(0))
			{
				// time to shutdown...
				break;
			}

			if(ok)
			{
				HandleDispatch(completionKey, dwNumBytes, pOverlapped);

				if(GetTickCount() - lastMaintainance >= m_poolMaintPeriod)
				{
					HandleDormantThreads();

					lastMaintainance = GetTickCount();
				}
			}
			else
			{
				DWORD lastError = ::GetLastError();

				if(lastError == WAIT_TIMEOUT)
				{
					HandleDormantThreads();
				}
				else
				{
					DEBUG_ONLY(OutputF(TTrace, TEXT("CThreadPool::Run() Unexpected - %s"), GetLastErrorMessage(lastError)));
				}
			}
		}

		StopWorkerThreads(m_activeThreads);

		m_poolEmptyEvent.Wait();
	}
	catch(const CException& e)
	{
		OutputF(TCritical, TEXT("CThreadPool::Run() - Exception: %s - %s"), e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Output(TCritical, TEXT("CThreadPool::Run() - Unexpected exception"));
	}

	return 0;
}

void CThreadPool::HandleDispatch(
	DWORD completionKey, 
	DWORD dwNumBytes, 
	OVERLAPPED *pOverlapped) 
{
	HANDLE handlesToWaitFor[2];

	handlesToWaitFor[0] = m_dispatchCompleteEvent.GetEvent();
	handlesToWaitFor[1] = m_shutdownEvent.GetEvent();

	m_dispatchCompleteEvent.Reset();

	bool processed = false;
   
	m_workPort.PostStatus(completionKey, dwNumBytes, pOverlapped); 

	// wait for someone to toggle the 'got message' event?

	DWORD timeoutMillis = DWORD(m_dispatchTimeout + (((unsigned)m_processingThreads == m_maxThreads) ? m_maxThreadsDispatchTimeout : 0));

	bool threadStarted = false;

	while(!processed)
	{
		DWORD result = ::WaitForMultipleObjects(2, handlesToWaitFor, false, timeoutMillis);

		if (result == WAIT_OBJECT_0)
		{
			processed = true;
		}
		else if (result == WAIT_OBJECT_0 + 1)
		{
			// Shutdown request...
			break;
		}
		else if (result == WAIT_TIMEOUT)
		{
			if (!threadStarted && m_processingThreads == m_activeThreads && (unsigned)m_activeThreads < m_maxThreads)
			{            
				StartWorkerThread();

				threadStarted = true;
			}
		}
		else
		{
			throw CWin32Exception(TEXT("CThreadPool::Dispatch() - WaitForMultipleObjects"), ::GetLastError());
		}
	}
}

void CThreadPool::HandleDormantThreads()
{
	if((unsigned)m_activeThreads > m_minThreads)
	{
		const unsigned dormantThreads = m_activeThreads - m_processingThreads;

		if(dormantThreads > m_maxDormantThreads)
		{
			const unsigned threadsToShutdown = (dormantThreads - m_maxDormantThreads) / 2 + 1;

			StopWorkerThreads(threadsToShutdown);
		}
	}
}

void CThreadPool::StopWorkerThreads(unsigned numThreads)
{
	for (unsigned i = 0; i < numThreads; ++i)
	{
		m_workPort.PostStatus(0);
	}
}

void CThreadPool::TerminateWorkerThreads()
{
	CCriticalSection::Owner lock(m_criticalSection);

	while(!m_ThreadList.Empty())
	{
		WorkerThread* pThread = m_ThreadList.Head();

		DEBUG_ONLY(Output(TTrace, TEXT("Terminating thread")));
		pThread->Terminate(0);

		ThreadStopped(pThread);
	}
}

void CThreadPool::StartWorkerThread()
{
	WorkerThread *pThread = CreateWorkerThread(
		m_workPort, 
		m_dispatchCompleteEvent, 
		*this);

	if(pThread)
	{
		{
			CCriticalSection::Owner lock(m_criticalSection);
			m_ThreadList.PushNode(pThread);
		}

		pThread->Start();
	}
}

void CThreadPool::ThreadStarted()
{
	::InterlockedIncrement(&m_activeThreads);
	
	OnThreadStarted();
}

void CThreadPool::ThreadInitialised()
{
	::InterlockedIncrement(&m_initialisedThreads);

	OnThreadInitialised();
}

void CThreadPool::ProcessingStarted()
{
	::InterlockedIncrement(&m_processingThreads);

	OnProcessingStarted();
}

void CThreadPool::ReceivedMessage()
{
	OnReceivedMessage();
}

void CThreadPool::ProcessingStopped()
{
	::InterlockedDecrement(&m_processingThreads);

	OnProcessingStopped();
}

void CThreadPool::ThreadStopped(WorkerThread *pThread)
{
	::InterlockedDecrement(&m_activeThreads);
	::InterlockedDecrement(&m_initialisedThreads);

	{
		CCriticalSection::Owner lock(m_criticalSection);
		pThread->RemoveFromList();
	}

	OnThreadStopped();
}

///////////////////////////////////////////////////////////////////////////////
// CThreadPool::WorkerThread
///////////////////////////////////////////////////////////////////////////////

CThreadPool::WorkerThread::WorkerThread(
	CIOCP &iocp,
	CEvent &messageReceivedEvent,
	CThreadPool &pool)
	: m_messageReceivedEvent(messageReceivedEvent)
	, m_iocp(iocp)
	, m_pool(pool)
{
}
      
CThreadPool::WorkerThread::~WorkerThread()
{
}

bool CThreadPool::WorkerThread::Initialise()
{
	return true;
}

void CThreadPool::WorkerThread::Shutdown()
{
}
 
int CThreadPool::WorkerThread::Run()
{
	m_pool.ThreadStarted();

	try
	{
		if(Initialise())
		{
			m_pool.ThreadInitialised();
      
			while(true)
			{
				DWORD completionKey;
				DWORD dwNumBytes;
				OVERLAPPED *pOverlapped;
      
				m_iocp.GetStatus(&completionKey, &dwNumBytes, &pOverlapped);
				m_messageReceivedEvent.Set();

				if(completionKey == 0)
				{
					break; // request to shutdown
				}
				else
				{
					try
					{
						m_pool.ReceivedMessage();
						m_pool.ProcessingStarted();

						Process(completionKey, dwNumBytes, pOverlapped);
                  
						m_pool.ProcessingStopped();
					}
					catch(const CException& e)
					{
						OutputF(TCritical, TEXT("CThreadPool::WorkerThread::Run() - Exception: %s - %s"), e.GetWhere(), e.GetMessage());
					}
					catch(...)
					{
						Output(TCritical, TEXT("CThreadPool::WorkerThread::Run() - Unexpected exception"));
					}
				}
			}

			Shutdown();
		}
	}
	catch(const CException& e)
	{
		OutputF(TCritical, TEXT("CThreadPool::WorkerThread::Run() - Exception: %s - %s"), e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Output(TCritical, TEXT("CThreadPool::WorkerThread::Run() - Unexpected exception"));
	}

	m_pool.ThreadStopped(this);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
