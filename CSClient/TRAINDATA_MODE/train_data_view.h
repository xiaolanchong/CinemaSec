#pragma once

#include "../CommonView/base_idle_view.h"

class CTrainDataDoc;

//-------------------------------------------------------------------------------------------------
/** \class CTrainDataView.
    \brief The basic class that incapsulates AVI stream playing methods. */
//-------------------------------------------------------------------------------------------------
class CTrainDataView : public BaseIdleView
{
protected:
  CBitmap         m_bitmap;       //!< persistent object allocated for drawing
  CSize           m_bmpSize;      //!< the current size of the bitmap
  int             m_stretchMode;  //!< stretching mode of a device context
  bool            m_bPlay;        //!< the flag indicates that a sequence is being played now
  bool            m_bPause;       //!< the flag indicates that a sequence is paused, but not stopped
  DWORD           m_sleepTime;    //!< the time to sleep between successive frames
  int             m_frame;        //!< the index of the current frame
  float           m_fps;          //!< frames per second (frame rate)
  int             m_speedNo;      //!< the index of playing speed of a videosequence

  int			      m_nAnimation;
  CImageList	  m_ilContent;

  Arr2ub   m_tmpImage;      //!< temporal image
  Arr2ub   m_auxImage;      //!< auxiliary image
  UByteArr m_tmpBuffer;     //!< temporal buffer

protected:
  virtual void InitPlayingFromSelectedFrame( int startFrame );

  CTrainDataView();
  DECLARE_DYNCREATE(CTrainDataView)

public:
  virtual void OnUpdate( CView * pSender, LPARAM lHint, CObject * pHint );
  virtual void OnDraw( CDC * pDC );
  virtual void DrawContent( CDC * pDC );
  virtual void UpdateStatusBarInformation();
  virtual void UpdateSpeedInStatusBar();
  virtual void UpdatePlayButtonImage();

  virtual void InitPlaying();
  virtual bool ProceedPlaying();
  virtual void DonePlaying( bool bInterrupted );
  virtual bool OnIdle();
  virtual void OnInitialUpdate();


  void			DrawChair(CDC& dc);

  CTrainDataDoc * GetDocument() const;
  void RecalculateLayout( CDC * pDC );
  void Clear();

  /** \brief Function returns non-zero, if a sequence is playing now. */
  bool IsPlayingNow() const { return (m_bPlay && !m_bPause); }

public:
  virtual ~CTrainDataView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnSize( UINT nType, int cx, int cy );
  afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
  afx_msg BOOL OnEraseBkgnd( CDC* pDC );

  afx_msg void OnFirstFrame();
  afx_msg void OnNFramesBackward();
  afx_msg void OnPrevFrame();
  afx_msg void OnNextFrame();
  afx_msg void OnNFramesForward();
  afx_msg void OnLastFrame();
  afx_msg void OnPlaySequence();
  afx_msg void OnStopSequence();
  afx_msg void OnSpeedUp();
  afx_msg void OnSpeedDown();

  afx_msg void OnUpdateFirstFrame( CCmdUI * pCmdUI );
  afx_msg void OnUpdateNFramesBackward( CCmdUI * pCmdUI );
  afx_msg void OnUpdatePrevFrame( CCmdUI * pCmdUI );
  afx_msg void OnUpdateNextFrame( CCmdUI * pCmdUI );
  afx_msg void OnUpdateNFramesForward( CCmdUI * pCmdUI );
  afx_msg void OnUpdateLastFrame( CCmdUI * pCmdUI );
  afx_msg void OnUpdatePlaySequence( CCmdUI * pCmdUI );
  afx_msg void OnUpdateStopSequence( CCmdUI * pCmdUI );
  afx_msg void OnUpdateSpeedUp( CCmdUI * pCmdUI );
  afx_msg void OnUpdateSpeedDown( CCmdUI * pCmdUI );
};

#ifndef _DEBUG  // debug version in view.cpp
inline CTrainDataDoc * CTrainDataView::GetDocument() const
{
  return reinterpret_cast<CTrainDataDoc*>( m_pDocument );
}
#endif




