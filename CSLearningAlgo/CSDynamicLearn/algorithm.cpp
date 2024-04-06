/****************************************************************************
  algorithm.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "../../CSAlgo/algorithm/utility.h"
#include "algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IGNORE_TRY_CATCH 1

//-------------------------------------------------------------------------------------------------
/** \brief Function creates the instance of any learning algorithm.

  \param  pInfo      pointer to the external object that could supply additional information.
  \param  pDebugOut  pointer to the object that prints debug information.
  \param  ppAlgo     address of pointer that will point to the instance of created object.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CreateLearningAlgorithmInstance( IN  csinterface::IInformationQuery   * pInfo,
                                         IN  IDebugOutput                     * pDebug,
                                         OUT csinterface::ILearningAlgorithm ** ppAlgo )
{
  if (ppAlgo != 0)
  {
    (*ppAlgo) = 0;
    DynamicLearningAlgo * p = new DynamicLearningAlgo();
    if ((p != 0) && (pInfo != 0))
    {
      (*ppAlgo) = p;
      p->m_pInfo = pInfo;
      p->m_pOutput = pDebug;
      return S_OK;
    }
  }
  return E_FAIL;
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
DynamicLearningAlgo::DynamicLearningAlgo() : m_parameters( FIRST_CONTROL_IDENTIFIER )
{
  Clear();
  m_pInfo = 0;
  m_pOutput = 0;
  m_parameters.set_default();
  m_lastError.reserve( 80 );
  m_resultPath.erase();
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
DynamicLearningAlgo::~DynamicLearningAlgo()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void DynamicLearningAlgo::Clear()
{
  m_bGood = false;
  m_bRunning = false;

  m_frameNo = 0;
  m_nDropFrame = 0;

  m_startTime = 0;
  m_previousTime = 0;
  m_currentTime = 0;

  m_dataLocker;
  m_parameters;
  m_chairs.clear();
  m_imgAcquirer.Clear();
  alib::CompleteClear( m_images );
  alib::CompleteClear( m_thresholds );

  m_resultPath;

  m_pInfo;
  m_pOutput;
  m_lastError.erase();
  m_logFileName.erase();
  if (m_log.is_open()) m_log.close();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function launches computational process.

  \param  startTime  the start time of surveillance process in seconds.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT DynamicLearningAlgo::Start( IN float startTime )
{
  CWaitCursor wait;
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;

#ifndef IGNORE_TRY_CATCH
  try
#endif // IGNORE_TRY_CATCH
  {
    Clear();
    ASSERT_ALIB( m_pInfo != 0 );

    // Get chairs of empty hall and initialize them.
    {
      csinterface::TEmptyHallArr emptyHallChairs;
      HRESULT_ASSERT( m_pInfo->GetData( &emptyHallChairs ) );
      ASSERT_ALIB( csalgo::ExtractChairSubSet( emptyHallChairs.data, -1, m_chairs ) );
      csalgo::InitializeChairs( m_chairs, 0, -1, true );
    }

    // Initialize image acquirer.
    ASSERT_ALIB( m_imgAcquirer.Init( 0, 0 ) );

    // Initialize array of image pairs.
    {
      LearnAlgoParams & param = m_parameters.s_learn;

      ASSERT_ALIB( (param.p_fastDecayNumber() > 1) && (param.p_decayRatioNumber() > 1) );
      ASSERT_ALIB( (param.p_lowFastDecay() > 0.001f) && (param.p_highFastDecay() > param.p_lowFastDecay()) );
      ASSERT_ALIB( (param.p_lowDecayRatio() > 1.001f) && (param.p_highDecayRatio() > param.p_lowDecayRatio()) );

      m_images.clear();
      m_images.reserve( param.p_fastDecayNumber() * param.p_decayRatioNumber() );

      for (int f = 0; f < param.p_fastDecayNumber(); f++)
      {
        float fastTau = (float)(param.p_lowFastDecay() +
          (f*(param.p_highFastDecay()-param.p_lowFastDecay()))/(param.p_fastDecayNumber()-1.0));

        for (int r = 0; r < param.p_decayRatioNumber(); r++)
        {
          float ratio = (float)(param.p_lowDecayRatio() +
            (r*(param.p_highDecayRatio()-param.p_lowDecayRatio()))/(param.p_decayRatioNumber()-1.0));

          m_images.push_back( ImagePair() );
          ImagePair & ip = m_images.back();
          ip.slowFrameNoise = 0.0f;
          ip.fastFrameNoise = 0.0f;
          ip.slowDecayTau = ratio * fastTau;
          ip.fastDecayTau = fastTau;
        }
      }
    }

    // Initialize thresholds (or threshold multipliers).
    {
      const float lowThr = m_parameters.s_learn.p_lowDynaThreshold();
      const float highThr = m_parameters.s_learn.p_highDynaThreshold();
      const int   N = m_parameters.s_learn.p_dynaThresholdNumber();

      ASSERT_ALIB( (lowThr > 0.0f) && (highThr > lowThr) && (N > 1) );
      m_thresholds.resize( N );
      for (int t = 0; t < N; t++)
      {
        m_thresholds[t] = lowThr + (float)(((highThr-lowThr)*t)/(N-1.0));
      }
    }

    // Initialize histograms.
    m_histograms.resize( (int)(m_images.size()), (int)(m_thresholds.size()) );
    m_histograms.fast_zero();

    // Check the path for intermediate and resultant files.
    ALIB_VERIFY( m_resultPath.empty() || PathIsDirectory( m_resultPath.c_str() ),
                 "The result path has not been specified or does not exist" );

    // Load previously accumulated histogram or reset them.
    LoadOrSaveHistograms( true );

    m_startTime = startTime;
    m_bGood = true;
    m_bRunning = true;
    retValue = S_OK;
  }
#ifndef IGNORE_TRY_CATCH
  catch (std::runtime_error & e)
  {
    m_bGood = false;
    PrintException( e );
  }
#endif // IGNORE_TRY_CATCH
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function does some preprocessing at the beginning of each iteration. */
//-------------------------------------------------------------------------------------------------
bool DynamicLearningAlgo::DoPreprocessing()
{
  const Frame & newFrame = m_imgAcquirer.GetFrame();

  for (ImagePairArr::iterator it = m_images.begin(); it != m_images.end(); ++it)
  {
    for (int q = 0; q < 2; q++)
    {
      Arr2f & frame = (q == 0) ? (*it).fastFrame      : (*it).slowFrame;
      float & noise = (q == 0) ? (*it).fastFrameNoise : (*it).slowFrameNoise;
      float   tau   = (q == 0) ? (*it).fastDecayTau   : (*it).slowDecayTau;
      float   a = exp( -(m_currentTime - m_previousTime)/tau );
      float   b = (1.0f - a);

      if (frame.empty())
      {
        frame.resize2( newFrame );
      }
      else ASSERT_ALIB( alib::AreDimensionsEqual( newFrame, frame ) );

      // Update current frame.
      for (int i = 0, n = frame.size(); i < n; i++)
      {
        frame[i] = frame[i]*a + newFrame[i]*b;
      }

      // Undate noise deviation.
      if (m_frameNo > 0)
        noise = sqrt( alib::Sqr( noise*a ) + alib::Sqr( m_imgAcquirer.GetNoiseDeviation()*b ) );
      else
        noise = m_imgAcquirer.GetNoiseDeviation();
    }
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function process chairs. */
//-------------------------------------------------------------------------------------------------
bool DynamicLearningAlgo::DoProcessing()
{
  const csalgo::AlgorithmParams & algoParams = m_parameters.s_algorithm;
  float dynaDecay = exp( -(m_currentTime-m_previousTime)/algoParams.p_dynaDecayTau() );

  // Get information about chair states.
  csinterface::TChairContentsMap chairContentsMap;
  HRESULT_ASSERT( m_pInfo->GetData( &chairContentsMap ) );

  // For all images obtaned with different updating parameters ...
  for (int imagePairNo = 0; imagePairNo < (int)(m_images.size()); imagePairNo++)
  {
    ImagePair & imgPair = m_images[imagePairNo];

    // For all chairs that are covered by selected camera ...
    for (int ch = 0; ch < (int)(m_chairs.size()); ch++)
    {
      ChairEx & chair = m_chairs[ch];

      csinterface::TChairContentsMap::data_type::iterator mapIt = chairContentsMap.data.find( chair.index );
      ASSERT_ALIB( mapIt != chairContentsMap.data.end() );

      // Allocate statistics storage per image pair.
      if (chair.statistics.empty())
        chair.statistics.resize( (int)(m_images.size()), (int)(m_thresholds.size()) );

      float corr = Correlate( imgPair.slowFrame, imgPair.fastFrame, chair );
      bool  bStartNewType = true;

      // Has a chair proper type?
      if (IsProperChairType( chair.type ))
      {
        // Do the types coincide? If yes, then proceed the current type interval.
        if (chair.type == mapIt->second)
        {
          for (int thresholdNo = 0; thresholdNo < (int)(m_thresholds.size()); thresholdNo++)
          {
            ChairEx::Statistics & stat = chair.statistics( imagePairNo, thresholdNo );
            float                 threshold = m_thresholds[thresholdNo];

            // Charge the accumulator according to the activity state.
            if (corr > threshold)
              stat.charge = csalgo::MAX_CORR_DIFF - (csalgo::MAX_CORR_DIFF - stat.charge) * dynaDecay;
            else
              stat.charge = stat.charge * dynaDecay;

            // If the state became active, then finalize previous calm interval and start new active one.
            // If the state became inactive, then finalize previous dynamic interval and start new calm one.
            if ((!(stat.activity) && ((stat.charge > threshold) || (corr > threshold)))
                ||
                ((stat.activity && ((stat.charge < threshold) && (corr < threshold)))))
            {
              FinalizeInterval( stat );
            }
          }
          bStartNewType = false; // proceed the current type interval
        }
        else // ... otherwise types are different, then finalize the current type interval.
        {
          // Is type interval long enough?
          if ((m_currentTime - chair.typeStartTime) > m_parameters.s_learn.p_minPeriodOfConstantType())
          {
            for (int thresholdNo = 0; thresholdNo < (int)(m_thresholds.size()); thresholdNo++)
            {
              ChairEx::Statistics & stat = chair.statistics( imagePairNo, thresholdNo );
              FinalizeInterval( stat );
              stat.dynaDuration = (float)(stat.dynaDuration/std::max( stat.dynaPeriodNum, 1 ));
              stat.calmDuration = (float)(stat.calmDuration/std::max( stat.calmPeriodNum, 1 ));
              AddToHistogram( stat, (chair.type == CHAIR_HUMAN), imagePairNo, thresholdNo );
            }
          }
        }
      }

      // Start new type interval if necessary.
      if (bStartNewType)
      {
        bool bProper = IsProperChairType( mapIt->second );
        chair.type = (bProper ? mapIt->second : CHAIR_UNDEFINED);
        chair.typeStartTime = (bProper ? m_currentTime : 0);

        for (int thresholdNo = 0; thresholdNo < (int)(m_thresholds.size()); thresholdNo++)
        {
          ChairEx::Statistics & stat = chair.statistics( imagePairNo, thresholdNo );

          stat.clear();
          if (bProper)
            stat.startTime = m_currentTime;
        }
      }
    }
  }

  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function does some postprocessing after each iteration. */
//-------------------------------------------------------------------------------------------------
bool DynamicLearningAlgo::DoPostprocessing()
{
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function receives a successive image and processes it.

  \param  pBI        pointer to the image's header.
  \param  pImage     pointer to the image's beginning.
  \param  timeStamp  the current time in seconds.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT DynamicLearningAlgo::ProcessImage( IN const BITMAPINFO * pBI,
                                           IN const __int8     * pImage,
                                           IN float              timeStamp )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;
  bool        bDropFrame = false;
  bool        bSkipFrame = false;

#ifndef IGNORE_TRY_CATCH
  try
#endif // IGNORE_TRY_CATCH
  {
    ASSERT_ALIB( (pBI != 0) && (pImage != 0) );
    ASSERT_ALIB( m_bGood );
    m_bGood = false;
    ASSERT_ALIB( m_bRunning );
    ASSERT_ALIB( m_startTime <= (timeStamp+0.001) );

    m_currentTime = timeStamp - m_startTime;

    // Acquire the current frame and measure some of its characteristics. Then do useful job.
    if (m_imgAcquirer.Acquire( &(pBI->bmiHeader), reinterpret_cast<LPCUBYTE>( pImage ), m_parameters.s_acquirer ))
    {
      csutility::CopyDIBToColorImage( pBI, reinterpret_cast<LPCUBYTE>( pImage ), &m_demoImage );

      ASSERT_ALIB( DoPreprocessing() );

      // Have enough frames passed?
      if ((m_frameNo > 25) &&
          (m_frameNo > m_parameters.s_algorithm.p_frameChainSize()) &&
          (m_currentTime > m_parameters.s_statistics.p_startDelay()))
      {
        ASSERT_ALIB( DoProcessing() );
      }
      else bSkipFrame = true;

      ASSERT_ALIB( DoPostprocessing() );
      ++m_frameNo;
    }
    else
    {
      bDropFrame = true;
      m_nDropFrame = m_imgAcquirer.DroppedFrameNumber();
    }

    m_previousTime = m_currentTime;
    m_bGood = true;
    retValue = (bDropFrame ? csalgo::HRESULT_DROP_FRAME :
               (bSkipFrame ? csalgo::HRESULT_SKIP_FRAME : S_OK));
  }
#ifndef IGNORE_TRY_CATCH
  catch (std::runtime_error & e)
  {
    m_bGood = false;
    PrintException( e );
  }
#endif // IGNORE_TRY_CATCH
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function finalizes computational process.

  \param  stopTime  the stop time of surveillance process in seconds.
  \return           Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT DynamicLearningAlgo::Stop( IN float stopTime, IN bool saveResult )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  HRESULT     retValue = E_FAIL;
  ParamSetArr paramSets;

#ifndef IGNORE_TRY_CATCH
  try
#endif // IGNORE_TRY_CATCH
  {
    ASSERT_ALIB( m_bGood && m_bRunning );
    ASSERT_ALIB( stopTime >= m_startTime );

    if (saveResult)
    {
      LoadOrSaveHistograms( false );
      paramSets.reserve( m_images.size() * m_thresholds.size() );

      // For all image pairs ...
      for (int imagePairNo = 0; imagePairNo < (int)(m_images.size()); imagePairNo++)
      {
        // For all thresholds ...
        for (int thresholdNo = 0; thresholdNo < (int)(m_thresholds.size()); thresholdNo++)
        {
          Histogram & h = m_histograms( imagePairNo, thresholdNo );
          double      scale[2][3] = {{0,0,0},{0,0,0}};
          double      err[3] = {0,0,0};

          // Compute normalization scale of each histogram.
          for (int i = 0; i < 2; i++)
          {
            for (int q = 0; q < Histogram::SIZE; q++)
            {
              scale[i][0] += h.intervalNum [i][q];
              scale[i][1] += h.dynaDuration[i][q];
              scale[i][2] += h.calmDuration[i][q];
            }

            scale[i][0] = (scale[i][0] >= 1.0) ? (1.0/scale[i][0]) : 0.0;
            scale[i][1] = (scale[i][1] >= 1.0) ? (1.0/scale[i][1]) : 0.0;
            scale[i][2] = (scale[i][2] >= 1.0) ? (1.0/scale[i][2]) : 0.0;
          }

          // Compute misclassification error for each parameter being tuned.
          for (int q = 0; q < Histogram::SIZE; q++)
          {
            float p[2][3];

            for (int i = 0; i < 2; i++)
            {
              p[i][0] = (float)(h.intervalNum [i][q] * scale[i][0]);
              p[i][1] = (float)(h.dynaDuration[i][q] * scale[i][1]);
              p[i][2] = (float)(h.calmDuration[i][q] * scale[i][2]);
            }

            err[0] += p[0][0] * p[1][0] * alib::Reciprocal( p[0][0] + p[1][0] );
            err[1] += p[0][1] * p[1][1] * alib::Reciprocal( p[0][1] + p[1][1] );
            err[2] += p[0][2] * p[1][2] * alib::Reciprocal( p[0][2] + p[1][2] );
          }

          // Compute the overall misclassification error and store parameter set.
          paramSets.push_back( ParamSet() );
          ParamSet & ps = paramSets.back();
          ps.misclassProb = (float)(err[0] * err[1] * err[2]);
          ps.slowDecayTau = (m_images[imagePairNo]).slowDecayTau;
          ps.fastDecayTau = (m_images[imagePairNo]).fastDecayTau;
          ps.threshold = m_thresholds[thresholdNo];
          ps.imagePairNo = imagePairNo;
          ps.thresholdNo = thresholdNo;
        }
      }

      // Sort parameter sets by the probability of misclassification in ascendant order.
      std::sort( paramSets.begin(), paramSets.end() );

      // Store sorted parameter sets.
      {
        const char   SPACE[] = "   ";
        std::string  fname;
        int          index = 0;
        std::fstream file;
        char         text[64];

        USES_CONVERSION;
        (fname = W2CA( m_resultPath.c_str() )) += "sorted_param_sets.txt";
        file.open( fname.c_str(), std::ios::trunc | std::ios::out );
        ASSERT_ALIB( file.good() );

        for (ParamSetArr::const_iterator it = paramSets.begin(); it != paramSets.end(); ++it)
        {
          sprintf( text, "img%04d_thr%04d.histo", it->imagePairNo, it->thresholdNo );
          file << (index++) << SPACE
               << "pr = "  << it->misclassProb << SPACE
               << "st = "  << it->slowDecayTau << SPACE
               << "ft = "  << it->fastDecayTau << SPACE
               << "th = "  << it->threshold    << SPACE
               << "file: " << ((fname = W2CA( m_resultPath.c_str() )) += text) << std::endl << std::endl;
        }
      }
    }

    m_bRunning = false;
    retValue = S_OK;
  }
#ifndef IGNORE_TRY_CATCH
  catch (std::runtime_error & e)
  {
    m_bGood = false;
    PrintException( e );
  }
#endif // IGNORE_TRY_CATCH
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Освобождение реализации интерфейса.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT DynamicLearningAlgo::Release()
{
  delete this;
  return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function prints exception message.

  \param  e  pointer to the exception object or 0 (unsupported exception). */
//-------------------------------------------------------------------------------------------------
void DynamicLearningAlgo::PrintException( const std::runtime_error & e )
{
  USES_CONVERSION;
  DynamicLearningAlgo * pThis = const_cast<DynamicLearningAlgo*>( this );

  (pThis->m_lastError = _T("Dynamic learning failed.\n")) += (LPCWSTR)(CString( e.what() ));

  if (m_pOutput != 0)
    m_pOutput->PrintW( IDebugOutput::mt_error, m_lastError.c_str() );

  if (m_log.is_open() && m_log.good())
    (pThis->m_log) << std::endl << std::endl << W2CA(m_lastError.c_str()) << std::endl << std::endl;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function uploads specified data to this object.

  \param  pData  pointer to the input data.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT DynamicLearningAlgo::SetData( IN const csinterface::IDataType * pData )
{
  CSingleLock lock( &m_dataLocker, TRUE );

#ifndef IGNORE_TRY_CATCH
  try
#endif // IGNORE_TRY_CATCH
  {
    ASSERT_ALIB( pData != 0 );
    switch (pData->type())
    {
      case csinterface::DATAID_RESULT_FILENAME:
      {
        ASSERT_ALIB( !m_bRunning );
        MY_DATA_CONST_CAST( TResultFilename );
        m_resultPath = pInfo->data;
        csutility::AddTrailingBackslashToPathName( &m_resultPath );
      }
      break;

      case csinterface::DATAID_LOG_FILE_NAME:
      {
        ASSERT_ALIB( !m_bRunning );
        MY_DATA_CONST_CAST( TLogFileName );
        m_logFileName = pInfo->data;
      }
      break;

      case csinterface::DATAID_MSG_SHOW_PARAM_DIALOG:
      {
        ASSERT_ALIB( !m_bRunning );
        (CParameterDialog( m_parameters, 0, 320, 240 )).DoModal();
        break;
      }
      break;

      case csinterface::DATAID_MSG_SAVE_PARAMETERS:
      {
        MY_DATA_CONST_CAST( TMsgSaveParameters )
        USES_CONVERSION;
        std::wfstream file( W2CA( pInfo->data.c_str() ), std::ios_base::out | std::ios_base::trunc );
        ASSERT_ALIB( file.good() && m_parameters.save( file ) && file.good() );
      }
      break;

      case csinterface::DATAID_MSG_LOAD_PARAMETERS:
      {
        ASSERT_ALIB( !m_bRunning );
        MY_DATA_CONST_CAST( TMsgLoadParameters )
        USES_CONVERSION;
        std::wfstream file( W2CA( pInfo->data.c_str() ), std::ios::in );
        ASSERT_ALIB( file.good() && m_parameters.load( file ) && file.good() );
      }
      break;

      case csinterface::DATAID_MSG_SET_DEFAULT_PARAMS:
      {
        ASSERT_ALIB( !m_bRunning );
        m_parameters.set_default();
      }
      break;

      default:
      {
#if 0
        if (m_pOutput != 0)
          m_pOutput->PrintW( csinterface::IDebugOutput::mt_warning, ERROR_UNSUPPORTED_DATA_FORMAT );
#endif
      }
    }
  }
#ifndef IGNORE_TRY_CATCH
  catch (std::runtime_error & e)
  {
    PrintException( e );
    return E_FAIL;
  }
#endif // IGNORE_TRY_CATCH
  return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function unloads specified data from this object.

  \param  pData  pointer to the output (possibly resizable) data storage, say STL container.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT DynamicLearningAlgo::GetData( OUT csinterface::IDataType * pData ) const
{
  DynamicLearningAlgo * pThis = const_cast<DynamicLearningAlgo*>( this );
  CSingleLock           lock( &(pThis->m_dataLocker), TRUE );

#ifndef IGNORE_TRY_CATCH
  try
#endif // IGNORE_TRY_CATCH
  {
    ASSERT_ALIB( pData != 0 );
    switch (pData->type())
    {
      case csinterface::DATAID_QIMAGE:
      {
        MY_DATA_CAST( TQImage );
        pInfo->data = m_demoImage;
      }
      break;

      case csinterface::DATAID_LAST_ERROR:
      {
        MY_DATA_CAST( TLastError );
        pInfo->data = m_lastError;
      }
      break;

      default:
      {
#if 0
        if (m_pOutput != 0)
          m_pOutput->PrintW( csinterface::IDebugOutput::mt_warning, ERROR_UNSUPPORTED_DATA_FORMAT );
#endif
      }
    }
  }
#ifndef IGNORE_TRY_CATCH
  catch (std::runtime_error & e)
  {
    pThis->PrintException( e );
    return E_FAIL;
  }
#endif // IGNORE_TRY_CATCH
  return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function sends request to the external application and receives filled up data storage.

  \param  pData  pointer to a data storage to be filled up.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT DynamicLearningAlgo::QueryData( IN OUT csinterface::IDataType * pData ) const
{
  DynamicLearningAlgo * pThis = const_cast<DynamicLearningAlgo*>( this );
  CSingleLock           lock( &(pThis->m_dataLocker), TRUE );
  HRESULT               retValue = E_FAIL;

  pData;
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes normalized cross-correlation with L2 norm.

  \param  image1  the first image to be correlated.
  \param  image2  the second image to be correlated.
  \param  chair   the chair of interest.
  \return         correlation difference. */
//-------------------------------------------------------------------------------------------------
float DynamicLearningAlgo::Correlate( const Arr2f & image1, const Arr2f & image2, const Chair & chair )
{
  const HScanArr & scans = chair.region();
  double           avr1 = 0.0, dev1 = 0.0, avr2 = 0.0, dev2 = 0.0, cross = 0.0;
  int              N = 0;

  for (HScanArr::const_iterator it = scans.begin(); it != scans.end(); ++it)
  {
    int           x1 = it->x1;
    int           x2 = it->x2;
    const float * row1 = image1.row_begin( it->y );
    const float * row2 = image2.row_begin( it->y );

    N += (x2-x1);
    for (int x = x1; x < x2; x++)
    {
      float i1 = row1[x];   avr1 += i1;   dev1 += i1*i1;
      float i2 = row2[x];   avr2 += i2;   dev2 += i2*i2;   cross += i1*i2;
    }
  }
  ASSERT_ALIB( N > 0 );

  avr1 /= N;
  avr2 /= N;

  dev1 = std::max<double>( sqrt( fabs( (dev1/N) - avr1*avr1 ) ), 1.0f );
  dev2 = std::max<double>( sqrt( fabs( (dev2/N) - avr2*avr2 ) ), 1.0f );

  float diff = (float)(1.0 - ((cross/N)-(avr1*avr2))/(dev1*dev2));
  ASSERT( ALIB_IS_LIMITED( diff, -0.001f, 2.001f ) );
  return alib::Limit( diff, 0.0f, 2.0f );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function .

  \param  stat         .
  \param  bHuman       .
  \param  imagePairNo  .
  \param  thresholdNo  . */
//-------------------------------------------------------------------------------------------------
void DynamicLearningAlgo::AddToHistogram( ChairEx::Statistics & stat,
                                          bool                  bHuman,
                                          int                   imagePairNo,
                                          int                   thresholdNo )
{
  const double SIZE = (double)(Histogram::SIZE);
  Histogram &  h = m_histograms( imagePairNo, thresholdNo );
  int          a = (int)((SIZE * stat.dynaPeriodNum) / (double)(SIZE + stat.dynaPeriodNum) + 0.5);
  int          b = (int)((SIZE * stat.dynaDuration) / (double)((SIZE/5) + stat.dynaDuration) + 0.5);
  int          c = (int)(((SIZE*10) * stat.calmDuration) / (double)((SIZE*10) + stat.calmDuration) + 0.5);
  int          i = (bHuman ? 0 : 1);

  h.intervalNum [i][ ALIB_LIMIT( a, 0, Histogram::SIZE-1 ) ] += 1;
  h.dynaDuration[i][ ALIB_LIMIT( b, 0, Histogram::SIZE-1 ) ] += 1;
  h.calmDuration[i][ ALIB_LIMIT( c, 0, Histogram::SIZE-1 ) ] += 1;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function .

  \param  stat         .
  \param  imagePairNo  .
  \param  thresholdNo  . */
//-------------------------------------------------------------------------------------------------
void DynamicLearningAlgo::LoadOrSaveHistograms( bool bLoad )
{
  std::wstring fname;
  wchar_t      text[64];

#ifndef IGNORE_TRY_CATCH
  try
#endif // IGNORE_TRY_CATCH
  {
    ASSERT_ALIB( SetCurrentDirectory( m_resultPath.c_str() ) );
    if (bLoad && !(CFileFind().FindFile( L"*.histo" )))
    {
      m_histograms.fast_zero(); // nothing to load, start from the beginning
      return;
    }

    USES_CONVERSION;
    for (int imagePairNo = 0; imagePairNo < (int)(m_images.size()); imagePairNo++)
    {
      for (int thresholdNo = 0; thresholdNo < (int)(m_thresholds.size()); thresholdNo++)
      {
        wsprintf( text, L"img%04d_thr%04d.histo", imagePairNo, thresholdNo );
        (fname = m_resultPath) += text;

        int          i, q;
        Histogram &  h = m_histograms( imagePairNo, thresholdNo );
        std::fstream file( W2CA(fname.c_str()), bLoad ? std::ios::in : (std::ios::trunc | std::ios::out) );

        ASSERT_ALIB( file.good() );

        // Print common block of GNUPLOT commands.
        if (bLoad) file.ignore( INT_MAX, '\n' );
        else file << "reset; set data style lines; set size 1.0, 1.0; "
                  << "set origin 0.0, 0.0; set noytics; set multiplot;" << std::endl;

        // Load or save histogram of number of activity intervals.
        if (bLoad) file.ignore( INT_MAX, '\n' );
        else file << "set title 'Number of activity intervals'; "
                  << "set size 0.5, 0.5; set origin 0, 0; plot '-' title 'H' , '-' title 'T'" << std::endl;

        for (i = 0; i < 2; i++)
        {
          for (q = 0; q < Histogram::SIZE; q++)
          {
            if (bLoad) file >> h.intervalNum[i][q];
            else       file << h.intervalNum[i][q] << std::endl;
          }
          if (bLoad) file.ignore( INT_MAX, '\n' ); else file << "e" << std::endl;
        }

        // Load or save histogram of dynamic interval duration.
        if (bLoad) file.ignore( INT_MAX, '\n' );
        else file << "set title 'Dynamic interval duration'; "
                  << "set size 0.5, 0.5; set origin 0, 0.5; plot '-' title 'H' , '-' title 'T'" << std::endl;

        for (i = 0; i < 2; i++)
        {
          for (q = 0; q < Histogram::SIZE; q++)
          {
            if (bLoad) file >> h.dynaDuration[i][q];
            else       file << h.dynaDuration[i][q] << std::endl;
          }
          if (bLoad) file.ignore( INT_MAX, '\n' ); else file << "e" << std::endl;
        }

        // Load or save histogram of calm interval duration.
        if (bLoad) file.ignore( INT_MAX, '\n' );
        else file << "set title 'Calm interval duration'; "
                  << "set size 0.5, 0.5; set origin 0.5, 0.5; plot '-' title 'H' , '-' title 'T'" << std::endl;

        for (i = 0; i < 2; i++)
        {
          for (q = 0; q < Histogram::SIZE; q++)
          {
            if (bLoad) file >> h.calmDuration[i][q];
            else       file << h.calmDuration[i][q] << std::endl;
          }
          if (bLoad) file.ignore( INT_MAX, '\n' ); else file << "e" << std::endl;
        }

        if (!bLoad) file << "unset multiplot; reset;";
        ASSERT_ALIB( file.good() );
      }
    }
  }
#ifndef IGNORE_TRY_CATCH
  catch (std::runtime_error & e)
  {
    e;
    if (bLoad)
    {
      m_histograms.fast_zero();
      if (m_pOutput != 0)
        m_pOutput->PrintW( IDebugOutput::mt_info,
                           L"Failed to load previously accumulated histograms.\n"
                           L"Files are missed, corrupted or inaccessible.\n"
                           L"Program will reset histograms and start from the beginning." );
    }
    else
    {
      if (m_pOutput != 0)
        m_pOutput->PrintW( IDebugOutput::mt_error,
                           L"Failed to save accumulated histograms.\n"
                           L"Histogram set might be corrupted!" );
      ASSERT_ALIB(0);
    }
  }
#endif // IGNORE_TRY_CATCH
}

