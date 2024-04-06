//=====================================================================================//
//                                                                                     //
//                                       CSChair                                       //
//                                                                                     //
//                           Copyright by ElVEES, 2005		                           //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   26.01.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __THREAD_ERROR_H_INCLUDED_2184132126714320__
#define __THREAD_ERROR_H_INCLUDED_2184132126714320__

#include <boost/shared_ptr.hpp>
#include <process.h>

#include "../../CSChair/debug_int.h"
#include "../IPC/NetUtil.h"
#include "../../CSChair/common/Exception.h"
#include "ThreadMessage.h"

#define ERR_OK			S_OK
#define ERR_ERROR		E_FAIL
#define ERR_NOTIMPL		E_NOTIMPL
#define ERR_INVALIDARG	E_INVALIDARG

MACRO_EXCEPTION(ThreadServerException, CommonException)
MACRO_EXCEPTION( ThreadServerInitException, ThreadServerException );

inline __int64 Plat_Int64Time()
{
	return __int64(Plat_FloatTime() * 1000);
}

__int64 Plat_AbsoluteTime();

struct ThreadSync
{
public:
	HANDLE		m_hThread;
	HANDLE		m_hEvent;

	ThreadSync() : m_hEvent( CreateEvent( 0, TRUE, FALSE, 0 ) ), m_hThread(0) 
	{};
	~ThreadSync() { CloseHandle( m_hEvent ) ;};

	// in parent thread
	void	Start( HANDLE h)	
	{ 
		m_hThread = h; 
		ResetEvent(m_hEvent); 
	}
	bool	Stop(DWORD dwTimeOut)				
	{ 
		SetEvent( m_hEvent );	
		return WaitForThread( dwTimeOut );
	}
	// into child thread
	bool	CanThreadWork(DWORD dwTimeOut = 0)	
	{
		return WaitForSingleObject ( m_hEvent, dwTimeOut ) == WAIT_TIMEOUT;	
	}
	void Resume(){ ResumeThread( m_hThread ); }
	std::pair<bool, DWORD> IsThreadEnds() const
	{
		DWORD dwCode;
	/*	BOOL res =*/ GetExitCodeThread( m_hThread, &dwCode );
		return std::make_pair( (dwCode == STILL_ACTIVE? false: true), dwCode );
	}
private:
	bool	WaitForThread( DWORD dwTimeOut )
	{
		bool bSafely = true;
		DWORD res = WaitForSingleObject ( m_hThread, dwTimeOut );
		if( res == WAIT_TIMEOUT )
		{
			// child thread is still working!!!
			TerminateThread( m_hThread , DWORD(~0) );
			bSafely = false;
		}
		CloseHandle( m_hThread );
		m_hThread = 0;
		return bSafely;
	}
};


struct  MultipleThreadSync
{
	const HANDLE*	m_pThreads;
	HANDLE*			m_pEvents;
	DWORD			m_dwSize;
	MultipleThreadSync( const HANDLE* m_pThreads, HANDLE*	pEvents, DWORD dwSize ):
	m_pThreads(m_pThreads), 
		m_pEvents(pEvents), 
		m_dwSize(dwSize)
	{}

	bool Stop(DWORD dwTimeOut)
	{
//		bool bSafely = true;
		for( DWORD i = 0 ; i < m_dwSize; ++i )
		{
			SetEvent( m_pEvents[i] );
		}
		DWORD res = WaitForMultipleObjects( m_dwSize, m_pThreads, TRUE, dwTimeOut );
		if( res == WAIT_TIMEOUT || res == WAIT_FAILED )
		{
			for( DWORD i = 0 ; i < m_dwSize; ++i )
			{
				// still work!!!
				TerminateThread( m_pThreads[i], DWORD(~0) );
			}
			for( DWORD i = 0 ; i < m_dwSize; ++i )
			{
				// still work!!!
				CloseHandle( m_pThreads[i] );
				ResetEvent( m_pEvents[i] );
			}
			return false;
		}
		return true;
	}
};

struct SyncCriticalSection : CRITICAL_SECTION
{
	SyncCriticalSection()		{	InitializeCriticalSection(this);	}
	~SyncCriticalSection()		{	DeleteCriticalSection(this);		}
	bool	Try() 	{ return ::TryEnterCriticalSection(this) == TRUE;	}
	void	Lock()	{	EnterCriticalSection(this);	}
	void	Unlock(){	LeaveCriticalSection(this);	}
};

struct AutoLockCriticalSection
{
	CRITICAL_SECTION &	m_cs;
	AutoLockCriticalSection(CRITICAL_SECTION& cs) : m_cs(cs)
	{
		EnterCriticalSection( &m_cs );
	}
	~AutoLockCriticalSection()
	{
		LeaveCriticalSection( &m_cs );
	}
};

class SimpleTimer
{
	__int64	m_nPeriod;
	__int64	m_nStartedTime;
public:
	void Reset(__int64 nPeriod)		{ Start(); m_nPeriod = nPeriod; }
	void Start()					{ m_nStartedTime = Plat_Int64Time();						}
	bool IsSignaled() const			{ return (Plat_Int64Time() - m_nStartedTime) > m_nPeriod ;	}
	SimpleTimer(__int64 nPeriod): m_nPeriod(nPeriod){ }
};

class AutoSimpleTimer
{
	__int64	m_nPeriod;
	__int64	m_nStartedTime;
	bool	m_bFirstSignaled;
public:
	AutoSimpleTimer(__int64 nPeriod) : 
		m_nPeriod( nPeriod), 
		m_bFirstSignaled(false),
		m_nStartedTime( Plat_Int64Time() ){}
	bool IsSignaled()
	{
		if( !m_bFirstSignaled ) 
		{
			m_bFirstSignaled	= true;
			m_nStartedTime		= Plat_Int64Time();
			return true;
		}
		bool res = (Plat_Int64Time() - m_nStartedTime) > m_nPeriod ;
		if( res ) m_nStartedTime = Plat_Int64Time();
		return res;
	}
	void Start()					{}
};

class SimpleTimerWithPeriod
{
	__int64	m_nPeriod;
	__int64	m_nStartedTime;
public:
	void Start(__int64 nNewPeriod)			{ m_nPeriod = nNewPeriod ;m_nStartedTime = Plat_Int64Time();}
	bool IsSignaled() const					{ return (Plat_Int64Time() - m_nStartedTime) > m_nPeriod ;	}
	SimpleTimerWithPeriod(__int64 nFirstPeriod): m_nPeriod(nFirstPeriod){ }
};

#define RETURN(x) { /*_endthreadex(x); */return (x); }

enum
{
	rqt_cam_data = 10,
	rsp_cam_data,
	hall_cam_data,

//	rqt_hall_load_data	  = 100,
	rqt_hall_spectators = 100,
	rsp_hall_spectators,
//	rsp_hall_data
};

class GenericThreadServer
{
public:
	enum ThreadError
	{
		err_noerror				= 0,
		err_general				= 1,
		err_exception			= 2,

		err_grabber_invalid		= 101,
		err_grabber_invalid_url	= 102,
		err_grabber_start		= 103,
		err_grabber_lock		= 104,
		err_grabber_unlock		= 105,

		// 200 - 299 reserved because STILL_LIVE=259

		err_camanalyzer_start	= 300,
		err_camanalyzer_process	= 301,
		err_camanalyzer_setdata	= 302,
		err_camanalyzer_getdata	= 303,
		err_camanalyzer_initialize	= 304,
		err_camanalyzer_stop		= 305,
		err_camanalyzer_invalid		= 306,
		
		err_hallanalyzer_invalid	= 400,
		err_hallanalyzer_initialize = 401,
		err_hallanalyzer_load		= 402,
		err_hallanalyzer_process	= 403
	};

	MyDebugOutputImpl					m_pDebug;
	ThreadMessage						m_ServerMessage;

	static LPCWSTR GetThreadErrorDescription( unsigned int Res );
};

// unsigned (__stdcall T::*Fn) - micro optimization

template<class T, unsigned (T::*Fn)(), const unsigned ExceptionCode>
	unsigned __stdcall ThreadProc_Handled(void* pParam)
{
	unsigned Res = ExceptionCode;
	T *self = reinterpret_cast<T*>(pParam);
	__try
	{
		Res = (self->*Fn)();
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("Working thread"))
#ifndef _DEBUG
		, EXCEPTION_EXECUTE_HANDLER
#endif
		)
	{
	}
	return Res;
}

#endif //__THREAD_ERROR_H_INCLUDED_2184132126714320__
