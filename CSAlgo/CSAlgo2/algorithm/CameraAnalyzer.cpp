///////////////////////////////////////////////////////////////////////////////////////////////////
// CameraAnalyzer.cpp
// ---------------------
// begin     : Aug 2004
// modified  : 12 Sep 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../CSAlgo2Dll.h"
#include "AlgoTypes.h"
#include "AlgoParameters.h"
#include "ImageAcquirer.h"
#include "ChairImageContainer.h"
#include "SubImage.h"
#include "DayNightDecision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgo2
{

float CorrelateChair( const Arr2f & frame,
                      const ARect & rect,
                      const Chair & chair,
                      const bool    bNccL1,
                      const float   brightnessThr,
                      UByteArr    & buffer ) throw(...);

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct CameraAnalyzer2.
/// \brief  Implementation of camera analyzer.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct CameraAnalyzer2 : public ICameraAnalyzer
{
  typedef csutility::AutoInterfacePtr<csinterface::ILearningAlgorithm> ILearningAlgoPtr;

  bool                      m_bGood;        //!< state flag: nonzero if computational process is successful
  bool                      m_bRunning;     //!< state flag: nonzero after Start() invocation and zero after Stop() invocation
  bool                      m_bInitialized; //!< state flag: nonzero if the object has been properly initialized
                                           
  int                       m_cameraNo;     //!< the index of camera that covers control zone of interest
  int                       m_frameNo;      //!< the index of the current frame
  int                       m_nDropFrame;   //!< the number of dropped frames
         
  __int64                   m_startTime;    //!< absolute starting time in milliseconds
  float                     m_previousTime; //!< relative previous time in seconds, counted off from the process beginning
  float                     m_currentTime;  //!< relative current time in seconds, counted off from the process beginning

  CCriticalSection          m_dataLocker;   //!< object locks/unlocks the data being transferred between threads
  csalgo2::Parameters       m_parameters;   //!< the object keeps parameter settings
  csalgo2::ChairArr         m_chairs;       //!< chair state data and precomputed chair positions
  csalgo2::ImageAcquirer    m_imgAcquirer;  //!< the object acquires and gauges raw video data
  Arr2f                     m_background;   //!< precomputed static backgrounds (empty hall frames)
  csalgo2::DayNightDecision m_dayNight;     //!< day/night analyzer

  IDebugOutput *            m_pOutput;      //!< pointer to the debug output object
  StdStr                    m_lastError;    //!< textual descriptor of the last error
  StdStr                    m_logFileName;  //!< the name of log file
  std::fstream              m_log;          //!< log file stream
  UByteArr                  m_tempBuffer;   //!< temporal buffer

  HINSTANCE                 m_hLearnLib;    //!< instance of online learning library
  ILearningAlgoPtr          m_pLearnAlgo;   //!< pointer to the learning algorithm instance

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function clears this object.
///////////////////////////////////////////////////////////////////////////////////////////////////
void Clear()
{
  m_bGood = false;
  m_bRunning = false;
  m_bInitialized = false;

  m_cameraNo = -1;
  m_frameNo = 0;
  m_nDropFrame = 0;

  m_startTime = 0;
  m_previousTime = 0;
  m_currentTime = 0;

  m_dataLocker;
  m_parameters.set_default();
  m_chairs.clear();
  m_imgAcquirer.Clear();
  m_background.clear();
  m_dayNight.Clear();

  m_pOutput = 0;
  m_lastError.clear();
  m_logFileName.clear();
  if (m_log.is_open()) m_log.close();
  m_tempBuffer.clear();

  m_hLearnLib = 0;
  m_pLearnAlgo.reset();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function prints exception message.
///
/// \param  e  pointer to the exception object or 0 (unsupported exception).
///////////////////////////////////////////////////////////////////////////////////////////////////
void PrintException( const std::runtime_error & e )
{
  USES_CONVERSION;
  CameraAnalyzer2 * pThis = const_cast<CameraAnalyzer2*>( this );

  (pThis->m_lastError = _T("Camera analyzer failed.\n")) += (LPCWSTR)(CString( e.what() ));

  if (m_pOutput != 0)
    m_pOutput->PrintW( IDebugOutput::mt_error, m_lastError.c_str() );

  if (m_log.is_open() && m_log.good())
    (pThis->m_log) << std::endl << std::endl << W2CA(m_lastError.c_str()) << std::endl << std::endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function does full chair processing.
///////////////////////////////////////////////////////////////////////////////////////////////////
void DoProcessing()
{
  const csalgo2::AlgorithmParams    & params = m_parameters.s_algorithm;
  ARect                               imgRect = m_background.rect(), shiftedRect;
  const int                           RANGE = params.p_correctionRange();
  const Arr2f                       & frame = m_imgAcquirer.GetSmoothImage();
  const csalgocommon::ImageProperty & properties = m_imgAcquirer.GetImageProperties();
  const float                         brightness = (properties.imgMean[0]+properties.imgMean[1])/2;
  const float                         brightnessThr = params.p_minStatIntensity();
  std::ostream                      * pLog = ((m_log.is_open() && m_log.good()) ? &m_log : 0);
  const bool                          bNormL1 = params.p_bCorrelationNormL1();

  // Is low brightness?
  if (1.5f*brightness < brightnessThr)
    return;

  if (pLog != 0) (*pLog) << "Elapsed time = " << (int)floor( m_currentTime+0.5 ) << std::endl << std::endl;

  // Correlate the current frame with static background (empty hall) and mark occupied chair.
  for (ChairArr::iterator chIt = m_chairs.begin(); chIt != m_chairs.end(); ++chIt)
  {
    Chair & ch = (*chIt);

    // Correlate the current chair with static background(s).
    {
      bool  bPrevOccupied = ch.bOccupied;
      float diff = csalgo2::CorrelateChair( frame, ch.correctedRect, ch, bNormL1, brightnessThr, m_tempBuffer );

      ch.bOccupied = (diff > params.p_statThreshold());

      // Correct chair position if necessary. Find location of minimal correlation.
      if (ch.bOccupied != bPrevOccupied)
      {
        ch.correctedRect = ch.rect;
        for (int y = -RANGE; y <= RANGE; y++)
        {
          for (int x = -RANGE; x <= RANGE; x++)
          {
            shiftedRect = ch.rect;
            shiftedRect.move( x, y );
            if (shiftedRect <= imgRect) // is inside?
            {
              float d = csalgo2::CorrelateChair( frame, shiftedRect, ch, bNormL1, brightnessThr, m_tempBuffer );
              if (d < diff)
              {
                diff = d;
                ch.correctedRect = shiftedRect;
              }
            }
          }
        }
        ch.bOccupied = (diff > params.p_statThreshold());
      }
      ch.statEntity = diff;
    }

    // Update the last static interval. Analyze chair.
    if (ch.bOccupied || ch.bHardOccupied || !(ch.intervals.empty()))
      ProcessLastInterval( ch );
    AnalyzeChair( ch );

    // Store intermediate state in the log-file.
    if (pLog != 0)
    {
      const int TEXTSIZE = (1<<14);
      char      text[TEXTSIZE+1];
      int       n = 0;

      n = _snprintf( text, TEXTSIZE,
        "Chair=%-3d  stat=%6.4f  dyna=%6.4f  move=(%2d,%2d)  occ=%c  humanity=%4.2f  OccT=%-4d  intervals={",
        ch.index, ch.statEntity, ch.dynaEntity,
        (ch.correctedRect.x1-ch.rect.x1), (ch.correctedRect.y1-ch.rect.y1),
        (ch.bOccupied ? '1' : (ch.bHardOccupied ? 'h' : '0')), ch.humanity, (int)(ch.occupationTime+0.5) );

      for (csalgo2::IntervalLst::iterator it = ch.intervals.begin(); it != ch.intervals.end(); ++it)
      {
        n += _snprintf( text+n, std::max( TEXTSIZE-n, 0 ), "([%d,%d],T=%d,H=%d),",
                        (it->frameNo[0]), (it->frameNo[1]), ((int)(it->duration()+0.5)),
                        ((int)(it->bHumanDetected ? 1 : 0)) );
      }
      n += _snprintf( text+n, std::max( TEXTSIZE-n, 0 ), "}" );
      (*pLog) << text << std::endl;
    }
  }
  if (pLog != 0) (*pLog) << std::endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function updates the list of 'static' intervals according to chair's state.
///
/// \param  chair  the current chair of interest.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessLastInterval( csalgo2::Chair & chair )
{
  // Launch new interval if interval list is empty or previous interval has been completed.
  if (chair.intervals.empty() || (chair.intervals.back()).bFinished)
  {
    if (chair.bOccupied)
    {
      chair.intervals.push_back( csalgo2::Interval() );
      csalgo2::Interval & interval = chair.intervals.back();
      interval.startTime = m_currentTime;
      interval.stopTime = m_currentTime;
      interval.frameNo[1] = (interval.frameNo[0] = m_frameNo);
      chair.dynaAccum.Reset();
      chair.dynaEntity = 0.0f;
    }
    else return; // nothing to do
  }

  const csalgo2::AlgorithmParams & algoParams = m_parameters.s_algorithm;
  const csalgo2::PeriodParams    & timeParams = m_parameters.s_period;
  csalgo2::Interval              & interval = chair.intervals.back();
  float                            timeStep = (m_currentTime - m_previousTime);
  float                            exponent = exp( -timeStep / algoParams.p_statDecayTau() );

  // Charge accumulator according to the current state of correlation.
  if (chair.bOccupied)
    interval.charge = csalgo2::MAX_CORR_DIFF - (csalgo2::MAX_CORR_DIFF - interval.charge) * exponent;
  else
    interval.charge *= exponent;

  // Memorize the latest moment of occupation or hard-occupation.
  interval.stopTime = m_currentTime;
  interval.frameNo[1] = m_frameNo;

  // Is accumulator charged enough?
  chair.bHardOccupied = (interval.charge > algoParams.p_statThreshold());

  // Does the static correlation finally fall below threshold?
  if (!(chair.bOccupied) && !(chair.bHardOccupied))
  {
    if (interval.duration() > timeParams.p_minStatInterval())
    {
      interval.bFinished = 1;           // complete the interval
      chair.dynaAccum.Reset();
      chair.dynaEntity = 0.0f;
    }
    else chair.intervals.pop_back();  // remove short static interval
  }
  else // update statics and dynamics
  {
    chair.dynaAccum.Update( m_imgAcquirer.GetSmoothImage(), chair.rect, chair.weights, m_tempBuffer );

    if (interval.duration() > timeParams.p_minStatInterval())
    {
      csalgo2::Interval::Activity & feeble = interval.feebleActivity;
      csalgo2::Interval::Activity & normal = interval.normalActivity;
      csalgo2::Interval::Activity & strong = interval.strongActivity;

      int N = (int)(m_chairs.size());
      if ((&chair == &(m_chairs[m_frameNo % N])) || (&chair == &(m_chairs[(m_frameNo+N/2) % N])))
      {
        chair.dynaEntity = chair.dynaAccum.GetDynamicCharacteristic( chair.weights );

        feeble.bAboveThr = 0;
        normal.bAboveThr = 0;
        strong.bAboveThr = 0;

        if (chair.dynaEntity > algoParams.p_feebleDynaThreshold()) { feeble.bAboveThr = 1;
        if (chair.dynaEntity > algoParams.p_normalDynaThreshold()) { normal.bAboveThr = 1;
        if (chair.dynaEntity > algoParams.p_strongDynaThreshold()) { strong.bAboveThr = 1; }}}
      }

      if (feeble.bAboveThr) { feeble += chair.statEntity;  feeble.duration += timeStep; }
      if (normal.bAboveThr) { normal += chair.statEntity;  normal.duration += timeStep; }
      if (strong.bAboveThr) { strong += chair.statEntity;  strong.duration += timeStep; }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function analyzes a chair.
///////////////////////////////////////////////////////////////////////////////////////////////////
void AnalyzeChair( csalgo2::Chair & chair )
{
  const csalgo2::AlgorithmParams & algoParams = m_parameters.s_algorithm;
  const csalgo2::PeriodParams    & timeParams = m_parameters.s_period;
  float                            humanDuration = 0.0f, occupationTime = 0.0f;

  // For all intervals of static occupation ...
  for (csalgo2::IntervalLst::iterator it = chair.intervals.begin(); it != chair.intervals.end(); ++it)
  {
    float statDuration = it->duration();                   // duration of a static interval
    if (statDuration <= timeParams.p_minStatInterval())
      continue;

    if (!(it->bHumanDetected))
    {
      if (it->normalActivity.duration > timeParams.p_minNormalActivityPeriod())
      {
        it->bHumanDetected = 1;
      }
      else if (it->feebleActivity.duration > timeParams.p_minFeebleActivityPeriod())
      {
        it->bHumanDetected = (it->feebleActivity.mean() > algoParams.p_strongStatThreshold());
      }
    }

    occupationTime += statDuration;
    if (it->bHumanDetected)
      humanDuration += statDuration;
  }

  chair.occupationTime = occupationTime;
  chair.bHuman = false;
  chair.humanity = 0;
  if ((occupationTime > timeParams.p_minTotalOccupationPeriod()) && (m_currentTime > 1.0f))
  {
    chair.humanity = humanDuration / m_currentTime;
    chair.bHuman = (chair.humanity > algoParams.p_chairHumanityThreshold());
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
CameraAnalyzer2() : m_parameters( csalgo2::FIRST_CONTROL_IDENTIFIER )
{
  Clear();
  m_lastError.reserve( 80 );
  m_tempBuffer.reserve( 128*128*sizeof(float) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Destructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual ~CameraAnalyzer2()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function initializes camera analyzer.
///
/// \param  pParameters  pointer to the binary image of parameters.
/// \param  pChairs      pointer to the array of chairs.
/// \param  pBackground  pointer to the image of empty hall.
/// \param  cameraNo     camera's number.
/// \param  pOutput      pointer to the object used to print debug information.
/// \return              Ok = S_OK.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Initialize( IN const Int8Arr      * pParameters,
                            IN const BaseChairArr * pChairs,
                            IN const Arr2f        * pBackground,
                            IN __int32              cameraNo,
                            IN IDebugOutput       * pOutput )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;

  try
  {
    Clear();
    m_pOutput = pOutput;
    ALIB_ASSERT( (pParameters != 0) && (pChairs != 0) && (pBackground != 0) && (cameraNo >= 0) );

    // First of all, load parameters.
    ALIB_ASSERT( csutility::BinaryImageToParameters( *pParameters, m_parameters, pOutput ) );

    // Copy the rest of input parameters.
    ALIB_ASSERT( !(pBackground->empty()) );
    m_background = (*pBackground);
    m_cameraNo = (int)cameraNo;

    // Initialize image acquirer.
    int brightnessThr = 0, percentageThr = 0;
    if (m_parameters.s_algorithm.p_bDetectDayNight())
    {
      if (!(csalgocommon::GetDayNightParametersFromFile( theDll.m_dllName.c_str(), cameraNo,
                                                         &brightnessThr, &percentageThr, pOutput )))
      {
        brightnessThr = csalgocommon::DAY_NIGHT_DEFAULT_BRIGHTNESS_THRESHOLD;
        percentageThr = csalgocommon::DAY_NIGHT_DEFAULT_PERCENTAGE_THRESHOLD;
      }
    }
    m_imgAcquirer.Initialize( pBackground->width(), pBackground->height(), m_parameters.s_acquirer,
                              brightnessThr, percentageThr );

    // Try to load and initialize chair templates.
    ChairImageContainer chairTemplates;
    bool bChairTemplates = InitializeChairImageContainer( &chairTemplates );
    if (!bChairTemplates && (m_pOutput != 0))
    {
      wchar_t msg[128];
      swprintf( msg, L"Camera %d: chair templates were not loaded", cameraNo );
      m_pOutput->PrintW( IDebugOutput::mt_warning, msg);
    }

    // Precompute some characteristics of chairs.
    InitializeChairs( (bChairTemplates ? &chairTemplates : 0), *pChairs );
    ALIB_ASSERT( !(m_chairs.empty()) );

    // Launch video file writing, if necessary.
    LaunchOnlineJob();

    m_bGood = true;
    m_bInitialized = true;
    retValue = S_OK;
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
  }
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Ќачало работы алгоритма.
///
/// \param  startTime  врем€ запуска процесса слежени€ в миллисекундах.
/// \return            Ok = S_OK.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Start( IN __int64 startTime )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( m_bInitialized && m_bGood && !m_bRunning );
    m_bGood = false;
    if (!(m_logFileName.empty()))
    {
      USES_CONVERSION;
      LPCWSTR name = m_logFileName.c_str();
      m_log.open( W2CA( name ), std::ios::trunc | std::ios::out );
      ALIB_ASSERT( m_log.is_open() && m_log.good() );
      PrintExplanationOfLogFileFormat();
    }

    if (m_pLearnAlgo.get() != 0)
      m_pLearnAlgo->Start( 0.0f );

    m_startTime = startTime;
    m_bGood = true;
    m_bRunning = true;
    retValue = S_OK;
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
  }
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief ѕередать очередное изображение дл€ алгоритма и обработать его.
///
/// \param  pBI        указатель на заголовок изображени€.
/// \param  pBytes     указатель на начало изображени€.
/// \param  timeStamp  текущее врем€ в миллисекундах.
/// \return            On success: S_OK, HRESULT_DROP_FRAME or HRESULT_SKIP_FRAME, otherwise E_FAIL.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI,
                              IN const void       * pBytes,
                              IN __int64            timeStamp )
{
  CSingleLock    lock( &m_dataLocker, TRUE );
  HRESULT        retValue = E_FAIL;
  const ubyte  * pImage = reinterpret_cast<const ubyte*>( pBytes );
  bool           bDropFrame = false;
  bool           bSkipFrame = false;
  std::ostream * pLog = ((m_log.is_open() && m_log.good()) ? &m_log : 0);

  try
  {
    ALIB_ASSERT( (pBI != 0) && (pBytes != 0) );
    ALIB_ASSERT( m_bGood );
    m_bGood = false;
    ALIB_ASSERT( m_bInitialized && m_bRunning );
    ALIB_ASSERT( m_startTime <= timeStamp );

    m_currentTime = csalgocommon::MilliToSeconds( m_startTime, timeStamp );

    // Acquire the current frame and measure some of its characteristics. Then do processing.
    if (m_imgAcquirer.Acquire( pBI, pImage, pLog ))
    {
      if (m_dayNight.AnalizeDayNight( (m_imgAcquirer.GetDayNightState() > 0), timeStamp, m_pOutput ))
      {
        if ((m_frameNo > 25) && (m_currentTime > m_parameters.s_period.p_startDelayPeriod()))
          DoProcessing();
        else
          bSkipFrame = true;

        ++m_frameNo;
      }
      else bSkipFrame = true;
    }
    else
    {
      bDropFrame = true;
      m_nDropFrame = m_imgAcquirer.DroppedFrameNumber();
    }

    // Write output video file if necessary.
    if (!bDropFrame && !bSkipFrame && (m_pLearnAlgo.get() != 0))
      m_pLearnAlgo->ProcessImage( pBI, reinterpret_cast<const __int8*>( pBytes ), m_currentTime );

    m_previousTime = m_currentTime;
    m_bGood = true;
    retValue = (bDropFrame ? csalgocommon::HRESULT_DROP_FRAME :
               (bSkipFrame ? csalgocommon::HRESULT_SKIP_FRAME : S_OK));
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
  }
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief ќстановка работы алгоритма.
///
/// \param  stopTime  the stop time of surveillance process in milliseconds.
/// \return           Ok = S_OK.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Stop( IN __int64 stopTime )
{
  CSingleLock    lock( &m_dataLocker, TRUE );
  HRESULT        retValue = E_FAIL;
  //std::ostream * pLog = ((m_log.is_open() && m_log.good()) ? &m_log : 0);

  try
  {
    ALIB_ASSERT( m_bGood && m_bRunning );
    ALIB_ASSERT( stopTime >= m_startTime );
    m_currentTime = csalgocommon::MilliToSeconds( m_startTime, stopTime );

    if (m_pLearnAlgo.get() != 0)
      m_pLearnAlgo->Stop( m_currentTime, true );

    m_bRunning = false;
    m_bInitialized = false;
    retValue = (m_dayNight.GetState() != csalgo2::DayNightDecision::BREAK_SURVEILLANCE) ? S_OK : E_FAIL;
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
  }
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function releases interface instance.
///
/// \return  Ok = S_OK.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Release()
{
  delete this;
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function uploads specified data to this object.
///
/// \param  pData  pointer to the input data.
/// \return        Ok = S_OK.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT SetData( IN csinterface::IDataType * pData )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( m_bGood && m_bInitialized && (pData != 0) );
    switch (pData->type())
    {
      case csalgo::DATAID_LOG_FILE_NAME: // store log file name
      {
        const csinterface::TLogFileName * pInfo = 0;
        csutility::DynamicCastPtr( pData, &pInfo );
        m_logFileName = pInfo->data;
        retValue = S_OK;
      }
      break;

      case csinterface::DATAID_SURVEILLANCE_INTERVAL:
      {
        const csinterface::TSurveillanceInterval * pInfo = 0;
        csutility::DynamicCastPtr( pData, &pInfo );
        m_dayNight.Initialize( pInfo->data.first, pInfo->data.second,
                               (__int64)(1000.0 * m_parameters.s_period.p_minDayDuration()) );
        retValue = S_OK;
      }
      break;

      case csinterface::DATAID_STATE_FLAG:
      {
        const csinterface::TStateFlag * pInfo = 0;
        csutility::DynamicCastPtr( pData, &pInfo );
        if (m_dayNight.MakeDecision( pInfo->data ))
        {
          for (ChairArr::iterator i = m_chairs.begin(); i != m_chairs.end(); ++i) // clear statistics
            i->clear_state_data();
        }
        retValue = S_OK;
      }
      break;

      default: retValue = csalgocommon::HRESULT_UNKNOWN_DATA;
    }
  }
  catch (std::runtime_error & e)
  {
    PrintException( e );
  }
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function unloads specified data from this object.
///
/// \param  pData  pointer to the output (possibly resizable) data storage, say STL container.
/// \return        Ok = S_OK.
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const
{
  CameraAnalyzer2 * pThis = const_cast<CameraAnalyzer2*>( this );
  CSingleLock       lock( &(pThis->m_dataLocker), TRUE );
  HRESULT           retValue = S_OK;

  try
  {
    ALIB_ASSERT( m_bGood && m_bInitialized && (pData != 0) );
    switch (pData->type())
    {
      case csinterface::DATAID_CAMERA_STATISTICS:
      {
        csalgocommon::TCameraStatistics * pInfo = 0;
        __int8                          * p = 0;
        __int32                           state = m_dayNight.GetState();

        csutility::DynamicCastPtr( pData, &pInfo );
        p = csutility::Serialize( p, state, csutility::SERIALIZE_SIZE );
        p = csutility::SerializeComplexContainer( p, pThis->m_chairs, csutility::SERIALIZE_SIZE );
        pInfo->data.resize( std::distance( (__int8*)0, p ) );
        p = &(pInfo->data[0]);
        p = csutility::Serialize( p, state, csutility::SERIALIZE_SAVE );
        p = csutility::SerializeComplexContainer( p, pThis->m_chairs, csutility::SERIALIZE_SAVE );
        ALIB_ASSERT( p == &(*(pInfo->data.end())) );
      }
      break;

      case csalgo::DATAID_FRAME_COUNTERS:
      {
        csalgocommon::TFrameCounters * pInfo = 0;
        csutility::DynamicCastPtr( pData, &pInfo );
        pInfo->data.first = m_frameNo;
        pInfo->data.second = m_nDropFrame;
      }
      break;

      case csalgo::DATAID_DEMO_IMAGE:
      {
        csalgocommon::TDemoImage * pInfo = 0;
        csutility::DynamicCastPtr( pData, &pInfo );
        const Arr2f & source = m_imgAcquirer.GetSmoothImage();
        csutility::CorrectBrightness( &source, 70, &(pInfo->data.second), &(pThis->m_tempBuffer) );
        pInfo->data.first = m_cameraNo;

//>>>>>
//#ifdef _DEBUG
//if (m_cameraNo == 17)
//{
//  for (ChairArr::const_iterator chIt = m_chairs.begin(); chIt != m_chairs.end(); ++chIt)
//  {
//    if (chIt->index == 54)
//    {
//      const csalgo2::Image2fSet & samples = chIt->samples;
//      int W = samples[0].width(), H = samples[0].height();
//      pThis->m_tempBuffer.resize( W * H * sizeof(QImage::value_type) );
//      QImage image;
//      image.wrap( W, H, reinterpret_cast<RGBQUAD*>( &(pThis->m_tempBuffer[0]) ) );
//      for (int i = 0; i < (int)(samples.size()); i++)
//      {
//        csutility::CopyFloatImageToColorImage( &(samples[i]), &image );
//        if ((i+1)*(W+1) <= pInfo->data.second.width())
//          pInfo->data.second.copy( i*(W+1), pInfo->data.second.height()-H, image, 0, 0, W, H  );
//      }
//    }
//  }
//}
//#endif // _DEBUG
//>>>>>

//        if (m_parameters.s_demo.p_bCamAnalyzerShowChairState())
//          csalgo::DrawResult( m_chairs, m_parameters.s_demo, *pInfo, -1 );
      }
      break;

      case csalgo::DATAID_LAST_ERROR:
      {
        csinterface::TLastError * pInfo = 0;
        csutility::DynamicCastPtr( pData, &pInfo );
        pInfo->data = m_lastError;
      }
      break;

      default: retValue = csalgocommon::HRESULT_UNKNOWN_DATA;
    }
  }
  catch (std::runtime_error & e)
  {
    pThis->PrintException( e );
    retValue = E_FAIL;
  }
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool InitializeChairImageContainer( ChairImageContainer * pChairTemplates )
{
  try
  {
    wchar_t fullName[_MAX_PATH+1], name[128];

    ALIB_ASSERT( !(theDll.m_dllName.empty()) && (pChairTemplates != 0) );
    memset( fullName, 0, sizeof(fullName) );
    swprintf( name, L"cam%02d_chair_templates.xml", m_cameraNo );
    ALIB_ASSERT( PathAppend( fullName, theDll.m_dllName.c_str() ) );
    ALIB_ASSERT( PathRemoveFileSpec( fullName ) );
    ALIB_ASSERT( PathAppend( fullName, name ) );
    if ((CFileFind()).FindFile( fullName ))
    {
      pChairTemplates->Load( fullName );
      return true;
    }
  }
  catch (std::runtime_error e)
  {
    if (m_log.is_open() && m_log.good())
      m_log << std::endl << e.what() << std::endl << std::endl;
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function.
///////////////////////////////////////////////////////////////////////////////////////////////////
void InitializeChairs( ChairImageContainer * pChairTemplates, const BaseChairArr & baseChairs )
{
  ARect                            boundRect = m_background.rect();
  Vec2iArr                         icurve;
  std::vector<Arr2ub>              auxImages;
  const csalgo2::AlgorithmParams & params = m_parameters.s_algorithm;

  // Extract chairs that correspond to specified camera.
  ALIB_ASSERT( csalgocommon::ExtractChairSubSet( baseChairs, m_cameraNo, m_chairs ) );

  icurve.reserve(1<<10);
  auxImages.reserve( (pChairTemplates != 0) ? 100 : 0 );

  for (ChairArr::iterator chIt = m_chairs.begin(); chIt != m_chairs.end(); ++chIt)
  {
    Chair & ch = (*chIt);

    // Precompute some common characteristics of chairs.
    csalgocommon::InitializeSingleChair( &ch, &icurve, &boundRect, m_cameraNo, true );

    // Load the set of typical views of empty chair.
    if (pChairTemplates != 0)
      pChairTemplates->GetChairImages( m_cameraNo, ch.index, auxImages );

    // Add chair templates to the chair data structure, the first one comes from the 'static' background.
    ch.samples.reserve( auxImages.size() + 1 );
    ch.samples.clear();
    for (int i = 0; i < (int)(auxImages.size()); i++)
    {
      if (alib::AreDimensionsEqual( auxImages[i], ch.rect ))
      {
        ch.samples.push_back( Arr2f() );
        (ch.samples.back()).resize2( ch.rect );
        alib::Copy( auxImages[i], ch.samples.back() );
      }
      //#ifdef _DEBUG
      else
      {
        std::wstringstream msg;
        msg << L"chair W=" << ch.rect.width() << L",  H=" << ch.rect.height()
            << L",  image W=" << (auxImages[i]).width() << L",  H=" << (auxImages[i]).height() << std::endl;
        if (m_pOutput != 0)
          m_pOutput->PrintW( IDebugOutput::mt_debug_info, (msg.str()).c_str() );
        ASSERT(0);
      }
      //#endif // _DEBUG
    }
    ch.samples.push_back( Arr2f() );
    (ch.samples.back()).resize2( ch.rect );
    csalgo2::CopySubImage( m_background, ch.rect, ch.samples.back() );

    // Initialize dynamic accumulator.
    ch.dynaAccum.Initialize( ch.rect );

    // Border points should be less influential than internal ones.
    {
      double wsum = 0.0;

      // Create weight mask of chair points.
      ch.weights.resize2( ch.rect );
      std::fill( ch.weights.begin(), ch.weights.end(), 0.0f );

      // Compute actual thickness of border layer.
      int sqRadius = (ch.rect.width() + ch.rect.height())/2;
      sqRadius = alib::Round( alib::Sqr( (double)(sqRadius * params.p_weakBorderThickness()) ) );

      // Assign weights to chair's internal points according their distances to the outline curve.
      const HScanArr & scans = ch.region();
      for (HScanArr::const_iterator scanIt = scans.begin(); scanIt != scans.end(); ++scanIt)
      {
        int x1 = scanIt->x1;
        int x2 = scanIt->x2;
        int y  = scanIt->y;

        ALIB_ASSERT( (ch.rect.x1 <= x1) && (x1 < x2) && (x2 <= ch.rect.x2) );
        ALIB_ASSERT( (ch.rect.y1 <= y) && (y < ch.rect.y2) );

        for (int x = x1; x < x2; x++)
        {
          Vec2iArr::iterator it = vislib::FindNearestPoint( icurve.begin(), icurve.end(), x, y );
          ALIB_ASSERT( it != icurve.end() );

          int   sqDistance = alib::Sqr( it->x - x ) + alib::Sqr( it->y - y );
          float w = ((sqDistance < sqRadius) ? sqrt( (float)sqDistance/(float)sqRadius ) : 1.0f);

          ch.weights( x - ch.rect.x1, y - ch.rect.y1 ) = w;
          wsum += w;
        }
      }

      alib::Multiply( ch.weights, (float)(alib::Reciprocal( wsum )) );
    }

    // Normalize all template images.
    {
      for (csalgo2::Image2fSet::iterator i = ch.samples.begin(); i != ch.samples.end(); ++i)
      {
        csalgo2::NormalizeSubImage( *i, ch.weights, m_parameters.s_algorithm.p_bCorrelationNormL1() );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function.
///////////////////////////////////////////////////////////////////////////////////////////////////
void LaunchOnlineJob()
{
  int scenario = csutility::ShouldTestVideoBeWritten( theDll.m_dllName.c_str(), m_cameraNo );
  if (scenario >= 0)
  {
    if (m_hLearnLib != 0)
      AfxFreeLibrary( m_hLearnLib );
    m_hLearnLib = 0;
    m_pLearnAlgo.reset();

    wchar_t name[_MAX_PATH+1];
    memset( name, 0, sizeof(name) );
    if (PathAppend( name, theDll.m_dllName.c_str() ) &&
        PathRemoveFileSpec( name )                   &&
        PathAppend( name, L"CSOnlineWriter.dll" )    &&
        (CFileFind()).FindFile( name )               &&
        ((m_hLearnLib = AfxLoadLibrary( name )) != 0))
    {
      PCreateLearningAlgorithmInstance p;
      p = (PCreateLearningAlgorithmInstance)GetProcAddress( m_hLearnLib, "CreateLearningAlgorithmInstance" );
      if (p != 0)
      {
        csinterface::ILearningAlgorithm * pAlgo;
        if ((*p)( 0, m_pOutput, &pAlgo ) != 0)
        {
          m_pLearnAlgo.reset( pAlgo );
          csinterface::TCameraNo cam( m_cameraNo );
          pAlgo->SetData( &cam );
          csinterface::TWorkingScenario sce( scenario );
          pAlgo->SetData( &sce );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function prints the text that explaines log-file format.
///////////////////////////////////////////////////////////////////////////////////////////////////
void PrintExplanationOfLogFileFormat()
{
  std::ostream * pLog = ((m_log.is_open() && m_log.good()) ? &m_log : 0);
  if (pLog != 0)
  {
    (*pLog) << "Format fields:" << std::endl
            << "Chair - chair identifier" << std::endl
            << "stat - static difference with the best empty chair template" << std::endl
            << "dyna - dynamic characteristic of the last static interval" << std::endl
            << "move - correction of the chair position" << std::endl
            << "occ - chair's occupation status: 1 - occupied, h - hard occupied, 0 - not occupied" << std::endl
            << "humanity - the level of humanity of the chair [0..1]" << std::endl
            << "OccT - the total period of chair occupation in seconds" << std::endl
            << "intervals - the list of static intervals" << std::endl
            << "Interval format: ([start frame, stop frame], T - duration in seconds, H - human detection (0 or 1))"
            << std::endl << std::endl << std::endl;
  }
}

};

} // namespace csalgo2


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function creates an instance of camera analyzer.
///
/// \param  ppAnalyzer   the address of pointer that points to the instance of created object.
/// \param  pParameters  pointer to the binary image of parameters.
/// \param  pChairs      pointer to the array of chairs.
/// \param  pBackground  pointer to the image of empty hall.
/// \param  cameraNo     camera's number.
/// \param  pOutput      pointer to the object used to print debug information.
/// \return              Ok = S_OK.
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CreateCameraAnalyzer( ICameraAnalyzer   ** ppAnalyzer,
                              const Int8Arr      * pParameters,
                              const BaseChairArr * pChairs,
                              const Arr2f        * pBackground,
                              __int32              cameraNo,
                              IDebugOutput       * pOutput )
{
  if (ppAnalyzer == 0)
    return E_FAIL;
  (*ppAnalyzer) = 0;

  csalgo2::CameraAnalyzer2 * p = new csalgo2::CameraAnalyzer2();
  if (p == 0)
    return E_FAIL;

  HRESULT res = p->Initialize( pParameters, pChairs, pBackground, cameraNo, pOutput );
  if (FAILED( res ))
    delete p;
  else
    (*ppAnalyzer) = p;

  return res;
}

