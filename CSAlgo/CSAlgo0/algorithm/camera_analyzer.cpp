/****************************************************************************
  camera_analyzer.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "algo_param.h"
#include "chair.h"
#include "image_acquirer.h"
#include "chair_correlator.h"
#include "camera_analyzer.h"
#include "utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function creates an instance of camera analyzer.

  \param  ppAnalyzer   the address of pointer that points to the instance of created object.
  \param  pParameters  pointer to the binary image of parameters.
  \param  pChairs      pointer to the array of chairs.
  \param  pBackground  pointer to the image of empty hall.
  \param  cameraNo     camera's number.
  \param  pDebugOut    pointer to the object used to print debug information.
  \return              Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CreateCameraAnalyzer( ICameraAnalyzer   ** ppAnalyzer,
                              const Int8Arr      * pParameters,
                              const BaseChairArr * pChairs,
                              const Arr2f        * pBackground,
                              __int32              cameraNo,
                              IDebugOutput       * pDebugOut )
{
  if (ppAnalyzer == 0)
    return E_FAIL;
  (*ppAnalyzer) = 0;

  CameraAnalyzer * p = new CameraAnalyzer();
  if (p == 0)
    return E_FAIL;

  HRESULT res = p->Initialize( pParameters, pChairs, pBackground, cameraNo, pDebugOut );
  if (FAILED( res ))
    delete p;
  else
    (*ppAnalyzer) = p;

  return res;
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CameraAnalyzer::CameraAnalyzer() : m_parameters( csalgo::FIRST_CONTROL_IDENTIFIER )
{
  Clear();
  m_lastError.reserve( 80 );
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CameraAnalyzer::~CameraAnalyzer()
{
  if (m_hLearnLib != 0)
    AfxFreeLibrary( m_hLearnLib );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void CameraAnalyzer::Clear()
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
  m_chairCorrelator.Clear();

  m_staticBackground.clear();
  m_dynamicBackground.clear();
  m_currentFrame.clear();

  m_pDebugOutput = 0;
  m_lastError.erase();
  m_logFileName.erase();
  if (m_log.is_open()) m_log.close();
  m_tempBuffer.clear();

  m_hLearnLib = 0;
  m_pLearnAlgo.reset();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes camera analyzer.

  \param  pParameters  pointer to the binary image of parameters.
  \param  pChairs      pointer to the array of chairs.
  \param  pBackground  pointer to the image of empty hall.
  \param  cameraNo     camera's number.
  \param  pDebugOut    pointer to the object used to print debug information.
  \return              Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CameraAnalyzer::Initialize( IN const Int8Arr      * pParameters,
                                    IN const BaseChairArr * pChairs,
                                    IN const Arr2f        * pBackground,
                                    IN __int32              cameraNo,
                                    IN IDebugOutput       * pDebugOut )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;

  try
  {
    Clear();
    m_pDebugOutput = pDebugOut;
    ALIB_ASSERT( (pParameters != 0) && (pChairs != 0) && (pBackground != 0) && (cameraNo >= 0) );

    // First of all, load parameters.
    HRESULT_ASSERT( csalgo::LoadParameters( *pParameters, m_parameters ) );

    int frameChainSize = m_parameters.s_algorithm.p_frameChainSize();
    ALIB_ASSERT( frameChainSize >= 1 );

    // Copy chair structure. Drop chairs that are not correspond to specified camera.
    ALIB_ASSERT( csalgo::ExtractChairSubSet( *pChairs, cameraNo, m_chairs ) );

    // Copy the rest of input parameters.
    m_staticBackground = (*pBackground);
    m_cameraNo = (int)cameraNo;

    // Initialize image acquirer.
    ALIB_ASSERT( m_imgAcquirer.Init( pBackground->width(), pBackground->height() ) );

    // Initialize chair correlator.
    ALIB_ASSERT( m_chairCorrelator.Initialize( m_parameters.s_algorithm ) );

    // Allocate temporal buffers.
    m_dynamicBackground.resize2( m_staticBackground );
    m_currentFrame.resize2( m_staticBackground );

    // Precompute some characteristics of chairs.
    ARect boundRect = m_staticBackground.rect();
    csalgo::InitializeChairs( m_chairs, &boundRect, cameraNo, true );

    // Launch video file writing, if necessary.
    int scenario = csutility::ShouldTestVideoBeWritten( theDll.m_dllName.c_str(), cameraNo );
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
          if ((*p)( 0, m_pDebugOutput, &pAlgo ) != 0)
          {
            m_pLearnAlgo.reset( pAlgo );
            csinterface::TCameraNo cam( cameraNo );
            pAlgo->SetData( &cam );
            csinterface::TWorkingScenario sce( scenario );
            pAlgo->SetData( &sce );
          }
        }
      }
    }

    m_bGood = true;
    m_bInitialized = true;
    retValue = S_OK;
  }
  catch (std::runtime_error & e) { PrintException( e ); }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Начало работы алгоритма.

  \param  startTime  время запуска процесса слежения в миллисекундах.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CameraAnalyzer::Start( IN __int64 startTime )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( m_bInitialized && m_bGood && !m_bRunning );
    m_bGood = false;
    if (m_parameters.s_demo.p_bLogCamAnalyzer() && !(m_logFileName.empty()))
    {
      USES_CONVERSION;
      LPCWSTR name = m_logFileName.c_str();
      m_log.open( W2CA( name ), std::ios::trunc | std::ios::out );
      ALIB_ASSERT( m_log.is_open() && m_log.good() );
    }
    m_startTime = startTime;
    m_bGood = true;
    m_bRunning = true;
    retValue = S_OK;

    if (m_pLearnAlgo.get() != 0)
      m_pLearnAlgo->Start( (float)(startTime * 0.001) );
  }
  catch (std::runtime_error & e) { PrintException( e ); }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function does some preprocessing at the beginning of each iteration. */
//-------------------------------------------------------------------------------------------------
bool CameraAnalyzer::DoPreprocessing()
{
  const Frame & newFrame = m_imgAcquirer.GetFrame();
  float         tau = m_parameters.s_algorithm.p_frameAccumulationDecayTau();
  float         a = exp( -(m_currentTime - m_previousTime)/tau );
  float         b = (1.0f - a);

  // Update current frame.
  for (int i = 0, n = m_currentFrame.size(); i < n; i++)
    m_currentFrame[i] = m_currentFrame[i]*a + newFrame[i]*b;

  // Undate noise deviation.
  m_currentFrame.noise = sqrt( alib::Sqr( m_currentFrame.noise*a ) + alib::Sqr( newFrame.noise*b ) );

  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function process chairs. */
//-------------------------------------------------------------------------------------------------
bool CameraAnalyzer::DoProcessing()
{
  const csalgo::AlgorithmParams & params = m_parameters.s_algorithm;
  ARect                           imgRect = m_staticBackground.rect(), shiftedRect;
  const int                       RANGE = params.p_statCorrSearchRange();

  // Correlate the current frame with static background (empty hall) and mark occupied chairs.
  {
    for (ChairArr::iterator chairIt = m_chairs.begin(); chairIt != m_chairs.end(); ++chairIt)
    {
      Chair & chair = (*chairIt);
      float   diff = 0.0f;

      ASSERT( chair.cameraNo == m_cameraNo );
      chair.bOccupied = false;

      // Correlate the current chair with static background.
      if (m_chairCorrelator.Correlate( m_currentFrame, m_staticBackground, chair, true, diff ))
      {
        bool bPrevOccupied = chair.bOccupied;
        chair.bOccupied = (diff > params.p_statDiffThreshold());

        // Correct chair position, if necessary.
        if (chair.bOccupied && (!bPrevOccupied || !(chair.bPosCorrected)))
        {
          Vec2i correction(0,0);
          float minDiff = diff;

          // Find position's correction that minimizes correlation.
          for (int y = -RANGE; y <= RANGE; y++)
          {
            for (int x = -RANGE; x <= RANGE; x++)
            {
              shiftedRect = chair.rect;
              shiftedRect.move( x, y );

              if (shiftedRect <= imgRect) // is inside?
              {
                chair.correction.set( x, y ); // set temporarily to compute correlation
                if (m_chairCorrelator.Correlate( m_currentFrame, m_staticBackground, chair, true, diff ))
                {
                  if (diff < minDiff)
                  {
                    minDiff = diff;
                    correction.set( x, y );
                  }
                }
              }
            }
          }

          diff = minDiff;
          chair.correction = correction;
          chair.bOccupied = (diff > params.p_statDiffThreshold());
          chair.bPosCorrected = true;
        }
      }
      chair.statNCC = diff;

      if (chair.bOccupied || chair.bHardOccupied)
        ProcessLastStaticInterval( chair );
    }
  }

  // Correlate the current frame with dynamic background to detect motion inside occupied chairs.
  {
    for (ChairArr::iterator chairIt = m_chairs.begin(); chairIt != m_chairs.end(); ++chairIt)
    {
      Chair & chair = (*chairIt);
      float   diff = 0.0f;

      ASSERT( chair.cameraNo == m_cameraNo );
      if (chair.bOccupied || chair.bHardOccupied)
      {
        if (m_chairCorrelator.Correlate( m_currentFrame, m_dynamicBackground, chair, false, diff ))
        {
          chair.bMovement = (diff > params.p_dynaDiffThreshold());
          if (chair.bMovement || chair.bHardMovement)
            ProcessLastDynamicInterval( chair );
        }
      }
      chair.dynaNCC = diff;
    }
  }

  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function does some postprocessing after each iteration. */
//-------------------------------------------------------------------------------------------------
bool CameraAnalyzer::DoPostprocessing()
{
  const Frame & newFrame = m_imgAcquirer.GetFrame();
  float         frameAccumTau = m_parameters.s_algorithm.p_frameAccumulationDecayTau();
  float         tau = frameAccumTau * m_parameters.s_algorithm.p_dynaToAccumDecayRatio();
  float         a = exp( -(m_currentTime - m_previousTime)/tau );
  float         b = (1.0f - a);

  // Update dynamic background.
  for (int i = 0, n = m_dynamicBackground.size(); i < n; i++)
  {
    m_dynamicBackground[i] = m_dynamicBackground[i]*a + newFrame[i]*b;
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Передать очередное изображение для алгоритма и обработать его.

  \param  pBI        указатель на заголовок изображения.
  \param  pBytes     указатель на начало изображения.
  \param  timeStamp  текущее время в миллисекундах.
  \return            On success: S_OK, HRESULT_DROP_FRAME or HRESULT_SKIP_FRAME, otherwise E_FAIL. */
//-------------------------------------------------------------------------------------------------
HRESULT CameraAnalyzer::ProcessImage( IN const BITMAPINFO * pBI,
                                      IN const void       * pBytes,
                                      IN __int64            timeStamp )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;
  LPCUBYTE    pImage = reinterpret_cast<LPCUBYTE>( pBytes );
  bool        bDropFrame = false;
  bool        bSkipFrame = false;

  try
  {
    ALIB_ASSERT( (pBI != 0) && (pBytes != 0) );
    ALIB_ASSERT( m_bGood );
    m_bGood = false;
    ALIB_ASSERT( m_bInitialized && m_bRunning );
    ALIB_ASSERT( m_startTime <= timeStamp );

    m_currentTime = csalgo::MilliToSeconds( m_startTime, timeStamp );

    // Acquire the current frame and measure some of its characteristics. Then do useful job.
    if (m_imgAcquirer.Acquire( &(pBI->bmiHeader), pImage, m_parameters.s_acquirer ))
    {
      ALIB_ASSERT( DoPreprocessing() );

      // Have enough frames passed?
      if ((m_frameNo > 25) &&
          (m_frameNo > m_parameters.s_algorithm.p_frameChainSize()) &&
          (m_currentTime > m_parameters.s_statistics.p_startDelay()))
      {
        ALIB_ASSERT( DoProcessing() );
      }
      else bSkipFrame = true;

      ALIB_ASSERT( DoPostprocessing() );
      ++m_frameNo;
    }
    else
    {
      bDropFrame = true;
      m_nDropFrame = m_imgAcquirer.DroppedFrameNumber();
    }

    // Write output video file if necessary.
    if (!bDropFrame && (m_pLearnAlgo.get() != 0))
      m_pLearnAlgo->ProcessImage( pBI, (const char*)pBytes, (float)(timeStamp * 0.001) );

    m_previousTime = m_currentTime;
    m_bGood = true;
    retValue = (bDropFrame ? csalgo::HRESULT_DROP_FRAME :
               (bSkipFrame ? csalgo::HRESULT_SKIP_FRAME : S_OK));
  }
  catch (std::runtime_error & e) { PrintException( e ); }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Остановка работы алгоритма.

  \param  stopTime  the stop time of surveillance process in milliseconds.
  \return           Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CameraAnalyzer::Stop( IN __int64 stopTime )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( m_bGood && m_bRunning );
    ALIB_ASSERT( stopTime >= m_startTime );

    m_currentTime = csalgo::MilliToSeconds( m_startTime, stopTime );
    m_bRunning = false;
    m_bInitialized = false;
    retValue = S_OK;

    if (m_pLearnAlgo.get() != 0)
      m_pLearnAlgo->Stop( (float)(stopTime * 0.001), true );
  }
  catch (std::runtime_error & e) { PrintException( e ); }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Освобождение реализации интерфейса.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CameraAnalyzer::Release()
{
  if (m_pLearnAlgo.get() != 0)
    m_pLearnAlgo.reset();
  delete this;
  return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function updates the list of static intervals of a chair according to its state.

   \param  chair  the current chair of interest. */
//-------------------------------------------------------------------------------------------------
void CameraAnalyzer::ProcessLastStaticInterval( Chair & chair )
{
  const csalgo::AlgorithmParams & params = m_parameters.s_algorithm;

  ALIB_ASSERT( chair.bOccupied || chair.bHardOccupied );

  // Launch new interval, if the interval list is empty or the previous interval has been completed.
  if (chair.staticIntervals.empty() || (chair.staticIntervals.back()).bReady)
  {
    chair.staticIntervals.push_back( StaticInterval() );
    StaticInterval & statInterval = chair.staticIntervals.back();
    statInterval.startTime = m_previousTime;
    statInterval.stopTime = m_currentTime;
  }

  StaticInterval & statInterval = chair.staticIntervals.back();
  float            exponent = exp( -(m_currentTime-m_previousTime)/params.p_statDecayTau() );

  // Charge the accumulator according to the current state of correlation.
  if (chair.bOccupied)
    statInterval.charge = csalgo::MAX_CORR_DIFF - (csalgo::MAX_CORR_DIFF - statInterval.charge) * exponent;
  else // if (chair.bHardOccupied)
    statInterval.charge = statInterval.charge * exponent;

  // Memorize the latest moment of occupation or hard-occupation.
  statInterval.stopTime = m_currentTime;

  // Is accumulator charged enough?
  chair.bHardOccupied = (statInterval.charge > params.p_statDiffThreshold());

  // Does the static correlation finally fall below threshold?
  if (!(chair.bOccupied) && !(chair.bHardOccupied))
  {
    if (statInterval.duration() > params.p_minStatInterval())
    {
      statInterval.bReady = true;   // complete the interval

      // Complete the last dynamic interval of the current static one.
      if (!(statInterval.dynamicIntervals.empty()))
      {
        Interval & dynaLast = statInterval.dynamicIntervals.back();
        if (!(dynaLast.bReady))
        {
          if (dynaLast.duration() > params.p_minDynaInterval())
          {
            dynaLast.bReady = true;
          }
          else statInterval.dynamicIntervals.pop_back();   // remove short dynamic interval
        }
      }
    }
    else chair.staticIntervals.pop_back();   // remove short static interval
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function updates the list of dynamic intervals of a chair according to its state.

  \param  chair  the current chair of interest. */
//-------------------------------------------------------------------------------------------------
void CameraAnalyzer::ProcessLastDynamicInterval( Chair & chair )
{
  const csalgo::AlgorithmParams & params = m_parameters.s_algorithm;

  ALIB_ASSERT( chair.bMovement || chair.bHardMovement );

  // Is there any static interval that has not been completed yet?
  if (chair.staticIntervals.empty() || (chair.staticIntervals.back()).bReady)
    return;

  IntervalLst & dynaIntLst = (chair.staticIntervals.back()).dynamicIntervals;

  // Launch new interval, if the interval list is empty or the previous interval has been completed.
  if (dynaIntLst.empty() || (dynaIntLst.back()).bReady)
  {
    dynaIntLst.push_back( Interval() );
    Interval & dynaInterval = dynaIntLst.back();
    dynaInterval.startTime = m_previousTime;
    dynaInterval.stopTime = m_currentTime;
  }

  Interval & dynaInterval = dynaIntLst.back();
  float      exponent = exp( -(m_currentTime-m_previousTime)/params.p_dynaDecayTau() );

  // Charge the accumulator according to the current state of correlation.
  if (chair.bMovement)
    dynaInterval.charge = csalgo::MAX_CORR_DIFF - (csalgo::MAX_CORR_DIFF - dynaInterval.charge) * exponent;
  else // if (chair.bHardMovement)
    dynaInterval.charge = dynaInterval.charge * exponent;

  // Memorize the latest moment of activity or hard-activity.
  dynaInterval.stopTime = m_currentTime;

  // Is accumulator charged enough?
  chair.bHardMovement = (dynaInterval.charge > params.p_dynaDiffThreshold());

  // Does the dynamic correlation finally fall below threshold?
  if (!(chair.bMovement) && !(chair.bHardMovement))
  {
    if (dynaInterval.duration() > params.p_minDynaInterval())
    {
      dynaInterval.bReady = true;   // complete the interval
    }
    else dynaIntLst.pop_back();   // remove short interval
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function prints exception message.

  \param  e  pointer to the exception object or 0 (unsupported exception). */
//-------------------------------------------------------------------------------------------------
void CameraAnalyzer::PrintException( const std::runtime_error & e )
{
  USES_CONVERSION;
  CameraAnalyzer * pThis = const_cast<CameraAnalyzer*>( this );

  (pThis->m_lastError = _T("Camera analyzer failed.\n")) += (LPCWSTR)(CString( e.what() ));

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
HRESULT CameraAnalyzer::SetData( IN csinterface::IDataType * pData )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( m_bGood && m_bInitialized && (pData != 0) );
    switch (pData->type())
    {
      case csinterface::DATAID_LOG_FILE_NAME: // store log file name
      {
        csinterface::TLogFileName * pInfo = dynamic_cast<csinterface::TLogFileName*>( pData );
        ALIB_ASSERT( pInfo != 0 );
        m_logFileName = pInfo->data;
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

  \param  pData  pointer to the output (possibly resizable) data storage, say STL container.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CameraAnalyzer::GetData( OUT csinterface::IDataType * pData ) const
{
  CameraAnalyzer * pThis = const_cast<CameraAnalyzer*>( this );
  CSingleLock      lock( &(pThis->m_dataLocker), TRUE );
  HRESULT          retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( m_bGood && m_bInitialized && (pData != 0) );
    switch (pData->type())
    {
      case csinterface::DATAID_CAMERA_STATISTICS: // transfer data to hall analyzer
      {
        csalgo::TCameraStatistics * pInfo = dynamic_cast<csalgo::TCameraStatistics*>( pData );
        __int8                    * p = 0;

        ALIB_ASSERT( pInfo != 0 );
        p = csutility::SerializeComplexContainer( p, pThis->m_chairs, csutility::SERIALIZE_SIZE );
        pInfo->data.resize( std::distance( (__int8*)0, p ) );
        p = &(*(pInfo->data.begin()));
        p = csutility::SerializeComplexContainer( p, pThis->m_chairs, csutility::SERIALIZE_SAVE );
        ALIB_ASSERT( p == &(*(pInfo->data.end())) );
        retValue = S_OK;
      }
      break;

      case csinterface::DATAID_CHAIR_LEARN_ARRAY: // transfer data needed for learning procedure
      {
        csalgo::TChairLearnArr * pInfo = dynamic_cast<csalgo::TChairLearnArr*>( pData );
        ALIB_ASSERT( pInfo != 0 );
        pInfo->data.resize( m_chairs.size() );
        for (int i = 0, n = (int)(pInfo->data.size()); i < n; i++)
        {
          ChairLearn  & dst = pInfo->data[i];
          const Chair & src = m_chairs[i];

          dst.cameraNo = src.cameraNo;
          dst.index    = src.index;
          dst.statNCC  = src.statNCC;
          dst.dynaNCC  = src.dynaNCC;
        }
        retValue = S_OK;
      }
      break;

      case csinterface::DATAID_FRAME_COUNTERS: // transfer some common state data
      {
        csalgo::TFrameCounters * pInfo = dynamic_cast<csalgo::TFrameCounters*>( pData );
        ALIB_ASSERT( pInfo != 0 );
        pInfo->data.first = m_frameNo;
        pInfo->data.second = m_nDropFrame;
        retValue = S_OK;
      }
      break;

      case csinterface::DATAID_DEMO_IMAGE: // fill out the demo image
      {
        csalgo::TDemoImage * pInfo = dynamic_cast<csalgo::TDemoImage*>( pData );
        ALIB_ASSERT( pInfo != 0 );
        // MyFloatImageToColorImage( m_currentFrame, pInfo->data.second );
        csutility::CorrectBrightness( &m_currentFrame, 70, &(pInfo->data.second),
                                      const_cast<UByteArr*>( &m_tempBuffer ) );
        pInfo->data.first = m_cameraNo;
        if (m_parameters.s_demo.p_bCamAnalyzerShowChairState())
          csalgo::DrawResult( m_chairs, m_parameters.s_demo, *pInfo, -1 );
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

      default: retValue = csalgo::HRESULT_UNKNOWN_DATA;
    }
  }
  catch (std::runtime_error & e) { pThis->PrintException( e ); }
  return retValue;
}


