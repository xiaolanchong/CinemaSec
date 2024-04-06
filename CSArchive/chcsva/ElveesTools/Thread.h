#ifndef ELVEES_THREAD_INCLUDED__
#define ELVEES_THREAD_INCLUDED__

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
// CThread
///////////////////////////////////////////////////////////////////////////////

class CThread 
{
public:
	CThread();
	virtual ~CThread();

	HANDLE GetHandle() const;

	void Wait() const;
	bool Wait(DWORD timeoutMillis) const;

	void Start();
	void Terminate(DWORD exitCode = 0);

	bool IsStarted();

private:
	virtual int Run() = 0;

	static unsigned int __stdcall ThreadFunction(void *pV);

	HANDLE m_hThread;

	// No copies do not implement
	CThread(const CThread &rhs);
	CThread &operator=(const CThread &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEES_THREAD_INCLUDED__
