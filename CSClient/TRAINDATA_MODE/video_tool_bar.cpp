#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "res/resource.h"
#include "video_tool_bar.h"
#include "train_data_frm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
VideoToolBar::VideoToolBar()
{
  m_pParent = 0;
  m_playButtonIndex = -1;
  m_playButtonStyle = 0;
  m_playButtonImage = -1;
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
VideoToolBar::~VideoToolBar()
{
}


BEGIN_MESSAGE_MAP(VideoToolBar, CAlphaToolBar)
  ON_WM_HSCROLL()
END_MESSAGE_MAP()


//-------------------------------------------------------------------------------------------------
/** \brief Function creates and initializes this tool-bar.

  \param  pParent    the parent, main-frame window.
  \param  bToolTips  not used. 
  \return            Ok = TRUE. */
//-------------------------------------------------------------------------------------------------
BOOL VideoToolBar::Init( CTrainDataFrame * pParent, BOOL bToolTips )
{
  if (pParent == 0)
    return FALSE;

  DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | /*CBRS_GRIPPER |*/ CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
  dwStyle |= (bToolTips ? CBRS_TOOLTIPS : 0);

  m_pParent = pParent;
  if (!CreateEx( pParent, TBSTYLE_FLAT, dwStyle ))
    return FALSE;

  if (!LoadToolBar( /*IDR_VideoToolbar*/IDR_PrepareData ))
    return FALSE;

  // Hide the pause button (set its style to separator) and receive information about the play button.
  int index = CommandToIndex( IDM_Play_Play_Sequence );
  if (index >= 0)
  {
    UINT id = IDM_Play_Play_Sequence;
    m_playButtonIndex = index;
    GetButtonInfo( index, id, m_playButtonStyle, m_playButtonImage );
    SetButtonInfo( index+1, IDM_Play_Play_Sequence, TBBS_SEPARATOR, -1 );
  }

  // Create frame slider.
  // FIXME : magic numbers
  CRect rect( 0, 0, FRAME_SCROLL_WIDTH, /*FRAME_SCROLL_HEIGHT*/ 24 + 6 );
  if (!m_frameScroll.Create( WS_CHILD | TBS_HORZ | TBS_NOTICKS | TBS_TOOLTIPS, rect, this, IDM_Edit_FrameSlider ))
    return FALSE;

  index = CommandToIndex( IDM_Edit_FrameSlider );
  SetButtonInfo( index, IDM_Edit_FrameSlider, TBBS_SEPARATOR, FRAME_SCROLL_WIDTH );
  if (m_frameScroll.m_hWnd != 0)
  {
    m_frameScroll.SetRange( 0, 0 );
    m_frameScroll.SetPos( 0 );

    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOCOPYBITS | SWP_SHOWWINDOW;
    GetItemRect( index, rect );
    m_frameScroll.SetWindowPos( 0, rect.left, rect.top, 0, 0, flags );
  }
  return TRUE;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function changes the image of the play button according to the current state.

  \param  bPlay  if true, then the "Play" image will be set, otherwise the "Pause" one. */
//-------------------------------------------------------------------------------------------------
void VideoToolBar::SetPlayButtonImage( bool bPlay )
{
  if ((m_playButtonIndex >= 0) && (m_playButtonImage >= 0))
    SetButtonInfo( m_playButtonIndex, IDM_Play_Play_Sequence, m_playButtonStyle,
                   m_playButtonImage + (bPlay ? 0 : 1) );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when
           the user clicks a window's horizontal scroll bar. */
//-------------------------------------------------------------------------------------------------
void VideoToolBar::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar )
{
  if (pScrollBar == 0)
    return;

  if (m_pParent != 0)
  {
    switch (nSBCode)
    {
      case SB_THUMBPOSITION:
        m_pParent->SetCurrentFrame( (int)nPos );
        break;

      case SB_THUMBTRACK:
        m_pParent->SetFrameIndexIndicator( (int)nPos );
        break;

      case SB_LEFT:
      case SB_LINELEFT:
      case SB_LINERIGHT:
      case SB_PAGELEFT:
      case SB_PAGERIGHT:
      case SB_RIGHT:
        nPos = m_frameScroll.GetPos();
        m_pParent->SetCurrentFrame( nPos );
        break;
    }

    pScrollBar->SetScrollPos( nPos, TRUE );
  }
  CAlphaToolBar::OnHScroll( nSBCode, nPos, pScrollBar );
}


void	VideoToolBar::StartEdit()
{
	m_frameScroll.StartEdit( /*GetCurrentContent()*/ );
}

void	VideoToolBar::EndEdit()
{
	m_frameScroll.EndEdit(  );
}

void	VideoToolBar::CancelEdit()
{
	m_frameScroll.CancelEdit(  );
}
