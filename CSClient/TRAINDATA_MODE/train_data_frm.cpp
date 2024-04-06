#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "res/resource.h"
#include "train_data_doc.h"
#include "train_data_view.h"
#include "train_data_frm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTrainDataFrame, CChildFrame)

BEGIN_MESSAGE_MAP(CTrainDataFrame, CChildFrame)
  ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
  ID_SEPARATOR,           // status line indicator
  INDICATOR_INDEX,
  INDICATOR_FPS,
  INDICATOR_SPEED,
  INDICATOR_CURSOR,
};


CTrainDataFrame::CTrainDataFrame()
{
}


CTrainDataFrame::~CTrainDataFrame()
{
}


BOOL CTrainDataFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext * pContext )
{
  if (!(CChildFrame::OnCreateClient( lpcs, pContext )))
    return FALSE;

  ASSERT( (pContext != 0) && (pContext->m_pCurrentDoc != 0) && 
          pContext->m_pCurrentDoc->IsKindOf( RUNTIME_CLASS( CTrainDataDoc ) ) );
  (reinterpret_cast<CTrainDataDoc*>( pContext->m_pCurrentDoc ))->m_pFrameWin = this;
  return TRUE;
}


int CTrainDataFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if (CChildFrame::OnCreate( lpCreateStruct ) == -1)
    return -1;
  if (!InitToolBar())
    return -1;
  if (!InitStatusBar())
    return -1;
  return 0;
}


BOOL CTrainDataFrame::InitToolBar()
{
  if( !	m_wndVideoBar.CreateEx( this, TBSTYLE_FLAT, WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS, CRect(0,0,0,0), 1111 ) ||
		! m_wndVideoBar.LoadToolBar( IDR_VideoToolbar ) )
	{
    TRACE0( "Failed to create toolbar\n" );
    return FALSE;
	}

  ASSERT( this->GetSafeHwnd() != 0 );
  if (!(m_wndPrepareDataBar.Init( this, true )))
  {
    TRACE0( "Failed to create toolbar\n" );
    return FALSE;
  }

  // TODO: Delete these three lines if you don't want the toolbar to be dockable.
  //m_wndVideoToolBar.EnableDocking( CBRS_ALIGN_ANY );
  //EnableDocking( CBRS_ALIGN_ANY );
  //DockControlBar( &m_wndVideoToolBar );
  return TRUE;
}


BOOL CTrainDataFrame::InitStatusBar()
{
  ASSERT( this->GetSafeHwnd() != 0 );
  if (!m_wndStatusBar.Create( this ) ||
      !m_wndStatusBar.SetIndicators( indicators, sizeof(indicators)/sizeof(UINT) ))
  {
    TRACE0( "Failed to create status bar\n" );
    return FALSE;
  }

  // Set small the width of the first pane of the status bar,
  // so that the other panes are still visible even when window's width decreases significantly.
  m_wndStatusBar.SetPaneInfo( 0, ID_SEPARATOR, SBPS_STRETCH, 20 );

  CClientDC dc( &m_wndStatusBar );
  CRect     rect;
  UINT      flags = DT_CALCRECT | DT_LEFT | DT_NOCLIP | DT_SINGLELINE | DT_INTERNAL;

  rect.SetRect( 0, 0, 10, 10 );
  dc.DrawText( _T("frame: 000000"), -1, &rect, flags );
  m_wndStatusBar.SetPaneInfo( 1, INDICATOR_INDEX, SBPS_NORMAL, rect.Width() );

  rect.SetRect( 0, 0, 10, 10 );
  dc.DrawText( _T("fps: 00.00"), -1, &rect, flags );
  m_wndStatusBar.SetPaneInfo( 2, INDICATOR_FPS, SBPS_NORMAL, rect.Width() );

  rect.SetRect( 0, 0, 10, 10 );
  dc.DrawText( _T("speed: 0.00"), -1, &rect, flags );
  m_wndStatusBar.SetPaneInfo( 3, INDICATOR_SPEED, SBPS_NORMAL, rect.Width() );

  rect.SetRect( 0, 0, 10, 10 );
  dc.DrawText( _T("x=-0000, y=-0000"), -1, &rect, flags );
  m_wndStatusBar.SetPaneInfo( 4, INDICATOR_CURSOR, SBPS_NORMAL, rect.Width() );
  return TRUE;
}


BOOL CTrainDataFrame::PreCreateWindow( CREATESTRUCT & cs )
{
  // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
  if (!(CChildFrame::PreCreateWindow( cs )))
    return FALSE;

  cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
             FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;
	cs.style &= ~FWS_ADDTOTITLE;
  return TRUE;
}


//-------------------------------------------------------------------------------------------------
// CTrainDataFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CTrainDataFrame::AssertValid() const
{
  CChildFrame::AssertValid();
}
void CTrainDataFrame::Dump( CDumpContext & dc ) const
{
  CChildFrame::Dump( dc );
}
CTrainDataDoc * CTrainDataFrame::GetDocument()
{
  CDocument * pDoc = GetActiveDocument();
  ASSERT( (pDoc != 0) && pDoc->IsKindOf( RUNTIME_CLASS( CTrainDataDoc ) ) );
  return (CTrainDataDoc*)pDoc;
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Function sets the current frame given new frame index.

  \param  frameNo  the index of desired frame.
  \return          the actually set frame index or -1 on failure. */
//-------------------------------------------------------------------------------------------------
int CTrainDataFrame::SetCurrentFrame( int frameNo )
{
  CWaitCursor wait;

  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc == 0) || !(pDoc->m_avi.IsOk()))
    return -1;

  CTrainDataView * pAviView = pDoc->GetTrainDataView();
  if (pAviView == 0)
    return -1;

  frameNo = alib::Limit( frameNo, 0, pDoc->m_avi.GetFrameNumber() );

  pAviView->OnStopSequence();
  if (frameNo != pDoc->m_avi.GetFrameIndex())
  {
    pDoc->m_avi.GetFrame( frameNo );
    frameNo = pDoc->m_avi.GetFrameIndex();
  }
  SetFrameIndexIndicator( frameNo );
  pAviView->Invalidate( FALSE );
  return frameNo;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes indicator of frame index of the status bar.

  \param  frameNo  the index of a frame to be shown,
                   or -1 - the index of current frame will be used. */
//-------------------------------------------------------------------------------------------------
void CTrainDataFrame::SetFrameIndexIndicator( int frameNo )
{
  USES_CONVERSION;

  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    int  index = (frameNo < 0) ? pDoc->m_avi.GetFrameIndex() : frameNo;
    int  nFrame = pDoc->m_avi.GetFrameNumber();
    char txt[64];

    if (nFrame > 0)
    {
      index = ALIB_LIMIT( index, 0, (nFrame-1) );
      sprintf( txt, " frame: %d", index );
      m_wndStatusBar.SetPaneText( 1, A2CT(txt) );
      m_wndPrepareDataBar.m_frameScroll.SetRange( 0, nFrame-1 );
      m_wndPrepareDataBar.m_frameScroll.SetPos( index );
    }
  }
  else
  {
    m_wndStatusBar.SetPaneText( 1, _T("") );
    m_wndPrepareDataBar.m_frameScroll.SetPos( 0 );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes "fps" indicator of the status bar.

  \param  fps    the frame-per-second value.
  \param  bShow  if true, then a fps value will be shown, otherwise hidden. */
//-------------------------------------------------------------------------------------------------
void CTrainDataFrame::SetFrameRateIndicator( float fps, bool bShow )
{
  USES_CONVERSION;

  char txt[64];

  if (bShow && (fps > 0.0f))
  {
    sprintf( txt, " fps: %3.1f", fps );
    m_wndStatusBar.SetPaneText( 2, A2CT(txt) );
  }
  else m_wndStatusBar.SetPaneText( 2, _T("") );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes the "speed" indicator of the status bar.

  \param  speed  the multiplier that accelerates or delays playing of a videosequence.
  \param  bShow  if true, then a speed value will be shown, otherwise hidden. */
//-------------------------------------------------------------------------------------------------
void CTrainDataFrame::SetSpeedIndicator( float speed, bool bShow )
{
  USES_CONVERSION;

  char txt[64];

  if (bShow && (speed > 0.0f))
  {
    sprintf( txt, " speed: %3.2f", speed );
    m_wndStatusBar.SetPaneText( 3, A2CT(txt) );
  }
  else m_wndStatusBar.SetPaneText( 3, _T("") );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes the "cursor" indicator of the status bar.

  \param  x  cursor's abscissa.
  \param  y  cursor's ordinate. */
//-------------------------------------------------------------------------------------------------
void CTrainDataFrame::SetCursorIndicator( int x, int y )
{
  USES_CONVERSION;

  char txt[64];

  if ((x >= 0) && (y >= 0))
  {
    sprintf( txt, " x=%d, y=%d", x, y );
    m_wndStatusBar.SetPaneText( 4, A2CT(txt) );
  }
  else m_wndStatusBar.SetPaneText( 4, _T("") );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function changes the image of the play button according to the current state.

  \param  bPlay  if true, then the "Play" image will be set, otherwise the "Pause" one. */
//-------------------------------------------------------------------------------------------------
void CTrainDataFrame::SetPlayButtonImage( bool bPlay )
{
	UNREFERENCED_PARAMETER(bPlay);
	//FIXME: play button now not in PrepareData Tool bar but in video control toolbar
 // m_wndVideoToolBar.SetPlayButtonImage( bPlay );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function enables/disables the frame scroller of the tool-bar.

  \param  bEnable  if true, then the scroller will be enabled, otherwise disabled. */
//-------------------------------------------------------------------------------------------------
void CTrainDataFrame::EnableFrameScroller( bool bEnable )
{
  m_wndPrepareDataBar.m_frameScroll.EnableWindow( bEnable == true );
}

