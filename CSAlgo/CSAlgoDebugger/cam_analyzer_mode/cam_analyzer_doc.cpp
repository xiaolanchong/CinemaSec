/****************************************************************************
  cam_analyzer_doc.cpp
  ---------------------
  begin     : Aug 2004
  modified  : 24 Aug 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "../algo_debugger.h"
#include "../utility/document_iterator.h"
#include "cam_analyzer_doc.h"
#include "cam_analyzer_view.h"
#include "cam_analyzer_frm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCamAnalyzerDoc, CDocument)

BEGIN_MESSAGE_MAP(CCamAnalyzerDoc, CDocument)
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CCamAnalyzerDoc::CCamAnalyzerDoc()
{
  m_bCamAnReady = false;
  m_bCamAnRunning = false;
  m_pGrabber = 0;
  m_cameraNo = -1;
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CCamAnalyzerDoc::~CCamAnalyzerDoc()
{
}


//-------------------------------------------------------------------------------------------------
// CCamAnalyzerDoc diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CCamAnalyzerDoc::AssertValid() const
{
  CDocument::AssertValid();
}
void CCamAnalyzerDoc::Dump( CDumpContext & dc ) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework to delete the document's data
           without destroying the CDocument object itself. */
//-------------------------------------------------------------------------------------------------
void CCamAnalyzerDoc::DeleteContents()
{
  StopCameraAnalyzer();

  // Clear global variables if no more than one document was opened.
  if (GetDocTemplate() != 0)
  {
    int count = 0;
    POSITION pos = (GetDocTemplate())->GetFirstDocPosition();
    while ((pos != 0) && ((GetDocTemplate())->GetNextDoc( pos ) != 0)) ++count;
    if (count <= 1)
      theApp.Clear( false );
  }

  m_bEmptyDoc = true;
  m_bCamAnReady = false;
  m_bCamAnRunning = false;
  m_prevTime = 0;
  m_prevFrameNo = 0;
  m_fps = 0.0f;
  m_videoFormat = MY_BI_Y800;
  m_city_address_hall_camera.erase();
  m_dataPath.erase();

  m_layout.clear();
  m_pGrabber = 0;
  m_grabberType = GRABBER_UNKNOWN;
  m_pCamAnalyzer.reset();
  m_chairs.clear();
  m_source.erase();
  m_cameraNo = -1;
  m_statusText.erase();
  m_frmCount.data.first = (m_frmCount.data.second = -1);
  m_demoImg.data.first = -1;
  m_demoImg.data.second.clear();
  m_background.clear();

  for (POSITION pos = GetFirstViewPosition(); pos != 0;)
  {
    CWnd * pView = GetNextView( pos );
    if ((pView != 0) && pView->IsKindOf( RUNTIME_CLASS( CCamAnalyzerView ) ))
    {
      CWnd * pFrame = pView->GetParentFrame();
      if (pFrame != 0)
      {
        pFrame->PostMessage( MYMESSAGE_UPDATE_STATUSBAR_TEXT, 0, 0 );
        pFrame->PostMessage( MYMESSAGE_UPDATE_STATE_INDICATORS, 0, 0 );
      }
      break;
    }
  }

  CDocument::DeleteContents();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes the computational process.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
bool CCamAnalyzerDoc::InitializeCameraAnalyzer()
{
  bool retValue = false;

  try
  {
    ALIB_ASSERT( !IsRunning() );
    StopCameraAnalyzer();
    ALIB_ASSERT( theApp.AreDataReady() );

    m_bCamAnReady = false;
    m_bCamAnRunning = false;
    ALIB_ASSERT((m_cameraNo >= 0) && !(m_source.empty()) && !(m_background.empty()));
    _tcslwr( const_cast<wchar_t*>( m_source.c_str() ) );

    // Create frame grabber.
    ALIB_ASSERT( CreateFrameGrabber() );

    // Create camera analyzer.
    {
      Int8Arr           parameters;
      ICameraAnalyzer * pca = 0;

      ALIB_ASSERT( m_pCamAnalyzer.null() );
      HRESULT_ASSERT( theApp.m_GetParameters( &parameters, &(theApp.m_output) ) );
      HRESULT_ASSERT( theApp.m_CreateCameraAnalyzer( &pca, &parameters, &m_chairs,
                                                     &m_background, m_cameraNo, &(theApp.m_output) ) );
      m_pCamAnalyzer.reset( pca );

      if (theApp.m_appParams.s_app.p_bLogCamAnalyzer())
      {
        wchar_t                   text[64];
        csinterface::TLogFileName log;

        swprintf( text, L"Camera%02d.log", m_cameraNo );
        (log.data = theApp.GetAppPath()) += text;
        pca->SetData( &log );
      }
    }

    m_bCamAnReady = true;
    m_bCamAnRunning = false;
    m_frmCount = FrameCounters();
    m_statusText.erase();
    retValue = true;
  }
  catch (std::runtime_error & e)
  {
    PrintException( &e );
  }
  
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function launches the computational process.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
bool CCamAnalyzerDoc::StartCameraAnalyzer()
{
  bool retValue = false;

  try
  {
    if (!IsReady() || IsRunning() || (m_pCamAnalyzer.null()) || (m_pGrabber == 0))
      return false;

    HRESULT_ASSERT( m_pCamAnalyzer->Start( theApp.Time() ) );
    ALIB_ASSERT( m_pGrabber->Start( m_source.c_str(), theApp.m_grabberParams ) );

    m_prevTime = theApp.Time();
    m_prevFrameNo = 0;
    m_bCamAnRunning = true;
    retValue = true;
  }
  catch (std::runtime_error & e)
  {
    PrintException( &e );
  }
  
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function terminates the computational process.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
bool CCamAnalyzerDoc::StopCameraAnalyzer()
{
  bool retValue = true;
  try
  {
    if (IsReady() && IsRunning())
    {
      ALIB_ASSERT( !(m_pCamAnalyzer.null()) );
      ALIB_ASSERT( m_pGrabber != 0 );
      HRESULT_ASSERT( m_pCamAnalyzer->Stop( theApp.Time() ) );
      ALIB_ASSERT( m_pGrabber->Stop() );
    }

    m_bCamAnReady = false;
    m_bCamAnRunning = false;
    m_pCamAnalyzer.reset();
    retValue = DestroyFrameGrabber();
  }
  catch (std::runtime_error & e)
  {
    PrintException( &e );
    return false;
  }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework when the document is closed,
           typically as part of the File Close command. */
//-------------------------------------------------------------------------------------------------
void CCamAnalyzerDoc::OnCloseDocument() 
{
  StopCameraAnalyzer();
  CDocument::OnCloseDocument();
  DocumentIterator<CCamAnalyzerDoc> iter( theApp.GetCamAnalyzerDocTempl() );
  if (iter.GetFirst() == 0)
    theApp.Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function is called during the idle cycle to make the next portion of sequential work.

  \return  non-zero if the idle cycle should proceed, otherwise false. */
//-------------------------------------------------------------------------------------------------
bool CCamAnalyzerDoc::OnIdleProcessing()
{
  if (!IsReady() || !IsRunning())
    return true;

  bool retValue = false;

  try
  {
    ALIB_ASSERT( (m_pGrabber != 0) && !(m_pCamAnalyzer.null()) );

    if (m_pGrabber->IsEnd())
      return StopCameraAnalyzer();

    if (m_pGrabber->HasFreshFrame())
    {
      LPCBITMAPINFO pHeader = 0;
      LPCUBYTE      pImage = 0;
      int           size = 0;
      __int64       currTime = theApp.Time();

      ALIB_ASSERT( m_pGrabber->LockLatestFrame( pHeader, pImage, size ) == true );
      if (theApp.m_appParams.s_app.p_bShowDemoImage() && theApp.m_appParams.s_app.p_bUseRawImage())
      {
        csutility::CopyDIBToColorImage( pHeader, pImage, &(m_demoImg.data.second) );
        m_demoImg.data.first = m_cameraNo;
      }
      HRESULT_ASSERT( m_pCamAnalyzer->ProcessImage( pHeader, pImage, currTime ) );
      ALIB_ASSERT( m_pGrabber->UnlockProcessedFrame() == true );

      if (theApp.m_appParams.s_app.p_bShowDemoImage() && !(theApp.m_appParams.s_app.p_bUseRawImage()))
        HRESULT_ASSERT( m_pCamAnalyzer->GetData( &m_demoImg ) );

      // Transfer data to hall analyzer.
      ALIB_ASSERT( theApp.TransferData( m_pCamAnalyzer.get() ) );

      // Update frame counters and FPS value.
      if ((currTime-m_prevTime) >= 1000)
      {
        HRESULT_ASSERT( m_pCamAnalyzer->GetData( &m_frmCount ) );
        m_fps = (float)((1000.0*(m_frmCount.data.first-m_prevFrameNo))/(double)(currTime-m_prevTime));
        m_prevFrameNo = m_frmCount.data.first;
        m_prevTime = currTime;
      }
    }
    retValue = true;
  }
  catch (std::runtime_error & e)
  {
    PrintException( &e );
  }

  // On failure try to terminate the process.
  if (!retValue)
  {
    try
    {
      StopCameraAnalyzer();
    }
    catch (std::runtime_error & e)
    {
      PrintException( &e );
    }
  }

  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function tries to load the first frame of selected video-sequence.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
bool CCamAnalyzerDoc::LoadFirstFrame()
{
  bool                   retValue = true;
  FrameGrabberParameters param;
  LPCBITMAPINFO          pHeader = 0;
  LPCUBYTE               pImage = 0;
  int                    imgSize = 0;

  try
  {
    ALIB_ASSERT( CreateFrameGrabber() );
    ALIB_ASSERT( m_pGrabber->Start( m_source.c_str(), param ) );
    ALIB_ASSERT( m_pGrabber->LockLatestFrame( pHeader, pImage, imgSize ) );

//>>>>>
{
  CImage tmp;
  ALIB_ASSERT( tmp.CreateEx( pHeader->bmiHeader.biWidth,
    pHeader->bmiHeader.biHeight,
    pHeader->bmiHeader.biBitCount,
    BI_RGB, //pHeader->bmiHeader.biCompression,
    reinterpret_cast<const DWORD*>( pImage ), 0 ) );
  tmp.Save( L"c:\\tmp\\frame.bmp" );
}
//>>>>>

    csutility::CopyDIBToColorImage( pHeader, pImage, &(m_demoImg.data.second) );
    ALIB_ASSERT( m_pGrabber->UnlockProcessedFrame() );
    ALIB_ASSERT( m_pGrabber->Stop() );
    retValue = DestroyFrameGrabber();
  }
  catch (std::runtime_error & e)
  {
    PrintException( &e, L"Failed to load the first frame" );
    if (m_pGrabber != 0) DestroyFrameGrabber();
    return false;
  }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function parses full-path name of input video-file
           and extracts cinema subdirectory, hall index and camera index.

  \param  fileName  the name of input video-file.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
void CCamAnalyzerDoc::ParseInputFileName( LPCWSTR fileName )
{
  m_source = fileName;
  m_cameraNo = -1;

  size_t first = m_source.find_last_of( L"\\/" );
  first = (first != m_source.npos) ? (first+1) : 0;
  m_dataPath.erase();
  m_dataPath.insert( m_dataPath.begin(), fileName, fileName+first );

  // Parse file's name.
  for (size_t i = first, count = 0; (i < m_source.size()) && (count < 4); i++)
  {
    if (m_source[i] == L'_')
    {
      ++count;
      if (count == 2)
      {
        if (theApp.m_city_address.empty())
        {
          theApp.m_city_address = m_source.substr( first, i-first );
        }
        else ALIB_VERIFY( theApp.m_city_address == m_source.substr( first, i-first ),
                          _T("Please, select video obtained for the same cinema") );
      }
      else if (count == 3)
      {
        if (theApp.m_city_address_hall.empty())
        {
          theApp.m_city_address_hall = m_source.substr( first, i-first );
        }
        else ALIB_VERIFY( theApp.m_city_address_hall == m_source.substr( first, i-first ),
                          _T("Please, select video obtained for the same hall") );
      }
      else if (count == 4)
      {
        m_city_address_hall_camera = m_source.substr( first, i-first );
        size_t pos = m_city_address_hall_camera.size();
        while ((pos > 0) && iswdigit( m_city_address_hall_camera[pos-1] )) --pos;
        ALIB_VERIFY( (pos < m_city_address_hall_camera.size()) &&
                     (swscanf( m_city_address_hall_camera.c_str()+pos, L"%d", &m_cameraNo ) == 1),
                     _T("Failed to extract camera index") );
      }
    }
  }

  // Check uniqueness of a camera.
  CDocTemplate * pTempl = GetDocTemplate();
  if (pTempl != 0)
  {
    for (POSITION pos = pTempl->GetFirstDocPosition(); pos != 0;)
    {
      CCamAnalyzerDoc * pDoc = dynamic_cast<CCamAnalyzerDoc*>( pTempl->GetNextDoc( pos ) );
      if (pDoc != this)
        ALIB_VERIFY( (pDoc != 0) && (pDoc->m_cameraNo != this->m_cameraNo),
                     _T("Please, select video obtained for different cameras") );
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function loades some data that had been off-line computed.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
bool CCamAnalyzerDoc::LoadPrecomputedData()
{
  // Load chairs.
  if (m_chairs.empty())
  {
    ALIB_ASSERT( !(theApp.m_playListItem.chairs.empty()) );

    if (LoadGaugeHallFromFile( theApp.m_playListItem.chairs.c_str(), m_chairs ) != 0)
    {
      alib::CompleteClear( m_chairs );
      theApp.m_output.PrintW( IDebugOutput::mt_error, L"Failed to load chairs" );
      return false;
    }
  }

  // Load background.
  ALIB_ASSERT( !(theApp.m_playListItem.background.empty()) );
  if (!(csutility::LoadFloatImageEx( theApp.m_playListItem.background.c_str(), &m_background, DOWNWARD_AXIS_Y )))
  {
    theApp.m_output.PrintW( IDebugOutput::mt_error, L"Failed to load background" );
    return false;
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework as part of the File New command. */
//-------------------------------------------------------------------------------------------------
BOOL CCamAnalyzerDoc::OnNewDocument()
{
  BOOL retValue = FALSE;

  try
  {
    CWaitCursor wait;
    ALIB_ASSERT( CDocument::OnNewDocument() );
    ParseInputFileName( theApp.m_playListItem.video.c_str() );
    ALIB_ASSERT( LoadPrecomputedData() );
    ALIB_ASSERT( LoadFirstFrame() );
    SetTitle( m_source.c_str() );
    UpdateStatusbar( true, true );
    m_bEmptyDoc = false;
    retValue = TRUE;
  }
  catch (std::runtime_error & e)
  {
    PrintException( &e, L"Failed to create new document" );
    DeleteContents();
  }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework as part of the File Open command. */
//-------------------------------------------------------------------------------------------------
BOOL CCamAnalyzerDoc::OnOpenDocument( LPCWSTR lpszPathName )
{
  BOOL retValue = FALSE;

  try
  {
    CWaitCursor wait;
    DeleteContents();
    ParseInputFileName( lpszPathName );
    (((theApp.m_playListItem.chairs = m_dataPath) += m_city_address_hall_camera) += L'_') += CHAIRS_XML;
    (((theApp.m_playListItem.background = m_dataPath) += m_city_address_hall_camera) += L'_') += BACKGROUND_BMP;
    ALIB_ASSERT( LoadPrecomputedData() );
    ALIB_ASSERT( LoadFirstFrame() );
    SetTitle( m_source.c_str() );
    UpdateStatusbar( true, true );
    m_bEmptyDoc = false;
    retValue = TRUE;
  }
  catch (std::runtime_error & e)
  {
    PrintException( &e, L"Failed to open document" );
    DeleteContents();
  }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework before a frame window displaying the document is closed. */
//-------------------------------------------------------------------------------------------------
BOOL CCamAnalyzerDoc::CanCloseFrame( CFrameWnd * pFrame )
{
  if (IsRunning() || theApp.IsRunning())
  {
    m_statusText = L"The document cannot be closed while running";
    UpdateStatusbar( true, true );
    return FALSE;
  }
  return CDocument::CanCloseFrame( pFrame );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function prints exception message.

  \param  e          pointer to the exception object that contains error information or 0.
  \param  szMessage  pointer to the additional text message or 0 */
//-------------------------------------------------------------------------------------------------
void CCamAnalyzerDoc::PrintException( const std::runtime_error * e, LPCWSTR szMessage )
{
  CString msg( L"Error in the camera analyzer's document\n" );
  wchar_t txt[64];

  if (e != 0)
    (msg += CString( e->what() )) += L"\n";

  if (szMessage != 0)
    (msg += szMessage) += L"\n";

  if (!(m_source.empty()))
    (msg += m_source.c_str()) += L"\n";

  if (m_cameraNo >= 0)
    (msg += L"camera index: ") += _itot( m_cameraNo, txt, 10 );

  theApp.m_output.PrintW( IDebugOutput::mt_error, (LPCWSTR)msg );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function updates status bar and window title.

  \param  bStatusText  non-zero if status bar text should be updated.
  \param  bIndicators  non-zero if status bar indocators should be updated. */
//-------------------------------------------------------------------------------------------------
void CCamAnalyzerDoc::UpdateStatusbar( bool bStatusText, bool bIndicators )
{
  for (POSITION pos = GetFirstViewPosition(); pos != 0;)
  {
    CWnd * pView = GetNextView( pos );
    if ((pView != 0) && pView->IsKindOf( RUNTIME_CLASS( CCamAnalyzerView ) ))
    {
      CWnd * pFrame = pView->GetParentFrame();
      if (pFrame != 0)
      {
        if (bStatusText) pFrame->PostMessage( MYMESSAGE_UPDATE_STATUSBAR_TEXT, 0, 0 );
        if (bIndicators) pFrame->PostMessage( MYMESSAGE_UPDATE_STATE_INDICATORS, 0, 0 );
      }
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function creates frame grabber.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
bool CCamAnalyzerDoc::CreateFrameGrabber()
{
  try
  {
    ALIB_ASSERT( (m_pGrabber == 0) || DestroyFrameGrabber() );
    m_pGrabber = 0;
    m_grabberType = GRABBER_UNKNOWN;
    if (m_source.find( L".avd" ) != m_source.npos)
    {
      m_pGrabber = csutility::CreateWrappedAVideoReader( m_source.c_str(), &(theApp.m_output) );
      if (m_pGrabber != 0) m_grabberType = GRABBER_AVD;
      ALIB_ASSERT( m_pGrabber != 0 );
    }
    else if (m_source.find( L".avi" ) != m_source.npos)
    {
      if (FAILED(CreateVideoFileGrabber( &m_pGrabber, m_videoFormat, theApp.m_appParams.s_app.p_bPerFrame(), false )))
      {
        m_pGrabber = 0;
        theApp.m_output.PrintW( IDebugOutput::mt_error, L"Failed to create AVI frame grabber" );
        ALIB_ASSERT(0);
      }
      else m_grabberType = GRABBER_AVI;
    }
    else
    {
      theApp.m_output.PrintW( IDebugOutput::mt_error, L"Unsupported video format" );
      ALIB_ASSERT(0);
    }
  }
  catch (std::runtime_error & e)
  {
    PrintException( &e );
    return false;
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function destroys frame grabber.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
bool CCamAnalyzerDoc::DestroyFrameGrabber()
{
  bool retValue = true;
  try
  {
    if (m_grabberType == GRABBER_AVD)
    {
      delete m_pGrabber;
    }
    else if (m_grabberType == GRABBER_AVI)
    {
      if ((m_pGrabber != 0) && FAILED(DeleteVideoFileGrabber( m_pGrabber )))
      {
        PrintException( 0, L"Failed to delete AVI frame grabber" );
        retValue = false;
      }
    }
    m_pGrabber = 0;
    m_grabberType = GRABBER_UNKNOWN;
  }
  catch (std::runtime_error & e)
  {
    PrintException( &e );
    return false;
  }
  return retValue;
}

