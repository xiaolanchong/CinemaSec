#ifndef ELVEES_THREADPOOL_INCLUDED__
#define ELVEES_THREADPOOL_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

///////////////////////////////////////////////////////////////////////////////

#include "IOCP.h"
#include "Events.h"
#include "Thread.h"
#include "NodeList.h"
#include "CriticalSection.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// CThreadPool
///////////////////////////////////////////////////////////////////////////////

class CThreadPool : private CThread
{
public:
	class WorkerThread;
	friend class WorkerThread;

	CThreadPool(
		unsigned initialThreads,
		unsigned minThreads,
		unsigned maxThreads,
		unsigned maxDormantThreads,
		unsigned poolMaintPeriod,
		unsigned dispatchTimeout,
		unsigned maxThreadsDispatchTimeout);

	virtual ~CThreadPool();

	using CThread::Start;

	void InitiateShutdown();
	void WaitForShutdownToComplete(
		unsigned timeout = INFINITE,
		bool forceTermination = false);

protected:
	void Dispatch(
		DWORD completionKey, 
		DWORD dwNumBytes = 0, 
		OVERLAPPED *pOverlapped = 0);

private:
	int Run();
	
	virtual void OnThreadStarted()     {};
	virtual void OnThreadInitialised() {};
	virtual void OnProcessingStarted() {};
	virtual void OnReceivedMessage()   {};
	virtual void OnProcessingStopped() {};
	virtual void OnThreadStopped()     {};

	virtual WorkerThread *CreateWorkerThread(
		CIOCP &iocp,
		CEvent &messageReceived,
		CThreadPool &pool) = 0;

	void HandleDispatch(
		DWORD completionKey, 
		DWORD dwNumBytes, 
		OVERLAPPED *pOverlapped);

	void HandleDormantThreads();

	void StartWorkerThread();
	
	void ThreadStarted();
	void ThreadInitialised();
	void ThreadStopped(WorkerThread *pThread);
	
	void ReceivedMessage();

	void ProcessingStarted();
	void ProcessingStopped();
	
	void StopWorkerThreads(unsigned numThreads);
	void TerminateWorkerThreads();

	CManualResetEvent m_shutdownEvent;
	CManualResetEvent m_dispatchCompleteEvent;
	CManualResetEvent m_poolEmptyEvent;

	CIOCP m_workPort;
	CIOCP m_dispatchPort;

	long m_activeThreads;
	long m_initialisedThreads;
	long m_processingThreads;

	const unsigned m_initialThreads;
	const unsigned m_minThreads;
	const unsigned m_maxThreads;
	const unsigned m_maxDormantThreads;

	const unsigned m_poolMaintPeriod;
	const unsigned m_dispatchTimeout;
	const unsigned m_maxThreadsDispatchTimeout;

	CCriticalSection m_criticalSection;
	
	typedef TNodeList<WorkerThread> WorkerThreadList;
	WorkerThreadList m_ThreadList;

	// No copies do not implement
	CThreadPool(const CThreadPool &rhs);
	CThreadPool &operator=(const CThreadPool &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CThreadPool::WorkerThread
///////////////////////////////////////////////////////////////////////////////

class CThreadPool::WorkerThread : public CThread, public CNodeList::Node
{
	friend class CThreadPool;

protected:
	WorkerThread(
		CIOCP &iocp,
		CEvent &messageReceivedEvent,
		CThreadPool &pool);
      
	virtual ~WorkerThread();

private:
	virtual int Run();
	virtual bool Initialise();
	virtual void Shutdown();
	
	virtual void Process(
		DWORD completionKey,
		DWORD dwNumBytes,
		OVERLAPPED *pOverlapped) = 0;

	CIOCP &m_iocp;
	CEvent &m_messageReceivedEvent;
	CThreadPool	&m_pool;

	// No copies do not implement
	WorkerThread(const WorkerThread &rhs);
	WorkerThread &operator=(const WorkerThread &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEES_THREADPOOL_INCLUDED__
