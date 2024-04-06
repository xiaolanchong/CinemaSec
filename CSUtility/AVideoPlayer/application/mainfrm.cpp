/********************************************************************************
  mainfrm.cpp
  ---------------------
  begin     : May 2003
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  e-mail    : aaah@mail.ru, aaahaaah@hotmail.com
********************************************************************************/

#include "stdafx.h"
#include "..\resource.h"
#include "view.h"
#include "toolbar.h"
#include "mainfrm.h"
#include "output_write_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
  ON_WM_CREATE()
  ON_WM_SETFOCUS()
  ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
  ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
  ON_COMMAND(IDM_View_VideoInfo, OnViewVideoInfo)
  ON_UPDATE_COMMAND_UI(IDM_View_VideoInfo, OnUpdateViewVideoInfo)
  ON_WM_DROPFILES()
END_MESSAGE_MAP()

static UINT indicators[] =
{
  ID_SEPARATOR,           // status line indicator
  INDICATOR_INDEX,
  INDICATOR_FPS,
  INDICATOR_SPEED,
  INDICATOR_CURSOR,
};


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CMainFrame::CMainFrame()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CMainFrame::~CMainFrame()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when an application requests that the Windows
           window be created by calling the Create or CreateEx member function. */
//-------------------------------------------------------------------------------------------------
int CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  // Create a view to occupy the client area of the frame.
  if (!m_view.Create( NULL, NULL, AFX_WS_DEFAULT_VIEW,
                      CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL ))
  {
    TRACE0( "Failed to create view window\n" );
    return -1;
  }

  // Create toolbar.
  if (!(m_wndToolBar.Init( this )))
  {
    TRACE0( "Failed to create toolbar\n" );
    return -1;
  }

  // Create status bar.
  if (!m_wndStatusBar.Create( this ) ||
      !m_wndStatusBar.SetIndicators( indicators, sizeof(indicators)/sizeof(UINT) ))
  {
    TRACE0( "Failed to create status bar\n" );
    return -1;
  }

  // TODO: Delete these three lines if you don't want the toolbar to be dockable.
  m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY );
  EnableDocking( CBRS_ALIGN_ANY );
  DockControlBar( &m_wndToolBar );

  // Set small width of the first pane of the status bar,
  // so that the other panes are still visible even when window's width decreases significantly.
  {
    m_wndStatusBar.SetPaneInfo( 0, ID_SEPARATOR, SBPS_STRETCH, 20 );

    CClientDC dc( &m_wndStatusBar );
    CRect     rect;

    rect.SetRect( 0, 0, 10, 10 );
    dc.DrawText( "frame: 000000", -1, &rect, DT_CALCRECT | DT_LEFT | DT_NOCLIP | DT_SINGLELINE | DT_INTERNAL );
    m_wndStatusBar.SetPaneInfo( 1, INDICATOR_INDEX, SBPS_NORMAL, rect.Width() );

    rect.SetRect( 0, 0, 10, 10 );
    dc.DrawText( "fps: 00.00", -1, &rect, DT_CALCRECT | DT_LEFT | DT_NOCLIP | DT_SINGLELINE | DT_INTERNAL);
    m_wndStatusBar.SetPaneInfo( 2, INDICATOR_FPS, SBPS_NORMAL, rect.Width() );

    rect.SetRect( 0, 0, 10, 10 );
    dc.DrawText( "speed: 0.00", -1, &rect, DT_CALCRECT | DT_LEFT | DT_NOCLIP | DT_SINGLELINE | DT_INTERNAL );
    m_wndStatusBar.SetPaneInfo( 3, INDICATOR_SPEED, SBPS_NORMAL, rect.Width() );

    rect.SetRect( 0, 0, 10, 10 );
    dc.DrawText( "x=-0000, y=-0000", -1, &rect, DT_CALCRECT | DT_LEFT | DT_NOCLIP | DT_SINGLELINE | DT_INTERNAL );
    m_wndStatusBar.SetPaneInfo( 4, INDICATOR_CURSOR, SBPS_NORMAL, rect.Width() );
  }

  return 0;
}


//-------------------------------------------------------------------------------------------------
/** \brief Called before the creation of the Windows window attached to this CWnd object. */
//-------------------------------------------------------------------------------------------------
BOOL CMainFrame::PreCreateWindow( CREATESTRUCT & cs )
{
  if (!CFrameWnd::PreCreateWindow( cs ))
    return FALSE;

  // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs.
  cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
  cs.lpszClass = AfxRegisterWndClass(0);
  return TRUE;
}


//-------------------------------------------------------------------------------------------------
// CMainFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
  CFrameWnd::AssertValid();
}
void CMainFrame::Dump( CDumpContext & dc ) const
{
  CFrameWnd::Dump(dc);
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Claims the input focus. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::OnSetFocus( CWnd * pOldWnd )
{
  pOldWnd;
  m_view.SetFocus(); // forward focus to the view window
}


//-------------------------------------------------------------------------------------------------
/** \brief Function is called when a new frame has been load. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::FrameHasChanged( BOOL bInvalidateView )
{
  SetFrameIndexIndicator();
  SetFrameRateIndicator();
  SetSpeedIndicator();
  m_view.Invalidate( bInvalidateView );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when the user selects an item from a menu, when
   a child control sends a notification message, or when an accelerator keystroke is translated. */
//-------------------------------------------------------------------------------------------------
BOOL CMainFrame::OnCommand( WPARAM wParam, LPARAM lParam )
{
  if ((HIWORD( wParam ) <= 1) && OnPlayCommand( LOWORD( wParam ) ))
    return TRUE;

  return CFrameWnd::OnCommand( wParam, lParam );
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework to route and dispatch command messages and to handle
           the update of command user-interface objects. */
//-------------------------------------------------------------------------------------------------
BOOL CMainFrame::OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo )
{
  // Let the view have first crack at the command.
  if (m_view.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ))
    return TRUE;

  if ((nCode == CN_UPDATE_COMMAND_UI) && OnUpdatePlayCommand( reinterpret_cast<CCmdUI*>( pExtra ) ))
    return TRUE;

  // Otherwise, do default handling.
  return CFrameWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function opens new video file given its fullpath name. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::OpenNewVideoFile( LPCTSTR name )
{
  m_view.Clear();
  m_view.m_file = name;
  m_wndToolBar.SetFrameScroller( 0, 0, 0 );

  // Select suitable video sequence reader.
  LPCTSTR ext = _tcsrchr( name, _T('.') );
  if (ext != 0)
  {
    ++ext;
    for (int i = 0; i < ALIB_LengOf(VIDEO_FILE_EXTENSION); i++)
    {
      if (_tcscmp( ext, VIDEO_FILE_EXTENSION[i] ) == 0)
      {
        m_view.m_grabberId = (GrabberTypeId)i;
        break;
      }
    }
  }

  // Try to create a frame-grabber and to load the first frame.
  if (m_view.m_grabberId == GRABBER_UNKNOWN)
  {
    m_view.m_output.Print( alib::IMessage::mt_error, _T("Unsupported file type") );
    m_view.Clear();
  }
  else if (m_view.CreateFrameGrabber())
  {
    int res = m_view.m_reader->ReadFrame( &(m_view.m_image), -1,
                                          &(m_view.m_frameNo), &(m_view.m_frameTime) );

    if (res == avideolib::IVideoReader::GENERAL_ERROR)
    {
      m_view.m_output.Print( alib::IMessage::mt_error, _T("Failed to load the first frame") );
      m_view.Clear();
    }
    else if (!(m_view.GetVideoInfo()) ||
              (m_view.m_frameNum < 1) ||
              (m_view.m_width  != m_view.m_image.width()) ||
              (m_view.m_height != m_view.m_image.height()))
    {
      m_view.m_output.Print( alib::IMessage::mt_error, _T("Empty or corrupted file") );
      m_view.Clear();
    }
    else m_view.m_reader->Seek(0);
  }

  // Update appearance.
  if (m_view.m_frameNum > 0)
  {
    LPCTSTR p = 0;
    for (int i = 0; name[i] != _T('\0'); i++)
    {
      if ((name[i] == _T('\\')) || (name[i] == _T('/')))
        p = name + i;
    }

    CString title( APP_TITLE );
    SetWindowText( (LPCTSTR)((title += _T(": ")) += ((p != 0) ? (p+1) : name)) );
  }
  else SetWindowText( (LPCTSTR)APP_TITLE );
  m_wndToolBar.SetFrameScroller( 0, 0, std::max( (m_view.m_frameNum-1), 0 ) );
  m_wndToolBar.EnableFrameScroller( true );
  FrameHasChanged( TRUE );
}


//-------------------------------------------------------------------------------------------------
/** \brief Implements the ID_FILE_OPEN command. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::OnFileOpen()
{
  CFileDialog dlg( TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
                   _T("Video Files (*.avd)|*.avd|Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"),
                   this );
  if (dlg.DoModal() == IDOK)
    OpenNewVideoFile( (LPCTSTR)(dlg.GetPathName()) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Enables the ID_FILE_OPEN command. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::OnUpdateFileOpen( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( m_view.m_bPlay == false );
}


//-------------------------------------------------------------------------------------------------
/** \brief Implements the IDM_View_VideoInfo command. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::OnViewVideoInfo()
{
  AfxMessageBox( m_view.m_videoInfo.c_str(), MB_ICONINFORMATION | MB_OK );
}


//-------------------------------------------------------------------------------------------------
/** \brief Enables the IDM_View_VideoInfo command. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::OnUpdateViewVideoInfo( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( !(m_view.m_videoInfo.empty()) && (m_view.m_bPlay == false) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function implements "Play" menu commands.

  \param  menuItemId  identifier of a menu item. */
//-------------------------------------------------------------------------------------------------
BOOL CMainFrame::OnPlayCommand( unsigned int menuItemId )
{
  const int MAX_SPEED_INDEX = ALIB_LengOf(PLAY_SPEED)-1;
  if (m_view.m_reader.get() == 0)
    return FALSE;

  try
  {
    CWaitCursor wait;

    switch (menuItemId)
    {
      case IDM_First_Frame:
      {
        m_view.m_reader->Seek( 0 );
        m_view.m_reader->ReadFrame( &(m_view.m_image), -1, &(m_view.m_frameNo), &(m_view.m_frameTime) );
        FrameHasChanged( FALSE );
      }
      break;

      case IDM_NFrames_Backward:
      {
        int n = m_view.m_frameNo - std::max( m_view.m_frameNum/100, 3 );
        n = alib::Limit( n, 0, m_view.m_frameNum-1 );
        if (n == 0) n = (m_view.m_frameNum-1);
        m_view.m_reader->Seek( n );
        m_view.m_reader->ReadFrame( &(m_view.m_image), -1, &(m_view.m_frameNo), &(m_view.m_frameTime) );
        FrameHasChanged( FALSE );
      }
      break;

      case IDM_Prev_Frame:
      {
        int n = alib::Limit( m_view.m_frameNo-1, 0, m_view.m_frameNum-1 );
        if (n == 0) n = (m_view.m_frameNum-1);
        m_view.m_reader->Seek( n );
        m_view.m_reader->ReadFrame( &(m_view.m_image), -1, &(m_view.m_frameNo), &(m_view.m_frameTime) );
        FrameHasChanged( FALSE );
      }
      break;

      case IDM_Next_Frame:
      {
        int n = alib::Limit( m_view.m_frameNo+1, 0, m_view.m_frameNum-1 );
        if ((n+1) >= m_view.m_frameNum) n = 0;
        m_view.m_reader->Seek( n );
        m_view.m_reader->ReadFrame( &(m_view.m_image), -1, &(m_view.m_frameNo), &(m_view.m_frameTime) );
        FrameHasChanged( FALSE );
      }
      break;

      case IDM_NFrames_Forward:
      {
        int n = m_view.m_frameNo + std::max( m_view.m_frameNum/100, 3 );
        n = alib::Limit( n, 0, m_view.m_frameNum-1 );
        if ((n+1) >= m_view.m_frameNum) n = 0;
        m_view.m_reader->Seek( n );
        m_view.m_reader->ReadFrame( &(m_view.m_image), -1, &(m_view.m_frameNo), &(m_view.m_frameTime) );
        FrameHasChanged( FALSE );
      }
      break;

      case IDM_Last_Frame:
      {
        m_view.m_reader->Seek( m_view.m_frameNum-1 );
        m_view.m_reader->ReadFrame( &(m_view.m_image), -1, &(m_view.m_frameNo), &(m_view.m_frameTime) );
        FrameHasChanged( FALSE );
      }
      break;

      case IDM_Play_Sequence:
      {
        if ((m_view.m_frameNo+1) >= m_view.m_frameNum)
        {
          m_view.m_reader->Seek( m_view.m_frameNo = 0 );
          m_view.m_reader->ReadFrame( &(m_view.m_image), -1, &(m_view.m_frameNo), &(m_view.m_frameTime) );
        }
        m_view.m_bPlay = true;
        m_wndToolBar.EnableFrameScroller( !(m_view.m_bPlay) );
        FrameHasChanged( FALSE );
      }
      break;

      case IDM_Stop_Sequence:
      {
        m_view.m_bPlay = false;
        m_view.m_sleepTime = 0;
        m_view.m_fps = -1.0;
        m_wndToolBar.EnableFrameScroller( !(m_view.m_bPlay) );
        m_view.m_writer.reset();
        FrameHasChanged( FALSE );
      }
      break;

      case IDM_Speed_Up:
      {
        m_view.m_speedNo = alib::Limit( ++(m_view.m_speedNo), 0, MAX_SPEED_INDEX );
        FrameHasChanged( FALSE );
      }
      break;

      case IDM_Speed_Down:
      {
        m_view.m_speedNo = alib::Limit( --(m_view.m_speedNo), 0, MAX_SPEED_INDEX );
        FrameHasChanged( FALSE );
      }
      break;

      case IDM_Write_Sequence:
      {
        COutputWriteDlg dlg;

        dlg.m_headFrame = 0;
        dlg.m_lastFrame = m_view.m_frameNum-1;
        dlg.m_fps = std::max<float>( m_view.m_fps, 1.0f );

        if (dlg.DoModal() == IDOK)
        {
          if (!(dlg.m_fname.IsEmpty()) && (m_view.m_reader.get() != 0))
          {
            bool bTrueColor = (m_view.m_colorScheme == avideolib::IBaseVideo::TRUECOLOR);
            m_view.m_writer.reset( avideolib::CreateAVideoWriter( (LPCTSTR)(dlg.m_fname),
                                   &(m_view.m_output), bTrueColor, avideolib::INVERT_AXIS_Y ) );

            int head = std::min( dlg.m_headFrame, dlg.m_lastFrame );
            int last = std::max( dlg.m_headFrame, dlg.m_lastFrame );

            m_view.m_headFrame = alib::Limit( head, 0, m_view.m_frameNum-1 );
            m_view.m_lastFrame = alib::Limit( last, 0, m_view.m_frameNum-1 );
            m_view.m_writeFPS = std::max<float>( dlg.m_fps, 1.0f );
            m_view.m_reader->Seek( head );
            OnPlayCommand( IDM_Play_Sequence );
          }
        }
      }
      break;

      default: return FALSE;
    }
  }
  catch (std::runtime_error & e)
  {
    ::AfxMessageBox( e.what() );
  }
  return TRUE;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function updates "Play" menu items that serve for stream playing. */
//-------------------------------------------------------------------------------------------------
BOOL CMainFrame::OnUpdatePlayCommand( CCmdUI * pCmdUI )
{
  const int MAX_SPEED_INDEX = ALIB_LengOf(PLAY_SPEED)-1;
  BOOL      bEnable = FALSE;

  switch (pCmdUI->m_nID)
  {
    case IDM_First_Frame:      bEnable = !(m_view.m_bPlay);                    break;
    case IDM_NFrames_Backward: bEnable = !(m_view.m_bPlay);                    break;
    case IDM_Prev_Frame:       bEnable = !(m_view.m_bPlay);                    break;
    case IDM_Next_Frame:       bEnable = !(m_view.m_bPlay);                    break;
    case IDM_NFrames_Forward:  bEnable = !(m_view.m_bPlay);                    break;
    case IDM_Last_Frame:       bEnable = !(m_view.m_bPlay);                    break;
    case IDM_Play_Sequence:    bEnable = !(m_view.m_bPlay);                    break;
    case IDM_Stop_Sequence:    bEnable =  (m_view.m_bPlay);                    break;
    case IDM_Speed_Up:         bEnable = (m_view.m_speedNo < MAX_SPEED_INDEX); break;
    case IDM_Speed_Down:       bEnable = (m_view.m_speedNo > 0);               break;
    case IDM_Write_Sequence:   bEnable = !(m_view.m_bPlay);                    break;
    default:                   return FALSE;
  }
  pCmdUI->Enable( bEnable && (m_view.m_reader.get() != 0) );
  return TRUE;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes indicator of frame index on the status bar. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::SetFrameIndexIndicator()
{
  if ((m_view.m_frameNum > 0) && alib::IsRange( m_view.m_frameNo, 0, m_view.m_frameNum ))
  {
    TCHAR txt[64];
    _stprintf( txt, _T(" frame: %d"), m_view.m_frameNo );
    m_wndStatusBar.SetPaneText( 1, txt );
    m_wndToolBar.SetFrameScroller( m_view.m_frameNo, 0, (m_view.m_frameNum-1) );
  }
  else
  {
    m_wndStatusBar.SetPaneText( 1, "" );
    m_wndToolBar.SetFrameScroller( 0, 0, std::max( (m_view.m_frameNum-1), 0 ) );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes "fps" indicator on the status bar. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::SetFrameRateIndicator()
{
  if (m_view.m_fps > 0.0f)
  {
    TCHAR txt[64];
    _stprintf( txt, _T(" fps: %3.1f"), m_view.m_fps );
    m_wndStatusBar.SetPaneText( 2, txt );
  }
  else m_wndStatusBar.SetPaneText( 2, "" );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes the "speed" indicator on the status bar. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::SetSpeedIndicator()
{
  if (alib::IsRange<int>( m_view.m_speedNo, 0, ALIB_LengOf(PLAY_SPEED) ))
  {
    TCHAR txt[64];
    _stprintf( txt, _T(" speed: %3.2f"), PLAY_SPEED[m_view.m_speedNo] );
    m_wndStatusBar.SetPaneText( 3, txt );
  }
  else m_wndStatusBar.SetPaneText( 3, "" );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when the user releases the left mouse button
           over a window that has registered itself as the recipient of dropped files. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::OnDropFiles( HDROP hDropInfo )
{
  UINT  nFiles = ::DragQueryFile( hDropInfo, (UINT)-1, NULL, 0 );
  TCHAR szFileName[_MAX_PATH];

  if (nFiles > 0)
  {
    ::DragQueryFile( hDropInfo, 0, szFileName, _MAX_PATH );
    OpenNewVideoFile( szFileName );
  }
  ::DragFinish( hDropInfo );
}

