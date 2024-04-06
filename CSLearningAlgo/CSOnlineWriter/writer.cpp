/****************************************************************************
  writer.cpp
  ---------------------
  begin     : 30 Aug 2005
  modified  : 31 Aug 2005
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "dll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float WRITE_INTERVAL = 4.0f;

//=================================================================================================
/** \struct OnlineWriter.
    \brief  OnlineWriter. */
//=================================================================================================
struct OnlineWriter : public csinterface::ILearningAlgorithm
{
  Arr2ub                    m_image;          //!< temporal image
  avideolib::IVideoWriter * m_pWriter;        //!< pointer to the writer
  IDebugOutput            * m_pOutput;        //!< pointer to the debug output object
  CCriticalSection          m_dataLocker;     //!< object locks/unlocks the data being transferred between threads
  __int32                   m_cameraNo;       //!< index of a camera
  __int32                   m_scenario;       //!< identifier of a working scenario
  csalgo2::ImageAcquirer    m_acquirer;       //!< advanced image acquirer
  bool                      m_bAcquirerReady; //!< state of image acquirer
  bool                      m_bGood;          //!< state of this object
  BaseChairExArr            m_chairs;         //!< chairs covered by specified camera
  float                     m_prevTime;       //!< previous time stamp


//-------------------------------------------------------------------------------------------------
/** \brief Function prints error message.

  \param  msg  message to print. */
//-------------------------------------------------------------------------------------------------
void PrintError( LPCWSTR msg )
{
  if (m_pOutput != 0)
  {
    StdStr text( L"OnlineWriter:\n" );
    if (msg != 0) text += msg; else text += L"General error";
    m_pOutput->PrintW( IDebugOutput::mt_error, text.c_str() );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function launches computational process.

  \param  startTime  the start time of surveillance process in seconds.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT Start( IN float startTime )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  try
  {
    startTime;
    m_bGood = false;

    delete m_pWriter;
    m_pWriter = 0;
    m_image.clear();

    if ((m_cameraNo < 0) || (m_scenario < 0))
    {
      PrintError( L"Camera index or working scenario was not specified" );
      return E_FAIL;
    }

    CTime   curTime = CTime::GetCurrentTime();
    int     year = curTime.GetYear();
    int     month = curTime.GetMonth();
    int     day = curTime.GetDay();
    int     hour = curTime.GetHour();
    wchar_t name[_MAX_PATH+1], text[128];

    // Create necessary paths.
    swprintf( text, L"cam%02d\\", m_cameraNo );
    memset( name, 0, sizeof(name) );
    if (!PathAppend( name, theDll.m_dllName.c_str() )                             ||
        !PathRemoveFileSpec( name )                                               ||
        !PathAppend( name, TESTVIDEO_DIR )                                        ||
        !(CreateDirectory( name, 0 ) || (GetLastError() == ERROR_ALREADY_EXISTS)) ||
        !PathAppend( name, text )                                                 ||
        !(CreateDirectory( name, 0 ) || (GetLastError() == ERROR_ALREADY_EXISTS)))
    {
      PrintError( L"Failed to create output folders" );
      return E_FAIL;
    }

    // Create full-path name.
    swprintf( text, L"cam%02d_%02d%s%02d_%02d_video.avd", m_cameraNo, day, MONTH[month], (year%100), hour );
    if (!PathAppend( name, text ))
    {
      PrintError( L"Wrong file name" );
      return E_FAIL;
    }

    // Create video writer.
    m_pWriter = avideolib::CreateAVideoWriter( name, m_pOutput, false, INVERT_AXIS_Y );
    if (m_pWriter == 0)
    {
      PrintError( L"Failed to create video writer" );
      return E_FAIL;
    }

    // Load chairs.
    m_chairs.clear();
    swprintf( text, L"*_cam%02d_chairs.xml", m_cameraNo );
    memset( name, 0, sizeof(name) );
    if (PathAppend( name, theDll.m_dllName.c_str() ) &&
        PathRemoveFileSpec( name )                   &&
        PathAppend( name, TESTVIDEO_DIR )            &&
        PathAppend( name, text ))
    {
      CFileFind ff;
      if (ff.FindFile( name ))
      {
        BaseChairArr baseChairs;
        ff.FindNextFile();
        if (LoadGaugeHallFromFile( (LPCWSTR)(ff.GetFilePath()), baseChairs ) == 0)
        {
          if (csalgocommon::ExtractChairSubSet( baseChairs, m_cameraNo, m_chairs ) )
            csalgocommon::InitializeChairs( m_chairs, 0, m_cameraNo, true );
        }
      }
    }
    if (m_chairs.empty())
    {
      PrintError( L"Failed to load or initialize chairs" );
      return E_FAIL;
    }

    m_bAcquirerReady = false;
    m_bGood = true;
    m_prevTime = startTime;
  }
  catch (alib::GeneralError &)
  {
    m_bGood = false;
    return E_FAIL;
  }
  return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function receives a successive image and processes it.

  \param  pBI        pointer to the image's header.
  \param  pImage     pointer to the image's beginning.
  \param  timeStamp  the current time in seconds.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI,
                              IN const __int8     * pImage,
                              IN float              timeStamp )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  try
  {
    if (m_bGood && (pBI != 0) && (pImage != 0) && (m_pWriter != 0))
    {
      __int32 milliTime = alib::RoundFloat( (float)(1000.0*timeStamp) );

      if (m_scenario == 0)
      {
        csutility::CopyDIBToGrayImage( pBI, reinterpret_cast<LPCUBYTE>( pImage ), &m_image );
        ALIB_ASSERT( m_pWriter->WriteFrame( &m_image, milliTime ) );
        m_prevTime = timeStamp;
      }
      else if (m_scenario == 1)
      {
        if (!m_bAcquirerReady)
        {
          int controlId = 0;
          csalgo2::ImageAcquireParams params( controlId );
          m_image.resize( pBI->bmiHeader.biWidth, abs( pBI->bmiHeader.biHeight ) );
          m_acquirer.Initialize( m_image.width(), m_image.height(), params );
          m_bAcquirerReady = true;
        }

        if ((timeStamp-m_prevTime) > WRITE_INTERVAL)
        {
          if (m_acquirer.Acquire( pBI, (const ubyte*)pImage, 0 ))
          {
            if ((m_acquirer.GetPassedFrameNumber() - m_acquirer.DroppedFrameNumber()) > 100)
            {
              const Arr2f & frame = m_acquirer.GetSmoothImage();

              for (int i = 0; i < (int)(m_chairs.size()); i++)
              {
                BaseChairEx & ch = m_chairs[i];
                int           k, n;
                float         vmax = 1.0f;

                for (k = 0, n = (int)(ch.points.size()); k < n; k++)
                {
                  const float * f = frame.get_if_in( ch.points[k] );
                  if (f != 0) vmax = std::max( vmax, *f );
                }

                for (k = 0, n = (int)(ch.points.size()); k < n; k++)
                {
                  const float * f = frame.get_if_in( ch.points[k] );
                  if (f != 0) m_image[ ch.points[k] ] = (ubyte)(alib::RoundFloat( ((*f)*UCHAR_MAX)/vmax ));
                }
              }

              ALIB_ASSERT( m_pWriter->WriteFrame( &m_image, milliTime ) );
            }
          }
          m_prevTime = timeStamp;
        }
      }
      else if (m_scenario == 2)
      {
        if ((timeStamp-m_prevTime) > WRITE_INTERVAL)
        {
          csutility::CopyDIBToGrayImage( pBI, reinterpret_cast<LPCUBYTE>( pImage ), &m_image );
          ALIB_ASSERT( m_pWriter->WriteFrame( &m_image, milliTime ) );
          m_prevTime = timeStamp;
        }
      }
      else // if (m_scenario == ...)
      {
        ALIB_ASSERT(0);
      }
    }
  }
  catch (alib::GeneralError &)
  {
    m_bGood = false;
    return E_FAIL;
  }
  return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function finalizes computational process.

  \param  stopTime   the stop time of surveillance process in seconds.
  \param  saveResult true if algorithm's result(s) should be saved.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT Stop( IN float stopTime, IN bool saveResult )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  stopTime;saveResult;
  delete m_pWriter;
  m_pWriter = 0;
  return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function releases the implementation of this interface.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT Release()
{
  CSingleLock lock( &m_dataLocker, TRUE );
  delete m_pWriter;
  m_pWriter = 0;
  return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
OnlineWriter()
{
  m_image;
  m_pWriter = 0;
  m_pOutput = 0;
  m_dataLocker;
  m_cameraNo = -1;
  m_scenario = -1;
  m_acquirer;
  m_bAcquirerReady = false;
  m_bGood = false;
  m_chairs.clear();
  m_prevTime = 0.0f;
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
~OnlineWriter()
{
  if (m_pWriter != 0)
  {
    CSingleLock lock( &m_dataLocker, TRUE );
    delete m_pWriter;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function uploads specified data to this object.

  \param  pData  pointer to the input data.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT SetData( IN const csinterface::IDataType * pData )
{
  CSingleLock lock( &m_dataLocker, TRUE );
  if (pData == 0)
  {
    return E_FAIL;
  }
  else if (pData->type() == csinterface::DATAID_CAMERA_NO)
  {
    const csinterface::TCameraNo * pInfo;
    pInfo = dynamic_cast<const csinterface::TCameraNo*>( pData );
    if (pInfo == 0) return E_FAIL;
    m_cameraNo = pInfo->data;
    return S_OK;
  }
  else if (pData->type() == csinterface::DATAID_WORKING_SCENARIO)
  {
    const csinterface::TWorkingScenario * pInfo;
    pInfo = dynamic_cast<const csinterface::TWorkingScenario*>( pData );
    if (pInfo == 0) return E_FAIL;
    m_scenario = pInfo->data;
    return S_OK;
  }
  return E_FAIL;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function unloads specified data from this object.

  \param  pData  pointer to the output (possibly resizable) data storage, say STL container.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const
{
  CSingleLock lock( &((const_cast<OnlineWriter*>(this))->m_dataLocker), TRUE );
  pData;
  return E_FAIL;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function sends request to the external application and receives filled up data storage.

  \param  pData  pointer to a data storage to be filled up.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT QueryData( IN OUT csinterface::IDataType * pData ) const
{
  CSingleLock lock( &((const_cast<OnlineWriter*>(this))->m_dataLocker), TRUE );
  pData;
  return E_FAIL;
}

};

//-------------------------------------------------------------------------------------------------
/** \brief Function creates the instance of any learning algorithm.

  \param  pInfo      pointer to the external object that could supply additional information.
  \param  pDebugOut  pointer to the object that prints debug information.
  \param  ppAlgo     address of pointer that will point to the instance of created object.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
// TODO: could be collisions if two or more instances of learning algorithms were loaded:
HRESULT CreateLearningAlgorithmInstance( IN  csinterface::IInformationQuery   * pInfo,
                                         IN  IDebugOutput                     * pDebug,
                                         OUT csinterface::ILearningAlgorithm ** ppAlgo )
{
  pInfo;
  if (ppAlgo == 0)
    return E_FAIL;
  OnlineWriter * p = new OnlineWriter();
  (*ppAlgo) = p;
  if (p != 0) p->m_pOutput = pDebug;
  return (p != 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns the name of learning algorithm implemented in a DLL.

  \return  the name of learning algorithm. */
//-------------------------------------------------------------------------------------------------
LPCWSTR GetLearningAlgorithmName()
{
  return L"Online AVD video writer";
}

