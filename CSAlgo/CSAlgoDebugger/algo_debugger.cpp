/****************************************************************************
  algo_debugger.cpp
  ---------------------
  begin     : Aug 2004
  modified  : 24 Aug 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "algo_debugger.h"
#include "about_dlg.h"
#include "mainfrm.h"
#include "cam_analyzer_mode/cam_analyzer_doc.h"
#include "cam_analyzer_mode/cam_analyzer_view.h"
#include "cam_analyzer_mode/cam_analyzer_frm.h"
#include "utility/document_iterator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CAlgoDebuggerApp, CWinApp)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
  // Standard file based document commands
  ON_COMMAND(ID_FILE_NEW, CAlgoDebuggerApp::OnFileNew)
  ON_COMMAND(ID_FILE_OPEN, CAlgoDebuggerApp::OnFileOpen)
//  ON_COMMAND(ID_FILE_SAVE, CWinApp::OnFileSave)
//  ON_COMMAND(ID_FILE_SAVE_AS, CWinApp::OnFileSaveAs)
  ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
END_MESSAGE_MAP()


CAlgoDebuggerApp theApp; // The one and only CAlgoDebuggerApp object

struct InitAviLibrary
{
  InitAviLibrary()  { ::AVIFileInit(); }
  ~InitAviLibrary() { ::AVIFileExit(); }
}
g_initAvi;

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CAlgoDebuggerApp::CAlgoDebuggerApp()
{
  m_chairs.reserve(1<<10);
  m_pCamAnalyzerDocTemplate = 0;
  m_hAlgoLib = 0;
  m_CreateBackgroundAccumulator = 0;
  m_CreateCameraAnalyzer = 0;
  m_CreateHallAnalyzer = 0;
  m_ShowParameterDialog = 0;
  m_SetDefaultParameters = 0;
  m_ShowParameterDialogEx = 0;
  m_SetDefaultParametersEx = 0;
  m_GetParameters = 0;
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CAlgoDebuggerApp::~CAlgoDebuggerApp()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Implements the ID_FILE_NEW command. */
//-------------------------------------------------------------------------------------------------
void CAlgoDebuggerApp::OnFileNew()
{
  static bool bFirst = true;
  if (!bFirst)
    CWinApp::OnFileNew();
  bFirst = false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Implements the ID_FILE_OPEN command. */
//-------------------------------------------------------------------------------------------------
void CAlgoDebuggerApp::OnFileOpen()
{
  if (m_bPlaylist)
  {
    CloseAllDocuments( FALSE );
    Clear();
  }
  CWinApp::OnFileOpen();
  CMainFrame * pWin = dynamic_cast<CMainFrame*>( m_pMainWnd );
  if (pWin != 0)
    pWin->MDITile( MDITILE_HORIZONTAL );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function updates menu item. */
//-------------------------------------------------------------------------------------------------
void CAlgoDebuggerApp::OnUpdateFileNew( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( FALSE );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void CAlgoDebuggerApp::Clear( bool bClearPlaylistItem )
{
  m_pCamAnalyzerDocTemplate;
  m_appPath;
  m_hAlgoLib;
  m_frequency;
  m_clockStart;

  m_bRunning = false;
  m_hallAnalyzerLogFile;
  m_pHallAnalyzer.reset();
  m_chairs.clear();
  m_camStatistics.data.clear();
  m_demoCounter = 0;

  m_city_address.clear();
  m_city_address_hall.clear();
  m_bPlaylist = false;
  if (bClearPlaylistItem) m_playListItem = PlaylistItem();

  m_grabberParams = FrameGrabberParameters();
  m_spectatorNum = 0;
  m_output;
  m_appParams;
  m_appParamFileName;

  m_CreateBackgroundAccumulator;
  m_CreateCameraAnalyzer;
  m_CreateHallAnalyzer;
  m_ShowParameterDialog;
  m_SetDefaultParameters;
  m_ShowParameterDialogEx;
  m_SetDefaultParametersEx;
  m_GetParameters;
}


BOOL CAlgoDebuggerApp::InitInstance()
{
  // InitCommonControls() is required on Windows XP if an application
  // manifest specifies use of ComCtl32.dll version 6 or later to enable
  // visual styles.  Otherwise, any window creation will fail.
  InitCommonControls();

  CWinApp::InitInstance();

  // Standard initialization
  // If you are not using these features and wish to reduce the size
  // of your final executable, you should remove from the following
  // the specific initialization routines you do not need.
  // Change the registry key under which our settings are stored.
  // TODO: You should modify this string to be something appropriate
  // such as the name of your company or organization.
/*
  SetRegistryKey( L"AlgoDebugger, 2004" );
*/

  // Load standard INI file options (including MRU).
  LoadStdProfileSettings(0); // 0 - no recent files available

  // Register the application's document templates. Document templates
  // serve as the connection between documents, frame windows and views.
  CMultiDocTemplate * pDocTemplate = new CMultiDocTemplate( IDR_CamAnalyzerTYPE,
                                                            RUNTIME_CLASS(CCamAnalyzerDoc),
                                                            RUNTIME_CLASS(CCamAnalyzerFrame),
                                                            RUNTIME_CLASS(CCamAnalyzerView));
  if (!pDocTemplate)
    return FALSE;
  AddDocTemplate( pDocTemplate );
  m_pCamAnalyzerDocTemplate = pDocTemplate;

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
  // DO NOT add any more document templates. It is assumed that only one document type exists. //
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

  // create main MDI Frame window.
  CMainFrame * pMainFrame = new CMainFrame;
  if (!pMainFrame || !(pMainFrame->LoadFrame( IDR_MAINFRAME )))
    return FALSE;
  m_pMainWnd = pMainFrame;

  // Call DragAcceptFiles only if there's a suffix. In an MDI app, this should
  // occur immediately after setting m_pMainWnd. Enable drag/drop open.
  m_pMainWnd->DragAcceptFiles();

  // Enable DDE Execute open.
  EnableShellOpen();
  RegisterShellFileTypes( TRUE );

  // Parse command line for standard shell commands, DDE, file open.
  CCommandLineInfo cmdInfo;
  ParseCommandLine( cmdInfo );

  // Dispatch commands specified on the command line.  Will return FALSE if
  // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
  if (!ProcessShellCommand( cmdInfo ))
    return FALSE;

  // The main window has been initialized, so show and update it.
  pMainFrame->ShowWindow( SW_SHOWMAXIMIZED );
  pMainFrame->UpdateWindow();

  // Get full application's path with trailing backslash.
  {
    wchar_t buffer[_MAX_PATH+2];
    if (GetModuleFileName( 0, buffer, _MAX_PATH+1 ) > _MAX_PATH)
      return FALSE;
    if (PathRemoveFileSpec( buffer ))
      PathAddBackslash( buffer );
    m_appPath = buffer;
  }

  // Create necessary file names.
  (m_hallAnalyzerLogFile = m_appPath) += HALL_ANALYZER_LOG_FILE;

  // Load application's parameters, save default settings on failure.
  m_appParams.set_default();
  (m_appParamFileName = m_appPath) += APPLICATION_PARAM_FILE_NAME;
  if (!((CFileFind()).FindFile( m_appParamFileName.c_str() )) ||
      !(csutility::LoadParameters( m_appParamFileName.c_str(), m_appParams, &m_output )))
  {
    m_appParams.set_default();
    csutility::SaveParameters( m_appParamFileName.c_str(), m_appParams, &m_output );
  }

  // Load algorithm library.
  ReloadAlgorithmLibrary();

  // Initialize timer's parameters.
  LARGE_INTEGER countPerSec;
  if (!QueryPerformanceFrequency( &countPerSec ))
    return FALSE;
  m_frequency.QuadPart = countPerSec.QuadPart / 1000;
  if (!QueryPerformanceCounter( &m_clockStart ))
    return FALSE;

  return TRUE;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function return the current time in milliseconds. */
//-------------------------------------------------------------------------------------------------
__int64 CAlgoDebuggerApp::Time()
{
  LARGE_INTEGER time;
  QueryPerformanceCounter( &time );
  return (__int64)((time.QuadPart - m_clockStart.QuadPart) / m_frequency.QuadPart);
}


//-------------------------------------------------------------------------------------------------
// App command to run the dialog.
//-------------------------------------------------------------------------------------------------
void CAlgoDebuggerApp::OnAppAbout()
{
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function performs idle-time processing. */
//-------------------------------------------------------------------------------------------------
BOOL CAlgoDebuggerApp::OnIdle( LONG lCount )
{
  BOOL result = CWinApp::OnIdle( lCount );
  BOOL algoResult = FALSE;

  if (m_bRunning && !(m_pHallAnalyzer.null()) && (m_pMainWnd != 0))
  {
    DocumentIterator<CCamAnalyzerDoc> iter( GetCamAnalyzerDocTempl() );
    CCamAnalyzerDoc *                 pDoc = 0;
    int                               runCount = 0;

    // Process each camera.
    for (pDoc = iter.GetFirst(); pDoc != 0; pDoc = iter.GetNext())
    {
      algoResult = (pDoc->OnIdleProcessing()) ? TRUE : algoResult;
      runCount += (pDoc->IsRunning() ? 1 : 0);
    }

    // Compute overall statistics.
    try
    {
      if (runCount == 0)
      {
        StopProcessing();
        return result;
      }

      if (m_pHallAnalyzer->ProcessData( &m_spectatorNum, Time(), 0 ) == S_OK)
      {
        wchar_t txt[64];
        _stprintf( txt, L" human: %d", m_spectatorNum );
        (reinterpret_cast<CMainFrame*>( m_pMainWnd ))->m_wndStatusBar.SetPaneText( 1, txt );
      }
      else
      {
        (reinterpret_cast<CMainFrame*>( m_pMainWnd ))->m_wndStatusBar.SetPaneText( 1, L"" );
        m_output.PrintW( IDebugOutput::mt_error, L"Hall analyzer failed to process data" );
        m_bRunning = false;
      }
    }
    catch (std::runtime_error & e)
    {
      CString msg( e.what() );
      m_output.PrintW( IDebugOutput::mt_error, (LPCWSTR)msg );
    }

    // Draw current results.
    for (pDoc = iter.GetFirst(); pDoc != 0; pDoc = iter.GetNext())
      DrawCurrentResult( pDoc );
  }
  return (result || algoResult);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function launches all camera analyzers. */
//-------------------------------------------------------------------------------------------------
void CAlgoDebuggerApp::LaunchProcessing()
{
  try
  {
    DocumentIterator<CCamAnalyzerDoc> iter( GetCamAnalyzerDocTempl() );
    CCamAnalyzerDoc *                 pDoc = 0;
    int                               initCount = 0, chairCount = 0;

    m_bRunning = false;
    m_spectatorNum = 0;
    ALIB_ASSERT( AreDataReady() );
    m_pHallAnalyzer.reset();

    // Initialize all camera analyzers.
    for (pDoc = iter.GetFirst(); pDoc != 0; pDoc = iter.GetNext())
    {
      initCount += (pDoc->InitializeCameraAnalyzer()) ? 1 : 0;
      chairCount += (int)(pDoc->m_chairs.size());
    }

    if (initCount == 0)
    {
      m_output.PrintW( IDebugOutput::mt_error, L"No camera analyzer has been launched" );
      return;
    }

    // Launch all camera analyzers.
    m_chairs.clear();
    m_chairs.reserve( chairCount );
    for (pDoc = iter.GetFirst(); pDoc != 0; pDoc = iter.GetNext())
    {
      pDoc->StartCameraAnalyzer();
      m_chairs.insert( m_chairs.end(), pDoc->m_chairs.begin(), pDoc->m_chairs.end() );
    }

    // Launch hall analyzer.
    {
      Int8Arr parameters;
      HRESULT_ASSERT( theApp.m_GetParameters( &parameters, &m_output ) );
      IHallAnalyzer * pha = 0;
      HRESULT_ASSERT( theApp.m_CreateHallAnalyzer( &pha, &parameters, &m_chairs, this->Time(), &m_output ) );
      m_pHallAnalyzer.reset( pha );
    }

    // Open log file.
    if (!(m_hallAnalyzerLogFile.empty()))
    {
      csalgo::TLogFileName logf;
      logf.data = m_hallAnalyzerLogFile;
      HRESULT_ASSERT( m_pHallAnalyzer->SetData( &logf ) );
    }
    
    m_bRunning = true;
    m_demoCounter = 0;
  }
  catch (std::runtime_error & e)
  {
    CloseAllDocuments( FALSE );
    Clear();
    CString msg( e.what() );
    m_output.PrintW( IDebugOutput::mt_error, (LPCWSTR)msg );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function stops the hall analyzer and all camera analyzers. */
//-------------------------------------------------------------------------------------------------
void CAlgoDebuggerApp::StopProcessing()
{
  try
  {
    m_bRunning = false;
    m_pHallAnalyzer.reset();
    DocumentIterator<CCamAnalyzerDoc> iter( GetCamAnalyzerDocTempl() );
    for (CCamAnalyzerDoc * pDoc = iter.GetFirst(); pDoc != 0; pDoc = iter.GetNext())
      pDoc->StopCameraAnalyzer();
  }
  catch (std::runtime_error & e)
  {
    CloseAllDocuments( FALSE );
    Clear();
    m_output.PrintA( IDebugOutput::mt_error, e.what() );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function checks whether application is ready to run.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
bool CAlgoDebuggerApp::AreDataReady() const
{
  bool bChairs = false;
  DocumentIterator<CCamAnalyzerDoc> iter( m_pCamAnalyzerDocTemplate );
  for (CCamAnalyzerDoc * pDoc = iter.GetFirst(); (pDoc != 0) && !bChairs; pDoc = iter.GetNext())
    bChairs = !(pDoc->m_chairs.empty());
  return ((m_hAlgoLib != 0) && !(m_city_address.empty()) && !(m_city_address_hall.empty()) && bChairs);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function transfers data to hall analyzer.

  \param  pCamAnalyzer  the source camera analyzer.
  \return               Ok = true. */
//-------------------------------------------------------------------------------------------------
bool CAlgoDebuggerApp::TransferData( ICameraAnalyzer * pCamAnalyzer )
{
  if ((pCamAnalyzer != 0) && (!(m_pHallAnalyzer.null())))
  {
    return (SUCCEEDED( pCamAnalyzer->GetData( &m_camStatistics ) ) &&
            SUCCEEDED( m_pHallAnalyzer->SetData( &m_camStatistics ) ));
  }
  return false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function draws the current result and updates view.

  \param  pDoc  pointer to the document class that owns camera analyzer.  */
//-------------------------------------------------------------------------------------------------
void CAlgoDebuggerApp::DrawCurrentResult( CCamAnalyzerDoc * pDoc )
{
  // Draw resultant image and update statusbar's indicators.
  if ((pDoc != 0) && (!(m_pHallAnalyzer.null())) && pDoc->IsRunning())
  {
    for (POSITION pos = pDoc->GetFirstViewPosition(); pos != 0;)
    {
      CWnd * pView = pDoc->GetNextView( pos );
      if ((pView != 0) && pView->IsKindOf( RUNTIME_CLASS( CCamAnalyzerView ) ))
      {
        if (m_appParams.s_app.p_bShowDemoImage() && pDoc->m_layout.ready())
        {
          pDoc->m_demoImg.data.first = pDoc->m_cameraNo;
          HRESULT_ASSERT( m_pHallAnalyzer->GetData( &(pDoc->m_demoImg) ) );

          // Save demo image.
          #if 0
          {
            TCHAR fname[_MAX_PATH+1];
            _stprintf( fname, L"d:\\demo\\%05d.bmp", m_demoCounter );
            if (!(csutility::SaveQImage( fname, &(pDoc->m_demoImg.data.second), INVERT_AXIS_Y )))
              theApp.m_output.PrintW( IDebugOutput::mt_error, L"Failed to save demo image" );
            else ++m_demoCounter;
          }
          #endif

          CClientDC dc( pView );
          csutility::DrawColorImage( &(pDoc->m_demoImg.data.second), dc.GetSafeHdc(), pDoc->m_layout.rectangle() );
        }

        CWnd * pFrame = pView->GetParentFrame();
        if (pFrame != 0)
          pFrame->PostMessage( MYMESSAGE_UPDATE_STATE_INDICATORS, 0, 0 );

        break;
      }
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function reloads algorithm library.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
bool CAlgoDebuggerApp::ReloadAlgorithmLibrary()
{
  #define __MY_GET_PROC_ADDR__(name) m_##name = (csalgocommon::P##name)(GetProcAddress( m_hAlgoLib, #name ))

  try
  {
    CloseAllDocuments( FALSE );
    Clear();
    ALIB_VERIFY( (m_hAlgoLib == 0) || AfxFreeLibrary( m_hAlgoLib ), L"Failed to free algorithm DLL" );
    m_hAlgoLib = 0;

    int     algo = m_appParams.s_app.p_algoVersion();
    wchar_t dllName[_MAX_PATH+1];
    ALIB_ASSERT( _snwprintf( dllName, _MAX_PATH, L"%sCSAlgorithm%d.dll", (m_appPath.c_str()), algo ) > 0 );
    ALIB_VERIFY( ((CFileFind()).FindFile( dllName )), L"Specified algorithm DLL does not exist" );

    m_hAlgoLib = AfxLoadLibrary( dllName );
    ALIB_VERIFY( (m_hAlgoLib != 0), L"Failed to load algorithm DLL" );

    ALIB_ASSERT( (__MY_GET_PROC_ADDR__( CreateBackgroundAccumulator )) != 0 );
    ALIB_ASSERT( (__MY_GET_PROC_ADDR__( CreateCameraAnalyzer        )) != 0 );
    ALIB_ASSERT( (__MY_GET_PROC_ADDR__( CreateHallAnalyzer          )) != 0 );
    ALIB_ASSERT( (__MY_GET_PROC_ADDR__( ShowParameterDialog         )) != 0 );
    ALIB_ASSERT( (__MY_GET_PROC_ADDR__( SetDefaultParameters        )) != 0 );
    ALIB_ASSERT( (__MY_GET_PROC_ADDR__( ShowParameterDialogEx       )) != 0 );
    ALIB_ASSERT( (__MY_GET_PROC_ADDR__( SetDefaultParametersEx      )) != 0 );
    ALIB_ASSERT( (__MY_GET_PROC_ADDR__( GetParameters               )) != 0 );
  }
  catch (alib::GeneralError & e)
  {
    Clear();
    if (m_hAlgoLib != 0)
      AfxFreeLibrary( m_hAlgoLib );

    m_hAlgoLib = 0;
    m_CreateBackgroundAccumulator = 0;
    m_CreateCameraAnalyzer = 0;
    m_CreateHallAnalyzer = 0;
    m_ShowParameterDialog = 0;
    m_SetDefaultParameters = 0;
    m_ShowParameterDialogEx = 0;
    m_SetDefaultParametersEx = 0;
    m_GetParameters = 0;

    m_output.PrintA( IDebugOutput::mt_error, e.what() );
    return false;
  }
  return true;

  #undef __MY_GET_PROC_ADDR__
}

