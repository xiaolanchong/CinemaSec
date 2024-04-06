#include "Events.h"
#include "Exceptions.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// CEvent
///////////////////////////////////////////////////////////////////////////////

CEvent::CEvent(
	LPSECURITY_ATTRIBUTES lpEventAttributes,
	bool bManualReset,
	bool bInitialState)
	: m_hEvent(::CreateEvent(lpEventAttributes, bManualReset, bInitialState, 0))
{
	if(m_hEvent == 0)
	{
		throw CWin32Exception(TEXT("CEvent::CEvent()"), ::GetLastError());
	}
}

CEvent::CEvent(
	LPSECURITY_ATTRIBUTES lpEventAttributes,
	bool bManualReset,
	bool bInitialState,
	LPCTSTR lpName)
	: m_hEvent(::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName))
{
	if(m_hEvent == 0)
	{
		throw CWin32Exception(TEXT("CEvent::CEvent()"), ::GetLastError());
	}
}

CEvent::~CEvent()
{
	::CloseHandle(m_hEvent);
}

HANDLE CEvent::GetEvent() const
{
	return m_hEvent;
}

void CEvent::Wait() const
{
	if(!Wait(INFINITE))
	{
		throw CException(TEXT("CEvent::Wait()"), TEXT("Unexpected timeout on infinite wait"));
	}
}

bool CEvent::Wait(DWORD timeoutMillis) const
{
	bool ok;

	DWORD result = ::WaitForSingleObject(m_hEvent, timeoutMillis);

	if(result == WAIT_TIMEOUT)
	{
		ok = false;
	}
	else if(result == WAIT_OBJECT_0)
	{
		ok = true;
	}
	else
	{
		throw CWin32Exception(TEXT("CEvent::Wait() - WaitForSingleObject"), ::GetLastError());
	}
    
	return ok;
}

void CEvent::Reset()
{
	if(!::ResetEvent(m_hEvent))
	{
		throw CWin32Exception(TEXT("CEvent::Reset()"), ::GetLastError());
	}
}

void CEvent::Set()
{
	if(!::SetEvent(m_hEvent))
	{
		throw CWin32Exception(TEXT("CEvent::Set()"), ::GetLastError());
	}
}

void CEvent::Pulse()
{
	if(!::PulseEvent(m_hEvent))
	{
		throw CWin32Exception(TEXT("CEvent::Pulse()"), ::GetLastError());
	}
}

///////////////////////////////////////////////////////////////////////////////
// CManualResetEvent
///////////////////////////////////////////////////////////////////////////////

CManualResetEvent::CManualResetEvent(bool initialState)
	: CEvent(0, true, initialState)
{
}

CManualResetEvent::CManualResetEvent(LPCTSTR lpName, bool initialState)
	: CEvent(0, true, initialState, lpName)
{
}

///////////////////////////////////////////////////////////////////////////////
// CAutoResetEvent
///////////////////////////////////////////////////////////////////////////////

CAutoResetEvent::CAutoResetEvent(bool initialState)
	: CEvent(0, false, initialState)
{
}

CAutoResetEvent::CAutoResetEvent(LPCTSTR lpName, bool initialState)
	: CEvent(0, false, initialState, lpName)
{
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees