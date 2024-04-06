/****************************************************************************
  HallAnalyzer.cpp
  ---------------------
  begin     : Aug 2004
  modified  : 13 Sep 2005
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "AlgoTypes.h"
#include "AlgoParameters.h"
#include "DayNightDecision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgo2
{

// DrawChairs.cpp:
void DrawChairResult( const csalgo2::ChairArr & chairs,
                      const wchar_t             whatToShow,
                      const int                 cameraNo,
                      const int                 spectatorNum,
                      QImage &                  image );

//=================================================================================================
/** \class HallAnalyzer2.
    \brief Implementation of hall analyzer. */
//=================================================================================================
struct HallAnalyzer2 : public IHallAnalyzer
{
  typedef  std::map<int,int>  HumanCounterMap;
  typedef  std::map<int,int>  DayNightStateMap;

  bool                m_bGood;            //!< state flag: non-zero if computational process is currently successful
  csalgo2::ChairArr   m_chairs;           //!< the data associated with all hall's chairs
  csalgo2::ChairArr   m_tmpChairs;        //!< temporal chairs
  CCriticalSection    m_dataLocker;       //!< object locks/unlocks the data being transferred between threads
  csalgo2::Parameters m_parameters;       //!< the object keeps parameter settings
  __int64             m_startTime;        //!< absolute starting time in milliseconds
  float               m_currentTime;      //!< relative current time in seconds, counted off from the process beginning
  HumanCounterMap     m_cameraHuman;      //!< spectator number counters of each camera
  int                 m_maxSpectatorNum;  //!< the maximal observed spectator number
  IDebugOutput *      m_pOutput;          //!< pointer to the debug output object
  StdStr              m_lastError;        //!< textual descriptor of the last error
  StdStr              m_logFileName;      //!< the name of log file
  std::fstream        m_log;              //!< log file stream
  DayNightStateMap    m_dayNightStates;   //!< day-night states of each camera

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HallAnalyzer2::HallAnalyzer2() : m_parameters( csalgo2::FIRST_CONTROL_IDENTIFIER )
{
  m_pOutput = 0;
  m_lastError.reserve( 80 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Destructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HallAnalyzer2::~HallAnalyzer2()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes camera analyzer.

  \param  pParameters  pointer to the binary image of parameters.
  \param  pChairs      pointer to the array of chairs.
  \param  startTime    launch time of hall analyzer.
  \param  pOutput      pointer to the object used to print debug information.
  \return              Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT HallAnalyzer2::Initialize( IN const Int8Arr      * pParameters,
                                   IN const BaseChairArr * pChairs,
                                   IN __int64              startTime,
                                   IN IDebugOutput       * pOutput )
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
    m_maxSpectatorNum = 0;
    m_pOutput = 0;
    m_lastError.clear();
    m_logFileName.clear();
    if (m_log.is_open()) m_log.close();
    m_dayNightStates.clear();

    m_pOutput = pOutput;
    ALIB_ASSERT( (pParameters != 0) && (pChairs != 0) && (startTime >= 0) );

    // First of all, load parameters.
    HRESULT_ASSERT( csutility::BinaryImageToParameters( *pParameters, m_parameters, pOutput ) );

    // Load chairs.
    m_tmpChairs.reserve( pChairs->size() );
    ALIB_ASSERT( csalgocommon::ExtractChairSubSet( *pChairs, -1, m_chairs ) );

    // Do some additional initialization.
    if (m_parameters.s_demo.p_demoMode() && !(m_chairs.empty()))
      csalgocommon::InitializeChairs( m_chairs );

    m_bGood = true;
    retValue = S_OK;
  }
  catch (std::runtime_error & e) { PrintException( e ); }
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function analyzes the current situation throughout the hall.

  \param  pSpectatorNum  количество зрителей сидевших в зале в течении заданного периода.
  \param  timeStamp      текущее время в миллисекундах.
  \param  period         интервал анализа статистики в миллисекундах, [timeStamp-period,...,timeStamp],
                         period <= 0 means statistics counted off from the very beginning.
  \return                Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT HallAnalyzer2::ProcessData( OUT __int32 * pSpectatorNum,
                                    IN  __int64   timeStamp,
                                    IN  __int64   period )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;

  timeStamp;period;
  try
  {
    //const csalgo2::AlgorithmParams & algoParams = m_parameters.s_algorithm;
    //const csalgo2::PeriodParams    & timeParams = m_parameters.s_period;
    int                              totalHumanCount = 0;

    if (pSpectatorNum != 0) (*pSpectatorNum) = 0;
    ALIB_ASSERT( m_bGood && (pSpectatorNum != 0) );

    // Is day-night state ok?
    int state = csalgo2::DayNightDecision::GetHallState( m_dayNightStates );
    if (state == csalgo2::DayNightDecision::BREAK_SURVEILLANCE)
    {
      if (m_pOutput != 0)
        m_pOutput->PrintW( IDebugOutput::mt_error, L"'Day' was unexpectedly detected, surveillance stopped" );
      return S_FALSE;
    }

    m_bGood = false;

    // Reset human counter of each camera.
    for (HumanCounterMap::iterator hIt = m_cameraHuman.begin(); hIt != m_cameraHuman.end(); ++hIt)
      (*hIt).second = 0;

    // For all chairs ...
    for (csalgo2::ChairArr::iterator chairIt = m_chairs.begin(); chairIt != m_chairs.end(); ++chairIt)
    {
      if (chairIt->bHuman)
      {
        ++totalHumanCount;
        ++(m_cameraHuman[chairIt->cameraNo]);
      }
    }

    m_maxSpectatorNum = std::max( m_maxSpectatorNum, totalHumanCount );
    (*pSpectatorNum) = m_maxSpectatorNum;
    m_bGood = true;
    retValue = S_OK;
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
  }
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function releases interface instance.

  \return Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT HallAnalyzer2::Release()
{
  std::fstream * f = (m_log.is_open() && m_log.good()) ? &m_log : 0;

  // Print final statistics.
  if (f != 0)
  {
    const char BARS[] = "########################################";
    const char HYPHENS[] = "----------------------------------------";
    const char TIME[] = "Time = ";
    const char CHAIR_IDENTIFIER[] = "Chair identifier = ";
    const char STATIC_INTERVAL_NUM[] = "Number of static intervals = ";
    const char STAT_INTERVAL_NO[] = "\tStatic interval's index = ";
    const char STAT_INTERVAL_DURATION[] = "\tStatic interval's duration = ";
    const char IS_HUMAN[] = "\t\tIs human = ";
    const char CHAIR_HUMANITY[] = "Chair humanity = ";
    const char SPECTATOR_NUM[] = "Spectator number = ";

    //const csalgo2::AlgorithmParams & algoParams = m_parameters.s_algorithm;
    const csalgo2::PeriodParams    & timeParams = m_parameters.s_period;
    //HRESULT                          retValue = S_OK;
    //__int32                          spectatorNum = 0;

    (*f) << BARS << std::endl << TIME << m_currentTime << std::endl << BARS << std::endl << std::endl;

    // For all chairs ...
    for (csalgo2::ChairArr::iterator chairIt = m_chairs.begin(); chairIt != m_chairs.end(); ++chairIt)
    {
      (*f) << HYPHENS << std::endl << CHAIR_IDENTIFIER << chairIt->index << std::endl
           << HYPHENS << std::endl << std::endl << STATIC_INTERVAL_NUM
           << (int)(chairIt->intervals.size()) << std::endl;

      csalgo2::IntervalLst::iterator statIt = chairIt->intervals.begin();
      for (int statCount = 0; statIt != chairIt->intervals.end(); ++statIt)
      {
        if (statIt->duration() > timeParams.p_minStatInterval())
        {
          (*f) << std::endl << STAT_INTERVAL_NO << statCount << std::endl
               << STAT_INTERVAL_DURATION << statIt->duration() << ",  frames "
               << "[" << statIt->frameNo[0] << "," << statIt->frameNo[1] << "]" << std::endl
               << IS_HUMAN << (statIt->bHumanDetected ? 1 : 0) << std::endl;
        }
        ++statCount;
      }

      (*f) << std::endl << CHAIR_HUMANITY << chairIt->humanity << std::endl << std::endl << std::endl;
    }

    (*f) << SPECTATOR_NUM << m_maxSpectatorNum << std::endl;
    m_log.close();
  }

  delete this;
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function prints exception message.

  \param  e  pointer to the exception object or 0 (unsupported exception). */
///////////////////////////////////////////////////////////////////////////////////////////////////
void HallAnalyzer2::PrintException( const std::runtime_error & e ) const
{
  USES_CONVERSION;
  HallAnalyzer2 * pThis = const_cast<HallAnalyzer2*>( this );

  (pThis->m_lastError = L"Hall analyzer failed.\n") += (LPCWSTR)(CString( e.what() ));

  if (m_pOutput != 0)
    m_pOutput->PrintW( IDebugOutput::mt_error, m_lastError.c_str() );

  if (m_log.is_open() && m_log.good())
    (pThis->m_log) << std::endl << std::endl << W2CA(m_lastError.c_str()) << std::endl << std::endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function uploads specified data to this object.

  \param  pData  pointer to the input data.
  \return        Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT HallAnalyzer2::SetData( IN csinterface::IDataType * pData )
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
        csinterface::TLogFileName * pInfo = dynamic_cast<csinterface::TLogFileName*>( pData );
        ALIB_ASSERT( pInfo != 0 );
        m_logFileName = pInfo->data;
        if (m_log.is_open()) m_log.close();
        if (!(m_logFileName.empty()))
        {
          USES_CONVERSION;
          LPCWSTR name = m_logFileName.c_str();
          m_log.open( W2CA( name ), std::ios::trunc | std::ios::out );
          ALIB_ASSERT( m_log.is_open() && m_log.good() );
        }
        retValue = S_OK;
      }
      break;

      case csinterface::DATAID_CAMERA_STATISTICS: // obtain data from camera analyzer
      {
        csalgocommon::TCameraStatistics * pInfo = dynamic_cast<csalgocommon::TCameraStatistics*>( pData );
        size_t                            totalChairNum = m_chairs.size();
        __int8                          * p = 0;
        int                               cameraNo = -1;
        __int32                           state = 0;

        ALIB_ASSERT( (pInfo != 0) && !(pInfo->data.empty()) );
        m_bGood = false;
        p = &(pInfo->data[0]);
        p = csutility::Serialize( p, state, csutility::SERIALIZE_LOAD );
        p = csutility::SerializeComplexContainer( p, m_tmpChairs, csutility::SERIALIZE_LOAD );
        ALIB_ASSERT( p == &(*(pInfo->data.end())) );
        cameraNo = (m_tmpChairs.empty()) ? -1 : (m_tmpChairs[0]).cameraNo;

        // Copy state data from temporal storage to chairs.
        for (size_t i = 0, c = 0, n = m_tmpChairs.size(); i < n; i++)
        {
          csalgo2::Chair & tmp = m_tmpChairs[i];
          ALIB_ASSERT( tmp.cameraNo == cameraNo );
          while ((c < totalChairNum) && ((m_chairs[c]).index != tmp.index)) ++c;
          ALIB_ASSERT( c < totalChairNum );
          csalgo2::Chair & chr = m_chairs[c];
          ALIB_ASSERT( chr.cameraNo == cameraNo );
          chr.swap_state_data( tmp );
        }

        // Store day-night state of the current camera.
        m_dayNightStates[cameraNo] = state;

        // Reset human counter.
        m_cameraHuman[cameraNo] = 0;
        m_bGood = true;
        retValue = S_OK;
      }
      break;

      default: retValue = csalgocommon::HRESULT_UNKNOWN_DATA;
    }
  }
  catch (std::runtime_error & e) { PrintException( e ); }
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function unloads specified data from this object.

  \param  pData  pointer to the output (possibly resizable) data storage.
  \return        Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT HallAnalyzer2::GetData( OUT csinterface::IDataType * pData ) const
{
  HallAnalyzer2 * pThis = const_cast<HallAnalyzer2*>( this );
  CSingleLock    lock( &(pThis->m_dataLocker), TRUE );
  HRESULT        retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( m_bGood && (pData != 0) );
    switch (pData->type())
    {
      case csinterface::DATAID_DEMO_IMAGE: // fill out the demo image
      {
        csalgocommon::TDemoImage * pInfo = dynamic_cast<csalgocommon::TDemoImage*>( pData );
        ALIB_ASSERT( (pInfo != 0) && (pInfo->data.first >= 0) && !(pInfo->data.second.empty()) );
        csalgo2::DrawChairResult( m_chairs, m_parameters.s_demo.p_showValue(), pInfo->data.first,
                                  pThis->m_cameraHuman[pInfo->data.first], pInfo->data.second );
        retValue = S_OK;
      }
      break;

      case csinterface::DATAID_LAST_ERROR: // fill out the text string by the last error message
      {
        csinterface::TLastError * pInfo = dynamic_cast<csinterface::TLastError*>( pData );
        ALIB_ASSERT( pInfo != 0 );
        pInfo->data = m_lastError;
        retValue = S_OK;
      }
      break;

      case csinterface::DATAID_STATE_FLAG:
      {
        csinterface::TStateFlag * pInfo = dynamic_cast<csinterface::TStateFlag*>( pData );
        ALIB_ASSERT( pInfo != 0 );
        pInfo->data = csalgo2::DayNightDecision::GetHallState( m_dayNightStates );
        retValue = S_OK;
      }
      break;

      default: retValue = csalgocommon::HRESULT_UNKNOWN_DATA;
    }
  }
  catch (std::runtime_error & e) { pThis->PrintException( e ); }
  return retValue;
}

}; // struct HallAnalyzer2

} // namespace csalgo2


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function creates an instance of hall analyzer.

  \param  ppAnalyzer   the address of pointer that points to the instance of created object.
  \param  pParameters  pointer to the binary image of parameters.
  \param  pChairs      pointer to the array of chairs.
  \param  startTime    launch time of hall analyzer.
  \param  pOutput      pointer to the object used to print debug information.
  \return              Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CreateHallAnalyzer( IHallAnalyzer     ** ppAnalyzer,
                            const Int8Arr      * pParameters,
                            const BaseChairArr * pChairs,
                            __int64              startTime,
                            IDebugOutput       * pOutput )
{
  if (ppAnalyzer == 0)
    return E_FAIL;
  (*ppAnalyzer) = 0;

  csalgo2::HallAnalyzer2 * p = new csalgo2::HallAnalyzer2();
  if (p == 0)
    return E_FAIL;

  HRESULT res = p->Initialize( pParameters, pChairs, startTime, pOutput );
  if (FAILED( res ))
    delete p;
  else
    (*ppAnalyzer) = p;

  return res;
}

