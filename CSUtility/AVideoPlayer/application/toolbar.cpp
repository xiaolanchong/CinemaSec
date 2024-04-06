#include "stdafx.h"
#include "../resource.h"
#include "view.h"
#include "toolbar.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
PlayToolBar::PlayToolBar()
{
  m_minPos = 0;
  m_maxPos = 0;
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
PlayToolBar::~PlayToolBar()
{
}


BEGIN_MESSAGE_MAP(PlayToolBar, CToolBar)
  ON_WM_HSCROLL()
END_MESSAGE_MAP()


//-------------------------------------------------------------------------------------------------
/** \brief Function creates and initializes this tool-bar.

  \param  pParent    the parent, main-frame window.
  \param  bToolTips  not used. 
  \return            Ok = TRUE. */
//-------------------------------------------------------------------------------------------------
BOOL PlayToolBar::Init( CMainFrame * pParent, BOOL bToolTips )
{
  bToolTips;
  if (pParent == 0)
    return FALSE;

  DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER |
                  CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;

  if (!CreateEx( pParent, TBSTYLE_FLAT, dwStyle ))
    return FALSE;

  if (!LoadToolBar( IDR_MAINFRAME ))
    return FALSE;

  // Create frame slider.
  CRect rect( 0, 0, FRAME_SCROLL_WIDTH, FRAME_SCROLL_HEIGHT );
  if (!m_frameScroll.Create( WS_CHILD | TBS_HORZ | TBS_NOTICKS, rect, this, IDM_Edit_FrameSlider ))
    return FALSE;

  int index = CommandToIndex( IDM_Edit_FrameSlider );
  SetButtonInfo( index, IDM_Edit_FrameSlider, TBBS_SEPARATOR, FRAME_SCROLL_WIDTH );
  if (m_frameScroll.m_hWnd != 0)
  {
    m_frameScroll.SetRange( 0, 0 );
    m_frameScroll.SetPos( 0 );
    m_minPos = 0;
    m_maxPos = 0;

    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOCOPYBITS | SWP_SHOWWINDOW;
    GetItemRect( index, rect );
    m_frameScroll.SetWindowPos( 0, rect.left, rect.top, 0, 0, flags );
  }

  return TRUE;
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when user clicks the frame scroll bar. */
//-------------------------------------------------------------------------------------------------
void PlayToolBar::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar )
{
  if (pScrollBar == 0)
    return;

  CMainFrame * pWin = reinterpret_cast<CMainFrame*>( ::AfxGetMainWnd() );

  if ((pWin != 0) && (pWin->m_view.m_reader.get() != 0))
  {
    int actualPos = ScrollerPositionToActualPosition( nPos );

    switch (nSBCode)
    {
      case SB_THUMBPOSITION:
        pWin->m_view.m_reader->ReadFrame( &(pWin->m_view.m_image), actualPos,
                                          &(pWin->m_view.m_frameNo), &(pWin->m_view.m_frameTime) );
        pWin->FrameHasChanged( FALSE );
        break;

      case SB_THUMBTRACK:
        pWin->m_view.m_frameNo = actualPos;
        pWin->SetFrameIndexIndicator();
        break;

      case SB_LEFT:
      case SB_LINELEFT:
      case SB_LINERIGHT:
      case SB_PAGELEFT:
      case SB_PAGERIGHT:
      case SB_RIGHT:
        nPos = m_frameScroll.GetPos();
        pWin->m_view.m_reader->ReadFrame( &(pWin->m_view.m_image), actualPos,
                                          &(pWin->m_view.m_frameNo), &(pWin->m_view.m_frameTime) );
        pWin->FrameHasChanged( FALSE );
        break;
    }

    pScrollBar->SetScrollPos( nPos, TRUE );
  }
  CToolBar::OnHScroll( nSBCode, nPos, pScrollBar );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function enables/disables frame scroller. */
//-------------------------------------------------------------------------------------------------
void PlayToolBar::EnableFrameScroller( bool bEnable )
{
  m_frameScroll.EnableWindow( bEnable == true );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function sets frame scroller's range and position. */
//-------------------------------------------------------------------------------------------------
void PlayToolBar::SetFrameScroller( int pos, int minPos, int maxPos )
{
  m_minPos = std::min( minPos, maxPos );
  m_maxPos = std::max( minPos, maxPos );
  pos = std::max( pos, m_minPos );
  pos = std::min( pos, m_maxPos );

  pos = (m_minPos < m_maxPos) ? MulDiv( pos-m_minPos, MAX_POS, m_maxPos-m_minPos ) : 0;
  m_frameScroll.SetRange( 0, MAX_POS );
  m_frameScroll.SetPos( pos );
}

