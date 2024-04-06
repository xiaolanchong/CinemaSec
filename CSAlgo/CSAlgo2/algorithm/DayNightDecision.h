///////////////////////////////////////////////////////////////////////////////////////////////////
// DayNightDecision.h
// ---------------------
// begin     : Nov 2005
// modified  : 16 Nov 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace csalgo2
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class DayNightDecision.
/// \brief Class analyzes day/night intervals and sets on appropriate state flags.
///////////////////////////////////////////////////////////////////////////////////////////////////
class DayNightDecision
{
public:
  enum { NORMAL_NIGHT = 0, RESET_STATISTICS, STOP_SURVEILLANCE, BREAK_SURVEILLANCE };

private:
  __int64 m_movieStartTime;  //!< the absolute time of movie beginning (in milliseconds)
  __int64 m_movieStopTime;   //!< the absolute time of movie ending (in milliseconds)
  __int64 m_dayStartTime;    //!< the absolute start time 'day' interval (in milliseconds)
  __int64 m_minDayDuration;  //!< threshold on duration of 'day' interval (in milliseconds)
  int     m_state;           //!< the current state
  bool    m_bInitialized;    //!< nonzero if this object has been initialized
  bool    m_bFirstWarning;   //!< flag is used to show warning just once

private:
///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function prints warning message of special type given a text information.
///////////////////////////////////////////////////////////////////////////////////////////////////
void PrintWarning( IDebugOutput * pOut, LPCWSTR text )
{
  wchar_t msg[256];
  swprintf( msg, L"'Day' was detected at the %s of a show", text );
  if (pOut != 0) pOut->PrintW( IDebugOutput::mt_warning, msg );
}

public:
///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function clears this object.
///////////////////////////////////////////////////////////////////////////////////////////////////
void Clear()
{
  m_movieStartTime = 0;
  m_movieStopTime = 0;
  m_dayStartTime = 0;
  m_minDayDuration = 0;
  m_state = NORMAL_NIGHT;
  m_bInitialized = false;
  m_bFirstWarning = true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function returns the current state.
///////////////////////////////////////////////////////////////////////////////////////////////////
int GetState() const
{
  return m_state;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
DayNightDecision()
{
  Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function initializes this object and sets start and stop time of a movie.
///
/// \param  startTime           the absolute time of movie beginning (in milliseconds).
/// \param  stopTime            the absolute time of movie ending (in milliseconds).
/// \param  minimalDayDuration  threshold of 'day' duration (in milliseconds).
///////////////////////////////////////////////////////////////////////////////////////////////////
void Initialize( __int64 startTime, __int64 stopTime, __int64 minimalDayDuration )
{
  Clear();
  m_movieStartTime = startTime;
  m_movieStopTime = stopTime;
  m_minDayDuration = minimalDayDuration;
  m_bInitialized = true;
  m_bFirstWarning = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function analyzes day/night intervals.
///
/// \param  bIsDayDetected  nonzero if 'day' has been detected in the current frame.
/// \param  timeStamp       the current time (in milliseconds).
/// \param  pOut            pointer to the message handler.
/// \return                 if nonzero, then surveillance can be proceeded, otherwise it must be stopped.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool AnalizeDayNight( bool bIsDayDetected, __int64 timeStamp, IDebugOutput * pOut )
{
  if (!m_bInitialized)
  {
    if (m_bFirstWarning && (pOut != 0))
      pOut->PrintW( IDebugOutput::mt_warning, L"Day-night analyzer was not intialized" );
    m_bFirstWarning = false;
    return true;
  }

  if (m_state != NORMAL_NIGHT)    // if 'day' has been detected, then one can proceed surveillance
    return false;                 //   process only after resetting of this analyzer

  if (bIsDayDetected)
  {
    if (m_dayStartTime == 0)
    {
      m_dayStartTime = timeStamp;    // start new 'day' time interval
    }
    else if ((timeStamp - m_dayStartTime) > m_minDayDuration) // is 'day' long?
    {
      __int64 movieDuration = (m_movieStopTime - m_movieStartTime);
      __int64 first = (movieDuration/3);
      __int64 second = 2*(movieDuration/3);
      __int64 moment = (timeStamp - m_movieStartTime);

      if (moment < first)
      {
        m_state = RESET_STATISTICS;
        PrintWarning( pOut, L"beginning" );
      }
      else if ((first <= moment) && (moment <= second))
      {
        m_state = BREAK_SURVEILLANCE;
        PrintWarning( pOut, L"middle" );
      }
      else // if (second < moment)
      {
        m_state = STOP_SURVEILLANCE;
        PrintWarning( pOut, L"end" );
      }
    }
  }
  else m_dayStartTime = 0;           // reset 'day' time interval

  return (m_state == NORMAL_NIGHT);  // proceed surveillance if normal 'night' was detected
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function makes a decision given day/night state of entire hall.
///
/// \param  hallState  day/night state of entire hall (several cameras).
/// \return            nonzero if statistics must be resetted.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MakeDecision( int hallState )
{
  if (m_bInitialized)
  {
    if ((hallState == NORMAL_NIGHT)      ||
        (hallState == STOP_SURVEILLANCE) ||
        (hallState == BREAK_SURVEILLANCE))
    {
      m_state = hallState;
    }
    else if (hallState == RESET_STATISTICS)
    {
      m_state = NORMAL_NIGHT;
      return true;
    }
    else ASSERT(0);
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes entire hall state from the states of separate cameras.
///
/// The larger state level - the more severe day-night impact.
///
/// \param  states  the map of camera states (key = camera index, value = camera state).
/// \return         the entire hall state.
///////////////////////////////////////////////////////////////////////////////////////////////////
static int GetHallState( const std::map<int,int> & states )
{
  int entire = NORMAL_NIGHT;
  for (std::map<int,int>::const_iterator i = states.begin(); i != states.end(); ++i)
  {
    ALIB_ASSERT( alib::IsLimited<int>( i->second, NORMAL_NIGHT, BREAK_SURVEILLANCE ) );
    entire = std::max( entire, i->second );
  }
  return entire;
}

};

} // namespace csalgo2

