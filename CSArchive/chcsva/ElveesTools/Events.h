#ifndef ELVEES_EVENTS_INCLUDED__
#define ELVEES_EVENTS_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// CEvent
///////////////////////////////////////////////////////////////////////////////

class CEvent
{
public:
	explicit CEvent(
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		bool manualReset,
		bool initialState);
   
	explicit CEvent(
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		bool manualReset,
		bool initialState,
		LPCTSTR lpName);
      
	virtual ~CEvent();

	HANDLE GetEvent() const;

	void Wait() const;
	bool Wait(DWORD timeoutMillis) const;

	void Reset();
	void Set();
	void Pulse();

private:
	HANDLE m_hEvent;

	// No copies do not implement
	CEvent(const CEvent &rhs);
	CEvent &operator=(const CEvent &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CManualResetEvent
///////////////////////////////////////////////////////////////////////////////

class CManualResetEvent : public CEvent
{
public:
	explicit CManualResetEvent(bool initialState = false);
	explicit CManualResetEvent(LPCTSTR lpName, bool initialState = false);

private:
	// No copies do not implement
	CManualResetEvent(const CManualResetEvent &rhs);
	CManualResetEvent &operator=(const CManualResetEvent &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CAutoResetEvent
///////////////////////////////////////////////////////////////////////////////

class CAutoResetEvent : public CEvent
{
public:
	explicit CAutoResetEvent(bool initialState = false);
	explicit CAutoResetEvent(LPCTSTR lpName, bool initialState = false);

private:
	// No copies do not implement
	CAutoResetEvent(const CAutoResetEvent &rhs);
	CAutoResetEvent &operator=(const CAutoResetEvent &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEES_EVENTS_INCLUDED__
