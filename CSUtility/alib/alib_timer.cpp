/****************************************************************************
  alib_timer.cpp
  ---------------------
  begin     : 1998
  modified  : 30 Aug 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaahaaah@hotmail.com, aaah@mail.ru
****************************************************************************/

#include "stdafx.h"

#ifdef ALIB_CHECK_MEMORY_LEAKAGE
#define new DEBUG_NEW
#endif

const TCHAR formatStr[] = _T("Time = %4.4f (sec)");

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
ATimer::ATimer()
{
#ifdef WIN32
  m_start.QuadPart = (m_stop.QuadPart = (m_previous.QuadPart = (LONGLONG)0));
  m_frequency.QuadPart = (LONGLONG)1;
#else
  m_start = (m_stop = (m_previous = (clock_t)0));
#endif

  memset( m_str, 0, sizeof(m_str) );
  m_bGo = false;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
void ATimer::Start()
{
  m_bGo = false;
  m_str[0] = _T('\0');

#ifdef WIN32
  m_bGo = (QueryPerformanceFrequency( &m_frequency ) && QueryPerformanceCounter( &m_start ));
  m_frequency.QuadPart = std::max<LONGLONG>( m_frequency.QuadPart, (LONGLONG)1 );
#else
  m_start = ::clock();
  m_bGo = true;
#endif

  m_previous = (m_stop = m_start);
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
void ATimer::Stop()
{
#ifdef WIN32
  QueryPerformanceCounter( &m_stop );
#else
  m_stop = ::clock();
#endif

  m_previous = m_stop;
  m_bGo = false;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
float ATimer::Time()
{
#ifdef WIN32
  LARGE_INTEGER current;
  if (m_bGo) QueryPerformanceCounter( &current ); else current = m_stop;
  return (float)((double)(current.QuadPart - m_start.QuadPart)/(double)(m_frequency.QuadPart));
#else
  clock_t current = (m_bGo ? ::clock() : m_stop);
  return (float)((double)(current - m_start)/(double)CLOCKS_PER_SEC);
#endif
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
const TCHAR * ATimer::TimeStr()
{
  float time = Time();
  _stprintf( m_str, formatStr, time );
  return m_str;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
float ATimer::TimeDiff()
{
  if (!m_bGo)
    return 0.0f;

#ifdef WIN32
  LARGE_INTEGER current;
  QueryPerformanceCounter( &current );
  float time = (float)((double)(current.QuadPart - m_previous.QuadPart)/(double)(m_frequency.QuadPart));
#else
  clock_t current = ::clock();
  float time = (float)((double)(current - m_previous)/(double)CLOCKS_PER_SEC);
#endif

  m_previous = current;
  return time;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
const TCHAR * ATimer::TimeDiffStr()
{
  float time = TimeDiff();
  _stprintf( m_str, formatStr, time );
  return m_str;
}

