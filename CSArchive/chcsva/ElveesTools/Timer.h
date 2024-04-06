#ifndef ELVEES_TIMER_INCLUDED__
#define ELVEES_TIMER_INCLUDED__

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
// CTimer
///////////////////////////////////////////////////////////////////////////////

class CTimer
{
public:
	CTimer();

	void Start();		// Start timer
	unsigned Stop();	// Returns time, in milliseconds

	bool GetCounter(LARGE_INTEGER* lpCounter);
	bool GetFrequency(LARGE_INTEGER* lpFrequency);

protected:
	void DetermineTimer();

private:
	typedef enum tagTimer
	{
		ttuUnknown,
		ttuHiRes,
		ttuClock
	} Timer;
	
	static Timer m_TimerToUse;		 // used timer

	static int m_OverheadTicks;		 // overhead in calling timer
	static int m_PerfFreqAdjust;	 // in case frequency is too big
	static LARGE_INTEGER m_PerfFreq; // ticks per second

	LARGE_INTEGER m_tStartTime;
	
	// No copies do not implement
	CTimer(const CTimer &rhs);
	CTimer &operator=(const CTimer &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEES_TIMER_INCLUDED__
