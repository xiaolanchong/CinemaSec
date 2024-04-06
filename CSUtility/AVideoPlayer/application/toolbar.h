#pragma once

class CMainFrame;

class PlayToolBar : public CToolBar
{
private:
  enum { FRAME_SCROLL_WIDTH  = 500,
         FRAME_SCROLL_HEIGHT = 20,
         MAX_POS = ((1<<15)-1) };     // Windows limitation

  CSliderCtrl m_frameScroll;      //!< the scroller of videosequence frames
  int         m_minPos;           //!< actual min. position
  int         m_maxPos;           //!< actual max. position

  int ScrollerPositionToActualPosition( int pos )
  {
    return (MulDiv( m_maxPos-m_minPos, pos, MAX_POS ) + m_minPos);
  }

public:
  PlayToolBar();
  virtual ~PlayToolBar();

  BOOL Init( CMainFrame * pWindow, BOOL bToolTips = FALSE );
  void EnableFrameScroller( bool bEnable );
  void SetFrameScroller( int pos, int min, int max );

protected:
  DECLARE_MESSAGE_MAP()
  afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar );
};

