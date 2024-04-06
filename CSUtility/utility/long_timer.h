/****************************************************************************
  long_timer.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

//=================================================================================================
/** \class MyLongTimer.
    \brief MyLongTimer. */
//=================================================================================================
class CS_UTIL_API MyLongTimer
{
private:
  __int64 m_start;           //!< long start time
  __int64 m_stop;            //!< long stop time
  __int64 m_prevTime;        //!< long previous time
  __int64 m_currTime;        //!< long current time
  __int64 m_lastStamp;       //!< the last time stamp (obtained as a DWORD value)
  wchar_t   m_str[64];         //!< time value converted to the string form
  bool    m_bRunning;        //!< nonzero while the timer is running

private:
  __int64 GetClock();

public:
  MyLongTimer();

  void Start();
  void Stop();

  float         TimeInSec();
  const wchar_t * TimeInSecString();

  float         TimeDiffInSec();
  const wchar_t * TimeDiffInSecString();
};

