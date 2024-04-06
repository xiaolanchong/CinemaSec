#ifndef ELVEES_CRITICALSECTION_INCLUDED__
#define ELVEES_CRITICALSECTION_INCLUDED__

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
// CCriticalSection
///////////////////////////////////////////////////////////////////////////////

class CCriticalSection
{
public:
	class Owner
	{
	public:
		explicit Owner(CCriticalSection &crit);
		~Owner();
      
	private:
		CCriticalSection &m_crit;

		// No copies do not implement
		Owner(const Owner &rhs);
		Owner &operator=(const Owner &rhs);
	};

	CCriticalSection();
	~CCriticalSection();

#if(_WIN32_WINNT >= 0x0400)
	bool TryEnter();
#endif
	void Enter();
	void Leave();

private:
	CRITICAL_SECTION m_crit;

	// No copies do not implement
	CCriticalSection(const CCriticalSection &rhs);
	CCriticalSection &operator=(const CCriticalSection &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEES_CRITICALSECTION_INCLUDED__
