#include "stdafx.h"
#include "../algo_debugger.h"
#include "cam_analyzer_doc.h"
#include "cam_analyzer_view.h"
#include "cam_analyzer_frm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCamAnalyzerFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CCamAnalyzerFrame, CMDIChildWnd)
  ON_WM_CREATE()
  ON_MESSAGE( MYMESSAGE_UPDATE_STATUSBAR_TEXT, OnMyUpdateStatusBarText )
  ON_MESSAGE( MYMESSAGE_UPDATE_STATE_INDICATORS, OnMyUpdateStateIndicators )
  ON_MESSAGE( MYMESSAGE_UPDATE_WINDOW_TITLE, OnMyUpdateWindowTitle )
END_MESSAGE_MAP()

static UINT indicators[] =
{
  ID_SEPARATOR,           // status line indicator
  INDICATOR_INDEX,
  INDICATOR_DROPPED_FRAME,
  INDICATOR_FPS
};


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CCamAnalyzerFrame::CCamAnalyzerFrame()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CCamAnalyzerFrame::~CCamAnalyzerFrame()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when an application requests that the Windows
           window be created by calling the Create() or CreateEx() member function. */
//-------------------------------------------------------------------------------------------------
int CCamAnalyzerFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if (CMDIChildWnd::OnCreate( lpCreateStruct ) == -1)
    return -1;

  // Create status bar.
  {
    if (!m_wndStatusBar.Create( this ) ||
        !m_wndStatusBar.SetIndicators( indicators, sizeof(indicators)/sizeof(UINT) ))
    {
      TRACE( L"Failed to create status bar\n" );
      return -1;
    }

    // Set the small width of the first pane of the status bar,
    // so that other panes are still visible even if window's width was decreased significantly.
    m_wndStatusBar.SetPaneInfo( 0, ID_SEPARATOR, SBPS_STRETCH, 20 );

    CClientDC dc( &m_wndStatusBar );
    CRect     rect;
    UINT      flags = DT_CALCRECT | DT_LEFT | DT_NOCLIP | DT_SINGLELINE | DT_INTERNAL;

    rect.SetRect( 0, 0, 1, 1 );
    dc.DrawText( L"frame: 00000000", -1, &rect, flags );
    m_wndStatusBar.SetPaneInfo( 1, INDICATOR_INDEX, SBPS_NORMAL, rect.Width() );

    rect.SetRect( 0, 0, 1, 1 );
    dc.DrawText( L"drop: 00000000", -1, &rect, flags );
    m_wndStatusBar.SetPaneInfo( 2, INDICATOR_DROPPED_FRAME, SBPS_NORMAL, rect.Width() );

    rect.SetRect( 0, 0, 1, 1 );
    dc.DrawText( L"fps: 00.00", -1, &rect, flags );
    m_wndStatusBar.SetPaneInfo( 3, INDICATOR_FPS, SBPS_NORMAL, rect.Width() );
  }

  return 0;
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework before the creation of the window attached to this object. */
//-------------------------------------------------------------------------------------------------
BOOL CCamAnalyzerFrame::PreCreateWindow( CREATESTRUCT & cs )
{
  // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
  if (!(CMDIChildWnd::PreCreateWindow( cs )))
    return FALSE;

  cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
             FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

  return TRUE;
}


//-------------------------------------------------------------------------------------------------
// CCamAnalyzerFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CCamAnalyzerFrame::AssertValid() const
{
  CMDIChildWnd::AssertValid();
}
void CCamAnalyzerFrame::Dump( CDumpContext & dc ) const
{
  CMDIChildWnd::Dump( dc );
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Function updates the main pane of the status bar. */
//-------------------------------------------------------------------------------------------------
LRESULT CCamAnalyzerFrame::OnMyUpdateStatusBarText( WPARAM, LPARAM )
{
  CCamAnalyzerDoc * pDoc = reinterpret_cast<CCamAnalyzerDoc*>( GetActiveDocument() );
  m_wndStatusBar.SetWindowText( (pDoc != 0) ? pDoc->m_statusText.c_str() : L"" );
  return 1;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function updates frame index indicator of the status bar. */
//-------------------------------------------------------------------------------------------------
LRESULT CCamAnalyzerFrame::OnMyUpdateStateIndicators( WPARAM, LPARAM )
{
  CCamAnalyzerDoc * pDoc = reinterpret_cast<CCamAnalyzerDoc*>( GetActiveDocument() );
  wchar_t           txt[64] = L"";

  if (pDoc != 0)
  {
    if (pDoc->m_frmCount.data.first >= 0) _stprintf( txt, L" frame: %d", pDoc->m_frmCount.data.first );
    else                                  _stprintf( txt, L" frame" );
    m_wndStatusBar.SetPaneText( 1, txt );

    if (pDoc->m_frmCount.data.second >= 0) _stprintf( txt, L" drop: %d", pDoc->m_frmCount.data.second );
    else                                   _stprintf( txt, L" drop" );
    m_wndStatusBar.SetPaneText( 2, txt );

    _stprintf( txt, L" fps: %3.1f", pDoc->m_fps );
    m_wndStatusBar.SetPaneText( 3, txt );
  }
  else
  {
    m_wndStatusBar.SetPaneText( 1, L"" );
    m_wndStatusBar.SetPaneText( 2, L"" );
    m_wndStatusBar.SetPaneText( 3, L"" );
  }
  return 1;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function updates the title of this child window. */
//-------------------------------------------------------------------------------------------------
LRESULT CCamAnalyzerFrame::OnMyUpdateWindowTitle( WPARAM, LPARAM )
{
  CCamAnalyzerDoc * pDoc = reinterpret_cast<CCamAnalyzerDoc*>( GetActiveDocument() );
  if (pDoc != 0)
  {
    wchar_t title[128];
    _stprintf( title, L"Camera %d", pDoc->m_cameraNo );
    pDoc->SetTitle( title );
  }
  return 1;
}

