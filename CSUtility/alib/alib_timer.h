/****************************************************************************
  alib_timer.h
  ---------------------
  begin     : 1998
  modified  : 30 Aug 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaahaaah@hotmail.com, aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \class ATimer.
    \brief Simple timer. */
//=================================================================================================
class ATimer
{
private:
#ifdef WIN32
  LARGE_INTEGER m_frequency;   //!< performance frequency in counts per second
  LARGE_INTEGER m_start;       //!< start time
  LARGE_INTEGER m_stop;        //!< stop time
  LARGE_INTEGER m_previous;    //!< previous time
#else
  clock_t       m_start;       //!< start time
  clock_t       m_stop;        //!< stop time
  clock_t       m_previous;    //!< previous time
#endif

  TCHAR m_str[64];             //!< time value converted to the text form
  bool  m_bGo;                 //!< timer's state: nonzero while timer is running

public:
  ATimer();

  void Start();
  void Stop();

  float         Time();
  const TCHAR * TimeStr();
  float         TimeDiff();
  const TCHAR * TimeDiffStr();
};

