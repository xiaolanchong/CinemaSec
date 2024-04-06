#pragma once

#ifndef  MY_ONE_IMAGE_LAYOUT_H
#include "../../CSUtility/utility/layout.h"
#endif

#include "../CommonView/base_idle_view.h"
#include "MultiImageLayout.h"



class CGaugeHallView : public BaseIdleView
{
protected:
  COLORREF  m_blinkColors[32];    //!< colors are used for blinking effect
  CMenu     m_menu;               //!< float menu


//  RectImageLayout				m_layout;
protected:
  DECLARE_DYNCREATE(CGaugeHallView)
  CGaugeHallView();
  virtual ~CGaugeHallView();

public:
  CGaugeHallDoc * GetDocument();

  static UINT	InvokeGrabDialog(CGaugeHallDoc* pDoc);
  bool	SetImageButtonText(const std::vector<int>& Images );

public:
  //{{AFX_VIRTUAL(CGaugeHallView)
  virtual void OnDraw( CDC * pDC );
  virtual bool OnIdle();
  virtual void OnInitialUpdate();
  virtual void OnUpdate( CView * pSender, LPARAM Hint, CObject * pHint );
  //}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  //{{AFX_MSG(CGaugeHallView)
  afx_msg int  OnCreate( LPCREATESTRUCT lpCreateStruct );
  afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
  afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
  afx_msg void OnMouseMove( UINT nFlags, CPoint point );
  afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
  afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
  afx_msg void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnCancelMode();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  
inline CGaugeHallDoc * CGaugeHallView::GetDocument() // non-debug version
{
  return reinterpret_cast<CGaugeHallDoc*>( m_pDocument );
}
#endif

