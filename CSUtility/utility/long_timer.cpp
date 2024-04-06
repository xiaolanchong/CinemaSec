/****************************************************************************
  long_timer.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const wchar_t formatStr[] = _T("Time = %4.4f (sec)");

MyLongTimer::MyLongTimer()
{
  ASSERT( sizeof(DWORD) == sizeof(__int32) );
  memset( m_str, 0, sizeof(m_str) );
  m_start = (m_stop = (m_prevTime = (m_currTime = (m_lastStamp = 0))));
  m_bRunning = false;
}


void MyLongTimer::Start()
{
  m_str[0] = _T('\0');
  m_start = (m_stop = (m_prevTime = (m_currTime = (m_lastStamp = ::timeGetTime()))));
  m_bRunning = true;
}


void MyLongTimer::Stop()
{
  m_stop = (m_prevTime = GetClock());
  m_bRunning = false;
}


__int64 MyLongTimer::GetClock()
{
  const __int64  TWO_POW_32 = (((__int64)1)<<32);
  ASSERT( m_bRunning );
  __int64 t = ::timeGetTime();
  __int64 dt = (t - m_lastStamp + TWO_POW_32) % TWO_POW_32;
  m_lastStamp = t;
  m_currTime += dt;
  return m_currTime;
}


float MyLongTimer::TimeInSec()
{
  __int64 t = (m_bRunning) ? GetClock() : m_stop;
  return (float)((t-m_start)*0.001);
}


const wchar_t * MyLongTimer::TimeInSecString()
{
  float time = TimeInSec();
  _stprintf( m_str, formatStr, time );
  return m_str;
}


float MyLongTimer::TimeDiffInSec()
{
  __int64 t = (m_bRunning) ? GetClock() : m_prevTime;
  float   time = (float)((t-m_prevTime)*0.001);

  m_prevTime = t;
  return time;
}


const wchar_t * MyLongTimer::TimeDiffInSecString()
{
  float time = TimeDiffInSec();
  _stprintf( m_str, formatStr, time );
  return m_str;
}

