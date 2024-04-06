#pragma once

#ifndef  MY_VIDEO_TOOL_BAR_H
#include "video_tool_bar.h"
#endif

#include "../ChildFrm.h"

class CTrainDataDoc;

class CTrainDataFrame : public CChildFrame
{
  DECLARE_DYNCREATE(CTrainDataFrame)

protected:
  CStatusBar    m_wndStatusBar;
public:
	CToolBar	m_wndVideoBar;
  VideoToolBar  m_wndPrepareDataBar;

public:
  CTrainDataFrame();
  virtual ~CTrainDataFrame();
  CTrainDataDoc * GetDocument();

  int  SetCurrentFrame( int frameNo );
  void SetFrameIndexIndicator( int frameNo );
  void SetFrameRateIndicator( float fps, bool bShow = true );
  void SetSpeedIndicator( float speed, bool bShow = true );
  void SetCursorIndicator( int x, int y );
  void SetPlayButtonImage( bool bPlay );
  void EnableFrameScroller( bool bEnable );

  virtual BOOL InitToolBar();
  virtual BOOL InitStatusBar();
  virtual BOOL PreCreateWindow( CREATESTRUCT & cs );

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
  afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
  virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext * pContext );
};


#ifndef _DEBUG  // non-debug version
inline CTrainDataDoc * CTrainDataFrame::GetDocument() 
{
  return reinterpret_cast<CTrainDataDoc*>( GetActiveDocument() );
}
#endif


