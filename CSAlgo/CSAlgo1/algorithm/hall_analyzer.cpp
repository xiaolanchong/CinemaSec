/****************************************************************************
  hall_analyzer.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "algo_param.h"
#include "hall_analyzer.h"
#include "utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function creates an instance of hall analyzer.

  \param  ppAnalyzer   the address of pointer that points to the instance of created object.
  \param  pParameters  pointer to the binary image of parameters.
  \param  pChairs      pointer to the array of chairs.
  \param  startTime    launch time of hall analyzer.
  \param  pDebugOut    pointer to the object used to print debug information.
  \return              Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CreateHallAnalyzer( IHallAnalyzer     ** ppAnalyzer,
                            const Int8Arr      * pParameters,
                            const BaseChairArr * pChairs,
                            __int64              startTime,
                            IDebugOutput       * pDebugOut )
{
  if (ppAnalyzer == 0)
    return E_FAIL;
  (*ppAnalyzer) = 0;

  HallAnalyzer * p = new HallAnalyzer();
  if (p == 0)
    return E_FAIL;

  HRESULT res = p->Initialize( pParameters, pChairs, startTime, pDebugOut );
  if (FAILED( res ))
    delete p;
  else
    (*ppAnalyzer) = p;

  return res;
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
HallAnalyzer::HallAnalyzer() : m_parameters( csalgo::FIRST_CONTROL_IDENTIFIER )
{
  m_pDebugOutput = 0;
  m_lastError.reserve( 80 );
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
HallAnalyzer::~HallAnalyzer()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes camera analyzer.

  \param  pParameters  pointer to the binary image of parameters.
  \param  pChairs      pointer to the array of chairs.
  \param  startTime    launch time of hall analyzer.
  \param  pDebugOut    pointer to the object used to print debug information.
  \return              Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT HallAnalyzer::Initialize( IN const Int8Arr      * pParameters,
                                  IN const BaseChairArr * pChairs,
                                  IN __int64              startTime,
                                  IN IDebugOutput       * pDebugOut )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  int         retValue = E_FAIL;

  try
  {
    m_bGood = false;
    m_chairs.clear();
    m_tmpChairs.clear();
    m_dataLocker;
    m_parameters.set_default();
    m_startTime = startTime;
    m_currentTime = 0;
    m_cameraHuman.clear();
    m_pDebugOutput = 0;
    m_lastError.erase();
    m_logFileName.erase();
    if (m_log.is_open()) m_log.close();
    m_bFinalStatistics = false;

    m_pDebugOutput = pDebugOut;
    ALIB_ASSERT( (pParameters != 0) && (pChairs != 0) && (startTime >= 0) );

    // First of all, load parameters.
    HRESULT_ASSERT( csalgo::LoadParameters( *pParameters, m_parameters ) );

    // Load chairs.
    m_tmpChairs.reserve( pChairs->size() );
    ALIB_ASSERT( csalgo::ExtractChairSubSet( *pChairs, -1, m_chairs ) );

    // Do some additional initialization.
    if (m_parameters.s_demo.p_demoMode() && !(m_chairs.empty()))
      csalgo::InitializeChairs( m_chairs );

    m_bGood = true;
    retValue = S_OK;
  }
  catch (std::runtime_error & e) { PrintException( e ); }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function analyzes the current situation throughout the hall.

  \param  pSpectatorNum  количество зрителей сидевших в зале в течении заданного периода.
  \param  timeStamp      текущее время в миллисекундах.
  \param  period         интервал анализа статистики в миллисекундах, [timeStamp-period,...,timeStamp],
                         period <= 0 means statistics counted off from the very beginning.
  \return                Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT HallAnalyzer::ProcessData( OUT __int32 * pSpectatorNum,
                                   IN __int64    timeStamp,
                                   IN __int64    period )
{
  const char BARS[] = "########################################";
  const char HYPHENS[] = "----------------------------------------";
  const char TIME[] = "Time = ";
  const char CHAIR_IDENTIFIER[] = "Chair identifier = ";
  const char STATIC_INTERVAL_NUM[] = "Number of static intervals = ";
  const char STAT_INTERVAL_NO[] = "\tStatic interval's index = ";
  const char STAT_INTERVAL_DURATION[] = "\tStatic interval's duration = ";
  const char DYNA_INTERVAL_DURATION[] = "\t\tDynamic interval's duration = ";
  const char DYNA_INTERVAL_NUM[] = "\tNumber of dynamic intervals = ";
  const char BAD_ACTIVITY_OCCUPATION_RATIO[] = "\t\tBad activity/occupation ratio";
  const char ACCEPTED_DYNA_INTERVAL_NUM[] = "\t\tNumber of accepted dynamic intervals = ";
  const char IS_HUMAN[] = "\t\tIs human = ";
  const char CHAIR_HUMANITY[] = "Chair humanity = ";
  const char SPECTATOR_NUM[] = "Spectator number = ";
  const char ACTIVITY_DURATION[] = "\tSummary activity period during the current static interval = ";

  CSingleLock    lock( &m_dataLocker, TRUE );
  HRESULT        retValue = E_FAIL;
  std::fstream * f = (m_bFinalStatistics && m_parameters.s_demo.p_bLogHallAnalyzer() &&
                      m_log.is_open() && m_log.good()) ? &m_log : 0;
  period;
  try
  {
    if (pSpectatorNum != 0) (*pSpectatorNum) = 0;
    ALIB_ASSERT( m_bGood );
    m_bGood = false;
    ALIB_ASSERT( pSpectatorNum != 0 );

    if (!m_bFinalStatistics)
    {
      ALIB_ASSERT( timeStamp >= m_startTime );
      m_currentTime = csalgo::MilliToSeconds( m_startTime, timeStamp );
    }

    if (f != 0)
      (*f) << BARS << std::endl << TIME << m_currentTime << std::endl << BARS << std::endl << std::endl;

    const csalgo::AlgorithmParams  & algoParams = m_parameters.s_algorithm;
    const csalgo::StatisticsParams & statParams = m_parameters.s_statistics;
    HumanCounter                     totalHumanCount;

    // Reset human counter of each camera.
    for (HumanCounterMap::iterator hIt = m_cameraHuman.begin(); hIt != m_cameraHuman.end(); ++hIt)
      (*hIt).second.Reset();

    // For all chairs ...
    for (ChairArr::iterator chairIt = m_chairs.begin(); chairIt != m_chairs.end(); ++chairIt)
    {
      Chair & chair = (*chairIt);
      float   humanDuration = 0;

      chair.bHuman = false;

      if (f != 0)
        (*f) << HYPHENS << std::endl << CHAIR_IDENTIFIER << chair.index << std::endl
             << HYPHENS << std::endl << std::endl << STATIC_INTERVAL_NUM
             << (int)(chair.staticIntervals.size()) << std::endl;

      // For all "static" intervals ...
      StaticIntervalLst::iterator statIt = chair.staticIntervals.begin();
      for (int statCount = 0; statIt != chair.staticIntervals.end(); ++statIt)
      {
        float statDuration = (*statIt).duration();  // duration of a static interval
        float dynaDuration = 0;
        int   dynaIntervalNum = 0;

        if (f != 0) (*f) << std::endl << STAT_INTERVAL_NO << statCount << std::endl;

        if (statDuration > algoParams.p_minStatInterval())
        {
          if (f != 0) (*f) << STAT_INTERVAL_DURATION << statDuration << std::endl
                           << DYNA_INTERVAL_NUM << (int)((*statIt).dynamicIntervals.size()) << std::endl;

          // For all "dynamic" intervals ...
          IntervalLst::iterator dynaIt = (*statIt).dynamicIntervals.begin();
          for (; dynaIt != (*statIt).dynamicIntervals.end(); ++dynaIt)
          {
            float d = (*dynaIt).duration();  // duration of a dynamic interval
            if (d > algoParams.p_minDynaInterval())
            {
              dynaDuration += d;
              ++dynaIntervalNum;
            }
            else d = 0;

            if (f != 0)
            {
              (*f) << DYNA_INTERVAL_DURATION << d;
              if (d > 0) (*f) << ",  [" << (*dynaIt).startTime << ", " << (*dynaIt).stopTime << "]";
              (*f) << std::endl;
            }
          }

          // Ratio (period of activity)/(period of chair occupation) must be small enough.
          if (dynaDuration < statParams.p_dynaToStatDurationRatio()*statDuration)
          {
            chair.bHuman = (dynaIntervalNum >= statParams.p_minDynaIntervalNumber());
            if (chair.bHuman)
              humanDuration += statDuration;

            if (f != 0) (*f) << std::endl << ACCEPTED_DYNA_INTERVAL_NUM << dynaIntervalNum
                             << std::endl << IS_HUMAN << chair.bHuman << std::endl;
          }
          else if (f != 0) (*f) << std::endl << BAD_ACTIVITY_OCCUPATION_RATIO << std::endl;
        }
        else if (f != 0) (*f) << STAT_INTERVAL_DURATION << 0 << std::endl;

        if (f != 0) (*f) << ACTIVITY_DURATION << dynaDuration << std::endl;

        ++statCount;
      }

      ALIB_ASSERT( humanDuration <= (m_currentTime+0.001) );
      chair.humanity = (m_currentTime > 1.0f) ? (humanDuration/m_currentTime) : 0.0f;
      totalHumanCount.Increment( chair );
      (m_cameraHuman[chair.cameraNo]).Increment( chair );
      if (f != 0) (*f) << std::endl << CHAIR_HUMANITY << chair.humanity << std::endl << std::endl << std::endl;
    }

    (*pSpectatorNum) = (__int32)(totalHumanCount.HumanNumber());
    if (f != 0) (*f) << SPECTATOR_NUM << (*pSpectatorNum) << std::endl;
    m_bGood = true;
    retValue = S_OK;
  }
  catch (std::runtime_error & e) { PrintException( e ); }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function releases interface instance.

  \return Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT HallAnalyzer::Release()
{
  HRESULT retValue = S_OK;
  __int32 spectatorNum = 0;

  m_bFinalStatistics = true;
  retValue = ProcessData( &spectatorNum, -1, 0 );
  if (m_log.is_open())
    m_log.close();

  delete this;
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function prints exception message.

  \param  e  pointer to the exception object or 0 (unsupported exception). */
//-------------------------------------------------------------------------------------------------
void HallAnalyzer::PrintException( const std::runtime_error & e ) const
{
  USES_CONVERSION;
  HallAnalyzer * pThis = const_cast<HallAnalyzer*>( this );

  (pThis->m_lastError = _T("Hall analyzer failed.\n")) += (LPCWSTR)(CString( e.what() ));

  if (m_pDebugOutput != 0)
    m_pDebugOutput->PrintW( IDebugOutput::mt_error, m_lastError.c_str() );

  if (m_log.is_open() && m_log.good())
    (pThis->m_log) << std::endl << std::endl << W2CA(m_lastError.c_str()) << std::endl << std::endl;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function uploads specified data to this object.

  \param  pData  pointer to the input data.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT HallAnalyzer::SetData( IN csalgo::IDataType * pData )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( m_bGood && (pData != 0) );
    switch (pData->type())
    {
      case csinterface::DATAID_LOG_FILE_NAME: // store log file name
      {
        csalgo::TLogFileName * pInfo = dynamic_cast<csalgo::TLogFileName*>( pData );
        ALIB_ASSERT( pInfo != 0 );
        m_logFileName = pInfo->data;

        // Open a log file.
        if (m_parameters.s_demo.p_bLogHallAnalyzer() && !(m_logFileName.empty()))
        {
          USES_CONVERSION;
          if (m_log.is_open()) m_log.close();
          LPCWSTR name = m_logFileName.c_str();
          m_log.open( W2CA( name ), std::ios::trunc | std::ios::out );
          ALIB_ASSERT( m_log.is_open() && m_log.good() );
        }

        retValue = S_OK;
      }
      break;

      case csinterface::DATAID_CAMERA_STATISTICS: // obtain data from camera analyzer
      {
        csalgo::TCameraStatistics * pInfo = dynamic_cast<csalgo::TCameraStatistics*>( pData );
        size_t                      totalChairNum = m_chairs.size();
        __int8                    * p = 0;
        int                         cameraNo = -1;

        ALIB_ASSERT( (pInfo != 0) && !(pInfo->data.empty()) );
        m_bGood = false;
        p = &(*(pInfo->data.begin()));
        p = csutility::SerializeComplexContainer( p, m_tmpChairs, csutility::SERIALIZE_LOAD );
        ALIB_ASSERT( p == &(*(pInfo->data.end())) );
        cameraNo = (m_tmpChairs.empty()) ? -1 : (m_tmpChairs[0]).cameraNo;

        // Copy state data from temporal storage to chairs.
        for (size_t i = 0, c = 0, n = m_tmpChairs.size(); i < n; i++)
        {
          Chair & tmp = m_tmpChairs[i];
          ALIB_ASSERT( tmp.cameraNo == cameraNo );
          while ((c < totalChairNum) && ((m_chairs[c]).index != tmp.index)) ++c;
          ALIB_ASSERT( c < totalChairNum );
          Chair & chr = m_chairs[c];
          ALIB_ASSERT( chr.cameraNo == cameraNo );
          chr.swap_state_data( tmp );
        }

        // Reset human counter.
        (m_cameraHuman[cameraNo]).Reset();
        m_bGood = true;
        retValue = S_OK;
      }
      break;

      default: retValue = csalgo::HRESULT_UNKNOWN_DATA;
    }
  }
  catch (std::runtime_error & e) { PrintException( e ); }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function unloads specified data from this object.

  \param  pData  pointer to the output (possibly resizable) data storage.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT HallAnalyzer::GetData( OUT csalgo::IDataType * pData ) const
{
  HallAnalyzer * pThis = const_cast<HallAnalyzer*>( this );
  CSingleLock    lock( &(pThis->m_dataLocker), TRUE );
  HRESULT        retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( m_bGood && (pData != 0) );
    switch (pData->type())
    {
      case csinterface::DATAID_DEMO_IMAGE: // fill out the demo image
      {
        csalgo::TDemoImage * pInfo = dynamic_cast<csalgo::TDemoImage*>( pData );
        ALIB_ASSERT( (pInfo != 0) && (pInfo->data.first >= 0) && !(pInfo->data.second.empty()) );
        csalgo::DrawResult( m_chairs, m_parameters.s_demo, *pInfo,
                            (pThis->m_cameraHuman[pInfo->data.first]).HumanNumber() );
        retValue = S_OK;
      }
      break;

      case csinterface::DATAID_LAST_ERROR: // fill out the text string by the last error message
      {
        csalgo::TLastError * pInfo = dynamic_cast<csalgo::TLastError*>( pData );
        ALIB_ASSERT( pInfo != 0 );
        pInfo->data = m_lastError;
        retValue = S_OK;
      }
      break;

      default: retValue = csalgo::HRESULT_UNKNOWN_DATA;
    }
  }
  catch (std::runtime_error & e) { pThis->PrintException( e ); }
  return retValue;
}


