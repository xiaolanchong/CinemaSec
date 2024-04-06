#pragma once
#if 0
#include "..\FixedControlBar.h"
#endif

#include "../gui/Splitter/FlatSplitterWnd.h"

class CArchiveFrame : public CChildFrame
{
public:
  DECLARE_DYNCREATE(CArchiveFrame)
  CArchiveFrame();
  virtual ~CArchiveFrame();

  CToolBar   m_wndToolBar;
#if 0
  CFixedControlBar	m_wndChooseBar;
#endif
public:
  //{{AFX_VIRTUAL(CAlgoDebugFrame)
  //}}AFX_VIRTUAL

	CFlatSplitterWnd	m_wndSplitter;
	bool			m_bSplitterCreated;

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  //{{AFX_MSG(CAlgoDebugFrame)
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize( UINT nType, int cx, int cy  );
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

